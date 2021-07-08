/*
 * Copyright 2021 Pedram Tiv
 *
 * Licensed under the Teave Public License (the "License");
 * You may obtain a copy of the License at https://www.teave.io/teave-public-license.html
 *
 * This software is distributed under the License on an "AS IS" BASIS, WITHOUT WARRANTIES 
 * OF ANY KIND, either express or implied. See the License for the specific language governing 
 * terms and conditions for use, reproduction, and distribution.
 */
#include <teave/socket/socket.h>
#include <unistd.h>
#include <cstring>

using std::string;
namespace Teave {

Socket::Socket(string path, bool blocking) : path(path), blocking(blocking) {
    int type = SOCK_STREAM | SOCK_CLOEXEC;
    if (!blocking)
        type = type | SOCK_NONBLOCK;

    fd = socket(AF_LOCAL, type, 0);

    if (fd < 0)
        throw Sys_err(errno, "failed to create socket", TE_ERR_LOC);

    load_addr(addr);
}

Socket::~Socket() {
    if (fd > -1) {
        if (type_server)
            unlink(path.c_str());
        close(fd);
    }
}

void Socket::set_opt_passcred() {
    int optval = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) < 0)
        throw Sys_err(errno, "setsockopt SO_PASSCRED 1 err, path: " + path,
                      TE_ERR_LOC);
}

int Socket::get_fd() { return fd; }

string Socket::get_path() { return path; }

void Socket::server_setup(bool rm_existing) {
    if (rm_existing)
        unlink(addr.sun_path);

    type_server = true;
    if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0)
        throw Sys_err(errno, "bind err", TE_ERR_LOC);

    if (listen(fd, 1) < 0) {
        unlink(addr.sun_path);
        throw Sys_err(
            errno, "listen err, path: " + path + " fd: " + std::to_string(fd),
            TE_ERR_LOC);
    }
}

void Socket::client_setup() {
    type_server = false;

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        throw Sys_err(
            errno, "connect err, path: " + path + " fd: " + std::to_string(fd),
            TE_ERR_LOC);
}

int Socket::accept(bool newfd_blocking) {
    errno = 0;
    int flags = SOCK_CLOEXEC;
    if (!newfd_blocking)
        flags = flags | SOCK_NONBLOCK;
    int new_fd = accept4(fd, NULL, NULL, flags);

    if (new_fd < 0) {
        if (blocking)
            throw Sys_err(errno, "accept err", TE_ERR_LOC);
        else if (errno != EAGAIN && errno != EWOULDBLOCK)
            throw Sys_err(errno, "accept err", TE_ERR_LOC);
    }

    return new_fd;
}

void Socket::send_data(int fd, void *data, std::size_t size, int flags) {
    if (send(fd, data, size, flags) < 0)
        throw Sys_err(errno, "send err", TE_ERR_LOC);
}

void Socket::send_fd(int so_fd, int msg_fd, void *data, std::size_t size,
                     int flags) {
    /* ref: http://man7.org/linuxz/man-pages/man3/cmsg.3.html  */

    struct iovec io = {.iov_base = data, .iov_len = size};

    char cbuf[CMSG_SPACE(sizeof(int))];
    struct msghdr msg = {0};
    msg.msg_iov = &io;
    msg.msg_iovlen = 1; // num of iovecs
    msg.msg_control = cbuf;
    msg.msg_controllen = sizeof(cbuf);

    struct cmsghdr *cmhp = CMSG_FIRSTHDR(&msg);
    cmhp->cmsg_level = SOL_SOCKET;
    cmhp->cmsg_type = SCM_RIGHTS;
    cmhp->cmsg_len = CMSG_LEN(sizeof(int));
    *(int *)CMSG_DATA(cmhp) = msg_fd;

    if (sendmsg(so_fd, &msg, flags) < 0)
        throw Sys_err(errno, "sendmsg err", TE_ERR_LOC);
}

ssize_t Socket::recv_data(int from_fd, void *data, size_t data_size) {
    ssize_t ret_size = recv(from_fd, data, data_size, 0);
    if (ret_size < 0)
        throw Sys_err(errno, "recv err", TE_ERR_LOC);
    return ret_size;
}

std::tuple<ssize_t, int> Socket::recv_fd(int from_fd, void *data,
                                         size_t data_size) {
    int ret_fd = -1;
    struct iovec io = {.iov_base = (void *)data, .iov_len = data_size};

    char cbuf[CMSG_SPACE(sizeof(int))];
    struct msghdr msgh = {0};
    msgh.msg_iov = &io;
    msgh.msg_iovlen = 1;
    msgh.msg_name = 0;
    msgh.msg_namelen = 0;
    msgh.msg_control = (caddr_t)cbuf;
    msgh.msg_controllen = sizeof(cbuf);

    ssize_t ret_size = recvmsg(from_fd, &msgh, MSG_CMSG_CLOEXEC);
    if (ret_size < 0)
        throw Sys_err(errno, "recvmsg err", TE_ERR_LOC);
    if (ret_size == 0)
        return {ret_size, ret_fd};

    struct cmsghdr *cmhp = nullptr;
    cmhp = CMSG_FIRSTHDR(&msgh);
    if (cmhp)
        std::memcpy(&ret_fd, CMSG_DATA(cmhp), sizeof(int));
    return {ret_size, ret_fd};
}

std::tuple<ssize_t, std::unique_ptr<ucred>>
Socket::recv_ucred_data(int from_fd, void *data, size_t data_size) {
    std::unique_ptr<ucred> ret_ucred = std::make_unique<ucred>();
    struct iovec io = {.iov_base = data, .iov_len = data_size};

    union {
        struct cmsghdr cmh;
        char control[CMSG_SPACE(sizeof(struct ucred))];
    } un;
    un.cmh.cmsg_level = SOL_SOCKET;
    un.cmh.cmsg_type = SCM_CREDENTIALS;
    un.cmh.cmsg_len = CMSG_LEN(sizeof(struct ucred));

    struct msghdr msgh = {0};
    msgh.msg_iov = &io;
    msgh.msg_iovlen = 1;
    msgh.msg_name = NULL;
    msgh.msg_namelen = 0;
    msgh.msg_control = un.control;
    msgh.msg_controllen = sizeof(un.control);

    size_t ret_size = recvmsg(from_fd, &msgh, 0);
    if (ret_size < 0)
        throw Sys_err(errno, "recvmsg err", TE_ERR_LOC);

    struct cmsghdr *cmhp = CMSG_FIRSTHDR(&msgh);
    if (cmhp == NULL)
        throw Run_err("cmhp == NULL", TE_ERR_LOC);
    if (cmhp->cmsg_len != CMSG_LEN(sizeof(struct ucred)))
        throw Run_err("cmhp->cmsg_len != sizeof struct ucred", TE_ERR_LOC);
    if (cmhp->cmsg_level != SOL_SOCKET)
        throw Run_err("cmhp->cmsg_level != SOL_SOCKET", TE_ERR_LOC);
    if (cmhp->cmsg_type != SCM_CREDENTIALS)
        throw Run_err("cmhp->cmsg_type != SCM_CREDENTIALS", TE_ERR_LOC);

    std::memcpy(ret_ucred.get(), CMSG_DATA(cmhp), sizeof(ucred));

    return {ret_size, std::move(ret_ucred)};
}

void Socket::load_addr(struct sockaddr_un &addr) {
    memset(&addr, 0, sizeof(sockaddr_un));

    addr.sun_family = AF_LOCAL;

    size_t sun_path_size = sizeof addr.sun_path;

    const char *c_path = path.c_str();

    if (sizeof(c_path) > sun_path_size)
        throw Run_err("path name is too long, path: " + path +
                          " fd: " + std::to_string(fd),
                      TE_ERR_LOC);

    std::strncpy(addr.sun_path, c_path, sun_path_size);
}

} // namespace Teave
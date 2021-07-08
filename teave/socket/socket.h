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
#ifndef TEAVE_SOCKET_H
#define TEAVE_SOCKET_H

#include <teave/err/err.h>
#include <sys/un.h>
#include <vector>
#include <sys/socket.h>

namespace Teave {

class Socket {
public:
    using string = std::string;
    using uptr = std::unique_ptr<Socket>;

    Socket(string path, bool blocking = true);
    ~Socket();
    void set_opt_passcred();
    int get_fd();
    string get_path();
    /*!
     * @param rm_exisiting remove the socket file if already exists at the
     * socket path
     */
    void server_setup(bool rm_existing = false);
    void client_setup();

    /*!
     *
     * @param newfd_blocking set flag SOCK_NONBLOCK for accept4
     * see: http://man7.org/linux/man-pages/man2/accept.2.html
     * note, SOCK_CLOEXEC is always set.
     * @return
     */
    int accept(bool newfd_blocking = true);
    void send_data(int fd, void *data, std::size_t size, int flags = 0);
    void send_fd(int so_fd, int msg_fd, void *data, std::size_t size,
                 int flags = 0);
    ssize_t recv_data(int from_fd, void *data, size_t data_size);
    std::tuple<ssize_t, int> recv_fd(int from_fd, void *data, size_t data_size);
    std::tuple<ssize_t, std::unique_ptr<ucred>>
    recv_ucred_data(int from_fd, void *data, size_t data_size);

private:
    int fd = -1;
    string path;
    bool type_server = true; // socket type, true if server otherwise it's a app
    bool blocking = true;    // blocking or non blocking socket connection
    struct sockaddr_un addr;
    void load_addr(struct sockaddr_un &addr);
};

} // namespace Teave

#endif /* TEAVE_SOCKET_H */
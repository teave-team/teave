
# Build, install, run
## Build and install
1. cd build

1. cmake -D CMAKE_CXX_COMPILER=g++ .
(or for debug build)
cmake -D CMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug .

1. make
1. sudo make install

## Run
1. start service: 
sudo systemctl start teave-sv.service

1. then run your application that links to Teave

## Run tests
1. cd bin
1. run tests: 
./tests
run tests by tags: 
./tests [tag name]

see https://github.com/catchorg/Catch2

# System requirements
1. Linux 3.11+
1. CMake 3.13+
1. UDEV 1.0+
1. EGL 1.0+
1. DRM 2.0+
1. GBM 1.0+
1. GLESv2 2.0+

cmake_minimum_required(VERSION 3.18)
project(eps4400_xpad)

find_package(Threads)

set(CMAKE_CXX_STANDARD 14)

add_executable(eps4400_xpad
        ## Main executable
        main.cpp

        ## simple-xpad library
        include/simple-xpad/src/XPad.cpp
        include/simple-xpad/src/XEvent.cpp

        ## Project files
        src/Controller.cpp
        src/CanBus.cpp
        )

target_link_libraries(eps4400_xpad ${CMAKE_THREAD_LIBS_INIT})

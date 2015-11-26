cmake_minimum_required (VERSION 3.0)
project (signal)

set (CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(signal_test
    test.cpp
    include/szabi/signals.h
)

install(FILES include/szabi/signals.h DESTINATION include/szabi)
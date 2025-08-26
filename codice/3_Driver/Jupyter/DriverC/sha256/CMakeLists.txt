# cmake_minimum_required(VERSION <specify CMake version here>)
project(sha256)

set(CMAKE_CXX_STANDARD 14)

add_executable(sha256 src/sha256_thread.c src/pynq_api.c src/pynq_api.h)
TARGET_LINK_LIBRARIES(sha256 cma pthread )

#add_executable(sha256 src/sha256_thread.c)
#TARGET_LINK_LIBRARIES(sha256 cma pthread )
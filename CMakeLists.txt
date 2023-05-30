cmake_minimum_required(VERSION 3.24)
project(bulk)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(${PROJECT_NAME}  main.cpp)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_CONTACT https://github.com/friendpostum/Otus_hw7)
include(CPack)


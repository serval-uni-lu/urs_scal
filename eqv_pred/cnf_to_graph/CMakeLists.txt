cmake_minimum_required(VERSION 3.10)

project(cnf_to_graph LANGUAGES CXX)
project(tw_to_graph LANGUAGES CXX)
project(fpt LANGUAGES CXX)
project(cnf_lp_deff LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(ASAN -fsanitize=address -fno-omit-frame-pointer -static-libasan)

add_compile_options(
        -Wall
        -Wextra
        -pedantic
        -g
        -pthread
        -lgmp
        -lgmpxx
        ${ASAN}
)
add_link_options(
        ${ASAN}
        -pthread
        -g
        -lgmp
        -lgmpxx
)

set(FL
    src/cnf.hpp
    src/uf.hpp

    src/cnf.cpp
    src/uf.cpp
)

add_executable(cnf_to_graph
        src/main.cpp
        ${FL})

add_executable(tw_to_graph
        src/tw.cpp
        ${FL})

add_executable(fpt
        src/fpt.cpp
        ${FL})

add_executable(cnf_lp_deff
        src/lp_deff.cpp
        ${FL})

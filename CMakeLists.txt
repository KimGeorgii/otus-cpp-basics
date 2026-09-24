cmake_minimum_required(VERSION 3.10)

project(calc)

add_executable(calc
    main.cpp
    func.cpp
    func.h
)

set_target_properties(calc PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(calc PRIVATE
        -Wall
        -Wextra
        -Werror
        -pedantic
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(calc PRIVATE
        /W4
        /WX
    )
endif()
cmake_minimum_required(VERSION 3.12)

project(cone_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cone_project
    main.cpp
    func.cpp
)

target_include_directories(cone_project PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
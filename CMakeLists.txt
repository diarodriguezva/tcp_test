cmake_minimum_required(VERSION 3.10)
project(EXERCISE_TCP CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(exerciseLib
    src/tcp_server.cpp
)

target_include_directories(exerciseLib
    PUBLIC
    include
)

add_executable(exercise
    src/main.cpp
)

target_link_libraries(exercise
    PRIVATE
        exerciseLib
)

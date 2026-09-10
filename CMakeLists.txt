project(GameOfLife
VERSION 4.0.1
LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

cmake_minimum_required(VERSION 4.3)
add_executable(gameoflife)
target_sources(gameoflife PRIVATE src/main.cpp)

install(
    TARGETS gameoflife
    RUNTIME DESTINATION bin
)

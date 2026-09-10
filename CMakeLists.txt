project(GameOfLife
VERSION 4.0.1
LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")

set(CURSES_NEED_NCURSES true)
find_package(Curses)
if(Curses_FOUND AND NOT TARGET Curses::Curses)
  add_library(Curses::Curses INTERFACE IMPORTED)
  set_target_properties(
    Curses::Curses
    PROPERTIES
      INTERFACE_LINK_LIBRARIES "${CURSES_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${CURSES_INCLUDE_DIRS}"
  )
endif()

cmake_minimum_required(VERSION 4.3)
add_executable(gameoflife)
target_sources(gameoflife PRIVATE src/main.cpp)
target_link_libraries(gameoflife PRIVATE Curses::Curses)

install(
    TARGETS gameoflife
    RUNTIME DESTINATION bin
)

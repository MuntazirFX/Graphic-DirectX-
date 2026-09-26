cmake_minimum_required(VERSION 3.16)
project(GraphicDirectX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(gdx STATIC
    src/XParser.cpp
    src/XBinary.cpp
)
target_include_directories(gdx PUBLIC include)

add_executable(xdump src/xdump.cpp)
target_link_libraries(xdump PRIVATE gdx)

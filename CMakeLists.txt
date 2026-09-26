cmake_minimum_required(VERSION 3.16)
project(GraphicDirectX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(gdx STATIC
    src/XParser.cpp
    src/XBinary.cpp
    src/DatLevel.cpp
    src/ElementCatalog.cpp
    src/Xpk.cpp
    src/Game.cpp
    src/Title.cpp
    src/D3D8Device.cpp
)
target_include_directories(gdx PUBLIC include)

add_executable(xdump src/xdump.cpp)
target_link_libraries(xdump PRIVATE gdx)
add_executable(datdump src/datdump.cpp)
target_link_libraries(datdump PRIVATE gdx)
add_executable(xpkdump src/xpkdump.cpp)
target_link_libraries(xpkdump PRIVATE gdx)
add_executable(d3d8dump src/d3d8dump.cpp)
target_link_libraries(d3d8dump PRIVATE gdx)

add_executable(gdx_tests tests/test_main.cpp)
target_link_libraries(gdx_tests PRIVATE gdx)
add_custom_command(TARGET gdx_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/samples
            ${CMAKE_BINARY_DIR}/samples)

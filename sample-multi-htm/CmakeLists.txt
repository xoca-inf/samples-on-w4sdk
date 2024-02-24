cmake_minimum_required(VERSION 3.19)

if(NOT DEFINED ENV{W4})
    message(FATAL_ERROR "W4 environment variable is not set, get W4 SDK Installer!!!")
endif ()
set(CMAKE_GENERATOR Ninja)
set(CMAKE_TOOLCHAIN_FILE "$ENV{W4}/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")

project(W4App)

find_package(Python 3.7 REQUIRED)

list(APPEND CMAKE_MODULE_PATH $ENV{W4}sdk\\buildtools)

include(W4User)

W4DeclareWebApp("${CMAKE_SOURCE_DIR}")


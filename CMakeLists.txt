cmake_minimum_required(VERSION 3.20)
project(NovaBrowser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(nova_browser src/main.cpp)
target_include_directories(nova_browser PRIVATE src)
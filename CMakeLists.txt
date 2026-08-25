cmake_minimum_required(VERSION 3.20)
project(NovaBrowser LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Путь к CEF ---
set(CEF_ROOT "${CMAKE_SOURCE_DIR}/third_party/cef")
list(APPEND CMAKE_MODULE_PATH "${CEF_ROOT}/cmake")

find_package(CEF REQUIRED)

# --- libcef_dll_wrapper ---
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

# --- Наш исполняемый файл ---
add_executable(nova_browser
        src/main.cpp
        src/browser/simple_app.cpp
        src/browser/simple_handler.cpp
        src/core/browser_manager.hpp
        src/core/browser_manager.cpp
)

target_include_directories(nova_browser PRIVATE
    src
    ${CEF_ROOT}
)

target_link_libraries(nova_browser PRIVATE
    libcef_dll_wrapper
    "${CEF_ROOT}/Release/libcef.so"
    pthread
    X11
)

# --- Копируем ресурсы и бинарники CEF ---
add_custom_command(TARGET nova_browser POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CEF_ROOT}/Resources" "$<TARGET_FILE_DIR:nova_browser>"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CEF_ROOT}/Release" "$<TARGET_FILE_DIR:nova_browser>"
)

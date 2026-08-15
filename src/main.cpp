#include <iostream>
#include <climits>
#include <unistd.h>
#include <string>
#include "include/cef_app.h"
#include "browser/simple_app.hpp"

int main(int argc, char* argv[]) {
    CefMainArgs main_args(argc, argv);

    CefRefPtr<SimpleApp> app(new SimpleApp());

    int exit_code = CefExecuteProcess(main_args, app.get(), nullptr);
    if (exit_code >= 0) {
        return exit_code;
    }

    // Получаем директорию, где реально лежит exe (build/),
    // чтобы построить АБСОЛЮТНЫЙ путь к ресурсам.
    // Это нужно, потому что libcef.so грузится из third_party/cef/Release/
    // (через RPATH), а не из build/, и CEF по умолчанию ищет
    // ресурсы относительно текущей директории, что может не совпадать.
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    std::string exe_dir = ".";
    if (len != -1) {
        exe_path[len] = '\0';
        std::string full_path(exe_path);
        exe_dir = full_path.substr(0, full_path.find_last_of('/'));
    }

    CefSettings settings;

    CefString(&settings.resources_dir_path).FromString(exe_dir);
    CefString(&settings.locales_dir_path).FromString(exe_dir + "/locales");
    settings.no_sandbox = true;

    CefInitialize(main_args, settings, app.get(), nullptr);

    std::cout << "Nova Browser core: CEF initialized, opening window..."
              << std::endl;

    CefRunMessageLoop();
    CefShutdown();

    return 0;
}

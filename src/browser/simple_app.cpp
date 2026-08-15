#include "browser/simple_app.hpp"
#include "browser/simple_handler.hpp"

#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"

#include <climits>
#include <unistd.h>
#include <string>

void SimpleApp::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();

    CefWindowInfo window_info;
    window_info.bounds.width = 1280;
    window_info.bounds.height = 800;

    CefBrowserSettings browser_settings;

    // Строим АБСОЛЮТНЫЙ путь к нашему frontend/index.html,
    // отталкиваясь от расположения исполняемого файла (build/),
    // чтобы это работало независимо от того, откуда запущена программа.
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    std::string frontend_url = "https://example.com"; // запасной вариант
    if (len != -1) {
        exe_path[len] = '\0';
        std::string exe_dir(exe_path);
        exe_dir = exe_dir.substr(0, exe_dir.find_last_of('/'));
        // build/ -> корень проекта -> frontend/index.html
        std::string project_root = exe_dir.substr(0, exe_dir.find_last_of('/'));
        frontend_url = "file://" + project_root + "/frontend/index.html";
    }

    CefRefPtr<SimpleHandler> handler(new SimpleHandler());

    CefBrowserHost::CreateBrowser(
        window_info,
        handler,
        frontend_url,
        browser_settings,
        nullptr,
        nullptr
    );
}

#include "browser/simple_app.hpp"
#include "browser/simple_handler.hpp"

#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"

void SimpleApp::OnContextInitialized() {
    // CEF требует, чтобы эта функция вызывалась строго в UI-потоке
    CEF_REQUIRE_UI_THREAD();

    // Настройки окна браузера (по умолчанию)
    CefWindowInfo window_info;
    window_info.bounds.width = 1280;
    window_info.bounds.height = 800;

    CefBrowserSettings browser_settings;

    // Стартовая страница
    std::string start_url = "https://example.com";

    // Наш CefClient — будет получать все события этого окна
    CefRefPtr<SimpleHandler> handler(new SimpleHandler());

    // Создаём сам браузер (открывается окно)
    CefBrowserHost::CreateBrowser(
        window_info,
        handler,
        start_url,
        browser_settings,
        nullptr,
        nullptr
    );
}
#include "browser/simple_handler.hpp"

#include "include/wrapper/cef_helpers.h"
#include "include/cef_app.h"
#include <algorithm>
#include <iostream>

void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    // Новое окно создано — запоминаем его
    browser_list_.push_back(browser);
}

bool SimpleHandler::DoClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    // Если это было последнее окно — начинаем закрытие приложения
    if (browser_list_.size() == 1) {
        is_closing_ = true;
    }
    // false = разрешаем CEF продолжить обычный процесс закрытия окна
    return false;
}

void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    // Убираем это окно из списка
    for (auto it = browser_list_.begin(); it != browser_list_.end(); ++it) {
        if ((*it)->IsSame(browser)) {
            browser_list_.erase(it);
            break;
        }
    }

    // Если окон больше нет — останавливаем главный цикл сообщений CEF
    if (browser_list_.empty()) {
        CefQuitMessageLoop();
    }
}

void SimpleHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                   const CefString& title) {
    CEF_REQUIRE_UI_THREAD();
    // Пока просто выводим в консоль — позже свяжем с нашим UI
    std::cout << "[Nova Browser] Title changed: " << title.ToString() << std::endl;
}

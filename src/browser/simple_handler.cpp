#include "browser/simple_handler.hpp"

#include "include/wrapper/cef_helpers.h"
#include "include/cef_app.h"
#include <algorithm>
#include <iostream>

SimpleHandler::SimpleHandler() {
    // Настраиваем имена JS-функций, через которые фронтенд будет
    // обращаться к нам. По умолчанию CEF предлагает "cefQuery" /
    // "cefQueryCancel" — используем стандартные имена, как в задании
    // напарника (window.cefQuery(...)).
    CefMessageRouterConfig config;
    config.js_query_function = "cefQuery";
    config.js_cancel_function = "cefQueryCancel";

    message_router_ = CefMessageRouterBrowserSide::Create(config);

    ipc_handler_ = new IPCHandler();
    // AddHandler НЕ забирает владение указателем — поэтому ipc_handler_
    // обязательно должен храниться как поле SimpleHandler, а не быть
    // временным объектом, иначе роутер останется с висячим указателем.
    message_router_->AddHandler(ipc_handler_.get(), false);
}

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

bool SimpleHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                              CefRefPtr<CefFrame> frame,
                                              CefProcessId source_process,
                                              CefRefPtr<CefProcessMessage> message) {
    CEF_REQUIRE_UI_THREAD();
    // Отдаём сообщение роутеру — если это действительно cefQuery-запрос,
    // он сам найдёт нужный handler (у нас он один — IPCHandler) и вызовет
    // его OnQuery(...). Возвращаемое значение говорит CEF, было ли
    // сообщение обработано здесь или нужно передать дальше.
    return message_router_->OnProcessMessageReceived(browser, frame, source_process, message);
}

#pragma once

#include "include/cef_client.h"
#include "include/wrapper/cef_message_router.h"

#include "ipc/ipc_handler.hpp"

#include <list>

// SimpleHandler — реализация CefClient.
// Обрабатывает события конкретного окна браузера:
// закрытие окна, изменение заголовка, а теперь ещё и IPC-сообщения от JS.
class SimpleHandler : public CefClient,
                       public CefLifeSpanHandler,
                       public CefDisplayHandler {
public:
    SimpleHandler();

    // --- CefClient: говорим, какие handler'ы мы поддерживаем ---
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }

    // --- CefLifeSpanHandler: жизненный цикл окна ---
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    bool DoClose(CefRefPtr<CefBrowser> browser) override;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // --- CefDisplayHandler: изменение заголовка окна ---
    void OnTitleChange(CefRefPtr<CefBrowser> browser,
                        const CefString& title) override;

    // --- CefClient: сюда прилетают IPC-сообщения, которые нужно
    // передать message router'у, чтобы он довёл их до IPCHandler::OnQuery ---
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefProcessId source_process,
                                   CefRefPtr<CefProcessMessage> message) override;

    // Проверка, все ли окна закрыты (для завершения программы)
    bool IsClosing() const { return is_closing_; }

private:
    // Список всех открытых окон браузера (нам пока хватит одного)
    std::list<CefRefPtr<CefBrowser>> browser_list_;
    bool is_closing_ = false;

    // Роутер, который сопоставляет window.cefQuery(...) из JS с нашим
    // IPCHandler::OnQuery(...) в C++. Создаётся в конструкторе.
    CefRefPtr<CefMessageRouterBrowserSide> message_router_;
    CefRefPtr<IPCHandler> ipc_handler_;

    IMPLEMENT_REFCOUNTING(SimpleHandler);
};
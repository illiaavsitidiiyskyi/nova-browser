#pragma once

#include "include/cef_client.h"
#include <list>

// SimpleHandler — реализация CefClient.
// Обрабатывает события конкретного окна браузера:
// закрытие окна, изменение заголовка и т.д.
class SimpleHandler : public CefClient,
                       public CefLifeSpanHandler,
                       public CefDisplayHandler {
public:
    SimpleHandler() = default;

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

    // Проверка, все ли окна закрыты (для завершения программы)
    bool IsClosing() const { return is_closing_; }

private:
    // Список всех открытых окон браузера (нам пока хватит одного)
    std::list<CefRefPtr<CefBrowser>> browser_list_;
    bool is_closing_ = false;

    IMPLEMENT_REFCOUNTING(SimpleHandler);
};
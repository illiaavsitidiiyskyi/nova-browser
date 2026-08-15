#pragma once

#include "include/cef_app.h"

// SimpleApp — реализация CefApp.
// Это точка входа CEF в наше приложение.
class SimpleApp : public CefApp, public CefBrowserProcessHandler {
public:
    SimpleApp() = default;

    // Возвращает обработчик для Browser Process (главного процесса)
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }

    // Вызывается CEF, когда Browser Process полностью готов к работе
    void OnContextInitialized() override;

private:
    IMPLEMENT_REFCOUNTING(SimpleApp);
};
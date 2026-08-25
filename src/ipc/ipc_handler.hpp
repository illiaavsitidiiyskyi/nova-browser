#pragma once

#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_message_router.h"

#include "core/browser_manager.hpp"
#include "message_types.hpp"

// IPCHandler — обработчик сообщений, приходящих из JS через window.cefQuery.
//
// Наследуемся от CefMessageRouterBrowserSide::Handler — это готовый интерфейс
// CEF именно под сценарий "запрос из JS -> ответ в JS" (в отличие от
// низкоуровневого CefProcessMessage, где пришлось бы вручную сопоставлять
// запросы с ответами).
//
// Регистрируется в SimpleHandler (см. simple_handler.hpp/.cpp), который
// пробрасывает вызовы message router'а из CefClient.
//
// Дополнительно наследуемся от CefBaseRefCounted: сам
// CefMessageRouterBrowserSide::Handler не поддерживает подсчёт ссылок,
// а SimpleHandler держит нас через CefRefPtr — значит, нужен AddRef/Release.
class IPCHandler : public CefMessageRouterBrowserSide::Handler,
                   public CefBaseRefCounted {
public:
    IPCHandler();
    ~IPCHandler() override = default;

    // Вызывается CEF, когда JS выполнил window.cefQuery(...).
    // Возвращает true, если мы взяли запрос в обработку (в нашем случае —
    // всегда true, даже для неизвестных команд: просто ответим ошибкой,
    // а не проигнорируем запрос молча).
    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64_t query_id,
                 const CefString& request,
                 bool persistent,
                 CefRefPtr<Callback> callback) override;

    // Вызывается CEF, если JS отменил запрос до получения ответа
    // (например, страница перезагрузилась). У нас все команды быстрые
    // и синхронные внутри, поэтому реального состояния для отмены нет,
    // но метод обязателен к переопределению по интерфейсу.
    void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          int64_t query_id) override;

private:
    // Разбирает распарсенный IPCRequest, вызывает нужный метод
    // BrowserManager и отправляет ответ через callback.
    void HandleCommand(const IPCRequest& request, CefRefPtr<Callback> callback);

    // Пока один BrowserManager на весь браузер (одно окно, одна сессия).
    // Когда появится поддержка нескольких окон — вынесем наружу и будем
    // передавать ссылку снаружи, а не создавать здесь.
    BrowserManager browser_manager_;

    IMPLEMENT_REFCOUNTING(IPCHandler);
};
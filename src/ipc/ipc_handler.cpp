#include "ipc/ipc_handler.hpp"
#include "ipc/message_types.hpp"

#include <iostream>

IPCHandler::IPCHandler() = default;

bool IPCHandler::OnQuery(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          int64_t query_id,
                          const CefString& request,
                          bool persistent,
                          CefRefPtr<Callback> callback) {
    // request прилетает как CefString — переводим в обычный std::string,
    // с ним удобнее работать в остальном коде.
    const std::string raw_json = request.ToString();

    std::optional<IPCRequest> parsed = ParseIPCRequest(raw_json);

    if (!parsed) {
        // Невалидный JSON или нет обязательного поля "command" —
        // requestId в этом случае неизвестен, поэтому шлём 0.
        std::cerr << "[IPCHandler] Failed to parse request: " << raw_json << std::endl;
        callback->Success(BuildErrorResponse(0, "Invalid request format"));
        // Возвращаем true в любом случае — мы ОБРАБОТАЛИ запрос (ответом
        // с ошибкой), просто не выполнили команду. false здесь означало бы
        // "этот handler не может обработать данный запрос вообще", и CEF
        // попробовал бы передать его следующему handler'у, которого у нас нет.
        return true;
    }

    HandleCommand(*parsed, callback);
    return true;
}

void IPCHandler::OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  int64_t query_id) {
    // Наши команды синхронные и мгновенные, отменять нечего.
    // Метод оставлен пустым намеренно — того требует интерфейс Handler.
}

void IPCHandler::HandleCommand(const IPCRequest& request, CefRefPtr<Callback> callback) {
    const int id = request.request_id;

    if (request.command == "openTab") {
        const std::string url = request.GetStringParam("url");
        if (url.empty()) {
            callback->Success(BuildErrorResponse(id, "Missing required param: url"));
            return;
        }
        const int tab_id = browser_manager_.OpenTab(url);

        CefRefPtr<CefDictionaryValue> result = CefDictionaryValue::Create();
        result->SetInt("tabId", tab_id);
        result->SetString("url", url);
        CefRefPtr<CefValue> result_value = CefValue::Create();
        result_value->SetDictionary(result);

        callback->Success(BuildSuccessResponse(id, result_value));
        return;
    }

    if (request.command == "closeTab") {
        const int tab_id = request.GetIntParam("tabId", -1);
        if (tab_id < 0) {
            callback->Success(BuildErrorResponse(id, "Missing required param: tabId"));
            return;
        }
        const bool closed = browser_manager_.CloseTab(tab_id);

        CefRefPtr<CefDictionaryValue> result = CefDictionaryValue::Create();
        result->SetBool("closed", closed);
        CefRefPtr<CefValue> result_value = CefValue::Create();
        result_value->SetDictionary(result);

        callback->Success(BuildSuccessResponse(id, result_value));
        return;
    }

    if (request.command == "goBack") {
        browser_manager_.GoBack();
        callback->Success(BuildSuccessResponse(id, nullptr));
        return;
    }

    if (request.command == "goForward") {
        browser_manager_.GoForward();
        callback->Success(BuildSuccessResponse(id, nullptr));
        return;
    }

    if (request.command == "reload") {
        browser_manager_.Reload();
        callback->Success(BuildSuccessResponse(id, nullptr));
        return;
    }

    if (request.command == "navigate") {
        const std::string url = request.GetStringParam("url");
        if (url.empty()) {
            callback->Success(BuildErrorResponse(id, "Missing required param: url"));
            return;
        }
        browser_manager_.Navigate(url);
        callback->Success(BuildSuccessResponse(id, nullptr));
        return;
    }

    if (request.command == "getCurrentURL") {
        const std::string url = browser_manager_.GetCurrentUrl();

        CefRefPtr<CefValue> result_value = CefValue::Create();
        result_value->SetString(url);

        callback->Success(BuildSuccessResponse(id, result_value));
        return;
    }

    // Неизвестная команда — не падаем, просто сообщаем об этом в JS.
    std::cerr << "[IPCHandler] Unknown command: " << request.command << std::endl;
    callback->Success(BuildErrorResponse(id, "Unknown command: " + request.command));
}

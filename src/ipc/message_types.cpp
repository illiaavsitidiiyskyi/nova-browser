#include "ipc/message_types.hpp"

std::optional<IPCRequest> ParseIPCRequest(const std::string& raw_json) {

    CefRefPtr<CefValue> root = CefParseJSON(raw_json, JSON_PARSER_RFC);

    if (!root || root->GetType() != VTYPE_DICTIONARY) {
        return std::nullopt;
    }

    CefRefPtr<CefDictionaryValue> dict = root->GetDictionary();

    // "command" обязателен — без него мы не знаем, что вообще делать.
    if (!dict->HasKey("command") || dict->GetType("command") != VTYPE_STRING) {
        return std::nullopt;
    }

    IPCRequest request;
    request.command = dict->GetString("command").ToString();

    // "requestId" опционален по факту, но фронтенд всегда его присылает —
    // если вдруг нет, используем 0 как fallback (не роняем парсинг из-за этого).
    if (dict->HasKey("requestId") && dict->GetType("requestId") == VTYPE_INT) {
        request.request_id = dict->GetInt("requestId");
    }

    // "params" тоже опционален (например, у reload/goBack параметров нет).
    if (dict->HasKey("params") && dict->GetType("params") == VTYPE_DICTIONARY) {
        request.params = dict->GetDictionary("params");
    }

    return request;
}

std::string BuildSuccessResponse(int request_id, CefRefPtr<CefValue> result) {
    CefRefPtr<CefDictionaryValue> response = CefDictionaryValue::Create();
    response->SetInt("requestId", request_id);
    response->SetBool("success", true);

    // result может быть nullptr (например, команды без содержательного
    // ответа вроде reload) — тогда просто не добавляем ключ "result".
    if (result) {
        response->SetValue("result", result);
    }

    CefRefPtr<CefValue> root = CefValue::Create();
    root->SetDictionary(response);

    return CefWriteJSON(root, JSON_WRITER_DEFAULT).ToString();
}

std::string BuildErrorResponse(int request_id, const std::string& error_message) {
    CefRefPtr<CefDictionaryValue> response = CefDictionaryValue::Create();
    response->SetInt("requestId", request_id);
    response->SetBool("success", false);
    response->SetString("error", error_message);

    CefRefPtr<CefValue> root = CefValue::Create();
    root->SetDictionary(response);

    return CefWriteJSON(root, JSON_WRITER_DEFAULT).ToString();
}
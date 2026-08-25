#pragma once

#include "include/cef_values.h"
#include "include/cef_parser.h"

#include <string>
#include <optional>


struct IPCRequest {
    std::string command;
    CefRefPtr<CefDictionaryValue> params; // может быть nullptr, если params не пришли
    int request_id = 0;

    // Достаём строковый параметр из params. Если его нет — возвращает fallback.
    std::string GetStringParam(const std::string& key, const std::string& fallback = "") const {
        if (!params || !params->HasKey(key)) {
            return fallback;
        }
        return params->GetString(key).ToString();
    }

    // Достаём числовой параметр из params.
    int GetIntParam(const std::string& key, int fallback = 0) const {
        if (!params || !params->HasKey(key)) {
            return fallback;
        }
        return params->GetInt(key);
    }
};

std::optional<IPCRequest> ParseIPCRequest(const std::string& raw_json);

std::string BuildSuccessResponse(int request_id, CefRefPtr<CefValue> result);

std::string BuildErrorResponse(int request_id, const std::string& error_message);
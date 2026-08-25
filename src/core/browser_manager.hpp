#pragma once

#include <string>
#include <vector>

// BrowserManager — пока ЗАГЛУШКА.
// Здесь будет реальное управление вкладками (Этап 4), но сигнатуры методов
// уже финальные, чтобы IPC-слой не пришлось переписывать позже.
//
// Почему не статический класс, а обычный с методами: в будущем сюда
// добавится состояние (список вкладок, активная вкладка), поэтому сразу
// делаем его объектом, а не набором свободных функций.
class BrowserManager {
public:
    BrowserManager() = default;

    // Открыть новую вкладку с указанным URL.
    // Возвращает id созданной вкладки.
    int OpenTab(const std::string& url);

    // Закрыть вкладку по id. Возвращает true, если вкладка была найдена и закрыта.
    bool CloseTab(int tab_id);

    // Перейти назад/вперёд/перезагрузить текущую активную вкладку.
    void GoBack();
    void GoForward();
    void Reload();

    // Перейти по URL в активной вкладке.
    void Navigate(const std::string& url);

    // Получить URL активной вкладки.
    std::string GetCurrentUrl() const;

private:
    // Пока просто счётчик id и "текущий URL" в памяти — заглушка состояния.
    int next_tab_id_ = 2; // 1 уже используется стартовой вкладкой (см. frontend)
    std::string current_url_ = "about:blank";
};
#include "core/browser_manager.hpp"

#include <iostream>

int BrowserManager::OpenTab(const std::string& url) {
    int tab_id = next_tab_id_++;
    current_url_ = url;
    std::cout << "[BrowserManager] OpenTab id=" << tab_id
              << " url=" << url << std::endl;
    return tab_id;
}

bool BrowserManager::CloseTab(int tab_id) {
    std::cout << "[BrowserManager] CloseTab id=" << tab_id << std::endl;
    // Заглушка: пока у нас нет реального списка вкладок, всегда "успех".
    return true;
}

void BrowserManager::GoBack() {
    std::cout << "[BrowserManager] GoBack" << std::endl;
}

void BrowserManager::GoForward() {
    std::cout << "[BrowserManager] GoForward" << std::endl;
}

void BrowserManager::Reload() {
    std::cout << "[BrowserManager] Reload" << std::endl;
}

void BrowserManager::Navigate(const std::string& url) {
    current_url_ = url;
    std::cout << "[BrowserManager] Navigate url=" << url << std::endl;
}

std::string BrowserManager::GetCurrentUrl() const {
    return current_url_;
}
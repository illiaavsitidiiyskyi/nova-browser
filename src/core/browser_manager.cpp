
#include "core/browser_manager.hpp"
#include <iostream>

int BrowserManager::OpenTab(const std::string& url) {
    Tab new_tab;
    new_tab.url = url;
    // back_stack пустой, forward_stack пустой
    new_tab.forward_stack = {};
    new_tab.back_stack = {};

    if (tabs_.empty()) {
        tabs_.push_back(new_tab);
        current_tab_index_ = 0;
    } else {
        // Сдвигаем текущую вперед в историю перед новым
        tabs_[current_tab_index_].back_stack.push_back(tabs_[current_tab_index_].url);
        tabs_[current_tab_index_].forward_stack.clear();
        current_tab_index_ = tabs_.size();
        tabs_.push_back(new_tab);
    }

    int tab_id = next_tab_id_++;
    std::cout << "[BrowserManager] OpenTab id=" << tab_id
              << " url=" << url << std::endl;
    return tab_id;
}

bool BrowserManager::CloseTab(int tab_id) {
    std::cout << "[BrowserManager] CloseTab id=" << tab_id << std::endl;
    for (size_t i = 0; i < tabs_.size(); ++i) {
        if (static_cast<int>(i) == tab_id) {
            tabs_.erase(tabs_.begin() + i);
            // Adjust current tab index
            if (current_tab_index_ >= static_cast<int>(tabs_.size())) {
                current_tab_index_ = std::max(0, static_cast<int>(tabs_.size()) - 1);
            }
            return true;
        }
    }
    return false;
}

void BrowserManager::GoBack() {
    if (current_tab_index_ < 0 || current_tab_index_ >= static_cast<int>(tabs_.size())) return;
    Tab& current = tabs_[current_tab_index_];
    if (current.back_stack.empty()) return;

    std::string prev_url = current.back_stack.back();
    current.back_stack.pop_back();
    current.forward_stack.push_back(current.url);
    current.url = prev_url;
    std::cout << "[BrowserManager] GoBack url=" << prev_url << std::endl;
}

void BrowserManager::GoForward() {
    if (current_tab_index_ < 0 || current_tab_index_ >= static_cast<int>(tabs_.size())) return;
    Tab& current = tabs_[current_tab_index_];
    if (current.forward_stack.empty()) return;

    std::string next_url = current.forward_stack.back();
    current.forward_stack.pop_back();
    current.back_stack.push_back(current.url);
    current.url = next_url;
    std::cout << "[BrowserManager] GoForward url=" << next_url << std::endl;
}

void BrowserManager::Reload() {
    if (current_tab_index_ < 0 || current_tab_index_ >= static_cast<int>(tabs_.size())) return;
    Tab& current = tabs_[current_tab_index_];
    std::cout << "[BrowserManager] Reload url=" << current.url << std::endl;
}

void BrowserManager::Navigate(const std::string& url) {
    if (current_tab_index_ < 0 || current_tab_index_ >= static_cast<int>(tabs_.size())) return;
    Tab& current = tabs_[current_tab_index_];
    current.back_stack.push_back(current.url);
    current.forward_stack.clear();
    current.url = url;
    std::cout << "[BrowserManager] Navigate url=" << url << std::endl;
}

std::string BrowserManager::GetCurrentUrl() const {
    if (current_tab_index_ < 0 || current_tab_index_ >= static_cast<int>(tabs_.size())) return "about:blank";
    return tabs_[current_tab_index_].url;

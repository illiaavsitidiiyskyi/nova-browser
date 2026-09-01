#pragma once

#include <string>
#include <vector>

struct Tab {
    std::string url;
    std::vector<std::string> back_stack;
    std::vector<std::string> forward_stack;
};

class BrowserManager {
public:
    BrowserManager() = default;

    int OpenTab(const std::string& url);
    bool CloseTab(int tab_id);
    void GoBack();
    void GoForward();
    void Reload();
    void Navigate(const std::string& url);
    std::string GetCurrentUrl() const;

private:
    std::vector<Tab> tabs_;
    int current_tab_index_ = 0;
    int next_tab_id_ = 2;
};
class NovaBrowser {
    constructor() {
        this.tabs = [];
        this.activeTabId = 1;
        this.nextTabId = 2;
        
        this.initializeElements();
        this.attachEventListeners();
        
        console.log('Nova Browser frontend initialized');
    }

    initializeElements() {
        this.addressBar = document.getElementById('address-bar');
        this.btnBack = document.getElementById('btn-back');
        this.btnForward = document.getElementById('btn-forward');
        this.btnReload = document.getElementById('btn-reload');
        this.btnHome = document.getElementById('btn-home');
        this.btnNewTab = document.getElementById('btn-new-tab');
        this.tabsContainer = document.getElementById('tabs-container');
        this.webview = document.getElementById('webview');
    }

    attachEventListeners() {
        this.btnBack.addEventListener('click', () => this.handleBackClick());
        this.btnForward.addEventListener('click', () => this.handleForwardClick());
        this.btnReload.addEventListener('click', () => this.handleReloadClick());
        this.btnHome.addEventListener('click', () => this.handleHomeClick());
        this.btnNewTab.addEventListener('click', () => this.handleNewTabClick());

        this.addressBar.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                this.handleNavigate(this.addressBar.value);
            }
        });

        this.addressBar.addEventListener('focus', () => {
            this.addressBar.select();
        });
    }

    async handleBackClick() {
        try {
            console.log('Going back...');
            await window.ipcClient.goBack();
            this.updateUI();
        } catch (error) {
            console.error('Back failed:', error);
            this.showError('Failed to go back');
        }
    }

    async handleForwardClick() {
        try {
            console.log('Going forward...');
            await window.ipcClient.goForward();
            this.updateUI();
        } catch (error) {
            console.error('Forward failed:', error);
            this.showError('Failed to go forward');
        }
    }

    async handleReloadClick() {
        try {
            console.log('Reloading...');
            await window.ipcClient.reload();
            this.updateUI();
        } catch (error) {
            console.error('Reload failed:', error);
            this.showError('Failed to reload');
        }
    }

    async handleHomeClick() {
        try {
            console.log('Going home...');
            const homePage = window.browser.settingsManager.settings.homePage;
            await this.handleNavigate(homePage);
        } catch (error) {
            console.error('Home failed:', error);
            this.showError('Failed to go home');
        }
    }

    async handleNavigate(url) {
        try {
            if (!url.startsWith('http://') && !url.startsWith('https://')) {
                url = 'https://' + url;
            }

            console.log('Navigating to:', url);
            await window.ipcClient.navigate(url);
            this.addressBar.value = url;
            window.browser.addToHistory(url);
            this.updateUI();
        } catch (error) {
            console.error('Navigate failed:', error);
            this.showError('Failed to navigate');
        }
    }

    async handleNewTabClick() {
        try {
            console.log('Opening new tab...');
            const result = await window.ipcClient.openTab('https://example.com');
            const tabId = result && result.tabId ? result.tabId : this.nextTabId++;
            this.createTab(tabId, 'New Tab', 'https://example.com');
            this.switchTab(tabId);
        } catch (error) {
            console.error('New tab failed:', error);
            this.showError('Failed to open new tab');
        }
    }

    createTab(tabId, title, url) {
        const tabElement = document.createElement('div');
        tabElement.className = 'tab';
        tabElement.dataset.tabId = tabId;
        tabElement.dataset.url = url;
        tabElement.innerHTML = '<span class="tab-title">' + title + '</span><button class="tab-close">×</button>';

        tabElement.addEventListener('click', (e) => {
            if (!e.target.classList.contains('tab-close')) {
                this.switchTab(tabId);
            }
        });

        tabElement.querySelector('.tab-close').addEventListener('click', async (e) => {
            e.stopPropagation();
            await this.closeTab(tabId);
        });

        this.tabsContainer.appendChild(tabElement);
    }

    async switchTab(tabId) {
        try {
            const tabs = document.querySelectorAll('.tab');
            tabs.forEach(tab => {
                tab.classList.remove('active');
            });
            const activeTab = document.querySelector('[data-tab-id="' + tabId + '"]');
            if (activeTab) {
                activeTab.classList.add('active');
            }
            
            this.activeTabId = tabId;
            
            const tabUrl = activeTab ? activeTab.dataset.url : '';
            this.addressBar.value = tabUrl;
            
            console.log('Switched to tab:', tabId);
            this.updateUI();
        } catch (error) {
            console.error('Switch tab failed:', error);
        }
    }

    async closeTab(tabId) {
        try {
            console.log('Closing tab:', tabId);
            await window.ipcClient.closeTab(tabId);
            
            const tabElement = document.querySelector('[data-tab-id="' + tabId + '"]');
            if (tabElement) {
                tabElement.remove();
            }
            
            if (this.activeTabId === tabId) {
                const firstTab = document.querySelector('.tab');
                if (firstTab) {
                    const firstTabId = parseInt(firstTab.dataset.tabId);
                    this.switchTab(firstTabId);
                } else {
                    this.handleNewTabClick();
                }
            }
        } catch (error) {
            console.error('Close tab failed:', error);
            this.showError('Failed to close tab');
        }
    }

    async updateUI() {
        try {
            const url = await window.ipcClient.getCurrentURL();
            this.addressBar.value = url || '';
        } catch (error) {
            console.error('Failed to update UI:', error);
        }
    }

    showError(message) {
        const errorDiv = document.createElement('div');
        errorDiv.style.cssText = 'position: fixed; top: 10px; right: 10px; background: #f44336; color: white; padding: 10px 15px; border-radius: 4px; z-index: 9999; font-size: 13px;';
        errorDiv.textContent = message;
        document.body.appendChild(errorDiv);
        
        setTimeout(function() {
            errorDiv.remove();
        }, 3000);
    }

    addToHistory(url) {
        if (window.browser.historyManager) {
            window.browser.historyManager.add(url);
        }
    }
}

document.addEventListener('DOMContentLoaded', function() {
    window.browser = new NovaBrowser();
    
    const btn = document.getElementById('btn-theme-toggle');
    const icon = document.getElementById('theme-icon');
    
    const savedTheme = localStorage.getItem('theme') || 'light';
    if (savedTheme === 'dark') {
        document.body.classList.add('dark-theme');
        icon.textContent = '☀️';
    }
    
    btn.onclick = function() {
        const isDark = document.body.classList.toggle('dark-theme');
        const newTheme = isDark ? 'dark' : 'light';
        localStorage.setItem('theme', newTheme);
        icon.textContent = isDark ? '☀️' : '🌙';
    };
    
    window.browser.historyManager = {
        list: [],
        filteredList: [],
        
        init: function() {
            try {
                const stored = localStorage.getItem('nova_history');
                if (stored) this.list = JSON.parse(stored);
            } catch (e) {}
            this.filteredList = this.list;
            this.render();
        },
        
        add: function(url) {
            const now = new Date();
            const time = now.toLocaleTimeString('en-US', { hour: '2-digit', minute: '2-digit', hour12: true });
            const date = now.toLocaleDateString('en-US', { month: 'short', day: 'numeric' });
            
            this.list.unshift({ url: url, time: time, date: date });
            
            if (this.list.length > 100) {
                this.list = this.list.slice(0, 100);
            }
            
            localStorage.setItem('nova_history', JSON.stringify(this.list));
            this.filteredList = this.list;
            this.render();
        },
        
        search: function(query) {
            if (!query) {
                this.filteredList = this.list;
            } else {
                const q = query.toLowerCase();
                this.filteredList = this.list.filter(function(item) {
                    return item.url.toLowerCase().includes(q);
                });
            }
            this.render();
        },
        
        render: function() {
            const historyList = document.getElementById('history-list');
            historyList.innerHTML = '';
            
            if (this.filteredList.length === 0) {
                historyList.innerHTML = '<div style="padding: 16px; text-align: center; color: var(--text-light); font-size: 12px;">No history found</div>';
                return;
            }
            
            this.filteredList.forEach(function(item, idx) {
                const div = document.createElement('div');
                div.className = 'history-item';
                const domain = item.url.split('//')[1] || item.url;
                div.innerHTML = '<div class="history-item-url">' + domain + '</div><div class="history-item-time">' + item.time + ' ' + item.date + '</div>';
                
                div.onclick = function() {
                    window.browser.handleNavigate(item.url);
                };
                
                historyList.appendChild(div);
            });
        }
    };
    
    const clearBtn = document.getElementById('btn-clear-history');
    clearBtn.onclick = function() {
        if (confirm('Clear all history?')) {
            window.browser.historyManager.list = [];
            window.browser.historyManager.filteredList = [];
            localStorage.setItem('nova_history', JSON.stringify([]));
            window.browser.historyManager.render();
        }
    };
    
    const searchHistoryInput = document.getElementById('search-history');
    searchHistoryInput.addEventListener('input', function() {
        window.browser.historyManager.search(this.value);
    });
    
    window.browser.historyManager.init();
    
    window.browser.settingsManager = {
        settings: {
            zoom: 100,
            homePage: 'https://example.com',
            doNotTrack: false,
            blockCookies: false
        },
        
        init: function() {
            try {
                const stored = localStorage.getItem('nova_settings');
                if (stored) {
                    this.settings = JSON.parse(stored);
                }
            } catch (e) {}
            this.loadUIFromSettings();
        },
        
        loadUIFromSettings: function() {
            document.getElementById('zoom-slider').value = this.settings.zoom;
            document.getElementById('zoom-value').textContent = this.settings.zoom + '%';
            document.getElementById('home-page-input').value = this.settings.homePage;
            document.getElementById('privacy-do-not-track').checked = this.settings.doNotTrack;
            document.getElementById('privacy-block-cookies').checked = this.settings.blockCookies;
        },
        
        saveSettings: function() {
            this.settings.zoom = parseInt(document.getElementById('zoom-slider').value);
            this.settings.homePage = document.getElementById('home-page-input').value;
            this.settings.doNotTrack = document.getElementById('privacy-do-not-track').checked;
            this.settings.blockCookies = document.getElementById('privacy-block-cookies').checked;
            
            localStorage.setItem('nova_settings', JSON.stringify(this.settings));
            console.log('Settings saved:', this.settings);
        },
        
        resetToDefaults: function() {
            this.settings = {
                zoom: 100,
                homePage: 'https://example.com',
                doNotTrack: false,
                blockCookies: false
            };
            localStorage.setItem('nova_settings', JSON.stringify(this.settings));
            this.loadUIFromSettings();
            console.log('Settings reset to defaults');
        }
    };
    
    window.browser.settingsManager.init();
    
    const zoomSlider = document.getElementById('zoom-slider');
    const zoomValue = document.getElementById('zoom-value');
    zoomSlider.addEventListener('input', function() {
        zoomValue.textContent = this.value + '%';
    });
    
    const settingsModal = document.getElementById('settings-modal');
    const btnSettings = document.getElementById('btn-settings');
    const btnCloseSettings = document.getElementById('btn-close-settings');
    const btnSaveSettings = document.getElementById('btn-save-settings');
    const btnResetSettings = document.getElementById('btn-reset-settings');
    
    btnSettings.onclick = function() {
        window.browser.settingsManager.loadUIFromSettings();
        settingsModal.style.display = 'flex';
    };
    
    btnCloseSettings.onclick = function() {
        settingsModal.style.display = 'none';
    };
    
    btnSaveSettings.onclick = function() {
        window.browser.settingsManager.saveSettings();
        settingsModal.style.display = 'none';
        window.browser.showError('Settings saved!');
    };
    
    btnResetSettings.onclick = function() {
        if (confirm('Reset all settings to default?')) {
            window.browser.settingsManager.resetToDefaults();
            window.browser.showError('Settings reset to default');
        }
    };
    
    settingsModal.onclick = function(e) {
        if (e.target === settingsModal) {
            settingsModal.style.display = 'none';
        }
    };
    
    const favBtn = document.getElementById('btn-add-favorite');
    const favList = document.getElementById('favorites-list');
    let favs = [];
    let filteredFavs = [];
    
    try {
        const stored = localStorage.getItem('nova_favs');
        if (stored) favs = JSON.parse(stored);
    } catch (e) {}
    
    const renderFavorites = function() {
        favList.innerHTML = '';
        
        if (filteredFavs.length === 0) {
            if (favs.length === 0) {
                favList.innerHTML = '<div style="padding: 16px; text-align: center; color: var(--text-light); font-size: 12px;">No favorites</div>';
            } else {
                favList.innerHTML = '<div style="padding: 16px; text-align: center; color: var(--text-light); font-size: 12px;">No matches</div>';
            }
            return;
        }
        
        filteredFavs.forEach(function(f, i) {
            const item = document.createElement('div');
            item.className = 'fav-item';
            item.innerHTML = f.name + ' <button class="fav-remove">×</button>';
            
            item.onclick = function(e) {
                if (e.target.classList.contains('fav-remove')) {
                    const idx = favs.indexOf(f);
                    if (idx > -1) {
                        favs.splice(idx, 1);
                    }
                    localStorage.setItem('nova_favs', JSON.stringify(favs));
                    filteredFavs = favs.slice();
                    renderFavorites();
                } else {
                    window.browser.handleNavigate(f.url);
                }
            };
            favList.appendChild(item);
        });
    };
    
    const searchFavoritesInput = document.getElementById('search-favorites');
    searchFavoritesInput.addEventListener('input', function() {
        const query = this.value.toLowerCase();
        if (!query) {
            filteredFavs = favs.slice();
        } else {
            filteredFavs = favs.filter(function(f) {
                return f.url.toLowerCase().includes(query) || f.name.toLowerCase().includes(query);
            });
        }
        renderFavorites();
    });
    
    if (favBtn) {
        favBtn.onclick = function() {
            const url = window.browser.addressBar.value;
            const name = url.split('/')[2] || url;
            favs.push({url: url, name: name});
            localStorage.setItem('nova_favs', JSON.stringify(favs));
            filteredFavs = favs.slice();
            renderFavorites();
        };
    }
    
    filteredFavs = favs.slice();
    renderFavorites();
});
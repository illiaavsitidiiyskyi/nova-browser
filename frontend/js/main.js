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
            await this.handleNavigate('https://example.com');
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
}

document.addEventListener('DOMContentLoaded', function() {
    window.browser = new NovaBrowser();
    
    const btn = document.getElementById('btn-add-favorite');
    const list = document.getElementById('favorites-list');
    let favs = [];
    
    try {
        const stored = localStorage.getItem('nova_favs');
        if (stored) favs = JSON.parse(stored);
    } catch (e) {}
    
    const render = function() {
        list.innerHTML = '';
        favs.forEach(function(f, i) {
            const item = document.createElement('div');
            item.className = 'fav-item';
            item.innerHTML = f.name + ' <button class="fav-remove">×</button>';
            
            item.onclick = function(e) {
                if (e.target.classList.contains('fav-remove')) {
                    favs.splice(i, 1);
                    localStorage.setItem('nova_favs', JSON.stringify(favs));
                    render();
                } else {
                    window.browser.handleNavigate(f.url);
                }
            };
            list.appendChild(item);
        });
    };
    
    if (btn) {
        btn.onclick = function() {
            const url = window.browser.addressBar.value;
            const name = url.split('/')[2] || url;
            favs.push({url: url, name: name});
            localStorage.setItem('nova_favs', JSON.stringify(favs));
            render();
        };
    }
    
    render();
});
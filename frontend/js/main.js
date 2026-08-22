// Simple frontend logic
class NovaBrowser {
    constructor() {
        this.tabs = [];
        this.activeTabId = 1;
        this.nextTabId = 2;
        
        this.initializeElements();
        this.attachEventListeners();
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
        this.btnBack.addEventListener('click', () => this.goBack());
        this.btnForward.addEventListener('click', () => this.goForward());
        this.btnReload.addEventListener('click', () => this.reload());
        this.btnHome.addEventListener('click', () => this.goHome());
        this.btnNewTab.addEventListener('click', () => this.newTab());

        this.addressBar.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                this.navigate(this.addressBar.value);
            }
        });
    }

    navigate(url) {
        console.log('Navigating to:', url);
        // TODO: Call C++ backend to navigate
        // For now, just update address bar
    }

    goBack() {
        console.log('Go back');
        // TODO: Call C++ backend
    }

    goForward() {
        console.log('Go forward');
        // TODO: Call C++ backend
    }

    reload() {
        console.log('Reload');
        // TODO: Call C++ backend
    }

    goHome() {
        console.log('Go home');
        this.navigate('https://example.com');
    }

    newTab() {
        console.log('New tab');
        const tabId = this.nextTabId++;
        this.createTab(tabId, 'New Tab');
    }

    createTab(tabId, title) {
        const tabElement = document.createElement('div');
        tabElement.className = 'tab active';
        tabElement.dataset.tabId = tabId;
        tabElement.innerHTML = `
            <span class="tab-title">${title}</span>
            <button class="tab-close">×</button>
        `;

        tabElement.addEventListener('click', () => this.switchTab(tabId));
        tabElement.querySelector('.tab-close').addEventListener('click', (e) => {
            e.stopPropagation();
            this.closeTab(tabId);
        });

        this.tabsContainer.appendChild(tabElement);
        this.activeTabId = tabId;
    }

    switchTab(tabId) {
        document.querySelectorAll('.tab').forEach(tab => {
            tab.classList.remove('active');
        });
        document.querySelector(`[data-tab-id="${tabId}"]`).classList.add('active');
        this.activeTabId = tabId;
        console.log('Switched to tab:', tabId);
    }

    closeTab(tabId) {
        const tabElement = document.querySelector(`[data-tab-id="${tabId}"]`);
        if (tabElement) {
            tabElement.remove();
        }
        console.log('Closed tab:', tabId);
    }
}

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    new NovaBrowser();
});
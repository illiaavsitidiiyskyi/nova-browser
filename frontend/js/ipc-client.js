class IPCClient {
    constructor() {
        this.requestId = 0;
        this.pendingRequests = new Map();
        this.listeners = new Map();
        
        if (window.cefQuery) {
            this.setupResponseHandler();
        } else {
            console.warn('CEF not available - IPC disabled');
        }
    }

    async send(command, params = {}) {
        return new Promise((resolve, reject) => {
            const requestId = ++this.requestId;
            
            const message = {
                command,
                params,
                requestId
            };

            this.pendingRequests.set(requestId, { resolve, reject });

            if (window.cefQuery) {
                window.cefQuery({
                    request: JSON.stringify(message),
                    onSuccess: (response) => {
                        try {
                            const data = JSON.parse(response);
                            if (data.success) {
                                resolve(data.result);
                            } else {
                                reject(new Error(data.error || 'Unknown error'));
                            }
                        } catch (e) {
                            reject(e);
                        }
                        this.pendingRequests.delete(requestId);
                    },
                    onFailure: (error) => {
                        reject(new Error(`IPC error: ${error}`));
                        this.pendingRequests.delete(requestId);
                    }
                });
            } else {
                reject(new Error('CEF not available'));
            }
        });
    }

    emit(event, data = {}) {
        if (window.cefQuery) {
            window.cefQuery({
                request: JSON.stringify({ event, data }),
                onSuccess: () => {},
                onFailure: () => console.error(`Failed to emit event: ${event}`)
            });
        }
    }

    on(event, callback) {
        if (!this.listeners.has(event)) {
            this.listeners.set(event, []);
        }
        this.listeners.get(event).push(callback);
    }

    setupResponseHandler() {
    }

    async openTab(url) {
        return this.send('openTab', { url });
    }

    async closeTab(tabId) {
        return this.send('closeTab', { tabId });
    }

    async goBack() {
        return this.send('goBack', {});
    }

    async goForward() {
        return this.send('goForward', {});
    }

    async reload() {
        return this.send('reload', {});
    }

    async navigate(url) {
        return this.send('navigate', { url });
    }

    async getCurrentURL() {
        return this.send('getCurrentURL', {});
    }
}

window.ipcClient = new IPCClient();
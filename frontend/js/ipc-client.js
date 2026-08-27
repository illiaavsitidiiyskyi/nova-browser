class IPCClient {
    constructor() {
        this.requestId = 0;
    }

    async send(command, params = {}) {
        return new Promise((resolve, reject) => {
            const requestId = ++this.requestId;
            
            const message = {
                command,
                params,
                requestId
            };

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
                    },
                    onFailure: (error) => {
                        reject(new Error(`IPC error: ${error}`));
                    }
                });
            } else {
                reject(new Error('CEF not available'));
            }
        });
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
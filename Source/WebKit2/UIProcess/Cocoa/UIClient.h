/*
 * Copyright (C) 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UIClient_h
#define UIClient_h

#import "WKFoundation.h"

#if WK_API_ENABLED

#import "APIUIClient.h"
#import "WeakObjCPtr.h"
#import <wtf/RetainPtr.h>

@class _WKActivatedElementInfo;
@class WKWebView;
@protocol WKUIDelegate;

namespace WebKit {

class UIClient : public API::UIClient {
public:
    explicit UIClient(WKWebView *);
    ~UIClient();

    RetainPtr<id <WKUIDelegate> > delegate();
    void setDelegate(id <WKUIDelegate>);

    NSArray *actionsForElement(_WKActivatedElementInfo *, NSArray *defaultActions);

private:
    // API::UIClient
    virtual void runJavaScriptAlert(WebKit::WebPageProxy*, const WTF::String&, WebKit::WebFrameProxy*, std::function<void ()> completionHandler) override;
    virtual void runJavaScriptConfirm(WebKit::WebPageProxy*, const WTF::String&, WebKit::WebFrameProxy*, std::function<void (bool)> completionHandler) override;
    virtual void runJavaScriptPrompt(WebKit::WebPageProxy*, const WTF::String&, const WTF::String&, WebKit::WebFrameProxy*, std::function<void (const WTF::String&)> completionHandler) override;

    WKWebView *m_webView;
    WeakObjCPtr<id <WKUIDelegate> > m_delegate;

    struct {
        bool webViewRunJavaScriptAlertPanelWithMessageInitiatedByFrameCompletionHandler : 1;
        bool webViewRunJavaScriptConfirmPanelWithMessageInitiatedByFrameCompletionHandler : 1;
        bool webViewRunJavaScriptTextInputPanelWithPromptDefaultTextInitiatedByFrameCompletionHandler : 1;
#if PLATFORM(IOS)
        bool webViewActionsForElementDefaultActions : 1;
#endif
    } m_delegateMethods;
};

} // namespace WebKit

#endif // WK_API_ENABLED

#endif // UIClient_h

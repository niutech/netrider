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

#import "config.h"
#import "UIClient.h"

#if WK_API_ENABLED

#import "WKFrameInfoInternal.h"
#import "WKUIDelegatePrivate.h"

namespace WebKit {

UIClient::UIClient(WKWebView *webView)
    : m_webView(webView)
{
}

UIClient::~UIClient()
{
}

RetainPtr<id <WKUIDelegate> > UIClient::delegate()
{
    return m_delegate.get();
}

void UIClient::setDelegate(id <WKUIDelegate> delegate)
{
    m_delegate = delegate;

    m_delegateMethods.webViewRunJavaScriptAlertPanelWithMessageInitiatedByFrameCompletionHandler = [delegate respondsToSelector:@selector(webView:runJavaScriptAlertPanelWithMessage:initiatedByFrame:completionHandler:)];
    m_delegateMethods.webViewRunJavaScriptConfirmPanelWithMessageInitiatedByFrameCompletionHandler = [delegate respondsToSelector:@selector(webView:runJavaScriptConfirmPanelWithMessage:initiatedByFrame:completionHandler:)];
    m_delegateMethods.webViewRunJavaScriptTextInputPanelWithPromptDefaultTextInitiatedByFrameCompletionHandler = [delegate respondsToSelector:@selector(webView:runJavaScriptTextInputPanelWithPrompt:defaultText:initiatedByFrame:completionHandler:)];
#if PLATFORM(IOS)
    m_delegateMethods.webViewActionsForElementDefaultActions = [delegate respondsToSelector:@selector(_webView:actionsForElement:defaultActions:)];
#endif
}

NSArray *UIClient::actionsForElement(_WKActivatedElementInfo *elementInfo, NSArray *defaultActions)
{
#if PLATFORM(IOS)
    if (!m_delegateMethods.webViewRunJavaScriptTextInputPanelWithPromptDefaultTextInitiatedByFrameCompletionHandler)
        return defaultActions;

    auto delegate = m_delegate.get();
    if (!delegate)
        return defaultActions;

    return [(id <WKUIDelegatePrivate>)delegate _webView:m_webView actionsForElement:elementInfo defaultActions:defaultActions];
#endif
    return defaultActions;
}

void UIClient::runJavaScriptAlert(WebKit::WebPageProxy*, const WTF::String& message, WebKit::WebFrameProxy* webFrameProxy, std::function<void ()> completionHandler)
{
    if (!m_delegateMethods.webViewRunJavaScriptAlertPanelWithMessageInitiatedByFrameCompletionHandler) {
        completionHandler();
        return;
    }

    auto delegate = m_delegate.get();
    if (!delegate) {
        completionHandler();
        return;
    }

    [delegate webView:m_webView runJavaScriptAlertPanelWithMessage:message initiatedByFrame:adoptNS([[WKFrameInfo alloc] initWithWebFrameProxy:*webFrameProxy]).get() completionHandler:[completionHandler] {
        completionHandler();
    }];
}

void UIClient::runJavaScriptConfirm(WebKit::WebPageProxy*, const WTF::String& message, WebKit::WebFrameProxy* webFrameProxy, std::function<void (bool)> completionHandler)
{
    if (!m_delegateMethods.webViewRunJavaScriptConfirmPanelWithMessageInitiatedByFrameCompletionHandler) {
        completionHandler(false);
        return;
    }

    auto delegate = m_delegate.get();
    if (!delegate) {
        completionHandler(false);
        return;
    }

    [delegate webView:m_webView runJavaScriptConfirmPanelWithMessage:message initiatedByFrame:adoptNS([[WKFrameInfo alloc] initWithWebFrameProxy:*webFrameProxy]).get() completionHandler:[completionHandler](BOOL result) {
        completionHandler(result);
    }];
}

void UIClient::runJavaScriptPrompt(WebKit::WebPageProxy*, const WTF::String& message, const WTF::String& defaultValue, WebKit::WebFrameProxy* webFrameProxy, std::function<void (const WTF::String&)> completionHandler)
{
    if (!m_delegateMethods.webViewRunJavaScriptTextInputPanelWithPromptDefaultTextInitiatedByFrameCompletionHandler) {
        completionHandler(String());
        return;
    }

    auto delegate = m_delegate.get();
    if (!delegate) {
        completionHandler(String());
        return;
    }

    [delegate webView:m_webView runJavaScriptTextInputPanelWithPrompt:message defaultText:defaultValue initiatedByFrame:adoptNS([[WKFrameInfo alloc] initWithWebFrameProxy:*webFrameProxy]).get() completionHandler:[completionHandler](NSString *result) {
        completionHandler(result);
    }];
}

} // namespace WebKit

#endif // WK_API_ENABLED

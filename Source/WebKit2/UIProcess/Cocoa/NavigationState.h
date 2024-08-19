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

#ifndef NavigationState_h
#define NavigationState_h

#import "WKFoundation.h"

#if WK_API_ENABLED

#import <wtf/HashMap.h>
#import <wtf/RetainPtr.h>
#import "APILoaderClient.h"
#import "APIPolicyClient.h"
#import "PageLoadState.h"
#import "WeakObjCPtr.h"

@class WKNavigation;
@class WKWebView;
@protocol WKHistoryDelegatePrivate;
@protocol WKNavigationDelegate;

namespace WebKit {

struct WebNavigationDataStore;

class NavigationState : private PageLoadState::Observer {
public:
    explicit NavigationState(WKWebView *);
    ~NavigationState();

    static NavigationState& fromWebPage(WebPageProxy&);

    std::unique_ptr<API::PolicyClient> createPolicyClient();
    std::unique_ptr<API::LoaderClient> createLoaderClient();

    RetainPtr<id <WKNavigationDelegate> > navigationDelegate();
    void setNavigationDelegate(id <WKNavigationDelegate>);

    RetainPtr<id <WKHistoryDelegatePrivate> > historyDelegate();
    void setHistoryDelegate(id <WKHistoryDelegatePrivate>);

    RetainPtr<WKNavigation> createLoadRequestNavigation(uint64_t navigationID, NSURLRequest *);

    // Called by the history client.
    void didNavigateWithNavigationData(const WebKit::WebNavigationDataStore&);
    void didPerformClientRedirect(const WTF::String& sourceURL, const WTF::String& destinationURL);
    void didPerformServerRedirect(const WTF::String& sourceURL, const WTF::String& destinationURL);
    void didUpdateHistoryTitle(const WTF::String& title, const WTF::String& url);

private:
    class PolicyClient : public API::PolicyClient {
    public:
        explicit PolicyClient(NavigationState&);
        ~PolicyClient();

    private:
        // API::PolicyClient
        virtual void decidePolicyForNavigationAction(WebPageProxy*, WebFrameProxy* destinationFrame, const NavigationActionData&, WebFrameProxy* sourceFrame, const WebCore::ResourceRequest& originalRequest, const WebCore::ResourceRequest&, RefPtr<WebFramePolicyListenerProxy>, API::Object* userData) override;
        virtual void decidePolicyForNewWindowAction(WebPageProxy*, WebFrameProxy* sourceFrame, const NavigationActionData&, const WebCore::ResourceRequest&, const WTF::String& frameName, RefPtr<WebFramePolicyListenerProxy>, API::Object* userData) override;
        virtual void decidePolicyForResponse(WebPageProxy*, WebFrameProxy*, const WebCore::ResourceResponse&, const WebCore::ResourceRequest&, bool canShowMIMEType, RefPtr<WebFramePolicyListenerProxy>, API::Object* userData) override;

        NavigationState& m_navigationState;
    };

    class LoaderClient : public API::LoaderClient {
    public:
        explicit LoaderClient(NavigationState&);
        ~LoaderClient();

    private:
        virtual void didStartProvisionalLoadForFrame(WebPageProxy*, WebFrameProxy*, uint64_t navigationID, API::Object*) override;
        virtual void didReceiveServerRedirectForProvisionalLoadForFrame(WebKit::WebPageProxy*, WebKit::WebFrameProxy*, uint64_t navigationID, API::Object*);
        virtual void didFailProvisionalLoadWithErrorForFrame(WebPageProxy*, WebFrameProxy*, uint64_t navigationID, const WebCore::ResourceError&, API::Object*) override;
        virtual void didCommitLoadForFrame(WebPageProxy*, WebFrameProxy*, uint64_t navigationID, API::Object*) override;
        virtual void didFinishLoadForFrame(WebPageProxy*, WebFrameProxy*, uint64_t navigationID, API::Object*) override;
        virtual void didFailLoadWithErrorForFrame(WebPageProxy*, WebFrameProxy*, uint64_t navigationID, const WebCore::ResourceError&, API::Object*) override;
        virtual void didLayout(WebKit::WebPageProxy*, WebCore::LayoutMilestones, API::Object*) override;
        virtual bool canAuthenticateAgainstProtectionSpaceInFrame(WebKit::WebPageProxy*, WebKit::WebFrameProxy*, WebKit::WebProtectionSpace*) override;
        virtual void didReceiveAuthenticationChallengeInFrame(WebKit::WebPageProxy*, WebKit::WebFrameProxy*, WebKit::AuthenticationChallengeProxy*) override;
        virtual void processDidCrash(WebKit::WebPageProxy*) override;
        virtual void didChangeBackForwardList(WebKit::WebPageProxy*, WebKit::WebBackForwardListItem* addedItem, Vector<RefPtr<WebKit::WebBackForwardListItem>> removedItems) override;

        NavigationState& m_navigationState;
    };

    // PageLoadState::Observer
    virtual void willChangeIsLoading() override;
    virtual void didChangeIsLoading() override;
    virtual void willChangeTitle() override;
    virtual void didChangeTitle() override;
    virtual void willChangeActiveURL() override;
    virtual void didChangeActiveURL() override;
    virtual void willChangeHasOnlySecureContent() override;
    virtual void didChangeHasOnlySecureContent() override;
    virtual void willChangeEstimatedProgress() override;
    virtual void didChangeEstimatedProgress() override;

    WKWebView *m_webView;
    WeakObjCPtr<id <WKNavigationDelegate> > m_navigationDelegate;

    struct {
        bool webViewDecidePolicyForNavigationActionDecisionHandler : 1;
        bool webViewDecidePolicyForNavigationResponseDecisionHandler : 1;

        bool webViewDidStartProvisionalNavigation : 1;
        bool webViewDidReceiveServerRedirectForProvisionalNavigation : 1;
        bool webViewDidFailProvisionalNavigationWithError : 1;
        bool webViewDidCommitNavigation : 1;
        bool webViewDidFinishLoadingNavigation : 1;
        bool webViewDidFailNavigationWithError : 1;

        bool webViewRenderingProgressDidChange : 1;
        bool webViewCanAuthenticateAgainstProtectionSpace : 1;
        bool webViewDidReceiveAuthenticationChallenge : 1;
        bool webViewWebProcessDidCrash : 1;
    } m_navigationDelegateMethods;

    HashMap<uint64_t, RetainPtr<WKNavigation>> m_navigations;

    WeakObjCPtr<id <WKHistoryDelegatePrivate> > m_historyDelegate;
    struct {
        bool webViewDidNavigateWithNavigationData : 1;
        bool webViewDidPerformClientRedirectFromURLToURL : 1;
        bool webViewDidPerformServerRedirectFromURLToURL : 1;
        bool webViewDidUpdateHistoryTitleForURL : 1;
    } m_historyDelegateMethods;
};

} // namespace WebKit

#endif

#endif // NavigationState_h

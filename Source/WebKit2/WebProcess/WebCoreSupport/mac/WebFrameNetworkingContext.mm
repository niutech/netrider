/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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

#include "config.h"

#include "SessionTracker.h"
#include "WebCookieManager.h"
#include "WebFrameNetworkingContext.h"
#include "WebPage.h"
#include <WebCore/Frame.h>
#include <WebCore/FrameLoader.h>
#include <WebCore/FrameLoaderClient.h>
#include <WebCore/NetworkStorageSession.h>
#include <WebCore/Page.h>
#include <WebCore/ResourceError.h>
#include <WebCore/Settings.h>
#include <WebKitSystemInterface.h>

using namespace WebCore;

namespace WebKit {
    
void WebFrameNetworkingContext::ensurePrivateBrowsingSession(SessionID sessionID)
{
    ASSERT(sessionID.isEphemeral());

    if (SessionTracker::session(sessionID))
        return;

    String base;
    if (SessionTracker::getIdentifierBase().isNull())
        base = [[NSBundle mainBundle] bundleIdentifier];
    else
        base = SessionTracker::getIdentifierBase();

    SessionTracker::setSession(sessionID, NetworkStorageSession::createPrivateBrowsingSession(base + '.' + String::number(sessionID.sessionID())));
}

void WebFrameNetworkingContext::setCookieAcceptPolicyForAllContexts(HTTPCookieAcceptPolicy policy)
{
    [[NSHTTPCookieStorage sharedHTTPCookieStorage] setCookieAcceptPolicy:static_cast<NSHTTPCookieAcceptPolicy>(policy)];

    if (RetainPtr<CFHTTPCookieStorageRef> cookieStorage = NetworkStorageSession::defaultStorageSession().cookieStorage())
        WKSetHTTPCookieAcceptPolicy(cookieStorage.get(), policy);

    for (const auto& session : SessionTracker::sessionMap().values()) {
        if (session)
            WKSetHTTPCookieAcceptPolicy(session->cookieStorage().get(), policy);
    }
}
    
bool WebFrameNetworkingContext::needsSiteSpecificQuirks() const
{
    return frame() && frame()->settings().needsSiteSpecificQuirks();
}

bool WebFrameNetworkingContext::localFileContentSniffingEnabled() const
{
    return frame() && frame()->settings().localFileContentSniffingEnabled();
}

SchedulePairHashSet* WebFrameNetworkingContext::scheduledRunLoopPairs() const
{
    if (!frame() || !frame()->page())
        return 0;
    return frame()->page()->scheduledRunLoopPairs();
}

RetainPtr<CFDataRef> WebFrameNetworkingContext::sourceApplicationAuditData() const
{
    return RetainPtr<CFDataRef>();
}

ResourceError WebFrameNetworkingContext::blockedError(const ResourceRequest& request) const
{
    return frame()->loader().client().blockedError(request);
}

NetworkStorageSession& WebFrameNetworkingContext::storageSession() const
{
    ASSERT(RunLoop::isMain());
    return *SessionTracker::session(frame() ? frame()->page()->sessionID() : SessionID::defaultSessionID());
}

WebFrameLoaderClient* WebFrameNetworkingContext::webFrameLoaderClient() const
{
    if (!frame())
        return 0;

    return toWebFrameLoaderClient(frame()->loader().client());
}

}

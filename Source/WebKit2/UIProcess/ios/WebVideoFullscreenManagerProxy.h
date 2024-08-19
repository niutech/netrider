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

#ifndef WebVideoFullscreenManagerProxy_h
#define WebVideoFullscreenManagerProxy_h

#if PLATFORM(IOS)

#include "MessageReceiver.h"
#include <WebCore/GraphicsLayer.h>
#include <WebCore/WebVideoFullscreenInterfaceAVKit.h>
#include <WebCore/WebVideoFullscreenModel.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>

OBJC_CLASS UIView;

namespace WebKit {

class WebPageProxy;

class WebVideoFullscreenManagerProxy : public WebCore::WebVideoFullscreenInterfaceAVKit, public WebCore::WebVideoFullscreenChangeObserver, public WebCore::WebVideoFullscreenModel, private IPC::MessageReceiver {
public:
    static PassRefPtr<WebVideoFullscreenManagerProxy> create(WebPageProxy&);
    virtual ~WebVideoFullscreenManagerProxy();

private:
    explicit WebVideoFullscreenManagerProxy(WebPageProxy&);
    virtual void didReceiveMessage(IPC::Connection*, IPC::MessageDecoder&) override;

    // Translate to FullscreenInterface
    virtual void enterFullscreenWithID(uint32_t);

    // Fullscreen Observer
    virtual void requestExitFullscreen() override;
    virtual void didExitFullscreen() override;
    virtual void didEnterFullscreen() override;
    
    // FullscreenModel
    virtual void play() override;
    virtual void pause() override;
    virtual void togglePlayState() override;
    virtual void seekToTime(double) override;
    virtual void setVideoLayerFrame(WebCore::FloatRect) override;
    virtual void setVideoLayerGravity(WebCore::WebVideoFullscreenModel::VideoGravity) override;

    WebPageProxy* m_page;
    RetainPtr<PlatformLayer> m_layerHost;
};
    
} // namespace WebKit

#endif // PLATFORM(IOS)

#endif // WebVideoFullscreenManagerProxy_h

/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RTCDataChannel_h
#define RTCDataChannel_h

#if ENABLE(MEDIA_STREAM)

#include "ArrayBufferView.h"
#include "EventTarget.h"
#include "RTCDataChannelHandlerClient.h"
#include "ScriptWrappable.h"
#include "Timer.h"
#include <wtf/RefCounted.h>

namespace JSC {
    class ArrayBuffer;
    class ArrayBufferView;
}

namespace WebCore {

class Blob;
class Dictionary;
class RTCDataChannelHandler;
class RTCPeerConnectionHandler;

class RTCDataChannel final : public RefCounted<RTCDataChannel>, public ScriptWrappable, public EventTargetWithInlineData, public RTCDataChannelHandlerClient {
public:
    static PassRefPtr<RTCDataChannel> create(ScriptExecutionContext*, std::unique_ptr<RTCDataChannelHandler>);
    static PassRefPtr<RTCDataChannel> create(ScriptExecutionContext*, RTCPeerConnectionHandler*, const String& , const Dictionary&, ExceptionCode&);
    ~RTCDataChannel();

    String label() const;
    bool ordered() const;
    unsigned short maxRetransmitTime() const;
    unsigned short maxRetransmits() const;
    String protocol() const;
    bool negotiated() const;
    unsigned short id() const;
    AtomicString readyState() const;
    unsigned long bufferedAmount() const;

    AtomicString binaryType() const;
    void setBinaryType(const AtomicString&, ExceptionCode&);

    void send(const String&, ExceptionCode&);
    void send(PassRefPtr<JSC::ArrayBuffer>, ExceptionCode&);
    void send(PassRefPtr<JSC::ArrayBufferView>, ExceptionCode&);
    void send(PassRefPtr<Blob>, ExceptionCode&);

    void close();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(open);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(error);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(close);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(message);

    void stop();

    // EventTarget
    virtual EventTargetInterface eventTargetInterface() const override { return RTCDataChannelEventTargetInterfaceType; }
    virtual ScriptExecutionContext* scriptExecutionContext() const override { return m_scriptExecutionContext; }

    using RefCounted<RTCDataChannel>::ref;
    using RefCounted<RTCDataChannel>::deref;

private:
    RTCDataChannel(ScriptExecutionContext*, std::unique_ptr<RTCDataChannelHandler>);

    void scheduleDispatchEvent(PassRefPtr<Event>);
    void scheduledEventTimerFired(Timer<RTCDataChannel>*);

    // EventTarget
    virtual void refEventTarget() override { ref(); }
    virtual void derefEventTarget() override { deref(); }

    ScriptExecutionContext* m_scriptExecutionContext;

    // RTCDataChannelHandlerClient
    virtual void didChangeReadyState(ReadyState) override;
    virtual void didReceiveStringData(const String&) override;
    virtual void didReceiveRawData(const char*, size_t) override;
    virtual void didDetectError() override;

    std::unique_ptr<RTCDataChannelHandler> m_handler;

    bool m_stopped;

    ReadyState m_readyState;
    enum BinaryType {
        BinaryTypeBlob,
        BinaryTypeArrayBuffer
    };
    BinaryType m_binaryType;

    Timer<RTCDataChannel> m_scheduledEventTimer;
    Vector<RefPtr<Event>> m_scheduledEvents;
};

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)

#endif // RTCDataChannel_h

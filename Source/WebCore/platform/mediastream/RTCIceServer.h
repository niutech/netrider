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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef RTCIceServer_h
#define RTCIceServer_h

#if ENABLE(MEDIA_STREAM)

#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class RTCIceServer : public RefCounted<RTCIceServer> {
public:
    static PassRefPtr<RTCIceServer> create(const Vector<String>& urls, const String& credential, const String& username)
    {
        return adoptRef(new RTCIceServer(urls, credential, username));
    }
    virtual ~RTCIceServer() { }

    const Vector<String>& urls() { return m_urls; }
    const String& credential() { return m_credential; }
    const String& username() { return m_username; }

private:
    RTCIceServer(const Vector<String>& urls, const String& credential, const String& username)
        : m_urls(urls)
        , m_credential(credential)
        , m_username(username)
    {
    }

    Vector<String> m_urls;
    String m_credential;
    String m_username;
};

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)

#endif // RTCIceServer_h

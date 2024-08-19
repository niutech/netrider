/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Nix_MediaStreamSource_h
#define Nix_MediaStreamSource_h

#include "Common.h"
#include "PrivatePtr.h"

namespace WebCore {
class MediaStreamSource;
}

namespace Nix {

class NIX_EXPORT MediaStreamSource {
public:

    enum Type {
        Audio,
        Video
    };

    enum ReadyState {
        ReadyStateLive = 0,
        ReadyStateMuted = 1,
        ReadyStateEnded = 2
    };

    MediaStreamSource() { }
    virtual ~MediaStreamSource();

    bool isNull() const;

    const char* id() const;
    Type type() const;
    const char* name() const;

    void setReadyState(ReadyState);
    ReadyState readyState() const;

    bool enabled() const;
    void setEnabled(bool);

    bool muted() const;
    void setMuted(bool);

    bool readonly() const;
    void setReadonly(bool);

    // FIXME Add support for capabilites.
#if 0 //gp
    DEFINE_NIX_API_ASSIGNMENT(MediaStreamSource);
#if BUILDING_NIX__
    DEFINE_BASE_NIX_API_INTERNAL(MediaStreamSource, MediaStreamSource);
#endif
#endif

#if BUILDING_NIX__
 MediaStreamSource(const WTF::PassRefPtr<WebCore::MediaStreamSource>&);
 MediaStreamSource& operator=(WebCore::MediaStreamSource*);
 operator WTF::PassRefPtr<WebCore::MediaStreamSource>() const;
 operator WebCore::MediaStreamSource*() const;
#endif
protected:
PrivatePtr<WebCore::MediaStreamSource> m_private;
// WTF::RefPtr<WebCore::MediaStreamSource> m_private;
//#endif
};

} // namespace Nix

#endif // Nix_MediaStreamSource_h


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

#ifndef CDMPrivateMediaPlayer_h
#define CDMPrivateMediaPlayer_h

#include "CDMPrivate.h"
#include <wtf/PassOwnPtr.h>
#include <wtf/RetainPtr.h>

#if ENABLE(ENCRYPTED_MEDIA_V2)

namespace WebCore {

class CDM;

class CDMPrivateMediaPlayer : public CDMPrivateInterface {
public:
    static PassOwnPtr<CDMPrivateInterface> create(CDM* cdm) { return adoptPtr(new CDMPrivateMediaPlayer(cdm)); }
    static bool supportsKeySystem(const String&);
    static bool supportsKeySystemAndMimeType(const String& keySystem, const String& mimeType);

    virtual ~CDMPrivateMediaPlayer() { }

    virtual bool supportsMIMEType(const String& mimeType) override;
    virtual std::unique_ptr<CDMSession> createSession() override;

    CDM* cdm() const { return m_cdm; }

protected:
    CDMPrivateMediaPlayer(CDM* cdm) : m_cdm(cdm) { }
    CDM* m_cdm;
};

}

#endif

#endif // CDMPriavateMediaPlayer_h

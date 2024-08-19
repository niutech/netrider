/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Portions Copyright (c) 2010 Motorola Mobility, Inc.  All rights reserved.
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
#include "WebCoreArgumentCoders.h"

#include "NotImplemented.h"
#include <WebCore/ResourceError.h>
#include <WebCore/ResourceRequest.h>
#include <WebCore/ResourceResponse.h>
#include <wtf/text/CString.h>

using namespace WebCore;

namespace IPC {

void ArgumentCoder<CertificateInfo>::encode(ArgumentEncoder& encoder, const CertificateInfo& certificateInfo)
{
    notImplemented();
}

bool ArgumentCoder<CertificateInfo>::decode(ArgumentDecoder& decoder, CertificateInfo& certificateInfo)
{
    notImplemented();
    return true;
}

void ArgumentCoder<ResourceRequest>::encodePlatformData(ArgumentEncoder&, const ResourceRequest&)
{
    notImplemented();
}

bool ArgumentCoder<ResourceRequest>::decodePlatformData(ArgumentDecoder&, ResourceRequest&)
{
    notImplemented();
    return true;
}


void ArgumentCoder<ResourceResponse>::encodePlatformData(ArgumentEncoder&, const ResourceResponse&)
{
    notImplemented();
}

bool ArgumentCoder<ResourceResponse>::decodePlatformData(ArgumentDecoder&, ResourceResponse&)
{
    notImplemented();
    return true;
}

void ArgumentCoder<ResourceError>::encodePlatformData(ArgumentEncoder& encoder, const ResourceError& resourceError)
{
    encoder << resourceError.sslErrors();
}

bool ArgumentCoder<ResourceError>::decodePlatformData(ArgumentDecoder& decoder, ResourceError& resourceError)
{
    unsigned sslError;
    if (!decoder.decode(sslError))
        return false;
    resourceError.setSSLErrors(sslError);
    return true;
}

}


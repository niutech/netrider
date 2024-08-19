/*
 *  Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef WKErrorNix_h
#define WKErrorNix_h

#include <WebKit2/WKBase.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    // The signing certificate authority is not known.
    NIXTlsErrorUnkownCA = 0x01,
    // The certificate does not match the expected identity of the site that it was retrieved from.
    NIXTlsErrorCertificateBadIdentity = 0x02,
    // The certificate's activation time is still in the future.
    NIXTlsErrorCertificateNotActivated = 0x04,
    // The certificate has expired.
    NIXTlsErrorCertificateExpired = 0x08,
    // The certificate has been revoked.
    NIXTlsErrorCertificateRevoked = 0x10,
    // The certificate's algorithm is considered insecure.
    NIXTlsErrorCertificateInsecure = 0x20,
    // Some error occurred validating the certificate.
    NIXTlsErrorCertificateGenericError = 0x40
};
typedef unsigned NIXTLSErrors;

WK_EXPORT void WKErrorGetTLSErrors(WKErrorRef error, NIXTLSErrors* tlsErrors);
WK_EXPORT bool WKIsSSLClientCertificateError(WKErrorRef error);

#ifdef __cplusplus
}
#endif

#endif /* WKErrorNix_h */

/*
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WKAPICastNix_h
#define WKAPICastNix_h

#ifndef WKAPICast_h
#error "Please #include \"WKAPICast.h\" instead of this file directly."
#endif

#include "WKPopupItem.h"
#include "TextDirection.h"

namespace WebKit {

class WebPopupMenuListener;
class WebPopupItemPlatform;

class WebView;

WK_ADD_API_MAPPING(WKViewRef, WebView)
WK_ADD_API_MAPPING(WKPopupItemRef, WebPopupItemPlatform)
WK_ADD_API_MAPPING(WKPopupMenuListenerRef, WebPopupMenuListener)

inline WKPopupItemTextDirection toAPI(WebCore::TextDirection direction)
{
    WKPopupItemTextDirection wkDirection = kWKPopupItemTextDirectionLTR;

    switch (direction) {
    case WebCore::RTL:
        wkDirection = kWKPopupItemTextDirectionRTL;
        break;
    case WebCore::LTR:
        wkDirection = kWKPopupItemTextDirectionLTR;
        break;
    }

    return wkDirection;
}

}

#endif // WKAPICastNix_h

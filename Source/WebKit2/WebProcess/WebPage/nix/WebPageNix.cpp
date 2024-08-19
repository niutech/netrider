/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
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

#include "config.h"
#include "WebPage.h"

#include "EventHandler.h"
#include "NotImplemented.h"
#include "NIXView.h"
#include "WebEvent.h"
#include "WindowsKeyboardCodes.h"
#include <WebCore/NixKeyboardUtilities.h>
#include <WebCore/FocusController.h>
#include <WebCore/Frame.h>
#include <WebCore/FrameView.h>
#include <WebCore/KeyboardEvent.h>
#include <WebCore/Page.h>
#include <WebCore/PlatformKeyboardEvent.h>
#include <WebCore/Settings.h>
#include <WebCore/html/HTMLInputElement.h>

using namespace WebCore;

namespace WebKit {

void WebPage::platformInitialize()
{
    notImplemented();
}

void WebPage::platformPreferencesDidChange(const WebPreferencesStore&)
{
    notImplemented();
}

static inline void scroll(Page* page, ScrollDirection direction, ScrollGranularity granularity)
{
    page->focusController().focusedOrMainFrame().eventHandler().scrollRecursively(direction, granularity);
}

bool WebPage::performDefaultBehaviorForKeyEvent(const WebKeyboardEvent& keyboardEvent)
{
    if (keyboardEvent.type() != WebEvent::KeyDown && keyboardEvent.type() != WebEvent::RawKeyDown)
        return false;

    switch (keyboardEvent.windowsVirtualKeyCode()) {
    case VK_LEFT:
        scroll(m_page.get(), ScrollLeft, ScrollByLine);
        break;
    case VK_RIGHT:
        scroll(m_page.get(), ScrollRight, ScrollByLine);
        break;
    case VK_UP:
        scroll(m_page.get(), ScrollUp, ScrollByLine);
        break;
    case VK_DOWN:
        scroll(m_page.get(), ScrollDown, ScrollByLine);
        break;
    case VK_HOME:
        logicalScroll(m_page.get(), ScrollBlockDirectionBackward, ScrollByDocument);
        break;
    case VK_END:
        logicalScroll(m_page.get(), ScrollBlockDirectionForward, ScrollByDocument);
        break;
    case VK_PRIOR:
        logicalScroll(m_page.get(), ScrollBlockDirectionBackward, ScrollByPage);
        break;
    case VK_NEXT:
        logicalScroll(m_page.get(), ScrollBlockDirectionForward, ScrollByPage);
        break;
    default:
        return false;
    }

    return true;
}

bool WebPage::platformHasLocalDataForURL(const URL&)
{
    notImplemented();
    return false;
}

String WebPage::cachedResponseMIMETypeForURL(const URL&)
{
    notImplemented();
    return String();
}

bool WebPage::platformCanHandleRequest(const ResourceRequest&)
{
    notImplemented();
    return true;
}

String WebPage::cachedSuggestedFilenameForURL(const URL&)
{
    notImplemented();
    return String();
}

PassRefPtr<SharedBuffer> WebPage::cachedResponseDataForURL(const URL&)
{
    notImplemented();
    return 0;
}

const char* WebPage::interpretKeyEvent(const KeyboardEvent* event)
{
    ASSERT(event->type() == eventNames().keydownEvent || event->type() == eventNames().keypressEvent);

    if (event->type() == eventNames().keydownEvent)
        return getKeyDownCommandName(event);

    return getKeyPressCommandName(event);
}

uint64_t getInputMethodHint(HTMLInputElement* input)
{
    uint64_t hint = 0;
    if (input->isTelephoneField())
        hint |= NIXImhDialableCharactersOnly;
    else if (input->isNumberField())
        hint |= NIXImhDigitsOnly;
    else if (input->isEmailField()) {
        hint |= NIXImhEmailCharactersOnly;
        hint |= NIXImhNoAutoUppercase;
    } else if (input->isURLField()) {
        hint |= NIXImhUrlCharactersOnly;
        hint |= NIXImhNoAutoUppercase;
    } else if (input->isPasswordField()) {
        hint |= NIXImhNoAutoUppercase;
        hint |= NIXImhNoPredictiveText;
        hint |= NIXImhSensitiveData;
    }
    return hint;
}

} // namespace WebKit

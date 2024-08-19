/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010, 2012 Nokia Corporation and/or its subsidiary(-ies).
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
#include "NotImplemented.h"
#include "WebEventFactoryNix.h"
#include "WindowsKeyboardCodes.h"

#include <WebCore/Scrollbar.h>
#include <wtf/text/WTFString.h>

using namespace WebCore;

namespace WebKit {

static WebEvent::Type convertToWebEventType(NIXInputEventType type)
{
    switch (type) {
    case kNIXInputEventTypeMouseDown:
        return WebEvent::MouseDown;
    case kNIXInputEventTypeMouseUp:
        return WebEvent::MouseUp;
    case kNIXInputEventTypeMouseMove:
        return WebEvent::MouseMove;
    case kNIXInputEventTypeWheel:
        return WebEvent::Wheel;
    case kNIXInputEventTypeKeyDown:
        return WebEvent::KeyDown;
    case kNIXInputEventTypeKeyUp:
        return WebEvent::KeyUp;
    case kNIXInputEventTypeTouchStart:
        return WebEvent::TouchStart;
    case kNIXInputEventTypeTouchMove:
        return WebEvent::TouchMove;
    case kNIXInputEventTypeTouchEnd:
        return WebEvent::TouchEnd;
    case kNIXInputEventTypeTouchCancel:
        return WebEvent::TouchCancel;
    default:
        notImplemented();
    }
    return WebEvent::MouseMove;
}

static WebEvent::Modifiers convertToWebEventModifiers(NIXInputEventModifiers modifiers)
{
    unsigned webModifiers = 0;
    if (modifiers & kNIXInputEventModifiersShiftKey)
        webModifiers |= WebEvent::ShiftKey;
    if (modifiers & kNIXInputEventModifiersControlKey)
        webModifiers |= WebEvent::ControlKey;
    if (modifiers & kNIXInputEventModifiersAltKey)
        webModifiers |= WebEvent::AltKey;
    if (modifiers & kNIXInputEventModifiersMetaKey)
        webModifiers |= WebEvent::MetaKey;
    if (modifiers & kNIXInputEventModifiersCapsLockKey)
        webModifiers |= WebEvent::CapsLockKey;
    return static_cast<WebEvent::Modifiers>(webModifiers);
}

static WebPlatformTouchPoint::TouchPointState convertToWebTouchState(NIXTouchPointState state)
{
    switch (state) {
    case kNIXTouchPointStateTouchReleased:
        return WebPlatformTouchPoint::TouchReleased;
    case kNIXTouchPointStateTouchPressed:
        return WebPlatformTouchPoint::TouchPressed;
    case kNIXTouchPointStateTouchMoved:
        return WebPlatformTouchPoint::TouchMoved;
    case kNIXTouchPointStateTouchStationary:
        return WebPlatformTouchPoint::TouchStationary;
    case kNIXTouchPointStateTouchCancelled:
        return WebPlatformTouchPoint::TouchCancelled;
    default:
        notImplemented();
    }
    return WebPlatformTouchPoint::TouchCancelled;
}

static WebMouseEvent::Button convertToWebMouseEventButton(WKEventMouseButton button)
{
    switch (button) {
    case kWKEventMouseButtonNoButton:
        return WebMouseEvent::NoButton;
    case kWKEventMouseButtonLeftButton:
        return WebMouseEvent::LeftButton;
    case kWKEventMouseButtonMiddleButton:
        return WebMouseEvent::MiddleButton;
    case kWKEventMouseButtonRightButton:
        return WebMouseEvent::RightButton;
    default:
        notImplemented();
    }
    return WebMouseEvent::NoButton;
}

static String keyIdentifierForNixKeyCode(NIXKeyEventKey keyCode)
{
    switch (keyCode) {
    case kNIXKeyEventKey_Menu:
    case kNIXKeyEventKey_Alt:
        return ASCIILiteral("Alt");
    case kNIXKeyEventKey_Clear:
        return ASCIILiteral("Clear");
    case kNIXKeyEventKey_Down:
        return ASCIILiteral("Down");
    case kNIXKeyEventKey_End:
        return ASCIILiteral("End");
    case kNIXKeyEventKey_Return:
    case kNIXKeyEventKey_Enter:
        return ASCIILiteral("Enter");
    case kNIXKeyEventKey_Execute:
        return ASCIILiteral("Execute");
    case kNIXKeyEventKey_F1:
        return ASCIILiteral("F1");
    case kNIXKeyEventKey_F2:
        return ASCIILiteral("F2");
    case kNIXKeyEventKey_F3:
        return ASCIILiteral("F3");
    case kNIXKeyEventKey_F4:
        return ASCIILiteral("F4");
    case kNIXKeyEventKey_F5:
        return ASCIILiteral("F5");
    case kNIXKeyEventKey_F6:
        return ASCIILiteral("F6");
    case kNIXKeyEventKey_F7:
        return ASCIILiteral("F7");
    case kNIXKeyEventKey_F8:
        return ASCIILiteral("F8");
    case kNIXKeyEventKey_F9:
        return ASCIILiteral("F9");
    case kNIXKeyEventKey_F10:
        return ASCIILiteral("F10");
    case kNIXKeyEventKey_F11:
        return ASCIILiteral("F11");
    case kNIXKeyEventKey_F12:
        return ASCIILiteral("F12");
    case kNIXKeyEventKey_F13:
        return ASCIILiteral("F13");
    case kNIXKeyEventKey_F14:
        return ASCIILiteral("F14");
    case kNIXKeyEventKey_F15:
        return ASCIILiteral("F15");
    case kNIXKeyEventKey_F16:
        return ASCIILiteral("F16");
    case kNIXKeyEventKey_F17:
        return ASCIILiteral("F17");
    case kNIXKeyEventKey_F18:
        return ASCIILiteral("F18");
    case kNIXKeyEventKey_F19:
        return ASCIILiteral("F19");
    case kNIXKeyEventKey_F20:
        return ASCIILiteral("F20");
    case kNIXKeyEventKey_F21:
        return ASCIILiteral("F21");
    case kNIXKeyEventKey_F22:
        return ASCIILiteral("F22");
    case kNIXKeyEventKey_F23:
        return ASCIILiteral("F23");
    case kNIXKeyEventKey_F24:
        return ASCIILiteral("F24");
    case kNIXKeyEventKey_Help:
        return ASCIILiteral("Help");
    case kNIXKeyEventKey_Home:
        return ASCIILiteral("Home");
    case kNIXKeyEventKey_Insert:
        return ASCIILiteral("Insert");
    case kNIXKeyEventKey_Left:
        return ASCIILiteral("Left");
    case kNIXKeyEventKey_PageDown:
        return ASCIILiteral("PageDown");
    case kNIXKeyEventKey_PageUp:
        return ASCIILiteral("PageUp");
    case kNIXKeyEventKey_Pause:
        return ASCIILiteral("Pause");
    case kNIXKeyEventKey_Print:
        return ASCIILiteral("PrintScreen");
    case kNIXKeyEventKey_Right:
        return ASCIILiteral("Right");
    case kNIXKeyEventKey_Select:
        return ASCIILiteral("Select");
    case kNIXKeyEventKey_Up:
        return ASCIILiteral("Up");
    case kNIXKeyEventKey_Delete:
        return ASCIILiteral("U+007F");
    case kNIXKeyEventKey_Backspace:
        return ASCIILiteral("U+0008");
    case kNIXKeyEventKey_Tab:
        return ASCIILiteral("U+0009");
    case kNIXKeyEventKey_Backtab:
        return ASCIILiteral("U+0009");
    default:
        return String::format("U+%04X", toASCIIUpper((int) keyCode));
    }
}

static int windowsKeyCodeForKeyEvent(NIXKeyEventKey keycode, bool isKeypad)
{
    if (isKeypad) {
        switch (keycode) {
        case kNIXKeyEventKey_0:
            return VK_NUMPAD0;
        case kNIXKeyEventKey_1:
            return VK_NUMPAD1;
        case kNIXKeyEventKey_2:
            return VK_NUMPAD2;
        case kNIXKeyEventKey_3:
            return VK_NUMPAD3;
        case kNIXKeyEventKey_4:
            return VK_NUMPAD4;
        case kNIXKeyEventKey_5:
            return VK_NUMPAD5;
        case kNIXKeyEventKey_6:
            return VK_NUMPAD6;
        case kNIXKeyEventKey_7:
            return VK_NUMPAD7;
        case kNIXKeyEventKey_8:
            return VK_NUMPAD8;
        case kNIXKeyEventKey_9:
            return VK_NUMPAD9;
        case kNIXKeyEventKey_Asterisk:
            return VK_MULTIPLY;
        case kNIXKeyEventKey_Plus:
            return VK_ADD;
        case kNIXKeyEventKey_Minus:
            return VK_SUBTRACT;
        case kNIXKeyEventKey_Period:
            return VK_DECIMAL;
        case kNIXKeyEventKey_Slash:
            return VK_DIVIDE;
        case kNIXKeyEventKey_PageUp:
            return VK_PRIOR;
        case kNIXKeyEventKey_PageDown:
            return VK_NEXT;
        case kNIXKeyEventKey_End:
            return VK_END;
        case kNIXKeyEventKey_Home:
            return VK_HOME;
        case kNIXKeyEventKey_Left:
            return VK_LEFT;
        case kNIXKeyEventKey_Up:
            return VK_UP;
        case kNIXKeyEventKey_Right:
            return VK_RIGHT;
        case kNIXKeyEventKey_Down:
            return VK_DOWN;
        case kNIXKeyEventKey_Enter:
        case kNIXKeyEventKey_Return:
            return VK_RETURN;
        case kNIXKeyEventKey_Insert:
            return VK_INSERT;
        case kNIXKeyEventKey_Delete:
            return VK_DELETE;
        default:
            return 0;
        }

    } else {
        switch (keycode) {
        case kNIXKeyEventKey_Backspace:
            return VK_BACK;
        case kNIXKeyEventKey_Backtab:
        case kNIXKeyEventKey_Tab:
            return VK_TAB;
        case kNIXKeyEventKey_Clear:
            return VK_CLEAR;
        case kNIXKeyEventKey_Enter:
        case kNIXKeyEventKey_Return:
            return VK_RETURN;
        case kNIXKeyEventKey_Shift:
            return VK_SHIFT;
        case kNIXKeyEventKey_Control:
            return VK_CONTROL;
        case kNIXKeyEventKey_Menu:
        case kNIXKeyEventKey_Alt:
            return VK_MENU;
        case kNIXKeyEventKey_F1:
            return VK_F1;
        case kNIXKeyEventKey_F2:
            return VK_F2;
        case kNIXKeyEventKey_F3:
            return VK_F3;
        case kNIXKeyEventKey_F4:
            return VK_F4;
        case kNIXKeyEventKey_F5:
            return VK_F5;
        case kNIXKeyEventKey_F6:
            return VK_F6;
        case kNIXKeyEventKey_F7:
            return VK_F7;
        case kNIXKeyEventKey_F8:
            return VK_F8;
        case kNIXKeyEventKey_F9:
            return VK_F9;
        case kNIXKeyEventKey_F10:
            return VK_F10;
        case kNIXKeyEventKey_F11:
            return VK_F11;
        case kNIXKeyEventKey_F12:
            return VK_F12;
        case kNIXKeyEventKey_F13:
            return VK_F13;
        case kNIXKeyEventKey_F14:
            return VK_F14;
        case kNIXKeyEventKey_F15:
            return VK_F15;
        case kNIXKeyEventKey_F16:
            return VK_F16;
        case kNIXKeyEventKey_F17:
            return VK_F17;
        case kNIXKeyEventKey_F18:
            return VK_F18;
        case kNIXKeyEventKey_F19:
            return VK_F19;
        case kNIXKeyEventKey_F20:
            return VK_F20;
        case kNIXKeyEventKey_F21:
            return VK_F21;
        case kNIXKeyEventKey_F22:
            return VK_F22;
        case kNIXKeyEventKey_F23:
            return VK_F23;
        case kNIXKeyEventKey_F24:
            return VK_F24;
        case kNIXKeyEventKey_Pause:
            return VK_PAUSE;
        case kNIXKeyEventKey_CapsLock:
            return VK_CAPITAL;
        case kNIXKeyEventKey_Kana_Lock:
        case kNIXKeyEventKey_Kana_Shift:
            return VK_KANA;
        case kNIXKeyEventKey_Hangul:
            return VK_HANGUL;
        case kNIXKeyEventKey_Hangul_Hanja:
            return VK_HANJA;
        case kNIXKeyEventKey_Kanji:
            return VK_KANJI;
        case kNIXKeyEventKey_Escape:
            return VK_ESCAPE;
        case kNIXKeyEventKey_Space:
            return VK_SPACE;
        case kNIXKeyEventKey_PageUp:
            return VK_PRIOR;
        case kNIXKeyEventKey_PageDown:
            return VK_NEXT;
        case kNIXKeyEventKey_End:
            return VK_END;
        case kNIXKeyEventKey_Home:
            return VK_HOME;
        case kNIXKeyEventKey_Left:
            return VK_LEFT;
        case kNIXKeyEventKey_Up:
            return VK_UP;
        case kNIXKeyEventKey_Right:
            return VK_RIGHT;
        case kNIXKeyEventKey_Down:
            return VK_DOWN;
        case kNIXKeyEventKey_Select:
            return VK_SELECT;
        case kNIXKeyEventKey_Print:
            return VK_SNAPSHOT;
        case kNIXKeyEventKey_Execute:
            return VK_EXECUTE;
        case kNIXKeyEventKey_Insert:
            return VK_INSERT;
        case kNIXKeyEventKey_Delete:
            return VK_DELETE;
        case kNIXKeyEventKey_Help:
            return VK_HELP;
        case kNIXKeyEventKey_0:
        case kNIXKeyEventKey_ParenLeft:
            return VK_0;
        case kNIXKeyEventKey_1:
            return VK_1;
        case kNIXKeyEventKey_2:
        case kNIXKeyEventKey_At:
            return VK_2;
        case kNIXKeyEventKey_3:
        case kNIXKeyEventKey_NumberSign:
            return VK_3;
        case kNIXKeyEventKey_4:
        case kNIXKeyEventKey_Dollar:
            return VK_4;
        case kNIXKeyEventKey_5:
        case kNIXKeyEventKey_Percent:
            return VK_5;
        case kNIXKeyEventKey_6:
        case kNIXKeyEventKey_AsciiCircum:
            return VK_6;
        case kNIXKeyEventKey_7:
        case kNIXKeyEventKey_Ampersand:
            return VK_7;
        case kNIXKeyEventKey_8:
        case kNIXKeyEventKey_Asterisk:
            return VK_8;
        case kNIXKeyEventKey_9:
        case kNIXKeyEventKey_ParenRight:
            return VK_9;
        case kNIXKeyEventKey_A:
            return VK_A;
        case kNIXKeyEventKey_B:
            return VK_B;
        case kNIXKeyEventKey_C:
            return VK_C;
        case kNIXKeyEventKey_D:
            return VK_D;
        case kNIXKeyEventKey_E:
            return VK_E;
        case kNIXKeyEventKey_F:
            return VK_F;
        case kNIXKeyEventKey_G:
            return VK_G;
        case kNIXKeyEventKey_H:
            return VK_H;
        case kNIXKeyEventKey_I:
            return VK_I;
        case kNIXKeyEventKey_J:
            return VK_J;
        case kNIXKeyEventKey_K:
            return VK_K;
        case kNIXKeyEventKey_L:
            return VK_L;
        case kNIXKeyEventKey_M:
            return VK_M;
        case kNIXKeyEventKey_N:
            return VK_N;
        case kNIXKeyEventKey_O:
            return VK_O;
        case kNIXKeyEventKey_P:
            return VK_P;
        case kNIXKeyEventKey_Q:
            return VK_Q;
        case kNIXKeyEventKey_R:
            return VK_R;
        case kNIXKeyEventKey_S:
            return VK_S;
        case kNIXKeyEventKey_T:
            return VK_T;
        case kNIXKeyEventKey_U:
            return VK_U;
        case kNIXKeyEventKey_V:
            return VK_V;
        case kNIXKeyEventKey_W:
            return VK_W;
        case kNIXKeyEventKey_X:
            return VK_X;
        case kNIXKeyEventKey_Y:
            return VK_Y;
        case kNIXKeyEventKey_Z:
            return VK_Z;
        case kNIXKeyEventKey_Meta:
            return VK_LWIN;
        case kNIXKeyEventKey_NumLock:
            return VK_NUMLOCK;
        case kNIXKeyEventKey_ScrollLock:
            return VK_SCROLL;
        case kNIXKeyEventKey_Semicolon:
        case kNIXKeyEventKey_Colon:
            return VK_OEM_1;
        case kNIXKeyEventKey_Plus:
        case kNIXKeyEventKey_Equal:
            return VK_OEM_PLUS;
        case kNIXKeyEventKey_Comma:
        case kNIXKeyEventKey_Less:
            return VK_OEM_COMMA;
        case kNIXKeyEventKey_Minus:
        case kNIXKeyEventKey_Underscore:
            return VK_OEM_MINUS;
        case kNIXKeyEventKey_Period:
        case kNIXKeyEventKey_Greater:
            return VK_OEM_PERIOD;
        case kNIXKeyEventKey_Slash:
        case kNIXKeyEventKey_Question:
            return VK_OEM_2;
        case kNIXKeyEventKey_AsciiTilde:
        case kNIXKeyEventKey_QuoteLeft:
            return VK_OEM_3;
        case kNIXKeyEventKey_BracketLeft:
        case kNIXKeyEventKey_BraceLeft:
            return VK_OEM_4;
        case kNIXKeyEventKey_Backslash:
        case kNIXKeyEventKey_Bar:
            return VK_OEM_5;
        case kNIXKeyEventKey_BracketRight:
        case kNIXKeyEventKey_BraceRight:
            return VK_OEM_6;
        case kNIXKeyEventKey_QuoteDbl:
            return VK_OEM_7;
        default:
            return 0;
        }
    }
}

static String keyTextForNixKeyEvent(const NIXKeyEvent& event)
{
    if (event.text)
        return String::fromUTF8(event.text);

    int keycode = static_cast<int>(event.key);
    if (isASCIIPrintable(keycode))
        return String::format("%c", event.shouldUseUpperCase ? toASCIIUpper(keycode) : toASCIILower(keycode));

    switch (event.key) {
    case kNIXKeyEventKey_Tab:
    case kNIXKeyEventKey_Backtab:
        return "\t";
    case kNIXKeyEventKey_Enter:
    case kNIXKeyEventKey_Return:
        return "\r";
    default:
        break;
    }

    return "";
}

WebMouseEvent WebEventFactory::createWebMouseEvent(const NIXMouseEvent& event, WebCore::IntPoint* lastCursorPosition)
{
    WebEvent::Type type = convertToWebEventType(event.type);
    WebMouseEvent::Button button = convertToWebMouseEventButton(event.button);

    float deltaX = event.x - lastCursorPosition->x();
    float deltaY = event.y - lastCursorPosition->y();
    int clickCount = event.clickCount;
    WebEvent::Modifiers modifiers = convertToWebEventModifiers(event.modifiers);
    double timestamp = event.timestamp;
    IntPoint globalPosition = IntPoint(event.globalX, event.globalY);
    lastCursorPosition->setX(event.x);
    lastCursorPosition->setY(event.y);

    return WebMouseEvent(type, button, WebCore::IntPoint(event.x, event.y), globalPosition, deltaX, deltaY, 0.0f, clickCount, modifiers, timestamp);
}

WebWheelEvent WebEventFactory::createWebWheelEvent(const NIXWheelEvent& event)
{
    WebEvent::Type type = convertToWebEventType(event.type);

    IntPoint position = IntPoint(event.x, event.y);
    IntPoint globalPosition = IntPoint(event.globalX, event.globalY);
    FloatSize delta = event.orientation == kNIXWheelEventOrientationVertical ? FloatSize(0, event.delta) : FloatSize(event.delta, 0);
    WebEvent::Modifiers modifiers = convertToWebEventModifiers(event.modifiers);
    double timestamp = event.timestamp;

    const float ticks = event.delta / float(Scrollbar::pixelsPerLineStep());
    FloatSize wheelTicks = event.orientation == kNIXWheelEventOrientationVertical ? FloatSize(0, ticks) : FloatSize(ticks, 0);

    return WebWheelEvent(type, position, globalPosition, delta, wheelTicks, WebWheelEvent::ScrollByPixelWheelEvent, modifiers, timestamp);
}

WebKeyboardEvent WebEventFactory::createWebKeyboardEvent(const NIXKeyEvent& event)
{
    WebEvent::Type type = convertToWebEventType(event.type);
    const WTF::String text = keyTextForNixKeyEvent(event);
    const WTF::String unmodifiedText = text;
    bool isAutoRepeat = false;
    bool isSystemKey = false;
    bool isKeypad = event.isKeypad;
    const WTF::String keyIdentifier = keyIdentifierForNixKeyCode(event.key);
    int windowsVirtualKeyCode = windowsKeyCodeForKeyEvent(event.key, isKeypad);
    int nativeVirtualKeyCode = 0;
    int macCharCode = 0;
    WebEvent::Modifiers modifiers = convertToWebEventModifiers(event.modifiers);
    double timestamp = event.timestamp;

    return WebKeyboardEvent(type, text, unmodifiedText, keyIdentifier, windowsVirtualKeyCode, nativeVirtualKeyCode, macCharCode, isAutoRepeat, isKeypad, isSystemKey, modifiers, timestamp);
}

#if ENABLE(TOUCH_EVENTS)
WebTouchEvent WebEventFactory::createWebTouchEvent(const NIXTouchEvent& event)
{
    WebEvent::Type type = convertToWebEventType(event.type);
    Vector<WebPlatformTouchPoint> touchPoints;
    WebEvent::Modifiers modifiers = convertToWebEventModifiers(event.modifiers);
    double timestamp = event.timestamp;

    for (unsigned i = 0; i < event.numTouchPoints; ++i) {
        const NIXTouchPoint& touch = event.touchPoints[i];
        uint32_t id = static_cast<uint32_t>(touch.id);
        WebPlatformTouchPoint::TouchPointState state = convertToWebTouchState(touch.state);
        IntPoint position = IntPoint(touch.x, touch.y);
        IntPoint globalPosition = IntPoint(touch.globalX, touch.globalY);
        IntSize radius = IntSize(touch.horizontalRadius, touch.verticalRadius);
        float rotationAngle = touch.rotationAngle;
        float force = touch.pressure;

        WebPlatformTouchPoint webTouchPoint = WebPlatformTouchPoint(id, state, globalPosition, position, radius, rotationAngle, force);
        touchPoints.append(webTouchPoint);
    }

    return WebTouchEvent(type, touchPoints, modifiers, timestamp);
}
#endif
} // namespace WebKit

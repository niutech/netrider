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

#ifndef NIXEvents_h
#define NIXEvents_h

#include <WebKit2/WKBase.h>
#include <WebKit2/WKEvent.h>

#ifdef __cplusplus
extern "C" {
#endif

// On Nix we use the same key mapping as Qt
enum NIXKeyEventKey {
    kNIXKeyEventKey_Escape = 0x01000000,                // misc keys
    kNIXKeyEventKey_Tab = 0x01000001,
    kNIXKeyEventKey_Backtab = 0x01000002,
    kNIXKeyEventKey_Backspace = 0x01000003,
    kNIXKeyEventKey_Return = 0x01000004,
    kNIXKeyEventKey_Enter = 0x01000005,
    kNIXKeyEventKey_Insert = 0x01000006,
    kNIXKeyEventKey_Delete = 0x01000007,
    kNIXKeyEventKey_Pause = 0x01000008,
    kNIXKeyEventKey_Print = 0x01000009,
    kNIXKeyEventKey_SysReq = 0x0100000a,
    kNIXKeyEventKey_Clear = 0x0100000b,
    kNIXKeyEventKey_Home = 0x01000010,                // cursor movement
    kNIXKeyEventKey_End = 0x01000011,
    kNIXKeyEventKey_Left = 0x01000012,
    kNIXKeyEventKey_Up = 0x01000013,
    kNIXKeyEventKey_Right = 0x01000014,
    kNIXKeyEventKey_Down = 0x01000015,
    kNIXKeyEventKey_PageUp = 0x01000016,
    kNIXKeyEventKey_PageDown = 0x01000017,
    kNIXKeyEventKey_Shift = 0x01000020,                // modifiers
    kNIXKeyEventKey_Control = 0x01000021,
    kNIXKeyEventKey_Meta = 0x01000022,
    kNIXKeyEventKey_Alt = 0x01000023,
    kNIXKeyEventKey_CapsLock = 0x01000024,
    kNIXKeyEventKey_NumLock = 0x01000025,
    kNIXKeyEventKey_ScrollLock = 0x01000026,
    kNIXKeyEventKey_F1 = 0x01000030,                // function keys
    kNIXKeyEventKey_F2 = 0x01000031,
    kNIXKeyEventKey_F3 = 0x01000032,
    kNIXKeyEventKey_F4 = 0x01000033,
    kNIXKeyEventKey_F5 = 0x01000034,
    kNIXKeyEventKey_F6 = 0x01000035,
    kNIXKeyEventKey_F7 = 0x01000036,
    kNIXKeyEventKey_F8 = 0x01000037,
    kNIXKeyEventKey_F9 = 0x01000038,
    kNIXKeyEventKey_F10 = 0x01000039,
    kNIXKeyEventKey_F11 = 0x0100003a,
    kNIXKeyEventKey_F12 = 0x0100003b,
    kNIXKeyEventKey_F13 = 0x0100003c,
    kNIXKeyEventKey_F14 = 0x0100003d,
    kNIXKeyEventKey_F15 = 0x0100003e,
    kNIXKeyEventKey_F16 = 0x0100003f,
    kNIXKeyEventKey_F17 = 0x01000040,
    kNIXKeyEventKey_F18 = 0x01000041,
    kNIXKeyEventKey_F19 = 0x01000042,
    kNIXKeyEventKey_F20 = 0x01000043,
    kNIXKeyEventKey_F21 = 0x01000044,
    kNIXKeyEventKey_F22 = 0x01000045,
    kNIXKeyEventKey_F23 = 0x01000046,
    kNIXKeyEventKey_F24 = 0x01000047,
    kNIXKeyEventKey_F25 = 0x01000048,                // F25 .. F35 only on X11
    kNIXKeyEventKey_F26 = 0x01000049,
    kNIXKeyEventKey_F27 = 0x0100004a,
    kNIXKeyEventKey_F28 = 0x0100004b,
    kNIXKeyEventKey_F29 = 0x0100004c,
    kNIXKeyEventKey_F30 = 0x0100004d,
    kNIXKeyEventKey_F31 = 0x0100004e,
    kNIXKeyEventKey_F32 = 0x0100004f,
    kNIXKeyEventKey_F33 = 0x01000050,
    kNIXKeyEventKey_F34 = 0x01000051,
    kNIXKeyEventKey_F35 = 0x01000052,
    kNIXKeyEventKey_Super_L = 0x01000053,                 // extra keys
    kNIXKeyEventKey_Super_R = 0x01000054,
    kNIXKeyEventKey_Menu = 0x01000055,
    kNIXKeyEventKey_Hyper_L = 0x01000056,
    kNIXKeyEventKey_Hyper_R = 0x01000057,
    kNIXKeyEventKey_Help = 0x01000058,
    kNIXKeyEventKey_Direction_L = 0x01000059,
    kNIXKeyEventKey_Direction_R = 0x01000060,
    kNIXKeyEventKey_Space = 0x20,                // 7 bit printable ASCII
    kNIXKeyEventKey_Any = kNIXKeyEventKey_Space,
    kNIXKeyEventKey_Exclam = 0x21,
    kNIXKeyEventKey_QuoteDbl = 0x22,
    kNIXKeyEventKey_NumberSign = 0x23,
    kNIXKeyEventKey_Dollar = 0x24,
    kNIXKeyEventKey_Percent = 0x25,
    kNIXKeyEventKey_Ampersand = 0x26,
    kNIXKeyEventKey_Apostrophe = 0x27,
    kNIXKeyEventKey_ParenLeft = 0x28,
    kNIXKeyEventKey_ParenRight = 0x29,
    kNIXKeyEventKey_Asterisk = 0x2a,
    kNIXKeyEventKey_Plus = 0x2b,
    kNIXKeyEventKey_Comma = 0x2c,
    kNIXKeyEventKey_Minus = 0x2d,
    kNIXKeyEventKey_Period = 0x2e,
    kNIXKeyEventKey_Slash = 0x2f,
    kNIXKeyEventKey_0 = 0x30,
    kNIXKeyEventKey_1 = 0x31,
    kNIXKeyEventKey_2 = 0x32,
    kNIXKeyEventKey_3 = 0x33,
    kNIXKeyEventKey_4 = 0x34,
    kNIXKeyEventKey_5 = 0x35,
    kNIXKeyEventKey_6 = 0x36,
    kNIXKeyEventKey_7 = 0x37,
    kNIXKeyEventKey_8 = 0x38,
    kNIXKeyEventKey_9 = 0x39,
    kNIXKeyEventKey_Colon = 0x3a,
    kNIXKeyEventKey_Semicolon = 0x3b,
    kNIXKeyEventKey_Less = 0x3c,
    kNIXKeyEventKey_Equal = 0x3d,
    kNIXKeyEventKey_Greater = 0x3e,
    kNIXKeyEventKey_Question = 0x3f,
    kNIXKeyEventKey_At = 0x40,
    kNIXKeyEventKey_A = 0x41,
    kNIXKeyEventKey_B = 0x42,
    kNIXKeyEventKey_C = 0x43,
    kNIXKeyEventKey_D = 0x44,
    kNIXKeyEventKey_E = 0x45,
    kNIXKeyEventKey_F = 0x46,
    kNIXKeyEventKey_G = 0x47,
    kNIXKeyEventKey_H = 0x48,
    kNIXKeyEventKey_I = 0x49,
    kNIXKeyEventKey_J = 0x4a,
    kNIXKeyEventKey_K = 0x4b,
    kNIXKeyEventKey_L = 0x4c,
    kNIXKeyEventKey_M = 0x4d,
    kNIXKeyEventKey_N = 0x4e,
    kNIXKeyEventKey_O = 0x4f,
    kNIXKeyEventKey_P = 0x50,
    kNIXKeyEventKey_Q = 0x51,
    kNIXKeyEventKey_R = 0x52,
    kNIXKeyEventKey_S = 0x53,
    kNIXKeyEventKey_T = 0x54,
    kNIXKeyEventKey_U = 0x55,
    kNIXKeyEventKey_V = 0x56,
    kNIXKeyEventKey_W = 0x57,
    kNIXKeyEventKey_X = 0x58,
    kNIXKeyEventKey_Y = 0x59,
    kNIXKeyEventKey_Z = 0x5a,
    kNIXKeyEventKey_BracketLeft = 0x5b,
    kNIXKeyEventKey_Backslash = 0x5c,
    kNIXKeyEventKey_BracketRight = 0x5d,
    kNIXKeyEventKey_AsciiCircum = 0x5e,
    kNIXKeyEventKey_Underscore = 0x5f,
    kNIXKeyEventKey_QuoteLeft = 0x60,
    kNIXKeyEventKey_BraceLeft = 0x7b,
    kNIXKeyEventKey_Bar = 0x7c,
    kNIXKeyEventKey_BraceRight = 0x7d,
    kNIXKeyEventKey_AsciiTilde = 0x7e,

    kNIXKeyEventKey_nobreakspace = 0x0a0,
    kNIXKeyEventKey_exclamdown = 0x0a1,
    kNIXKeyEventKey_cent = 0x0a2,
    kNIXKeyEventKey_sterling = 0x0a3,
    kNIXKeyEventKey_currency = 0x0a4,
    kNIXKeyEventKey_yen = 0x0a5,
    kNIXKeyEventKey_brokenbar = 0x0a6,
    kNIXKeyEventKey_section = 0x0a7,
    kNIXKeyEventKey_diaeresis = 0x0a8,
    kNIXKeyEventKey_copyright = 0x0a9,
    kNIXKeyEventKey_ordfeminine = 0x0aa,
    kNIXKeyEventKey_guillemotleft = 0x0ab,        // left angle quotation mark
    kNIXKeyEventKey_notsign = 0x0ac,
    kNIXKeyEventKey_hyphen = 0x0ad,
    kNIXKeyEventKey_registered = 0x0ae,
    kNIXKeyEventKey_macron = 0x0af,
    kNIXKeyEventKey_degree = 0x0b0,
    kNIXKeyEventKey_plusminus = 0x0b1,
    kNIXKeyEventKey_twosuperior = 0x0b2,
    kNIXKeyEventKey_threesuperior = 0x0b3,
    kNIXKeyEventKey_acute = 0x0b4,
    kNIXKeyEventKey_mu = 0x0b5,
    kNIXKeyEventKey_paragraph = 0x0b6,
    kNIXKeyEventKey_periodcentered = 0x0b7,
    kNIXKeyEventKey_cedilla = 0x0b8,
    kNIXKeyEventKey_onesuperior = 0x0b9,
    kNIXKeyEventKey_masculine = 0x0ba,
    kNIXKeyEventKey_guillemotright = 0x0bb,        // right angle quotation mark
    kNIXKeyEventKey_onequarter = 0x0bc,
    kNIXKeyEventKey_onehalf = 0x0bd,
    kNIXKeyEventKey_threequarters = 0x0be,
    kNIXKeyEventKey_questiondown = 0x0bf,
    kNIXKeyEventKey_Agrave = 0x0c0,
    kNIXKeyEventKey_Aacute = 0x0c1,
    kNIXKeyEventKey_Acircumflex = 0x0c2,
    kNIXKeyEventKey_Atilde = 0x0c3,
    kNIXKeyEventKey_Adiaeresis = 0x0c4,
    kNIXKeyEventKey_Aring = 0x0c5,
    kNIXKeyEventKey_AE = 0x0c6,
    kNIXKeyEventKey_Ccedilla = 0x0c7,
    kNIXKeyEventKey_Egrave = 0x0c8,
    kNIXKeyEventKey_Eacute = 0x0c9,
    kNIXKeyEventKey_Ecircumflex = 0x0ca,
    kNIXKeyEventKey_Ediaeresis = 0x0cb,
    kNIXKeyEventKey_Igrave = 0x0cc,
    kNIXKeyEventKey_Iacute = 0x0cd,
    kNIXKeyEventKey_Icircumflex = 0x0ce,
    kNIXKeyEventKey_Idiaeresis = 0x0cf,
    kNIXKeyEventKey_ETH = 0x0d0,
    kNIXKeyEventKey_Ntilde = 0x0d1,
    kNIXKeyEventKey_Ograve = 0x0d2,
    kNIXKeyEventKey_Oacute = 0x0d3,
    kNIXKeyEventKey_Ocircumflex = 0x0d4,
    kNIXKeyEventKey_Otilde = 0x0d5,
    kNIXKeyEventKey_Odiaeresis = 0x0d6,
    kNIXKeyEventKey_multiply = 0x0d7,
    kNIXKeyEventKey_Ooblique = 0x0d8,
    kNIXKeyEventKey_Ugrave = 0x0d9,
    kNIXKeyEventKey_Uacute = 0x0da,
    kNIXKeyEventKey_Ucircumflex = 0x0db,
    kNIXKeyEventKey_Udiaeresis = 0x0dc,
    kNIXKeyEventKey_Yacute = 0x0dd,
    kNIXKeyEventKey_THORN = 0x0de,
    kNIXKeyEventKey_ssharp = 0x0df,
    kNIXKeyEventKey_division = 0x0f7,
    kNIXKeyEventKey_ydiaeresis = 0x0ff,

    // International & multi-key character composition
    kNIXKeyEventKey_AltGr               = 0x01001103,
    kNIXKeyEventKey_Multi_key           = 0x01001120,  // Multi-key character compose
    kNIXKeyEventKey_Codeinput           = 0x01001137,
    kNIXKeyEventKey_SingleCandidate     = 0x0100113c,
    kNIXKeyEventKey_MultipleCandidate   = 0x0100113d,
    kNIXKeyEventKey_PreviousCandidate   = 0x0100113e,

    // Misc Functions
    kNIXKeyEventKey_Mode_switch         = 0x0100117e,  // Character set switch

    // Japanese keyboard support
    kNIXKeyEventKey_Kanji               = 0x01001121,  // Kanji, Kanji convert
    kNIXKeyEventKey_Muhenkan            = 0x01001122,  // Cancel Conversion
    kNIXKeyEventKey_Henkan              = 0x01001123,  // Alias for Henkan_Mode
    kNIXKeyEventKey_Romaji              = 0x01001124,  // to Romaji
    kNIXKeyEventKey_Hiragana            = 0x01001125,  // to Hiragana
    kNIXKeyEventKey_Katakana            = 0x01001126,  // to Katakana
    kNIXKeyEventKey_Hiragana_Katakana   = 0x01001127,  // Hiragana/Katakana toggle
    kNIXKeyEventKey_Zenkaku             = 0x01001128,  // to Zenkaku
    kNIXKeyEventKey_Hankaku             = 0x01001129,  // to Hankaku
    kNIXKeyEventKey_Zenkaku_Hankaku     = 0x0100112a,  // Zenkaku/Hankaku toggle
    kNIXKeyEventKey_Touroku             = 0x0100112b,  // Add to Dictionary
    kNIXKeyEventKey_Massyo              = 0x0100112c,  // Delete from Dictionary
    kNIXKeyEventKey_Kana_Lock           = 0x0100112d,  // Kana Lock
    kNIXKeyEventKey_Kana_Shift          = 0x0100112e,  // Kana Shift
    kNIXKeyEventKey_Eisu_Shift          = 0x0100112f,  // Alphanumeric Shift
    kNIXKeyEventKey_Eisu_toggle         = 0x01001130,  // Alphanumeric toggle

    // Korean keyboard support
    //
    // In fact, many Korean users need only 2 keys, kNIXKeyEventKey_Hangul and
    // kNIXKeyEventKey_Hangul_Hanja. But rest of the keys are good for future.
    kNIXKeyEventKey_Hangul              = 0x01001131,  // Hangul start/stop(toggle)
    kNIXKeyEventKey_Hangul_Start        = 0x01001132,  // Hangul start
    kNIXKeyEventKey_Hangul_End          = 0x01001133,  // Hangul end, English start
    kNIXKeyEventKey_Hangul_Hanja        = 0x01001134,  // Start Hangul->Hanja Conversion
    kNIXKeyEventKey_Hangul_Jamo         = 0x01001135,  // Hangul Jamo mode
    kNIXKeyEventKey_Hangul_Romaja       = 0x01001136,  // Hangul Romaja mode
    kNIXKeyEventKey_Hangul_Jeonja       = 0x01001138,  // Jeonja mode
    kNIXKeyEventKey_Hangul_Banja        = 0x01001139,  // Banja mode
    kNIXKeyEventKey_Hangul_PreHanja     = 0x0100113a,  // Pre Hanja conversion
    kNIXKeyEventKey_Hangul_PostHanja    = 0x0100113b,  // Post Hanja conversion
    kNIXKeyEventKey_Hangul_Special      = 0x0100113f,  // Special symbols

    // dead keys (X keycode - 0xED00 to avoid the conflict)
    kNIXKeyEventKey_Dead_Grave          = 0x01001250,
    kNIXKeyEventKey_Dead_Acute          = 0x01001251,
    kNIXKeyEventKey_Dead_Circumflex     = 0x01001252,
    kNIXKeyEventKey_Dead_Tilde          = 0x01001253,
    kNIXKeyEventKey_Dead_Macron         = 0x01001254,
    kNIXKeyEventKey_Dead_Breve          = 0x01001255,
    kNIXKeyEventKey_Dead_Abovedot       = 0x01001256,
    kNIXKeyEventKey_Dead_Diaeresis      = 0x01001257,
    kNIXKeyEventKey_Dead_Abovering      = 0x01001258,
    kNIXKeyEventKey_Dead_Doubleacute    = 0x01001259,
    kNIXKeyEventKey_Dead_Caron          = 0x0100125a,
    kNIXKeyEventKey_Dead_Cedilla        = 0x0100125b,
    kNIXKeyEventKey_Dead_Ogonek         = 0x0100125c,
    kNIXKeyEventKey_Dead_Iota           = 0x0100125d,
    kNIXKeyEventKey_Dead_Voiced_Sound   = 0x0100125e,
    kNIXKeyEventKey_Dead_Semivoiced_Sound = 0x0100125f,
    kNIXKeyEventKey_Dead_Belowdot       = 0x01001260,
    kNIXKeyEventKey_Dead_Hook           = 0x01001261,
    kNIXKeyEventKey_Dead_Horn           = 0x01001262,

    // multimedia/internet keys - ignored by default - see QKeyEvent c'tor
    kNIXKeyEventKey_Back  = 0x01000061,
    kNIXKeyEventKey_Forward  = 0x01000062,
    kNIXKeyEventKey_Stop  = 0x01000063,
    kNIXKeyEventKey_Refresh  = 0x01000064,
    kNIXKeyEventKey_VolumeDown = 0x01000070,
    kNIXKeyEventKey_VolumeMute  = 0x01000071,
    kNIXKeyEventKey_VolumeUp = 0x01000072,
    kNIXKeyEventKey_BassBoost = 0x01000073,
    kNIXKeyEventKey_BassUp = 0x01000074,
    kNIXKeyEventKey_BassDown = 0x01000075,
    kNIXKeyEventKey_TrebleUp = 0x01000076,
    kNIXKeyEventKey_TrebleDown = 0x01000077,
    kNIXKeyEventKey_MediaPlay  = 0x01000080,
    kNIXKeyEventKey_MediaStop  = 0x01000081,
    kNIXKeyEventKey_MediaPrevious  = 0x01000082,
    kNIXKeyEventKey_MediaNext  = 0x01000083,
    kNIXKeyEventKey_MediaRecord = 0x01000084,
    kNIXKeyEventKey_MediaPause = 0x1000085,
    kNIXKeyEventKey_MediaTogglePlayPause = 0x1000086,
    kNIXKeyEventKey_HomePage  = 0x01000090,
    kNIXKeyEventKey_Favorites  = 0x01000091,
    kNIXKeyEventKey_Search  = 0x01000092,
    kNIXKeyEventKey_Standby = 0x01000093,
    kNIXKeyEventKey_OpenUrl = 0x01000094,
    kNIXKeyEventKey_LaunchMail  = 0x010000a0,
    kNIXKeyEventKey_LaunchMedia = 0x010000a1,
    kNIXKeyEventKey_Launch0  = 0x010000a2,
    kNIXKeyEventKey_Launch1  = 0x010000a3,
    kNIXKeyEventKey_Launch2  = 0x010000a4,
    kNIXKeyEventKey_Launch3  = 0x010000a5,
    kNIXKeyEventKey_Launch4  = 0x010000a6,
    kNIXKeyEventKey_Launch5  = 0x010000a7,
    kNIXKeyEventKey_Launch6  = 0x010000a8,
    kNIXKeyEventKey_Launch7  = 0x010000a9,
    kNIXKeyEventKey_Launch8  = 0x010000aa,
    kNIXKeyEventKey_Launch9  = 0x010000ab,
    kNIXKeyEventKey_LaunchA  = 0x010000ac,
    kNIXKeyEventKey_LaunchB  = 0x010000ad,
    kNIXKeyEventKey_LaunchC  = 0x010000ae,
    kNIXKeyEventKey_LaunchD  = 0x010000af,
    kNIXKeyEventKey_LaunchE  = 0x010000b0,
    kNIXKeyEventKey_LaunchF  = 0x010000b1,
    kNIXKeyEventKey_MonBrightnessUp = 0x010000b2,
    kNIXKeyEventKey_MonBrightnessDown = 0x010000b3,
    kNIXKeyEventKey_KeyboardLightOnOff = 0x010000b4,
    kNIXKeyEventKey_KeyboardBrightnessUp = 0x010000b5,
    kNIXKeyEventKey_KeyboardBrightnessDown = 0x010000b6,
    kNIXKeyEventKey_PowerOff = 0x010000b7,
    kNIXKeyEventKey_WakeUp = 0x010000b8,
    kNIXKeyEventKey_Eject = 0x010000b9,
    kNIXKeyEventKey_ScreenSaver = 0x010000ba,
    kNIXKeyEventKey_WWW = 0x010000bb,
    kNIXKeyEventKey_Memo = 0x010000bc,
    kNIXKeyEventKey_LightBulb = 0x010000bd,
    kNIXKeyEventKey_Shop = 0x010000be,
    kNIXKeyEventKey_History = 0x010000bf,
    kNIXKeyEventKey_AddFavorite = 0x010000c0,
    kNIXKeyEventKey_HotLinks = 0x010000c1,
    kNIXKeyEventKey_BrightnessAdjust = 0x010000c2,
    kNIXKeyEventKey_Finance = 0x010000c3,
    kNIXKeyEventKey_Community = 0x010000c4,
    kNIXKeyEventKey_AudioRewind = 0x010000c5,
    kNIXKeyEventKey_BackForward = 0x010000c6,
    kNIXKeyEventKey_ApplicationLeft = 0x010000c7,
    kNIXKeyEventKey_ApplicationRight = 0x010000c8,
    kNIXKeyEventKey_Book = 0x010000c9,
    kNIXKeyEventKey_CD = 0x010000ca,
    kNIXKeyEventKey_Calculator = 0x010000cb,
    kNIXKeyEventKey_ToDoList = 0x010000cc,
    kNIXKeyEventKey_ClearGrab = 0x010000cd,
    kNIXKeyEventKey_Close = 0x010000ce,
    kNIXKeyEventKey_Copy = 0x010000cf,
    kNIXKeyEventKey_Cut = 0x010000d0,
    kNIXKeyEventKey_Display = 0x010000d1,
    kNIXKeyEventKey_DOS = 0x010000d2,
    kNIXKeyEventKey_Documents = 0x010000d3,
    kNIXKeyEventKey_Excel = 0x010000d4,
    kNIXKeyEventKey_Explorer = 0x010000d5,
    kNIXKeyEventKey_Game = 0x010000d6,
    kNIXKeyEventKey_Go = 0x010000d7,
    kNIXKeyEventKey_iTouch = 0x010000d8,
    kNIXKeyEventKey_LogOff = 0x010000d9,
    kNIXKeyEventKey_Market = 0x010000da,
    kNIXKeyEventKey_Meeting = 0x010000db,
    kNIXKeyEventKey_MenuKB = 0x010000dc,
    kNIXKeyEventKey_MenuPB = 0x010000dd,
    kNIXKeyEventKey_MySites = 0x010000de,
    kNIXKeyEventKey_News = 0x010000df,
    kNIXKeyEventKey_OfficeHome = 0x010000e0,
    kNIXKeyEventKey_Option = 0x010000e1,
    kNIXKeyEventKey_Paste = 0x010000e2,
    kNIXKeyEventKey_Phone = 0x010000e3,
    kNIXKeyEventKey_Calendar = 0x010000e4,
    kNIXKeyEventKey_Reply = 0x010000e5,
    kNIXKeyEventKey_Reload = 0x010000e6,
    kNIXKeyEventKey_RotateWindows = 0x010000e7,
    kNIXKeyEventKey_RotationPB = 0x010000e8,
    kNIXKeyEventKey_RotationKB = 0x010000e9,
    kNIXKeyEventKey_Save = 0x010000ea,
    kNIXKeyEventKey_Send = 0x010000eb,
    kNIXKeyEventKey_Spell = 0x010000ec,
    kNIXKeyEventKey_SplitScreen = 0x010000ed,
    kNIXKeyEventKey_Support = 0x010000ee,
    kNIXKeyEventKey_TaskPane = 0x010000ef,
    kNIXKeyEventKey_Terminal = 0x010000f0,
    kNIXKeyEventKey_Tools = 0x010000f1,
    kNIXKeyEventKey_Travel = 0x010000f2,
    kNIXKeyEventKey_Video = 0x010000f3,
    kNIXKeyEventKey_Word = 0x010000f4,
    kNIXKeyEventKey_Xfer = 0x010000f5,
    kNIXKeyEventKey_ZoomIn = 0x010000f6,
    kNIXKeyEventKey_ZoomOut = 0x010000f7,
    kNIXKeyEventKey_Away = 0x010000f8,
    kNIXKeyEventKey_Messenger = 0x010000f9,
    kNIXKeyEventKey_WebCam = 0x010000fa,
    kNIXKeyEventKey_MailForward = 0x010000fb,
    kNIXKeyEventKey_Pictures = 0x010000fc,
    kNIXKeyEventKey_Music = 0x010000fd,
    kNIXKeyEventKey_Battery = 0x010000fe,
    kNIXKeyEventKey_Bluetooth = 0x010000ff,
    kNIXKeyEventKey_WLAN = 0x01000100,
    kNIXKeyEventKey_UWB = 0x01000101,
    kNIXKeyEventKey_AudioForward = 0x01000102,
    kNIXKeyEventKey_AudioRepeat = 0x01000103,
    kNIXKeyEventKey_AudioRandomPlay = 0x01000104,
    kNIXKeyEventKey_Subtitle = 0x01000105,
    kNIXKeyEventKey_AudioCycleTrack = 0x01000106,
    kNIXKeyEventKey_Time = 0x01000107,
    kNIXKeyEventKey_Hibernate = 0x01000108,
    kNIXKeyEventKey_View = 0x01000109,
    kNIXKeyEventKey_TopMenu = 0x0100010a,
    kNIXKeyEventKey_PowerDown = 0x0100010b,
    kNIXKeyEventKey_Suspend = 0x0100010c,
    kNIXKeyEventKey_ContrastAdjust = 0x0100010d,

    kNIXKeyEventKey_LaunchG  = 0x0100010e,
    kNIXKeyEventKey_LaunchH  = 0x0100010f,

    kNIXKeyEventKey_TouchpadToggle = 0x01000110,
    kNIXKeyEventKey_TouchpadOn = 0x01000111,
    kNIXKeyEventKey_TouchpadOff = 0x01000112,

    kNIXKeyEventKey_MediaLast = 0x0100ffff,

    // Keypad navigation keys
    kNIXKeyEventKey_Select = 0x01010000,
    kNIXKeyEventKey_Yes = 0x01010001,
    kNIXKeyEventKey_No = 0x01010002,

    // Newer misc keys
    kNIXKeyEventKey_Cancel  = 0x01020001,
    kNIXKeyEventKey_Printer = 0x01020002,
    kNIXKeyEventKey_Execute = 0x01020003,
    kNIXKeyEventKey_Sleep   = 0x01020004,
    kNIXKeyEventKey_Play    = 0x01020005, // Not the same as kNIXKeyEventKey_MediaPlay
    kNIXKeyEventKey_Zoom    = 0x01020006,

    // Device keys
    kNIXKeyEventKey_Context1 = 0x01100000,
    kNIXKeyEventKey_Context2 = 0x01100001,
    kNIXKeyEventKey_Context3 = 0x01100002,
    kNIXKeyEventKey_Context4 = 0x01100003,
    kNIXKeyEventKey_Call = 0x01100004,      // set absolute state to in a call (do not toggle state)
    kNIXKeyEventKey_Hangup = 0x01100005,    // set absolute state to hang up (do not toggle state)
    kNIXKeyEventKey_Flip = 0x01100006,
    kNIXKeyEventKey_ToggleCallHangup = 0x01100007, // a toggle key for answering, or hanging up, based on current call state
    kNIXKeyEventKey_VoiceDial = 0x01100008,
    kNIXKeyEventKey_LastNumberRedial = 0x01100009,

    kNIXKeyEventKey_Camera = 0x01100020,
    kNIXKeyEventKey_CameraFocus = 0x01100021,

    kNIXKeyEventKey_unknown = 0x01ffffff
};
typedef enum NIXKeyEventKey NIXKeyEventKey;

enum NIXInputEventType {
    kNIXInputEventTypeMouseDown,
    kNIXInputEventTypeMouseUp,
    kNIXInputEventTypeMouseMove,
    kNIXInputEventTypeWheel,
    kNIXInputEventTypeKeyDown,
    kNIXInputEventTypeKeyUp,
    kNIXInputEventTypeTouchStart,
    kNIXInputEventTypeTouchMove,
    kNIXInputEventTypeTouchEnd,
    kNIXInputEventTypeTouchCancel,
    kNIXInputEventTypeGestureSingleTap
};
typedef enum NIXInputEventType NIXInputEventType;

enum {
    kNIXInputEventModifiersShiftKey    = 1 << 0,
    kNIXInputEventModifiersControlKey  = 1 << 1,
    kNIXInputEventModifiersAltKey      = 1 << 2,
    kNIXInputEventModifiersMetaKey     = 1 << 3,
    kNIXInputEventModifiersCapsLockKey = 1 << 4,
    kNIXInputEventModifiersNumLockKey  = 1 << 5
};
typedef uint32_t NIXInputEventModifiers;

#define NIX_INPUT_EVENT_BASE \
    NIXInputEventType type; \
    NIXInputEventModifiers modifiers; \
    double timestamp; \
    int x; \
    int y; \
    int globalX; \
    int globalY;

struct NIXInputEvent {
    NIX_INPUT_EVENT_BASE
};
typedef struct NIXInputEvent NIXInputEvent;

struct NIXKeyEvent {
    NIX_INPUT_EVENT_BASE
    // If a symbol has both lower and upper cases available, pass upper case as "key".
    NIXKeyEventKey key;
    // Use "shouldUseUpperCase" to disambiguate whether you really want to use its upper case.
    bool shouldUseUpperCase;
    // isKeypad is needed to distinguish on WebKit some keys (e.g. KEY_9 becomes VK_NUMPAD9).
    bool isKeypad;
    // Used to map UTF-8 characters. When the character is not ASCII printable, but it is a
    // printable character, the user must fill this char* with its string representation.
    const char* text;
};
typedef struct NIXKeyEvent NIXKeyEvent;

struct NIXMouseEvent {
    NIX_INPUT_EVENT_BASE
    WKEventMouseButton button;
    int clickCount;
};
typedef struct NIXMouseEvent NIXMouseEvent;

enum NIXWheelEventOrientation {
    kNIXWheelEventOrientationVertical,
    kNIXWheelEventOrientationHorizontal
};
typedef enum NIXWheelEventOrientation NIXWheelEventOrientation;

struct NIXWheelEvent {
    NIX_INPUT_EVENT_BASE
    float delta;
    NIXWheelEventOrientation orientation;
};
typedef struct NIXWheelEvent NIXWheelEvent;

enum NIXTouchPointState {
    kNIXTouchPointStateTouchReleased,
    kNIXTouchPointStateTouchPressed,
    kNIXTouchPointStateTouchMoved,
    kNIXTouchPointStateTouchStationary,
    kNIXTouchPointStateTouchCancelled
};
typedef enum NIXTouchPointState NIXTouchPointState;

struct NIXTouchPoint {
    NIXTouchPointState state;
    int x;
    int y;
    int globalX;
    int globalY;
    int verticalRadius;
    int horizontalRadius;
    float rotationAngle;
    float pressure;
    unsigned id;
};
typedef struct NIXTouchPoint NIXTouchPoint;

enum { kNIXMaximumTouchPointsPerTouchEvent = 10 };

struct NIXTouchEvent {
    NIX_INPUT_EVENT_BASE
    unsigned numTouchPoints;
    NIXTouchPoint touchPoints[kNIXMaximumTouchPointsPerTouchEvent];
};
typedef struct NIXTouchEvent NIXTouchEvent;

struct NIXGestureEvent {
    NIX_INPUT_EVENT_BASE
    int width;
    int height;
};
typedef struct NIXGestureEvent NIXGestureEvent;

#ifdef __cplusplus
}
#endif

#endif // NIXEvents_h

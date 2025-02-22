/*
 * Copyright (C) 2008, 2014 Apple Inc. All Rights Reserved.
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
 *
 */

#ifndef ThreadGlobalData_h
#define ThreadGlobalData_h

#include <wtf/HashMap.h>
#include <wtf/HashSet.h>
#include <wtf/Noncopyable.h>
#include <wtf/OwnPtr.h>
#include <wtf/text/StringHash.h>

#include <wtf/ThreadSpecific.h>
#include <wtf/Threading.h>
using WTF::ThreadSpecific;

namespace WebCore {

    class EventNames;
    class ReplayInputTypes;
    class ThreadTimers;

    struct CachedResourceRequestInitiators;
    struct ICUConverterWrapper;
    struct TECConverterWrapper;

    class ThreadGlobalData {
        WTF_MAKE_NONCOPYABLE(ThreadGlobalData);
    public:
        ThreadGlobalData();
        ~ThreadGlobalData();
        void destroy(); // called on workers to clean up the ThreadGlobalData before the thread exits.

        const CachedResourceRequestInitiators& cachedResourceRequestInitiators() { return *m_cachedResourceRequestInitiators; }
        EventNames& eventNames() { return *m_eventNames; }
        ThreadTimers& threadTimers() { return *m_threadTimers; }
#if ENABLE(WEB_REPLAY)
        ReplayInputTypes& inputTypes() { return *m_inputTypes; }
#endif

        ICUConverterWrapper& cachedConverterICU() { return *m_cachedConverterICU; }

#if PLATFORM(MAC)
        TECConverterWrapper& cachedConverterTEC() { return *m_cachedConverterTEC; }
#endif

#if USE(WEB_THREAD)
        void setWebCoreThreadData();
#endif

    private:
        OwnPtr<CachedResourceRequestInitiators> m_cachedResourceRequestInitiators;
        OwnPtr<EventNames> m_eventNames;
        OwnPtr<ThreadTimers> m_threadTimers;

#if ENABLE(WEB_REPLAY)
        std::unique_ptr<ReplayInputTypes> m_inputTypes;
#endif

#ifndef NDEBUG
        bool m_isMainThread;
#endif

        OwnPtr<ICUConverterWrapper> m_cachedConverterICU;

#if PLATFORM(MAC)
        OwnPtr<TECConverterWrapper> m_cachedConverterTEC;
#endif

        static ThreadSpecific<ThreadGlobalData>* staticData;
#if USE(WEB_THREAD)
        static ThreadGlobalData* sharedMainThreadStaticData;
#endif
        friend ThreadGlobalData& threadGlobalData();
    };

#if USE(WEB_THREAD)
ThreadGlobalData& threadGlobalData();
#else
ThreadGlobalData& threadGlobalData() PURE_FUNCTION;
#endif
    
} // namespace WebCore

#endif // ThreadGlobalData_h

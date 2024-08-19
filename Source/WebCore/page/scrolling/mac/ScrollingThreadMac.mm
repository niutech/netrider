/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
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
#include "ScrollingThread.h"

#if ENABLE(ASYNC_SCROLLING)

namespace WebCore {

void ScrollingThread::initializeRunLoop()
{
    // Initialize the run loop.
    {
        std::lock_guard<std::mutex> lock(m_initializeRunLoopMutex);

        m_threadRunLoop = CFRunLoopGetCurrent();

        CFRunLoopSourceContext context = { 0, this, 0, 0, 0, 0, 0, 0, 0, threadRunLoopSourceCallback };
        m_threadRunLoopSource = adoptCF(CFRunLoopSourceCreate(0, 0, &context));
        CFRunLoopAddSource(CFRunLoopGetCurrent(), m_threadRunLoopSource.get(), kCFRunLoopDefaultMode);

        m_initializeRunLoopConditionVariable.notify_all();
    }

    ASSERT(isCurrentThread());

    CFRunLoopRun();
}

void ScrollingThread::wakeUpRunLoop()
{
    CFRunLoopSourceSignal(m_threadRunLoopSource.get());
    CFRunLoopWakeUp(m_threadRunLoop.get());
}

void ScrollingThread::threadRunLoopSourceCallback(void* scrollingThread)
{
    static_cast<ScrollingThread*>(scrollingThread)->threadRunLoopSourceCallback();
}

void ScrollingThread::threadRunLoopSourceCallback()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    dispatchFunctionsFromScrollingThread();
    [pool drain];
}

} // namespace WebCore

#endif // ENABLE(ASYNC_SCROLLING)

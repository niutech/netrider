/*
 * Copyright (C) 2013 University of Washington. All rights reserved.
 * Copyright (C) 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ReplaySessionSegment_h
#define ReplaySessionSegment_h

#if ENABLE(WEB_REPLAY)

#include <replay/NondeterministicInput.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class CapturingInputCursor;
class EventLoopInputDispatcherClient;
class FunctorInputCursor;
class Page;
class ReplayingInputCursor;
class SegmentedInputStorage;

class ReplaySessionSegment : public RefCounted<ReplaySessionSegment> {
public:
    static PassRefPtr<ReplaySessionSegment> create();
    ~ReplaySessionSegment();

    unsigned identifier() const { return m_identifier; }
    double timestamp() const { return m_timestamp; }

    PassRefPtr<CapturingInputCursor> createCapturingCursor(Page&);
    PassRefPtr<ReplayingInputCursor> createReplayingCursor(Page&, EventLoopInputDispatcherClient*);
    std::unique_ptr<FunctorInputCursor> createFunctorCursor();
private:
    ReplaySessionSegment();

    std::unique_ptr<SegmentedInputStorage> m_storage;
    unsigned m_identifier;
    bool m_canCapture;
    double m_timestamp;
};

} // namespace WebCore

#endif // ENABLE(WEB_REPLAY)

#endif // ReplaySessionSegment_h

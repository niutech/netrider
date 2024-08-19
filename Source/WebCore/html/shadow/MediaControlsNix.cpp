/*
 * Copyright (C) 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2011, 2012 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS&#39;&#39; AND ANY
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
 */

#include "config.h"

#if ENABLE(VIDEO)

#include "MediaControlsNix.h"

using namespace std;

namespace WebCore {

MediaControlsNix::MediaControlsNix(Document& document)
    : MediaControls(document)
    , m_durationDisplay(0)
    , m_enclosure(0)
{
}

PassRefPtr<MediaControls> MediaControls::create(Document& document)
{
    return MediaControlsNix::createControls(document);
}

PassRefPtr<MediaControlsNix> MediaControlsNix::createControls(Document& document)
{
    if (!document.page())
        return 0;

    RefPtr<MediaControlsNix> controls = adoptRef(new MediaControlsNix(document));

    if (controls->initializeControls(document))
        return controls.release();

    return 0;
}

bool MediaControlsNix::initializeControls(Document& document)
{
    // Create an enclosing element for the panel so we can visually offset the controls correctly.
    RefPtr<MediaControlPanelEnclosureElement> enclosure = MediaControlPanelEnclosureElement::create(document);

    RefPtr<MediaControlPanelElement> panel = MediaControlPanelElement::create(document);

    ExceptionCode exceptionCode;

    RefPtr<MediaControlPlayButtonElement> playButton = MediaControlPlayButtonElement::create(document);
    m_playButton = playButton.get();
    panel->appendChild(playButton.release(), exceptionCode, AttachLazily);
    if (exceptionCode)
        return false;

    RefPtr<MediaControlTimelineElement> timeline = MediaControlTimelineElement::create(document, this);
    m_timeline = timeline.get();
    panel->appendChild(timeline.release(), exceptionCode, AttachLazily);
    if (exceptionCode)
        return false;

    RefPtr<MediaControlCurrentTimeDisplayElement> currentTimeDisplay = MediaControlCurrentTimeDisplayElement::create(document);
    m_currentTimeDisplay = currentTimeDisplay.get();
    m_currentTimeDisplay->hide();
    panel->appendChild(currentTimeDisplay.release(), exceptionCode, AttachLazily);
    if (exceptionCode)
        return false;

    RefPtr<MediaControlTimeRemainingDisplayElement> durationDisplay = MediaControlTimeRemainingDisplayElement::create(document);
    m_durationDisplay = durationDisplay.get();
    panel->appendChild(durationDisplay.release(), exceptionCode, AttachLazily);
    if (exceptionCode)
        return false;

    RefPtr<MediaControlPanelMuteButtonElement> panelMuteButton = MediaControlPanelMuteButtonElement::create(document, this);
    m_panelMuteButton = panelMuteButton.get();
    panel->appendChild(panelMuteButton.release(), exceptionCode, AttachLazily);
    if (exceptionCode)
        return false;

    RefPtr<MediaControlPanelVolumeSliderElement> slider = MediaControlPanelVolumeSliderElement::create(document);
    m_volumeSlider = slider.get();
    m_volumeSlider->setClearMutedOnUserInteraction(true);
    panel->appendChild(slider.release(), exceptionCode, AttachLazily);
    if (exceptionCode)
        return false;

    RefPtr<MediaControlFullscreenButtonElement> fullscreenButton = MediaControlFullscreenButtonElement::create(document);
    m_fullScreenButton = fullscreenButton.get();
    panel->appendChild(fullscreenButton.release(), exceptionCode, AttachLazily);
    if (exceptionCode)
        return false;

    m_panel = panel.get();
    enclosure->appendChild(panel.release(), exceptionCode, AttachLazily);
    if (exceptionCode)
        return false;

    m_enclosure = enclosure.get();
    appendChild(enclosure.release(), exceptionCode, AttachLazily);
    if (exceptionCode)
        return false;

    return true;
}

void MediaControlsNix::setMediaController(MediaControllerInterface* controller)
{
    if (m_mediaController == controller)
        return;

    MediaControls::setMediaController(controller);

    if (m_durationDisplay)
        m_durationDisplay->setMediaController(controller);
    if (m_enclosure)
        m_enclosure->setMediaController(controller);
}

void MediaControlsNix::reset()
{
    Page* page = document().page();
    if (!page)
        return;

    double duration = m_mediaController->duration();
    m_durationDisplay->setInnerText(RenderTheme::defaultTheme()->formatMediaControlsTime(duration), ASSERT_NO_EXCEPTION);
    m_durationDisplay->setCurrentValue(duration);

    MediaControls::reset();
}

void MediaControlsNix::playbackStarted()
{
    m_currentTimeDisplay->show();
    m_durationDisplay->hide();

    MediaControls::playbackStarted();
}

void MediaControlsNix::updateCurrentTimeDisplay()
{
    double now = m_mediaController->currentTime();
    double duration = m_mediaController->duration();

    Page* page = document().page();
    if (!page)
        return;

    // After seek, hide duration display and show current time.
    if (now > 0) {
        m_currentTimeDisplay->show();
        m_durationDisplay->hide();
    }

    // Allow the theme to format the time.
    m_currentTimeDisplay->setInnerText(RenderTheme::defaultTheme()->formatMediaControlsCurrentTime(now, duration), IGNORE_EXCEPTION);
    m_currentTimeDisplay->setCurrentValue(now);
}

void MediaControlsNix::changedMute()
{
    MediaControls::changedMute();

    if (m_mediaController->muted())
        m_volumeSlider->setVolume(0);
    else
        m_volumeSlider->setVolume(m_mediaController->volume());
}

}
#endif // ENABLE(VIDEO)

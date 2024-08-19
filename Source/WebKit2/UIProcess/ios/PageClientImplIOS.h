/*
 * Copyright (C) 2012, 2013 Apple Inc. All rights reserved.
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

#ifndef PageClientImplIOS_h
#define PageClientImplIOS_h

#if PLATFORM(IOS)

#import "PageClient.h"
#import "WebFullScreenManagerProxy.h"
#import <wtf/RetainPtr.h>

OBJC_CLASS WKContentView;
OBJC_CLASS WKWebView;

namespace WebKit {
    
class PageClientImpl : public PageClient
#if ENABLE(FULLSCREEN_API)
    , public WebFullScreenManagerProxyClient
#endif
    {
public:
    explicit PageClientImpl(WKContentView *, WKWebView *);
    virtual ~PageClientImpl();
    
private:
    // PageClient
    virtual std::unique_ptr<DrawingAreaProxy> createDrawingAreaProxy() override;
    virtual void setViewNeedsDisplay(const WebCore::IntRect&) override;
    virtual void displayView() override;
    virtual bool canScrollView() override;
    virtual void scrollView(const WebCore::IntRect& scrollRect, const WebCore::IntSize& scrollOffset) override;
    virtual WebCore::IntSize viewSize() override;
    virtual bool isViewWindowActive() override;
    virtual bool isViewFocused() override;
    virtual bool isViewVisible() override;
    virtual bool isViewInWindow() override;
    virtual void processDidExit() override;
    virtual void didRelaunchProcess() override;
    virtual void pageClosed() override;
    virtual void preferencesDidChange() override;
    virtual void toolTipChanged(const String&, const String&) override;
    virtual bool decidePolicyForGeolocationPermissionRequest(WebFrameProxy&, WebSecurityOrigin&, GeolocationPermissionRequestProxy&) override;
    virtual void didCommitLoadForMainFrame(const String& mimeType, bool useCustomContentProvider) override;
    virtual void setCursor(const WebCore::Cursor&) override;
    virtual void setCursorHiddenUntilMouseMoves(bool) override;
    virtual void didChangeViewportProperties(const WebCore::ViewportAttributes&) override;
    virtual void registerEditCommand(PassRefPtr<WebEditCommandProxy>, WebPageProxy::UndoOrRedo) override;
    virtual void clearAllEditCommands() override;
    virtual bool canUndoRedo(WebPageProxy::UndoOrRedo) override;
    virtual void executeUndoRedo(WebPageProxy::UndoOrRedo) override;
    virtual void accessibilityWebProcessTokenReceived(const IPC::DataReference&) override;
    virtual bool executeSavedCommandBySelector(const String& selector) override;
    virtual void setDragImage(const WebCore::IntPoint& clientPosition, PassRefPtr<ShareableBitmap> dragImage, bool isLinkDrag) override;
    virtual void updateSecureInputState() override;
    virtual void resetSecureInputState() override;
    virtual void notifyInputContextAboutDiscardedComposition() override;
    virtual void makeFirstResponder() override;
    virtual WebCore::FloatRect convertToDeviceSpace(const WebCore::FloatRect&) override;
    virtual WebCore::FloatRect convertToUserSpace(const WebCore::FloatRect&) override;
    virtual WebCore::IntPoint screenToRootView(const WebCore::IntPoint&) override;
    virtual WebCore::IntRect rootViewToScreen(const WebCore::IntRect&) override;
    virtual void doneWithKeyEvent(const NativeWebKeyboardEvent&, bool wasEventHandled) override;
#if ENABLE(TOUCH_EVENTS)
    virtual void doneWithTouchEvent(const NativeWebTouchEvent&, bool wasEventHandled) override;
#endif
    virtual PassRefPtr<WebPopupMenuProxy> createPopupMenuProxy(WebPageProxy*) override;
    virtual PassRefPtr<WebContextMenuProxy> createContextMenuProxy(WebPageProxy*) override;
    virtual void setFindIndicator(PassRefPtr<FindIndicator>, bool fadeOut, bool animate) override;

    virtual void enterAcceleratedCompositingMode(const LayerTreeContext&) override;
    virtual void exitAcceleratedCompositingMode() override;
    virtual void updateAcceleratedCompositingMode(const LayerTreeContext&) override;
    virtual void setAcceleratedCompositingRootLayer(LayerOrView *) override;
    virtual LayerOrView *acceleratedCompositingRootLayer() const override;
    virtual LayerHostingMode viewLayerHostingMode() override { return LayerHostingMode::OutOfProcess; }

    virtual RetainPtr<CGImageRef> takeViewSnapshot() override;
    virtual void wheelEventWasNotHandledByWebCore(const NativeWebWheelEvent&) override;
    virtual void clearCustomSwipeViews() override;

    virtual void didGetTapHighlightGeometries(uint64_t requestID, const WebCore::Color&, const Vector<WebCore::FloatQuad>& highlightedQuads, const WebCore::IntSize& topLeftRadius, const WebCore::IntSize& topRightRadius, const WebCore::IntSize& bottomLeftRadius, const WebCore::IntSize& bottomRightRadius) override;

    virtual void didCommitLayerTree(const RemoteLayerTreeTransaction&) override;

    virtual void startAssistingNode(const AssistedNodeInformation&) override;
    virtual void stopAssistingNode() override;
    virtual void selectionDidChange() override;
    virtual bool interpretKeyEvent(const NativeWebKeyboardEvent&, bool isCharEvent) override;
    virtual void positionInformationDidChange(const InteractionInformationAtPosition&);
    virtual void saveImageToLibrary(PassRefPtr<WebCore::SharedBuffer>);
    virtual void didUpdateBlockSelectionWithTouch(uint32_t touch, uint32_t flags, float growThreshold, float shrinkThreshold);

    // Auxiliary Client Creation
#if ENABLE(FULLSCREEN_API)
    virual WebFullScreenManagerProxyClient& fullScreenManagerProxyClient() override;
#endif

#if ENABLE(FULLSCREEN_API)
    // WebFullScreenManagerProxyClient
    virtual void closeFullScreenManager() override;
    virtual bool isFullScreen() override;
    virtual void enterFullScreen() override;
    virtual void exitFullScreen() override;
    virtual void beganEnterFullScreen(const WebCore::IntRect& initialFrame, const WebCore::IntRect& finalFrame) override;
    virtual void beganExitFullScreen(const WebCore::IntRect& initialFrame, const WebCore::IntRect& finalFrame) override;
#endif

    virtual void didFinishLoadingDataForCustomContentProvider(const String& suggestedFilename, const IPC::DataReference&) override;

    WKContentView *m_contentView;
    WKWebView *m_webView;
};
} // namespace WebKit

#endif // PLATFORM(IOS)

#endif // PageClientImplIOS_h

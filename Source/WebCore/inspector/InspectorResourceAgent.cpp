/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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

#include "config.h"
#include "InspectorResourceAgent.h"

#if ENABLE(INSPECTOR)

#include "CachedRawResource.h"
#include "CachedResource.h"
#include "CachedResourceLoader.h"
#include "Document.h"
#include "DocumentLoader.h"
#include "ExceptionCodePlaceholder.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "HTTPHeaderMap.h"
#include "IconController.h"
#include "InspectorClient.h"
#include "InspectorPageAgent.h"
#include "InspectorWebFrontendDispatchers.h"
#include "InstrumentingAgents.h"
#include "JSMainThreadExecState.h"
#include "MemoryCache.h"
#include "NetworkResourcesData.h"
#include "Page.h"
#include "ProgressTracker.h"
#include "ResourceBuffer.h"
#include "ResourceError.h"
#include "ResourceLoader.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"
#include "ScriptableDocumentParser.h"
#include "Settings.h"
#include "SubresourceLoader.h"
#include "URL.h"
#include "WebSocketFrame.h"
#include "XMLHttpRequest.h"
#include <inspector/IdentifiersFactory.h>
#include <inspector/InspectorValues.h>
#include <inspector/ScriptCallStack.h>
#include <inspector/ScriptCallStackFactory.h>
#include <wtf/CurrentTime.h>
#include <wtf/HexNumber.h>
#include <wtf/ListHashSet.h>
#include <wtf/RefPtr.h>
#include <wtf/text/StringBuilder.h>

using namespace Inspector;

typedef Inspector::InspectorNetworkBackendDispatcherHandler::LoadResourceCallback LoadResourceCallback;

namespace WebCore {

void InspectorResourceAgent::didCreateFrontendAndBackend(Inspector::InspectorFrontendChannel* frontendChannel, InspectorBackendDispatcher* backendDispatcher)
{
    m_frontendDispatcher = std::make_unique<InspectorNetworkFrontendDispatcher>(frontendChannel);
    m_backendDispatcher = InspectorNetworkBackendDispatcher::create(backendDispatcher, this);
}

void InspectorResourceAgent::willDestroyFrontendAndBackend(InspectorDisconnectReason)
{
    m_frontendDispatcher = nullptr;
    m_backendDispatcher.clear();

    ErrorString error;
    disable(&error);
}

static PassRefPtr<InspectorObject> buildObjectForHeaders(const HTTPHeaderMap& headers)
{
    RefPtr<InspectorObject> headersObject = InspectorObject::create();
    
    for (const auto& header : headers)
        headersObject->setString(header.key.string(), header.value);
    return headersObject;
}

static PassRefPtr<Inspector::TypeBuilder::Network::ResourceTiming> buildObjectForTiming(const ResourceLoadTiming& timing, DocumentLoader* loader)
{
    return Inspector::TypeBuilder::Network::ResourceTiming::create()
        .setRequestTime(loader->timing()->monotonicTimeToPseudoWallTime(timing.convertResourceLoadTimeToMonotonicTime(0)))
        .setProxyStart(timing.proxyStart)
        .setProxyEnd(timing.proxyEnd)
        .setDnsStart(timing.dnsStart)
        .setDnsEnd(timing.dnsEnd)
        .setConnectStart(timing.connectStart)
        .setConnectEnd(timing.connectEnd)
        .setSslStart(timing.sslStart)
        .setSslEnd(timing.sslEnd)
        .setSendStart(timing.sendStart)
        .setSendEnd(timing.sendEnd)
        .setReceiveHeadersEnd(timing.receiveHeadersEnd)
        .release();
}

static PassRefPtr<Inspector::TypeBuilder::Network::Request> buildObjectForResourceRequest(const ResourceRequest& request)
{
    RefPtr<Inspector::TypeBuilder::Network::Request> requestObject = Inspector::TypeBuilder::Network::Request::create()
        .setUrl(request.url().string())
        .setMethod(request.httpMethod())
        .setHeaders(buildObjectForHeaders(request.httpHeaderFields()));
    if (request.httpBody() && !request.httpBody()->isEmpty())
        requestObject->setPostData(request.httpBody()->flattenToString());
    return requestObject;
}

static PassRefPtr<Inspector::TypeBuilder::Network::Response> buildObjectForResourceResponse(const ResourceResponse& response, DocumentLoader* loader)
{
    if (response.isNull())
        return nullptr;

    double status = response.httpStatusCode();
    RefPtr<InspectorObject> headers = buildObjectForHeaders(response.httpHeaderFields());

    RefPtr<Inspector::TypeBuilder::Network::Response> responseObject = Inspector::TypeBuilder::Network::Response::create()
        .setUrl(response.url().string())
        .setStatus(status)
        .setStatusText(response.httpStatusText())
        .setHeaders(headers)
        .setMimeType(response.mimeType())
        .setConnectionReused(response.connectionReused())
        .setConnectionId(response.connectionID());

    responseObject->setFromDiskCache(response.wasCached());
    if (response.resourceLoadTiming())
        responseObject->setTiming(buildObjectForTiming(*response.resourceLoadTiming(), loader));

    return responseObject;
}

static PassRefPtr<Inspector::TypeBuilder::Network::CachedResource> buildObjectForCachedResource(CachedResource* cachedResource, DocumentLoader* loader)
{
    RefPtr<Inspector::TypeBuilder::Network::CachedResource> resourceObject = Inspector::TypeBuilder::Network::CachedResource::create()
        .setUrl(cachedResource->url())
        .setType(InspectorPageAgent::cachedResourceTypeJson(*cachedResource))
        .setBodySize(cachedResource->encodedSize());

    RefPtr<Inspector::TypeBuilder::Network::Response> resourceResponse = buildObjectForResourceResponse(cachedResource->response(), loader);
    if (resourceResponse)
        resourceObject->setResponse(resourceResponse);

    String sourceMappingURL = InspectorPageAgent::sourceMapURLForResource(cachedResource);
    if (!sourceMappingURL.isEmpty())
        resourceObject->setSourceMapURL(sourceMappingURL);

    return resourceObject;
}

InspectorResourceAgent::~InspectorResourceAgent()
{
    if (m_enabled) {
        ErrorString error;
        disable(&error);
    }
    ASSERT(!m_instrumentingAgents->inspectorResourceAgent());
}

void InspectorResourceAgent::willSendRequest(unsigned long identifier, DocumentLoader* loader, ResourceRequest& request, const ResourceResponse& redirectResponse)
{
    if (request.hiddenFromInspector()) {
        m_hiddenRequestIdentifiers.add(identifier);
        return;
    }

    String requestId = IdentifiersFactory::requestId(identifier);
    m_resourcesData->resourceCreated(requestId, m_pageAgent->loaderId(loader));

    CachedResource* cachedResource = loader ? InspectorPageAgent::cachedResource(loader->frame(), request.url()) : nullptr;
    InspectorPageAgent::ResourceType type = cachedResource ? InspectorPageAgent::cachedResourceType(*cachedResource) : m_resourcesData->resourceType(requestId);
    if (type == InspectorPageAgent::OtherResource) {
        if (m_loadingXHRSynchronously)
            type = InspectorPageAgent::XHRResource;
        else if (equalIgnoringFragmentIdentifier(request.url(), loader->frameLoader()->icon().url()))
            type = InspectorPageAgent::ImageResource;
        else if (equalIgnoringFragmentIdentifier(request.url(), loader->url()) && !loader->isCommitted())
            type = InspectorPageAgent::DocumentResource;
    }

    m_resourcesData->setResourceType(requestId, type);

    if (m_extraRequestHeaders) {
        InspectorObject::const_iterator end = m_extraRequestHeaders->end();
        for (InspectorObject::const_iterator it = m_extraRequestHeaders->begin(); it != end; ++it) {
            String value;
            if (it->value->asString(&value))
                request.setHTTPHeaderField(it->key, value);
        }
    }

    request.setReportLoadTiming(true);
    request.setReportRawHeaders(true);

    if (m_cacheDisabled) {
        request.setHTTPHeaderField("Pragma", "no-cache");
        request.setCachePolicy(ReloadIgnoringCacheData);
        request.setHTTPHeaderField("Cache-Control", "no-cache");
    }

    Inspector::TypeBuilder::Page::ResourceType::Enum resourceType = InspectorPageAgent::resourceTypeJson(type);

    RefPtr<Inspector::TypeBuilder::Network::Initiator> initiatorObject = buildInitiatorObject(loader->frame() ? loader->frame()->document() : nullptr);
    m_frontendDispatcher->requestWillBeSent(requestId, m_pageAgent->frameId(loader->frame()), m_pageAgent->loaderId(loader), loader->url().string(), buildObjectForResourceRequest(request), currentTime(), initiatorObject, buildObjectForResourceResponse(redirectResponse, loader), type != InspectorPageAgent::OtherResource ? &resourceType : nullptr);
}

void InspectorResourceAgent::markResourceAsCached(unsigned long identifier)
{
    if (m_hiddenRequestIdentifiers.contains(identifier))
        return;

    m_frontendDispatcher->requestServedFromCache(IdentifiersFactory::requestId(identifier));
}

void InspectorResourceAgent::didReceiveResponse(unsigned long identifier, DocumentLoader* loader, const ResourceResponse& response, ResourceLoader* resourceLoader)
{
    if (m_hiddenRequestIdentifiers.contains(identifier))
        return;

    String requestId = IdentifiersFactory::requestId(identifier);
    RefPtr<Inspector::TypeBuilder::Network::Response> resourceResponse = buildObjectForResourceResponse(response, loader);

    bool isNotModified = response.httpStatusCode() == 304;

    CachedResource* cachedResource = nullptr;
    if (resourceLoader && resourceLoader->isSubresourceLoader() && !isNotModified)
        cachedResource = static_cast<SubresourceLoader*>(resourceLoader)->cachedResource();
    if (!cachedResource)
        cachedResource = InspectorPageAgent::cachedResource(loader->frame(), response.url());

    if (cachedResource) {
        // Use mime type from cached resource in case the one in response is empty.
        if (resourceResponse && response.mimeType().isEmpty())
            resourceResponse->setString(Inspector::TypeBuilder::Network::Response::MimeType, cachedResource->response().mimeType());
        m_resourcesData->addCachedResource(requestId, cachedResource);
    }

    InspectorPageAgent::ResourceType type = m_resourcesData->resourceType(requestId);
    InspectorPageAgent::ResourceType newType = cachedResource ? InspectorPageAgent::cachedResourceType(*cachedResource) : type;

    // FIXME: XHRResource is returned for CachedResource::RawResource, it should be OtherResource unless it truly is an XHR.
    // RawResource is used for loading worker scripts, and those should stay as ScriptResource and not change to XHRResource.
    if (type != newType && newType != InspectorPageAgent::XHRResource && newType != InspectorPageAgent::OtherResource)
        type = newType;

    m_resourcesData->responseReceived(requestId, m_pageAgent->frameId(loader->frame()), response);
    m_resourcesData->setResourceType(requestId, type);

    m_frontendDispatcher->responseReceived(requestId, m_pageAgent->frameId(loader->frame()), m_pageAgent->loaderId(loader), currentTime(), InspectorPageAgent::resourceTypeJson(type), resourceResponse);

    // If we revalidated the resource and got Not modified, send content length following didReceiveResponse
    // as there will be no calls to didReceiveData from the network stack.
    if (isNotModified && cachedResource && cachedResource->encodedSize())
        didReceiveData(identifier, nullptr, cachedResource->encodedSize(), 0);
}

static bool isErrorStatusCode(int statusCode)
{
    return statusCode >= 400;
}

void InspectorResourceAgent::didReceiveData(unsigned long identifier, const char* data, int dataLength, int encodedDataLength)
{
    if (m_hiddenRequestIdentifiers.contains(identifier))
        return;

    String requestId = IdentifiersFactory::requestId(identifier);

    if (data) {
        NetworkResourcesData::ResourceData const* resourceData = m_resourcesData->data(requestId);
        if (resourceData && !m_loadingXHRSynchronously && (!resourceData->cachedResource() || resourceData->cachedResource()->dataBufferingPolicy() == DoNotBufferData || isErrorStatusCode(resourceData->httpStatusCode())))
            m_resourcesData->maybeAddResourceData(requestId, data, dataLength);
    }

    m_frontendDispatcher->dataReceived(requestId, currentTime(), dataLength, encodedDataLength);
}

void InspectorResourceAgent::didFinishLoading(unsigned long identifier, DocumentLoader* loader, double finishTime)
{
    if (m_hiddenRequestIdentifiers.remove(identifier))
        return;

    String requestId = IdentifiersFactory::requestId(identifier);
    if (m_resourcesData->resourceType(requestId) == InspectorPageAgent::DocumentResource) {
        RefPtr<ResourceBuffer> buffer = loader->frameLoader()->documentLoader()->mainResourceData();
        m_resourcesData->addResourceSharedBuffer(requestId, buffer ? buffer->sharedBuffer() : nullptr, loader->frame()->document()->inputEncoding());
    }

    m_resourcesData->maybeDecodeDataToContent(requestId);

    // FIXME: The finishTime that is passed in is from the NetworkProcess and is more accurate.
    // However, all other times passed to the Inspector are generated from the web process. Mixing
    // times from different processes can cause the finish time to be earlier than the response
    // received time due to inter-process communication lag.
    finishTime = currentTime();

    String sourceMappingURL;
    NetworkResourcesData::ResourceData const* resourceData = m_resourcesData->data(requestId);
    if (resourceData && resourceData->cachedResource())
        sourceMappingURL = InspectorPageAgent::sourceMapURLForResource(resourceData->cachedResource());

    m_frontendDispatcher->loadingFinished(requestId, finishTime, !sourceMappingURL.isEmpty() ? &sourceMappingURL : nullptr);
}

void InspectorResourceAgent::didFailLoading(unsigned long identifier, DocumentLoader* loader, const ResourceError& error)
{
    if (m_hiddenRequestIdentifiers.remove(identifier))
        return;

    String requestId = IdentifiersFactory::requestId(identifier);

    if (m_resourcesData->resourceType(requestId) == InspectorPageAgent::DocumentResource) {
        Frame* frame = loader ? loader->frame() : nullptr;
        if (frame && frame->loader().documentLoader() && frame->document()) {
            RefPtr<ResourceBuffer> buffer = frame->loader().documentLoader()->mainResourceData();
            m_resourcesData->addResourceSharedBuffer(requestId, buffer ? buffer->sharedBuffer() : nullptr, frame->document()->inputEncoding());
        }
    }

    bool canceled = error.isCancellation();
    m_frontendDispatcher->loadingFailed(requestId, currentTime(), error.localizedDescription(), canceled ? &canceled : nullptr);
}

void InspectorResourceAgent::didLoadResourceFromMemoryCache(DocumentLoader* loader, CachedResource* resource)
{
    String loaderId = m_pageAgent->loaderId(loader);
    String frameId = m_pageAgent->frameId(loader->frame());
    unsigned long identifier = loader->frame()->page()->progress().createUniqueIdentifier();
    String requestId = IdentifiersFactory::requestId(identifier);
    m_resourcesData->resourceCreated(requestId, loaderId);
    m_resourcesData->addCachedResource(requestId, resource);
    if (resource->type() == CachedResource::RawResource) {
        CachedRawResource* rawResource = toCachedRawResource(resource);
        String rawRequestId = IdentifiersFactory::requestId(rawResource->identifier());
        m_resourcesData->reuseXHRReplayData(requestId, rawRequestId);
    }

    RefPtr<Inspector::TypeBuilder::Network::Initiator> initiatorObject = buildInitiatorObject(loader->frame() ? loader->frame()->document() : nullptr);

    m_frontendDispatcher->requestServedFromMemoryCache(requestId, frameId, loaderId, loader->url().string(), currentTime(), initiatorObject, buildObjectForCachedResource(resource, loader));
}

void InspectorResourceAgent::setInitialScriptContent(unsigned long identifier, const String& sourceString)
{
    m_resourcesData->setResourceContent(IdentifiersFactory::requestId(identifier), sourceString);
}

void InspectorResourceAgent::didReceiveScriptResponse(unsigned long identifier)
{
    m_resourcesData->setResourceType(IdentifiersFactory::requestId(identifier), InspectorPageAgent::ScriptResource);
}

void InspectorResourceAgent::documentThreadableLoaderStartedLoadingForClient(unsigned long identifier, ThreadableLoaderClient* client)
{
    if (!client)
        return;

    PendingXHRReplayDataMap::iterator it = m_pendingXHRReplayData.find(client);
    if (it == m_pendingXHRReplayData.end())
        return;

    XHRReplayData* xhrReplayData = it->value.get();
    String requestId = IdentifiersFactory::requestId(identifier);
    m_resourcesData->setXHRReplayData(requestId, xhrReplayData);
}

void InspectorResourceAgent::willLoadXHR(ThreadableLoaderClient* client, const String& method, const URL& url, bool async, PassRefPtr<FormData> formData, const HTTPHeaderMap& headers, bool includeCredentials)
{
    RefPtr<XHRReplayData> xhrReplayData = XHRReplayData::create(method, url, async, formData, includeCredentials);
    for (const auto& header : headers)
        xhrReplayData->addHeader(header.key, header.value);
    m_pendingXHRReplayData.set(client, xhrReplayData);
}

void InspectorResourceAgent::didFailXHRLoading(ThreadableLoaderClient* client)
{
    m_pendingXHRReplayData.remove(client);
}

void InspectorResourceAgent::didFinishXHRLoading(ThreadableLoaderClient* client, unsigned long identifier, const String& sourceString)
{
    // For Asynchronous XHRs, the inspector can grab the data directly off of the CachedResource. For sync XHRs, we need to
    // provide the data here, since no CachedResource was involved.
    if (m_loadingXHRSynchronously)
        m_resourcesData->setResourceContent(IdentifiersFactory::requestId(identifier), sourceString);
    m_pendingXHRReplayData.remove(client);
}

void InspectorResourceAgent::didReceiveXHRResponse(unsigned long identifier)
{
    m_resourcesData->setResourceType(IdentifiersFactory::requestId(identifier), InspectorPageAgent::XHRResource);
}

void InspectorResourceAgent::willLoadXHRSynchronously()
{
    m_loadingXHRSynchronously = true;
}

void InspectorResourceAgent::didLoadXHRSynchronously()
{
    m_loadingXHRSynchronously = false;
}

void InspectorResourceAgent::willDestroyCachedResource(CachedResource* cachedResource)
{
    Vector<String> requestIds = m_resourcesData->removeCachedResource(cachedResource);
    if (!requestIds.size())
        return;

    String content;
    bool base64Encoded;
    if (!InspectorPageAgent::cachedResourceContent(cachedResource, &content, &base64Encoded))
        return;
    Vector<String>::iterator end = requestIds.end();
    for (Vector<String>::iterator it = requestIds.begin(); it != end; ++it)
        m_resourcesData->setResourceContent(*it, content, base64Encoded);
}

void InspectorResourceAgent::willRecalculateStyle()
{
    m_isRecalculatingStyle = true;
}

void InspectorResourceAgent::didRecalculateStyle()
{
    m_isRecalculatingStyle = false;
    m_styleRecalculationInitiator = nullptr;
}

void InspectorResourceAgent::didScheduleStyleRecalculation(Document* document)
{
    if (!m_styleRecalculationInitiator)
        m_styleRecalculationInitiator = buildInitiatorObject(document);
}

PassRefPtr<Inspector::TypeBuilder::Network::Initiator> InspectorResourceAgent::buildInitiatorObject(Document* document)
{
    RefPtr<ScriptCallStack> stackTrace = createScriptCallStack(JSMainThreadExecState::currentState(), ScriptCallStack::maxCallStackSizeToCapture);
    if (stackTrace && stackTrace->size() > 0) {
        RefPtr<Inspector::TypeBuilder::Network::Initiator> initiatorObject = Inspector::TypeBuilder::Network::Initiator::create()
            .setType(Inspector::TypeBuilder::Network::Initiator::Type::Script);
        initiatorObject->setStackTrace(stackTrace->buildInspectorArray());
        return initiatorObject;
    }

    if (document && document->scriptableDocumentParser()) {
        RefPtr<Inspector::TypeBuilder::Network::Initiator> initiatorObject = Inspector::TypeBuilder::Network::Initiator::create()
            .setType(Inspector::TypeBuilder::Network::Initiator::Type::Parser);
        initiatorObject->setUrl(document->url().string());
        initiatorObject->setLineNumber(document->scriptableDocumentParser()->textPosition().m_line.oneBasedInt());
        return initiatorObject;
    }

    if (m_isRecalculatingStyle && m_styleRecalculationInitiator)
        return m_styleRecalculationInitiator;

    return Inspector::TypeBuilder::Network::Initiator::create()
        .setType(Inspector::TypeBuilder::Network::Initiator::Type::Other)
        .release();
}

#if ENABLE(WEB_SOCKETS)

void InspectorResourceAgent::didCreateWebSocket(unsigned long identifier, const URL& requestURL)
{
    m_frontendDispatcher->webSocketCreated(IdentifiersFactory::requestId(identifier), requestURL.string());
}

void InspectorResourceAgent::willSendWebSocketHandshakeRequest(unsigned long identifier, const ResourceRequest& request)
{
    RefPtr<Inspector::TypeBuilder::Network::WebSocketRequest> requestObject = Inspector::TypeBuilder::Network::WebSocketRequest::create()
        .setHeaders(buildObjectForHeaders(request.httpHeaderFields()));
    m_frontendDispatcher->webSocketWillSendHandshakeRequest(IdentifiersFactory::requestId(identifier), currentTime(), requestObject);
}

void InspectorResourceAgent::didReceiveWebSocketHandshakeResponse(unsigned long identifier, const ResourceResponse& response)
{
    RefPtr<Inspector::TypeBuilder::Network::WebSocketResponse> responseObject = Inspector::TypeBuilder::Network::WebSocketResponse::create()
        .setStatus(response.httpStatusCode())
        .setStatusText(response.httpStatusText())
        .setHeaders(buildObjectForHeaders(response.httpHeaderFields()));
    m_frontendDispatcher->webSocketHandshakeResponseReceived(IdentifiersFactory::requestId(identifier), currentTime(), responseObject);
}

void InspectorResourceAgent::didCloseWebSocket(unsigned long identifier)
{
    m_frontendDispatcher->webSocketClosed(IdentifiersFactory::requestId(identifier), currentTime());
}

void InspectorResourceAgent::didReceiveWebSocketFrame(unsigned long identifier, const WebSocketFrame& frame)
{
    RefPtr<Inspector::TypeBuilder::Network::WebSocketFrame> frameObject = Inspector::TypeBuilder::Network::WebSocketFrame::create()
        .setOpcode(frame.opCode)
        .setMask(frame.masked)
        .setPayloadData(String(frame.payload, frame.payloadLength));
    m_frontendDispatcher->webSocketFrameReceived(IdentifiersFactory::requestId(identifier), currentTime(), frameObject);
}

void InspectorResourceAgent::didSendWebSocketFrame(unsigned long identifier, const WebSocketFrame& frame)
{
    RefPtr<Inspector::TypeBuilder::Network::WebSocketFrame> frameObject = Inspector::TypeBuilder::Network::WebSocketFrame::create()
        .setOpcode(frame.opCode)
        .setMask(frame.masked)
        .setPayloadData(String(frame.payload, frame.payloadLength));
    m_frontendDispatcher->webSocketFrameSent(IdentifiersFactory::requestId(identifier), currentTime(), frameObject);
}

void InspectorResourceAgent::didReceiveWebSocketFrameError(unsigned long identifier, const String& errorMessage)
{
    m_frontendDispatcher->webSocketFrameError(IdentifiersFactory::requestId(identifier), currentTime(), errorMessage);
}

#endif // ENABLE(WEB_SOCKETS)

// called from Internals for layout test purposes.
void InspectorResourceAgent::setResourcesDataSizeLimitsFromInternals(int maximumResourcesContentSize, int maximumSingleResourceContentSize)
{
    m_resourcesData->setResourcesDataSizeLimits(maximumResourcesContentSize, maximumSingleResourceContentSize);
}

void InspectorResourceAgent::enable(ErrorString*)
{
    enable();
}

void InspectorResourceAgent::enable()
{
    if (!m_frontendDispatcher)
        return;
    m_enabled = true;
    m_instrumentingAgents->setInspectorResourceAgent(this);
}

void InspectorResourceAgent::disable(ErrorString*)
{
    m_enabled = false;
    m_instrumentingAgents->setInspectorResourceAgent(nullptr);
    m_resourcesData->clear();
    m_extraRequestHeaders.clear();
}

void InspectorResourceAgent::setExtraHTTPHeaders(ErrorString*, const RefPtr<InspectorObject>& headers)
{
    m_extraRequestHeaders = headers;
}

void InspectorResourceAgent::getResponseBody(ErrorString* errorString, const String& requestId, String* content, bool* base64Encoded)
{
    NetworkResourcesData::ResourceData const* resourceData = m_resourcesData->data(requestId);
    if (!resourceData) {
        *errorString = "No resource with given identifier found";
        return;
    }

    if (resourceData->hasContent()) {
        *base64Encoded = resourceData->base64Encoded();
        *content = resourceData->content();
        return;
    }

    if (resourceData->isContentEvicted()) {
        *errorString = "Request content was evicted from inspector cache";
        return;
    }

    if (resourceData->buffer() && !resourceData->textEncodingName().isNull()) {
        *base64Encoded = false;
        if (InspectorPageAgent::sharedBufferContent(resourceData->buffer(), resourceData->textEncodingName(), *base64Encoded, content))
            return;
    }

    if (resourceData->cachedResource()) {
        if (InspectorPageAgent::cachedResourceContent(resourceData->cachedResource(), content, base64Encoded))
            return;
    }

    *errorString = "No data found for resource with given identifier";
}

void InspectorResourceAgent::replayXHR(ErrorString*, const String& requestId)
{
    RefPtr<XMLHttpRequest> xhr = XMLHttpRequest::create(*m_pageAgent->mainFrame()->document());
    String actualRequestId = requestId;

    XHRReplayData* xhrReplayData = m_resourcesData->xhrReplayData(requestId);
    if (!xhrReplayData)
        return;

    ResourceRequest request(xhrReplayData->url());
#if ENABLE(CACHE_PARTITIONING)
    request.setCachePartition(m_pageAgent->mainFrame()->document()->topOrigin()->cachePartition());
#endif

    CachedResource* cachedResource = memoryCache()->resourceForRequest(request, m_pageAgent->page()->sessionID());
    if (cachedResource)
        memoryCache()->remove(cachedResource);

    xhr->open(xhrReplayData->method(), xhrReplayData->url(), xhrReplayData->async(), IGNORE_EXCEPTION);
    for (const auto& header : xhrReplayData->headers())
        xhr->setRequestHeader(header.key, header.value, IGNORE_EXCEPTION);
    xhr->sendForInspectorXHRReplay(xhrReplayData->formData(), IGNORE_EXCEPTION);
}

namespace {

class LoadResourceListener final : public EventListener {
    WTF_MAKE_NONCOPYABLE(LoadResourceListener);
public:
    static PassRefPtr<LoadResourceListener> create(PassRefPtr<LoadResourceCallback> callback)
    {
        return adoptRef(new LoadResourceListener(callback));
    }

    virtual ~LoadResourceListener() { }

    virtual bool operator==(const EventListener& other) override
    {
        return this == &other;
    }

    virtual void handleEvent(ScriptExecutionContext*, Event* event) override
    {
        if (!m_callback->isActive())
            return;

        if (event->type() == eventNames().errorEvent) {
            m_callback->sendFailure(ASCIILiteral("Error loading resource"));
            return;
        }

        if (event->type() != eventNames().readystatechangeEvent) {
            m_callback->sendFailure(ASCIILiteral("Unexpected event type"));
            return;
        }

        XMLHttpRequest* xhr = static_cast<XMLHttpRequest*>(event->target());
        if (xhr->readyState() != XMLHttpRequest::DONE)
            return;

        m_callback->sendSuccess(xhr->responseTextIgnoringResponseType(), xhr->responseMIMEType());
    }

private:
    LoadResourceListener(PassRefPtr<LoadResourceCallback> callback)
        : EventListener(EventListener::CPPEventListenerType)
        , m_callback(callback) { }

    RefPtr<LoadResourceCallback> m_callback;
};

} // namespace

void InspectorResourceAgent::loadResource(ErrorString* errorString, const String& frameId, const String& urlString, PassRefPtr<LoadResourceCallback> callback)
{
    Frame* frame = m_pageAgent->assertFrame(errorString, frameId);
    if (!frame)
        return;

    Document* document = frame->document();
    if (!document) {
        *errorString = ASCIILiteral("No Document instance for the specified frame");
        return;
    }

    RefPtr<XMLHttpRequest> xhr = XMLHttpRequest::create(*document);

    ExceptionCode ec = 0;
    xhr->open(ASCIILiteral("GET"), document->completeURL(urlString), ec);
    if (ec) {
        *errorString = ASCIILiteral("Error creating request");
        return;
    }

    RefPtr<LoadResourceListener> loadResourceListener = LoadResourceListener::create(callback);
    xhr->addEventListener(eventNames().abortEvent, loadResourceListener, false);
    xhr->addEventListener(eventNames().errorEvent, loadResourceListener, false);
    xhr->addEventListener(eventNames().readystatechangeEvent, loadResourceListener, false);
    xhr->sendForInspector(ec);

    if (ec) {
        *errorString = ASCIILiteral("Error sending request");
        return;
    }
}

void InspectorResourceAgent::canClearBrowserCache(ErrorString*, bool* result)
{
    *result = m_client->canClearBrowserCache();
}

void InspectorResourceAgent::clearBrowserCache(ErrorString*)
{
    m_client->clearBrowserCache();
}

void InspectorResourceAgent::canClearBrowserCookies(ErrorString*, bool* result)
{
    *result = m_client->canClearBrowserCookies();
}

void InspectorResourceAgent::clearBrowserCookies(ErrorString*)
{
    m_client->clearBrowserCookies();
}

void InspectorResourceAgent::setCacheDisabled(ErrorString*, bool cacheDisabled)
{
    m_cacheDisabled = cacheDisabled;
    if (cacheDisabled)
        memoryCache()->evictResources();
}

void InspectorResourceAgent::mainFrameNavigated(DocumentLoader* loader)
{
    if (m_cacheDisabled)
        memoryCache()->evictResources();

    m_resourcesData->clear(m_pageAgent->loaderId(loader));
}

InspectorResourceAgent::InspectorResourceAgent(InstrumentingAgents* instrumentingAgents, InspectorPageAgent* pageAgent, InspectorClient* client)
    : InspectorAgentBase(ASCIILiteral("Network"), instrumentingAgents)
    , m_pageAgent(pageAgent)
    , m_client(client)
    , m_resourcesData(std::make_unique<NetworkResourcesData>())
    , m_enabled(false)
    , m_cacheDisabled(false)
    , m_loadingXHRSynchronously(false)
    , m_isRecalculatingStyle(false)
{
}

} // namespace WebCore

#endif // ENABLE(INSPECTOR)

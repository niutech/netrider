list(APPEND WebKit2_LINK_FLAGS
    ${CAIRO_LDFLAGS}
)

list(APPEND WebKit2_SOURCES
    NetworkProcess/unix/NetworkProcessMainUnix.cpp

    Platform/gtk/ModuleGtk.cpp
    Platform/gtk/LoggingGtk.cpp
    Platform/gtk/WorkQueueGtk.cpp
    Platform/unix/SharedMemoryUnix.cpp

    Platform/IPC/unix/ConnectionUnix.cpp
    Platform/IPC/unix/AttachmentUnix.cpp

    Shared/API/c/cairo/WKImageCairo.cpp

    Shared/API/c/nix/WKPopupItem.cpp
    Shared/API/c/nix/WKPopupMenuListener.cpp

    Shared/cairo/ShareableBitmapCairo.cpp

    Shared/nix/LayerTreeContextNix.cpp
    Shared/nix/NativeWebMouseEventNix.cpp
    Shared/nix/NativeWebWheelEventNix.cpp
    Shared/nix/NativeWebKeyboardEventNix.cpp
    Shared/nix/NativeWebTouchEventNix.cpp
    Shared/nix/WebEventFactoryNix.cpp
    Shared/nix/WebPopupItemPlatform.cpp
    Shared/nix/WebPopupMenuListener.cpp
    Shared/nix/WebUIPopupMenuClient.cpp

    Shared/efl/ProcessExecutablePathEfl.cpp

    UIProcess/API/C/CoordinatedGraphics/WKView.cpp
    UIProcess/API/C/nix/WKContextNix.cpp
    UIProcess/API/C/nix/WKErrorNix.cpp
    UIProcess/API/C/nix/WKPageNix.cpp
    UIProcess/API/nix/NIXView.cpp

    UIProcess/cairo/BackingStoreCairo.cpp

    UIProcess/CoordinatedGraphics/WebView.cpp
    UIProcess/CoordinatedGraphics/WebViewClient.cpp

    UIProcess/nix/WebViewNix.cpp
    UIProcess/nix/WebViewClientNix.cpp
    UIProcess/nix/TextCheckerNix.cpp
    UIProcess/nix/WebContextNix.cpp
    UIProcess/nix/WebInspectorProxyNix.cpp
    UIProcess/nix/WebPageProxyNix.cpp
    UIProcess/nix/WebPopupMenuListenerNix.cpp
    UIProcess/efl/WebPreferencesEfl.cpp

    UIProcess/Launcher/nix/ProcessLauncherNix.cpp
    UIProcess/nix/WebProcessProxyNix.cpp

    UIProcess/Plugins/unix/PluginInfoStoreUnix.cpp

    WebProcess/nix/WebProcessMainNix.cpp

    WebProcess/InjectedBundle/gtk/InjectedBundleGtk.cpp

    WebProcess/InjectedBundle/API/nix/NixTestSupport.cpp

    WebProcess/WebCoreSupport/nix/WebErrorsNix.cpp
    WebProcess/WebCoreSupport/nix/WebPopupMenuNix.cpp
    WebProcess/WebCoreSupport/nix/WebContextMenuClientNix.cpp

    WebProcess/WebPage/nix/WebInspectorNix.cpp
    WebProcess/WebPage/nix/WebPageNix.cpp

    UIProcess/Storage/StorageManager.cpp

    UIProcess/DefaultUndoController.cpp
)

list(APPEND WebKit2_INCLUDE_DIRECTORIES
    NetworkProcess/unix
    Shared/nix
    Shared/API/c/nix
    WebProcess/nix
    WebProcess/WebCoreSupport/nix
    UIProcess/nix
    UIProcess/API/nix
    UIProcess/API/C/nix
    UIProcess/API/C/CoordinatedGraphics
    "${JAVASCRIPTCORE_DIR}/llint"
    "${WEBCORE_DIR}/platform/nix"
    "${WEBCORE_DIR}/platform/graphics/cairo"
    "${WEBCORE_DIR}/platform/graphics/opentype"
    "${WEBCORE_DIR}/platform/graphics/filters/texmap"
    "${WEBCORE_DIR}/svg/graphics"
    "${WTF_DIR}/wtf/gobject"
    ${CAIRO_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${LIBXSLT_INCLUDE_DIRS}
    ${SQLITE_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${HARFBUZZ_INCLUDE_DIRS}
    "${THIRDPARTY_DIR}/ANGLE/include/GLSLANG"
    "${THIRDPARTY_DIR}/ANGLE/include/KHR"
    ${ICU_INCLUDE_DIRS}
)

if (WTF_USE_OPENGL_ES_2)
    list(APPEND WebKit2_INCLUDE_DIRECTORIES ${OPENGLES2_INCLUDE_DIRS})
    list(APPEND WebProcess_INCLUDE_DIRECTORIES ${OPENGLES2_INCLUDE_DIRS})
    list(APPEND WebKit2_LIBRARIES ${OPENGLES2_LIBRARIES})
    list(APPEND WebProcess_LIBRARIES ${OPENGLES2_LIBRARIES})
    add_definitions(${OPENGLES2_DEFINITIONS})
else ()
    list(APPEND WebKit2_LIBRARIES ${OPENGL_LIBRARIES})
    list(APPEND WebProcess_LIBRARIES ${OPENGL_LIBRARIES})
    include_directories(${OPENGL_INCLUDE_DIR})
endif ()

if (ENABLE_INSPECTOR_SERVER)
    list(APPEND WebKit2_INCLUDE_DIRECTORIES
       UIProcess/InspectorServer
    )

    list(APPEND WebKit2_SOURCES
        UIProcess/InspectorServer/nix/WebSocketServerNix.cpp
        UIProcess/InspectorServer/nix/WebInspectorServerNix.cpp
    )
endif ()

if (ENABLE_MEDIA_STREAM)
    list(APPEND WebKit2_INCLUDE_DIRECTORIES
        "${WEBCORE_DIR}/Modules/mediastream"
        "${WEBCORE_DIR}/platform/audio"
        "${WEBCORE_DIR}/platform/mediastream"
        "${WEBCORE_DIR}/platform/mediastream/nix"
    )
endif ()

list(APPEND WebKit2_LIBRARIES
    WebCoreTestSupport
    ${CAIRO_LIBRARIES}
    ${Freetype_LIBRARIES}
    ${LIBXML2_LIBRARIES}
    ${SQLITE_LIBRARIES}
    ${FONTCONFIG_LIBRARIES}
    ${PNG_LIBRARY}
    ${JPEG_LIBRARY}
    ${CMAKE_DL_LIBS}
    ${GLIB_LIBRARIES}
    ${GLIB_GIO_LIBRARIES}
    ${GLIB_GOBJECT_LIBRARIES}
)

list(APPEND WebProcess_SOURCES
    nix/MainNix.cpp
)

list(APPEND WebProcess_LIBRARIES
    ${CAIRO_LIBRARIES}
    ${LIBXML2_LIBRARIES}
    ${LIBXSLT_LIBRARIES}
    ${SQLITE_LIBRARIES}
)

add_definitions(-DDEFAULT_THEME_PATH=\"${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DIR}/themes\")

if (WTF_USE_CURL)
    list(APPEND WebKit2_SOURCES
        Shared/curl/WebCoreArgumentCodersCurl.cpp
        WebProcess/curl/WebCurlRequestManager.cpp
        WebProcess/curl/WebProcessCurl.cpp
        WebProcess/Cookies/curl/WebCookieManagerCurl.cpp
        UIProcess/API/C/curl/WKContextCurl.cpp
        UIProcess/curl/WebCurlRequestManagerProxy.cpp
        Shared/Downloads/curl/DownloadCurl.cpp

        WebProcess/WebCoreSupport/curl/WebFrameNetworkingContext.cpp
    )

    list(APPEND WebKit2_MESSAGES_IN_FILES
        WebProcess/curl/WebCurlRequestManager.messages.in
    )

    list(APPEND WebKit2_INCLUDE_DIRECTORIES
        "${WEBCORE_DIR}/platform/network/curl"
        "${WEBKIT2_DIR}/Shared/curl"
        "${WEBKIT2_DIR}/WebProcess/WebCoreSupport/curl"
        WebProcess/curl
        UIProcess/API/C/curl
        UIProcess/curl
    )

    list(APPEND WebKit2_LIBRARIES
         ${GLIB_GMODULE_LIBRARIES}
    )

    add_custom_target(forwarding-headerNetwork
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include curl
    )
else ()
    list(APPEND WebKit2_SOURCES
        NetworkProcess/soup/NetworkProcessSoup.cpp
        NetworkProcess/soup/NetworkResourceLoadSchedulerSoup.cpp
        NetworkProcess/soup/RemoteNetworkingContextSoup.cpp

        # Uncomment it after https://bugs.webkit.org/show_bug.cgi?id=118343 is fixed and merged to Nix.
        # Shared/soup/CacheModelHelper.cpp
        Shared/soup/WebCoreArgumentCodersSoup.cpp

        UIProcess/API/C/soup/WKContextSoup.cpp
        UIProcess/API/C/soup/WKSoupRequestManager.cpp

        UIProcess/soup/WebContextSoup.cpp
        UIProcess/soup/WebCookieManagerProxySoup.cpp
        UIProcess/soup/WebSoupRequestManagerClient.cpp
        UIProcess/soup/WebSoupRequestManagerProxy.cpp

        UIProcess/Network/soup/NetworkProcessProxySoup.cpp

        WebProcess/Cookies/soup/WebCookieManagerSoup.cpp
        WebProcess/Cookies/soup/WebKitSoupCookieJarSqlite.cpp

        Shared/Downloads/nix/DownloadSoupErrorsNix.cpp
        Shared/Downloads/soup/DownloadSoup.cpp

        WebProcess/soup/WebProcessSoup.cpp
        WebProcess/soup/WebSoupRequestManager.cpp
        WebProcess/soup/WebKitSoupRequestGeneric.cpp
        WebProcess/soup/WebKitSoupRequestInputStream.cpp
        WebProcess/soup/WebSoupRequestManager.cpp
        WebProcess/soup/WebKitSoupRequestGeneric.cpp
        WebProcess/soup/WebKitSoupRequestInputStream.cpp
        WebProcess/WebCoreSupport/soup/WebFrameNetworkingContext.cpp
    )

    list(APPEND WebKit2_MESSAGES_IN_FILES
        UIProcess/soup/WebSoupRequestManagerProxy.messages.in
        WebProcess/soup/WebSoupRequestManager.messages.in
    )

    list(APPEND WebKit2_INCLUDE_DIRECTORIES
        "${WEBCORE_DIR}/platform/network/soup"
        Shared/soup
        UIProcess/API/C/soup
        UIProcess/soup
        Shared/Downloads/soup
        WebProcess/soup
        WebProcess/WebCoreSupport/soup
        ${LIBSOUP_INCLUDE_DIRS}
    )
    list(APPEND WebKit2_LIBRARIES
        ${LIBSOUP_LIBRARIES}
        rt
    )

    add_custom_target(forwarding-headerNetwork
        COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include soup
    )
endif ()

add_custom_target(forwarding-headerNix
     COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include nix
     COMMAND ${PERL_EXECUTABLE} ${WEBKIT2_DIR}/Scripts/generate-forwarding-headers.pl ${WEBKIT2_DIR} ${DERIVED_SOURCES_WEBKIT2_DIR}/include CoordinatedGraphics
)

set(WEBKIT2_EXTRA_DEPENDENCIES
     forwarding-headerNix
     forwarding-headerNetwork
)

configure_file(nix/WebKitNix.pc.in ${CMAKE_BINARY_DIR}/WebKit2/nix/WebKitNix.pc @ONLY)
set(WebKitNix_HEADERS
    UIProcess/API/nix/NIXEvents.h
    UIProcess/API/nix/NIXView.h
)

macro(PROCESS_HEADER_IFDEFS _file _enable _prefix _output)
    get_filename_component(_absolute "${_file}" ABSOLUTE)
    get_filename_component(_filename "${_file}" NAME)
    set(${_output} "${CMAKE_BINARY_DIR}/${_prefix}/${_filename}")
    file(READ "${_absolute}" _contents)
    foreach (_define ${_enable})
        string(REGEX REPLACE "#if defined\\(${_define}\\)\n*([^\n]*)\n#endif" "\\1" _contents "${_contents}")
    endforeach ()
    file(WRITE "${${_output}}" "${_contents}")
endmacro()

PROCESS_HEADER_IFDEFS(Shared/API/c/WKBase.h "BUILDING_NIX__" "WebKit2" _Shared_API_c_WKBase_h)

set(WebKitNix_WebKit2_HEADERS
    "${_Shared_API_c_WKBase_h}"
    Shared/API/c/WKArray.h
    Shared/API/c/WKCertificateInfo.h
    Shared/API/c/WKConnectionRef.h
    Shared/API/c/WKContextMenuItem.h
    Shared/API/c/WKContextMenuItemTypes.h
    Shared/API/c/WKData.h
    Shared/API/c/WKDeclarationSpecifiers.h
    Shared/API/c/WKDictionary.h
    Shared/API/c/WKError.h
    Shared/API/c/WKEvent.h
    Shared/API/c/WKFindOptions.h
    Shared/API/c/WKGeometry.h
    Shared/API/c/WKImage.h
    Shared/API/c/WKMutableArray.h
    Shared/API/c/WKMutableDictionary.h
    Shared/API/c/WKNumber.h
    Shared/API/c/WKPageLoadTypes.h
    Shared/API/c/WKPageVisibilityTypes.h
    Shared/API/c/nix/WKPopupItem.h
    Shared/API/c/nix/WKPopupMenuListener.h
    Shared/API/c/WKRenderLayer.h
    Shared/API/c/WKRenderObject.h
    Shared/API/c/WKSecurityOrigin.h
    Shared/API/c/WKSerializedScriptValue.h
    Shared/API/c/WKSerializedScriptValuePrivate.h
    Shared/API/c/WKString.h
    Shared/API/c/WKStringPrivate.h
    Shared/API/c/WKType.h
    Shared/API/c/WKURL.h
    Shared/API/c/WKURLRequest.h
    Shared/API/c/WKURLResponse.h
    Shared/API/c/WKUserContentInjectedFrames.h
    Shared/API/c/WKUserContentURLPattern.h
    Shared/API/c/WKUserScriptInjectionTime.h

    Shared/API/c/nix/WKBaseNix.h

    UIProcess/API/C/WKApplicationCacheManager.h
    UIProcess/API/C/WKAuthenticationChallenge.h
    UIProcess/API/C/WKAuthenticationDecisionListener.h
    UIProcess/API/C/WKBackForwardListRef.h
    UIProcess/API/C/WKBackForwardListItemRef.h
    UIProcess/API/C/WKBatteryManager.h
    UIProcess/API/C/WKBatteryStatus.h
    UIProcess/API/C/WKContext.h
    UIProcess/API/C/WKContextConnectionClient.h
    UIProcess/API/C/WKContextDownloadClient.h
    UIProcess/API/C/WKContextHistoryClient.h
    UIProcess/API/C/WKContextInjectedBundleClient.h
    UIProcess/API/C/WKContextPrivate.h
    UIProcess/API/C/WKCookieManager.h
    UIProcess/API/C/WKCredential.h
    UIProcess/API/C/WKCredentialTypes.h
    UIProcess/API/C/WKDatabaseManager.h
    UIProcess/API/C/WKDownload.h
    UIProcess/API/C/WKFormSubmissionListener.h
    UIProcess/API/C/WKFrame.h
    UIProcess/API/C/WKFramePolicyListener.h
    UIProcess/API/C/WKGeolocationManager.h
    UIProcess/API/C/WKGeolocationPermissionRequest.h
    UIProcess/API/C/WKGeolocationPosition.h
    UIProcess/API/C/WKGrammarDetail.h
    UIProcess/API/C/WKHitTestResult.h
    UIProcess/API/C/WKIconDatabase.h
    UIProcess/API/C/WKInspector.h
    UIProcess/API/C/WKKeyValueStorageManager.h
    UIProcess/API/C/WKMediaCacheManager.h
    UIProcess/API/C/WKNativeEvent.h
    UIProcess/API/C/WKNavigationDataRef.h
    UIProcess/API/C/WKNetworkInfo.h
    UIProcess/API/C/WKNetworkInfoManager.h
    UIProcess/API/C/WKNotification.h
    UIProcess/API/C/WKNotificationManager.h
    UIProcess/API/C/WKNotificationPermissionRequest.h
    UIProcess/API/C/WKNotificationProvider.h
    UIProcess/API/C/WKOpenPanelParameters.h
    UIProcess/API/C/WKOpenPanelResultListener.h
    UIProcess/API/C/WKPage.h
    UIProcess/API/C/WKPageContextMenuClient.h
    UIProcess/API/C/WKPageFindClient.h
    UIProcess/API/C/WKPageFindMatchesClient.h
    UIProcess/API/C/WKPageFormClient.h
    UIProcess/API/C/WKPageGroup.h
    UIProcess/API/C/WKPageLoaderClient.h
    UIProcess/API/C/WKPagePolicyClient.h
    UIProcess/API/C/WKPagePrivate.h
    UIProcess/API/C/WKPageUIClient.h
    UIProcess/API/C/WKPluginSiteDataManager.h
    UIProcess/API/C/WKPreferencesRef.h
    UIProcess/API/C/WKPreferencesPrivate.h
    UIProcess/API/C/WKProtectionSpace.h
    UIProcess/API/C/WKProtectionSpaceTypes.h
    UIProcess/API/C/WKResourceCacheManager.h
    UIProcess/API/C/WKSessionRef.h
    UIProcess/API/C/WKTextChecker.h
    UIProcess/API/C/WKVibration.h
    UIProcess/API/C/WebKit2_C.h

    UIProcess/API/cpp/WKRetainPtr.h

    UIProcess/API/CoordinatedGraphics/WKCoordinatedScene.h
    UIProcess/API/C/CoordinatedGraphics/WKView.h
    UIProcess/API/C/WKViewportAttributes.h

    UIProcess/API/C/nix/WKContextNix.h
    UIProcess/API/C/nix/WKErrorNix.h
    UIProcess/API/C/nix/WKPageNix.h

    WebProcess/InjectedBundle/API/c/WKBundle.h
    WebProcess/InjectedBundle/API/c/WKBundlePageContextMenuClient.h
    WebProcess/InjectedBundle/API/c/WKBundlePageDiagnosticLoggingClient.h
    WebProcess/InjectedBundle/API/c/WKBundlePageEditorClient.h
    WebProcess/InjectedBundle/API/c/WKBundlePageFormClient.h
    WebProcess/InjectedBundle/API/c/WKBundlePageFullScreenClient.h
    WebProcess/InjectedBundle/API/c/WKBundlePageLoaderClient.h
    WebProcess/InjectedBundle/API/c/WKBundlePagePolicyClient.h
    WebProcess/InjectedBundle/API/c/WKBundlePageResourceLoadClient.h
    WebProcess/InjectedBundle/API/c/WKBundlePageUIClient.h
    WebProcess/InjectedBundle/API/c/WKBundleBackForwardList.h
    WebProcess/InjectedBundle/API/c/WKBundleBackForwardListItem.h
    WebProcess/InjectedBundle/API/c/WKBundleDOMWindowExtension.h
    WebProcess/InjectedBundle/API/c/WKBundleFrame.h
    WebProcess/InjectedBundle/API/c/WKBundleFramePrivate.h
    WebProcess/InjectedBundle/API/c/WKBundleHitTestResult.h
    WebProcess/InjectedBundle/API/c/WKBundleInitialize.h
    WebProcess/InjectedBundle/API/c/WKBundleInspector.h
    WebProcess/InjectedBundle/API/c/WKBundleNavigationAction.h
    WebProcess/InjectedBundle/API/c/WKBundleNodeHandle.h
    WebProcess/InjectedBundle/API/c/WKBundleNodeHandlePrivate.h
    WebProcess/InjectedBundle/API/c/WKBundlePage.h
    WebProcess/InjectedBundle/API/c/WKBundlePageGroup.h
    WebProcess/InjectedBundle/API/c/WKBundlePageOverlay.h
    WebProcess/InjectedBundle/API/c/WKBundlePagePrivate.h
    WebProcess/InjectedBundle/API/c/WKBundlePrivate.h
    WebProcess/InjectedBundle/API/c/WKBundleRangeHandle.h
    WebProcess/InjectedBundle/API/c/WKBundleScriptWorld.h
)

set(WebKitNix_JavaScriptCore_HEADERS
    "${JAVASCRIPTCORE_DIR}/API/APICast.h"
    "${JAVASCRIPTCORE_DIR}/API/JSBase.h"
    "${JAVASCRIPTCORE_DIR}/API/JSBasePrivate.h"
    "${JAVASCRIPTCORE_DIR}/API/JSCallbackConstructor.h"
    "${JAVASCRIPTCORE_DIR}/API/JSCallbackFunction.h"
    "${JAVASCRIPTCORE_DIR}/API/JSCallbackObject.h"
    "${JAVASCRIPTCORE_DIR}/API/JSCallbackObjectFunctions.h"
    "${JAVASCRIPTCORE_DIR}/API/JSClassRef.h"
    "${JAVASCRIPTCORE_DIR}/API/JSContextRef.h"
    "${JAVASCRIPTCORE_DIR}/API/JSContextRefPrivate.h"
    "${JAVASCRIPTCORE_DIR}/API/JSObjectRef.h"
    "${JAVASCRIPTCORE_DIR}/API/JSObjectRefPrivate.h"
    "${JAVASCRIPTCORE_DIR}/API/JSProfilerPrivate.h"
    "${JAVASCRIPTCORE_DIR}/API/JSRetainPtr.h"
    "${JAVASCRIPTCORE_DIR}/API/JSStringRef.h"
    "${JAVASCRIPTCORE_DIR}/API/JSStringRefBSTR.h"
    "${JAVASCRIPTCORE_DIR}/API/JSStringRefCF.h"
    "${JAVASCRIPTCORE_DIR}/API/JSValueRef.h"
    "${JAVASCRIPTCORE_DIR}/API/JSWeakObjectMapRefInternal.h"
    "${JAVASCRIPTCORE_DIR}/API/JSWeakObjectMapRefPrivate.h"
    "${JAVASCRIPTCORE_DIR}/API/JavaScript.h"
    "${JAVASCRIPTCORE_DIR}/API/JavaScriptCore.h"
    "${JAVASCRIPTCORE_DIR}/API/OpaqueJSString.h"
    "${JAVASCRIPTCORE_DIR}/API/WebKitAvailability.h"
)

install(FILES ${CMAKE_BINARY_DIR}/WebKit2/nix/WebKitNix.pc DESTINATION lib/pkgconfig)
install(FILES ${WebKitNix_HEADERS} DESTINATION include/${WebKit2_OUTPUT_NAME}-${PROJECT_VERSION_MAJOR})
install(FILES ${WebKitNix_WebKit2_HEADERS} DESTINATION include/${WebKit2_OUTPUT_NAME}-${PROJECT_VERSION_MAJOR}/WebKit2)
install(FILES ${WebKitNix_JavaScriptCore_HEADERS} DESTINATION include/${WebKit2_OUTPUT_NAME}-${PROJECT_VERSION_MAJOR}/JavaScriptCore)

add_definitions(-DLIBEXECDIR=\"${CMAKE_INSTALL_PREFIX}/${EXEC_INSTALL_DIR}\"
    -DWEBPROCESSNAME=\"WebProcess\"
    -DPLUGINPROCESSNAME=\"PluginProcess\"
    -DNETWORKPROCESSNAME=\"NetworkProcess\"
)

if (ENABLE_INSPECTOR)
    set(WEBINSPECTORUI_DIR "${CMAKE_SOURCE_DIR}/Source/WebInspectorUI")
    set(WK2_WEB_INSPECTOR_DIR ${CMAKE_BINARY_DIR}/WebKit2/nix/webinspector)
    set(WK2_WEB_INSPECTOR_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DIR})
    add_definitions(-DWK2_WEB_INSPECTOR_DIR="${WK2_WEB_INSPECTOR_DIR}")
    add_definitions(-DWK2_WEB_INSPECTOR_INSTALL_DIR="${WK2_WEB_INSPECTOR_INSTALL_DIR}/webinspector")
    add_custom_target(
        wk2-web-inspector-resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${WEBINSPECTORUI_DIR}/UserInterface/ ${WK2_WEB_INSPECTOR_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${WEBINSPECTORUI_DIR}/Localizations/en.lproj/localizedStrings.js ${WK2_WEB_INSPECTOR_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy ${WEBKIT2_DIR}/UIProcess/InspectorServer/front-end/inspectorPageIndex.html ${WK2_WEB_INSPECTOR_DIR}
        DEPENDS WebCore
    )
    install(DIRECTORY ${WK2_WEB_INSPECTOR_DIR}
        DESTINATION ${WK2_WEB_INSPECTOR_INSTALL_DIR}
        FILES_MATCHING PATTERN "*.js"
                       PATTERN "*.html"
                       PATTERN "*.css"
                       PATTERN "*.gif"
                       PATTERN "*.png")
endif ()

if (ENABLE_NETWORK_PROCESS)
    set(NetworkProcess_EXECUTABLE_NAME NetworkProcess)
    list(APPEND NetworkProcess_INCLUDE_DIRECTORIES
        "${WEBKIT2_DIR}/NetworkProcess"
    )

    include_directories(${NetworkProcess_INCLUDE_DIRECTORIES})

    list(APPEND NetworkProcess_SOURCES
        unix/NetworkMainUnix.cpp
    )

    set(NetworkProcess_LIBRARIES
        WebKit2
    )

    add_executable(${NetworkProcess_EXECUTABLE_NAME} ${NetworkProcess_SOURCES})
    target_link_libraries(${NetworkProcess_EXECUTABLE_NAME} ${NetworkProcess_LIBRARIES})
    install(TARGETS ${NetworkProcess_EXECUTABLE_NAME} DESTINATION "${EXEC_INSTALL_DIR}")
endif ()

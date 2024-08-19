include(FindEXPAT)
find_package(EXPAT REQUIRED)

# Try to find WebRTC code based on PKG_CONFIG_PATH.
# This ugly hack is needed because WebRTC lies in some very odd location when building from JHBuild.
string(REPLACE ":" "/../../../Source/libwebrtc/trunk;" _webrtc_include_hint "$ENV{PKG_CONFIG_PATH}")
# Also adding /usr/include/libwebrtc, in case it exists.
list(APPEND _webrtc_include_hint "/usr/include/libwebrtc;")

string(REPLACE ":" "/../../libwebrtc;" _webrtc_library_hint "$ENV{PKG_CONFIG_PATH}")
# Also looking at the usual /usr/lib, in case libWebRTC is there.
list(APPEND _webrtc_library_hint "/usr/lib;")

find_path(WEBRTCLIB_INCLUDE_DIRS
    NAMES talk/app/webrtc/mediaconstraintsinterface.h
    HINTS ${_webrtc_include_hint}
    NO_DEFAULT_PATH
)

find_library(WEBRTCLIB_LIBRARIES
    NAMES WebRTC
    HINTS ${_webrtc_library_hint}
    NO_DEFAULT_PATH
)

list(APPEND WEBRTCLIB_LIBRARIES ${EXPAT_LIBRARIES})
find_package_handle_standard_args(WebRTC DEFAULT_MSG WEBRTCLIB_INCLUDE_DIRS WEBRTCLIB_LIBRARIES)

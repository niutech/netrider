find_package(X11 REQUIRED)

set(netrider_DIR "${TOOLS_DIR}/netrider")

add_definitions(-std=c++0x)

set(netrider_SOURCES
    netrider.cpp
)

set(netrider_LIBRARIES
    WebKit2
    ${OPENGL_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${X11_LIBRARIES}
    ${PNG_LIBRARY}
    fltk
    fltk_images
    fltk_gl
    fontconfig
    dl
    Xfixes
    Xft
    Xinerama
)

set(netrider_INCLUDE_DIRECTORIES
    ${CMAKE_BINARY_DIR}
    ${GLIB_INCLUDE_DIRS}
    ${DERIVED_SOURCES_WEBKIT2_DIR}/include
    ${WEBKIT2_DIR}/UIProcess/API/nix
    ${WEBKIT2_DIR}/Shared/API/c
    ${WEBKIT2_DIR}/UIProcess/API/C
    ${WEBKIT2_DIR}/UIProcess/API/C/nix
    ${WEBKIT2_DIR}/UIProcess/API/C/soup
    ${WEBKIT2_DIR}
    ${CMAKE_SOURCE_DIR}/Source
    ${WTF_DIR}
    ${X11_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${JAVASCRIPTCORE_DIR}
    ${PNG_INCLUDE_DIRS}
)

include_directories(${netrider_INCLUDE_DIRECTORIES})

add_executable(netrider ${netrider_SOURCES})
target_link_libraries(netrider ${netrider_LIBRARIES})

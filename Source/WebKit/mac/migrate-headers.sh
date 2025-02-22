#
# Copyright (C) 2006, 2014 Apple Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
#

mkdir -p "${TARGET_BUILD_DIR}/${PRIVATE_HEADERS_FOLDER_PATH}"
mkdir -p "${TARGET_BUILD_DIR}/${PUBLIC_HEADERS_FOLDER_PATH}"
mkdir -p "${BUILT_PRODUCTS_DIR}/DerivedSources/WebKit"

# If we didn't build WebCore, use the production copy of the headers
if [ ! -d "${WEBCORE_PRIVATE_HEADERS_DIR}" ]; then
    export WEBCORE_PRIVATE_HEADERS_DIR="`eval 'echo ${WEBCORE_PRIVATE_HEADERS_DIR_'${PLATFORM_NAME}'_Production}'`"
fi

# If we didn't build WebKit2, use the production copy of the headers
if [ ! -d "${WEBKIT2_FRAMEWORKS_DIR}/WebKit2.framework" ]; then
    export WEBKIT2_FRAMEWORKS_DIR="`eval 'echo ${WEBKIT2_FRAMEWORKS_DIR_Production}'/WebKit2.framework`"
fi

if [ "${ACTION}" = "build" -o "${ACTION}" = "install" -o "${ACTION}" = "installhdrs" ]; then
    make -C mac -f "MigrateHeaders.make" -j `/usr/sbin/sysctl -n hw.activecpu`
fi

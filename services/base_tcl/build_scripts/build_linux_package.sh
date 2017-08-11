#!/usr/bin/env bash

# Copyright AllSeen Alliance. All rights reserved.
#
#    Permission to use, copy, modify, and/or distribute this software for any
#    purpose with or without fee is hereby granted, provided that the above
#    copyright notice and this permission notice appear in all copies.
#
#    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
#    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
#    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
#    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
#    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
#    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# This script collects all the pieces to make the SDK for the cpp builds

set -eux

env | sort

if [[ -z "${AJ_ROOT+notempty}" ]]; then
   AJ_ROOT="$(pwd)/../../.."
fi

[ -d "${WORKSPACE}" ] || { echo "WORKSPACE does not exist"; exit 1; }

zip_base="alljoyn-tcl-service-framework-${BUILD_VERSION}-linux-x86_64-sdk-${variant}"
sdk_dir="${WORKSPACE}/${zip_base}"

rm -rf $sdk_dir
mkdir -p $sdk_dir

##
## Generate Doxygen docs
##
doxyfile=Doxyfile

pushd "${AJ_ROOT}/services/base_tcl"
#pushd services/base_tcl/$SERVICE # todo
rm -rf html dist/docs/html
doxygen $doxyfile ### 2> doxy.out >> /dev/null
mkdir -p dist/docs || : ok
mv -f html dist/docs/html
cp -rp dist/*    $sdk_dir
popd

pushd "${WORKSPACE}"
rm -rf $zip_base.zip
zip -q -r $zip_base.zip $zip_base
md5sum $zip_base.zip > md5sum-$zip_base.txt
popd

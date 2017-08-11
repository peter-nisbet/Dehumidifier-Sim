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

set -eux

function finish {
   popd
}
trap finish EXIT

SCRIPT_DIR=$( dirname "${BASH_SOURCE[0]}" )
pushd "${SCRIPT_DIR}"

while getopts 'V:W:S:' opts; do
   case ${opts} in
      V) export VARIANT=${OPTARG} ;;
      W) export WS=${OPTARG} ;;
      S) export SERVICE=${OPTARG} ;; # TODO explict service build
      *) echo "Invalid argument: ${OPTARG}"; #exit 1  # illegal option
   esac
done

if [[ -z "${AJ_ROOT+notempty}" ]]; then
   AJ_ROOT="$(pwd)/../../.."
fi

if [[ -z "${VARIANT+notempty}" ]]; then
    VARIANT="debug"
fi

if [[ -z "${WS+notempty}" ]]; then
    WS="off"
fi

# build core
pushd "${AJ_ROOT}/core/ajtcl"
scons -c VARIANT="$VARIANT" WS="$WS"
scons VARIANT="$VARIANT" WS="$WS" --config=force
popd

# build service
pushd "${AJ_ROOT}/services/base_tcl"
#pushd services/base_tcl/$SERVICE # TODO explict service build
scons -c VARIANT="$VARIANT" WS="$WS" docs=html
scons VARIANT="$VARIANT" WS="$WS" docs=html --config=force
popd

# packaging
./build_linux_package.sh

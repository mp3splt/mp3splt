#!/bin/sh

cd $(readlink -f $(dirname $(readlink -f $0))/..)
if [ -n "$1" ]; then
    APPEND="$1"
else
    APPEND="0"
fi
UPSTREAM_VERSION=$(dpkg-parsechangelog | awk '/^Version:/{sub("-.+$", "", $2); print $2}')
# TODO: make this work without requiring gawk
NEXT_UPSTREAM=$(echo ${UPSTREAM_VERSION} | gawk '{split($0, a, "."); sub(a[length(a)] "$",  a[length(a)] + 1, $0); print}')
echo "libmp3splt$APPEND (>= ${UPSTREAM_VERSION}), libmp3splt$APPEND (<< ${NEXT_UPSTREAM}~)"

#!/bin/sh

export BASE_DIR="`dirname $0`"
top_dir="$BASE_DIR/.."

if test -z "$NO_MAKE"; then
    make -C $top_dir > /dev/null || exit 1
fi

if test -z "$CUTTER"; then
    CUTTER="`make -s -C $BASE_DIR echo-cutter`"
fi

($CUTTER --version) > /dev/null 2>&1 ||
{
  echo 1>&2
  echo "Error: '$CUTTER' not found." 1>&2
  echo "       Please install the cutter testing framework or configure with '--disable-cutter'" 1>&2
  echo 1>&2

  exit 1
}

$CUTTER -s $BASE_DIR "$@" $BASE_DIR


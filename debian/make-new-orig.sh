#!/bin/sh

set -e

if [ -z $1 ]; then
	echo "Please provide the directory to tar up as an argument"
	exit 1
fi

TARBALL=$(echo $1 | tr - _).orig.tar.gz

tar -czvf $TARBALL --exclude=debian $1


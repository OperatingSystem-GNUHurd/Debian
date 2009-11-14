#!/bin/sh

set -e

if [ -z $1 ]; then
	echo "Please provide the directory to tar up as an argument"
	exit 1
fi

BASENAME=$(basename $1)
DIRNAME=$(dirname $1)

tar -czvf hurd.tgz \
    --exclude=.git --exclude=.gitignore --exclude=debian \
    -C $DIRNAME $BASENAME


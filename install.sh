#!/bin/sh

if [ "$#" -eq 0 ]; then
	echo "Usage: $0 install_path"
	exit 1
fi

cp -v *.py "$1/"
sync

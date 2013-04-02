#!/bin/bash

for file in $(ls)
do
	if [ "$file" != "$0" ]; then
		mv $file $file.old
		iconv -f gbk -t utf8 $file.old > $file
	fi
done


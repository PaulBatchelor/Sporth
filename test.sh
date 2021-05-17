#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
   MD5=md5sum
else
   MD5=md5
fi 

test_example() {
    ./sporth -b raw examples/$1 | $MD5 | grep -q $2

    if [ $? == 0 ]; then
        echo "$1 PASSED"
    else
        echo "$1 FAILED. Expected $2. Got `./sporth -b raw examples/$1 | $MD5`"
    fi 
}

test_example dialtone.sp 4ec984e80addbbef6a77e99d1029a726
test_example crossfade.sp a66b3eefa1c442d83dec94bf245b9e99 

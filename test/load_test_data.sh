#!/bin/bash

FOLDER_PATH="test_data"

if [ -d "$FOLDER_PATH" ]; then
    cd $FOLDER_PATH
    git pull
    cd ..
else
    git clone -b v1.0.0 https://github.com/joda01/imagec_test.git test_data
    cd test_data
    git lfs fetch --all
    git lfs checkout
    cd ..
fi

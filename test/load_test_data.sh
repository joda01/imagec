#!/bin/bash

FOLDER_PATH="test_data"

if [ -d "$FOLDER_PATH" ]; then
    cd $FOLDER_PATH
    git pull
    cd ..
else
    git clone https://github.com/joda01/imagec_test.git test_data
fi

#!/bin/bash

SKETCH_PATH="$1"
BOARD_LIST_OUTPUT=$(arduino-cli board list --format json)
FQBN=$(echo $BOARD_LIST_OUTPUT | sed -n "s/.*\"fqbn\": \"\([0-9A-Za-z\-\:]*\)\".*/\1/p")

if [[ ! "$FQBN" ]]; then
    echo "❌ No device found. Have you plugged in your board?"
    exit 1
fi

DEVICE_NAME=$(echo $BOARD_LIST_OUTPUT | sed -n "s/.*\"name\": \"\([0-9A-Za-z\-\ ]*\)\".*/\1/p")
PORT_NAME=$(arduino-cli board list | grep $FQBN  -m 1 | awk -F' ' '{print $1}')
echo "✅ Found device '$DEVICE_NAME' with FQBN $FQBN at $PORT_NAME"

echo "🔧 Compiling for $FQBN ..."
cd $SKETCH_PATH
arduino-cli compile --fqbn $FQBN --libraries "../../"

if [ $? -eq 0 ]; then
    echo "🆙 Uploading to $PORT_NAME..."
    arduino-cli upload -p $PORT_NAME --fqbn $FQBN
fi
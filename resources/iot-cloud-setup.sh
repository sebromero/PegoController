#!/bin/bash
# SEE: https://github.com/arduino/arduino-cloud-cli
# SEE: https://github.com/bcmi-labs/iot-api/blob/master/internal/property/service/property_types.yaml

DEVICE_NAME="Pegolino"
THING_NAME="Cold Store Monitoring"

IOT_CLOUD_SKETCH_PATH="../examples/ColdStoreMonitoring"
THING_FILE="./thing-monitoring.yml"
DASHBOARD_FILE="./dashboard-monitoring.yml"
DASHBOARD_NAME=$THING_NAME
DASHBOARD_FILE_MINIMAL="./dashboard-monitoring-minimal.yml"
DASHBOARD_NAME_MINIMAL="$THING_NAME (minimal)"

# Check if commands exist

command -v arduino-cli >/dev/null 2>&1 || { echo >&2 "❌ arduino-cli is required but it's not installed."; exit 1; }
command -v arduino-cloud-cli >/dev/null 2>&1 || { echo >&2 "❌ arduino-cloud-cli is required but it's not installed."; exit 1; }

# Check if credentials exist
arduino-cloud-cli credentials find > /dev/null 2>&1

if [ $? -ne 0 ]; then    
    arduino-cloud-cli credentials init
else
    echo "✅ Arduino IoT Cloud credentials found."
fi

# Create build folder to store json response files
mkdir "build" > /dev/null 2>&1

# Check if thing is already created
arduino-cloud-cli thing list --format json | grep '"name": "$THING_NAME"' > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "Creating thing '$THING_NAME' ..."
    arduino-cloud-cli thing create --name "$THING_NAME" --template $THING_FILE --format json > build/thing.json
    if [ $? -ne 0 ]; then
        echo "❌ Couldn't create thing '$THING_NAME'"
        exit 1
    fi
else
    echo "✅ Thing '$THING_NAME' already exists."
fi

THING_ID=$(cat build/thing.json | sed -n "s/.*\"id\": \"\([0-9A-Za-z\-]*\)\".*/\1/p")

function maybeCreateDashboard(){
    local dashboardName=$1
    local dashboardFile=$2
    local thingID=$3
    local outputFilename=$4

    # Check if dashboard already exists
    arduino-cloud-cli dashboard list --format json | grep '"name": "$dashboardName"' > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Creating dashboard '$dashboardName' using thing ID $THING_ID ..."
        arduino-cloud-cli dashboard create --name "$dashboardName" --template $dashboardFile --override template-thing-id=$thingID --format json > $outputFilename
        if [ $? -ne 0 ]; then
            echo "❌ Couldn't create dashboard '$dashboardName'"
            exit 1
        fi
    else
        echo "✅ Dashboard '$dashboardName' already exists."
    fi
}

maybeCreateDashboard "$DASHBOARD_NAME" "$DASHBOARD_FILE" "$THING_ID" "build/dashboard.json"
maybeCreateDashboard "$DASHBOARD_NAME_MINIMAL" "$DASHBOARD_FILE_MINIMAL" "$THING_ID" "build/dashboard-minimal.json"

# Check if device is already registerd to Arduino IoT Cloud
BOARD_LIST_OUTPUT=$(arduino-cli board list --format json)
DEVICE_SERIAL_NUMBER=$(echo "$BOARD_LIST_OUTPUT" | sed -n "s/.*\"serialNumber\": \"\([0-9A-Za-z\-]*\)\".*/\1/p")

if [[ ! $DEVICE_SERIAL_NUMBER ]]; then
    echo "❌ Device serial number of attached device could not be retrieved."
    exit 1
fi

arduino-cloud-cli device list | grep $DEVICE_SERIAL_NUMBER > /dev/null 2>&1

if [ $? -ne 0 ]; then    
    echo "Registering device with SN $DEVICE_SERIAL_NUMBER with Arduino IoT Cloud..."
    arduino-cloud-cli device create --name "$DEVICE_NAME" --format json > build/device.json
    if [ $? -ne 0 ]; then
        echo "❌ Couldn't register your device"
        exit 1
    fi
else
    echo "✅ Device with SN $DEVICE_SERIAL_NUMBER already registered with Arduino IoT Cloud."
fi

# Check if device is already associated
DEVICE_ID=`cat build/device.json | sed -n "s/.*\"id\": \"\([0-9A-Za-z\-]*\)\".*/\1/p"`

if [[ ! $DEVICE_ID ]]; then
    echo "❌ Device ID of attached device could not be read."
    exit 1
fi

THING_LIST_OUTPUT=`arduino-cloud-cli thing list --device-id "$DEVICE_ID" --format json`
ASSOCIATED_THING_ID=`echo "$THING_LIST_OUTPUT" | sed -n "s/.*\"id\": \"\([0-9A-Za-z\-]*\)\".*/\1/p"`

if [ "$THING_LIST_OUTPUT" = "null" ]; then
    echo "Binding attached device to thing..."
    arduino-cloud-cli thing bind --id $THING_ID --device-id $DEVICE_ID
elif [ "$THING_ID" = "$ASSOCIATED_THING_ID" ]; then
    echo "✅ Device already associated with thing ID $THING_ID"
else
    echo "❌ Device is already associated with other thing ID $ASSOCIATED_THING_ID"
    exit 1
fi


# Compile and flash firmware to the connected board
read -p "🆙 Would you like to upload the IoT Cloud firmware to you board? [y/n] " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    FQBN=$(echo $BOARD_LIST_OUTPUT | sed -n "s/.*\"fqbn\": \"\([0-9A-Za-z\_\-\:]*\)\".*/\1/p")

    if [[ $FQBN == *"arduino:samd:mkrgsm1400"* ]]; then
        echo "Using default Arduino SIM credentials."
        echo "Writing values to arduino_secrets.h ..."
        echo "#define SECRET_APN \"prepay.pelion\"" > "$IOT_CLOUD_SKETCH_PATH/arduino_secrets.h"
        echo "#define SECRET_PIN \"0000\"" >> "$IOT_CLOUD_SKETCH_PATH/arduino_secrets.h"
        echo "#define SECRET_USERNAME \"arduino\"" >> "$IOT_CLOUD_SKETCH_PATH/arduino_secrets.h"
        echo "#define SECRET_PASSWORD \"arduino\"" >> "$IOT_CLOUD_SKETCH_PATH/arduino_secrets.h"
    elif [[ $FQBN == *"arduino:samd:mkrwan"* ]]; then
        echo "Enter your LoRaWAN App EUI:"
        read APP_EUI
        echo "Enter your LoRaWAN App Key:"
        read APP_KEY
        echo "Writing values to arduino_secrets.h ..."
        echo "#define SECRET_APP_EUI \"$APP_EUI\"" > "$IOT_CLOUD_SKETCH_PATH/arduino_secrets.h"
        echo "#define SECRET_APP_KEY \"$APP_KEY\"" >> "$IOT_CLOUD_SKETCH_PATH/arduino_secrets.h"
    else
        echo "Enter your WiFi SSID: "
        read WIFI_SSID
        echo "Enter your WiFi password: "
        read WIFI_PASSWORD
        echo "Writing values to arduino_secrets.h ..."
        echo "#define SECRET_SSID \"$WIFI_SSID\"" > "$IOT_CLOUD_SKETCH_PATH/arduino_secrets.h"
        echo "#define SECRET_PASS \"$WIFI_PASSWORD\"" >> "$IOT_CLOUD_SKETCH_PATH/arduino_secrets.h"
    fi

    ./build-and-upload.sh $IOT_CLOUD_SKETCH_PATH
fi

echo "✅ Done"
<!--
  Copyright (c) 2024 Eclipse Foundation
 
  This program and the accompanying materials are made available 
  under the terms of the MIT license which is available at
  https://opensource.org/license/mit.
 
  SPDX-License-Identifier: MIT
 
  Contributors: 
      Frédéric Desbiens - Initial version.

-->

# Eclipse ThreadX hackathon challenge - uProtocol / ThreadX

## Application 

### Microcontroller

The application initializes a wifi connection, connects to an MQTT broker and subscribes to the topic
```
"d/Cloud/3039/1/8001"
```
which is a topic message from a cloud directed at this device will be received. This conforms to the uProtocol spec (https://github.com/eclipse-uprotocol/up-spec). This message will trigger an Honk&Flash operation where the LED of the microcontroller will blink. The application will also consistently monitor acceleration and will send  uMessage to the cloud via the topic
```
"d/MXChip/5BA0/1/8001"
```
indicating to the backend that a crash has happend. The backend will then forward this to the Mobile App. In a real scenario this could be an alternative communication path for the emergency call which should be much faster than traditionell SMS.

#### Architecture

*Deployment view*

![Description of SVG](sdv.svg)

The application uses ThreadX and spwawns 3 threads. A fourth (Logging thread) is optional.

- A networking thread

Networking thread sets up the Wifi and MQTT connection. An event flag
```
    status = tx_event_flags_create(&mqtt_app_flag, "my app event");
```
is used to signal two conditions to the networking thread. It either shows that

a) A message is ready to be reveived 
b) A message is ready to be send

Upon message receiption the command is send to the Honk&Flash thread via the mqtt queue which is created as follows:

```
    UINT status = tx_queue_create(&queue_mqtt, "queue 0", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE * sizeof(ULONG));
```

- Crash detection thread

Constantly measures the accelerometer data and signals a crash via the event_flag to the networking thread. This thread runs with an elevated priority since it has to meet certain timing constraints. 

- Honk & Flash Thread

Listens on the mqtt_queue and starts the honk and flash process when it receives the message:

```
        queue_status = tx_queue_receive(&queue_mqtt, &received_message, TX_WAIT_FOREVER);
```

To work on the microcontroller protobuf fields with a variable length like BYTES, STRING etc. we limit those types to a length of 128 bytes.  

* Shortcomings / Workarounds*

To overcome the problem that the MQTT client only support version 3.1 uMessages are serialized via protobuf to their wire format and send as the payload of the MQTT message. This does not directly conform to the uTransport but it would be a compatibel addition to it. We also patched the https://github.com/eclipse-uprotocol/up-transport-mqtt5-rust/ to support the transportation as payload.  

### Backend

A backend application written in Rust which leverages a patched version of https://github.com/eclipse-uprotocol/up-transport-mqtt5-rust/ to subscribe to the MQTT broker and converts the msg payloads to uMessages (protobuf) and dispatches them to the mobile application. It will also send the honk&flash commands downstream.

### Android App

Mobile application which talks to the backend via HTTP(S) and sends uProtocol (Honk&Flash) messages to the device. It also receives and displays the crash detection events.


## Cloning this repository
Eclipse ThreadX and Eclipse ThreadX NetX Duo are included as submodules.

When cloning, you must specify the `--recurse-submodules` option to get the code for the submodules. If you forget this option, just run the following commands in the root folder of your clone. 

```
git submodule init
git submodule update --recursive
```

## Prerequisites

### Computer
Theoretically, any recent laptop running Windows 11, Linux, or MacOS should do.

The board code was developed with MacOS running on Mac M3 chips. 

### Developer tools
In terms of tooling, all you need to work on the challenge is CMake, Ninja, and a suitable C compiler. Naturally, having Git installed could help as well. ;-)

The source code for ThreadX and related modules is very portable and compliant with all "required" and "mandatory" rules of MISRA-C:2004 and MISRA C:2012. Most modern C compilers should be able to compile it. The official build pipelines rely on Arm's embedded GNU toolchain.

If you wish to generate the proto C bindings you can use nanopb (https://github.com/nanopb/nanopb). There is a short description on what has to be done in the file 
```
protobuf_generation.txt
```


Below are instructions to install the tools.

**OSX**
We assume that brew is used as a package manager. To install the ARM toolchain use:

```
brew install --cask gcc-arm-embedded
```

to use command line flashing also install openocd and for debugging we used minicom. 

**Ubuntu**
```
apt install ninja-build cmake 
```

Then, download and install Arm's embedded GNU toolchain, available at [https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

The following will download and unpack version 13.3.rel1 of the software to `/opt`.
``` 
wget https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz
sudo tar xJf arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz -C /opt
```

To test, you can run the following commands:
```
export PATH=$PATH:/opt/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi/bin
arm-none-eabi-gcc --version
```

**Windows**
```
winget install --id=Arm.GnuArmEmbeddedToolchain  -e
winget install --id=Ninja-build.Ninja  -e
winget install --id=Kitware.CMake  -e
```

## Compiling and running the application

### Microcontroller
To compile the application, simply execute the relevant script found in the `MXChip/AZ3166/scripts` folder. You can use either build.sh or build_fresh.sh where the latter one does a full recompile. 

To flash the application onto the device you can use 
```
flash.sh
```
### Backend

### Mobile App

### WiFi configuration
To connect the board to a WiFi network, edit the following constants found in `cloud_config.h`:

- `HOSTNAME`
- `WIFI_SSID`
- `WIFI_PASSWORD`

Make sure to select an appropriate value for `WIFI_MODE` as well.

If the WiFi is properly configured, you will get the output below at application startup:

> ```
>Initializing WiFi
>        MAC address: C8:93:46:87:88:D1
>SUCCESS: WiFi initialized
>
>Connecting WiFi
> ```

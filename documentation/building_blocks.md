# Building Blocks

## MXChip Microcontroller Firmware

The Microcontroller of type MXAZ is shown in schema here:

![Description of MXAZ](static/mxchip.svg)

It has a few LEDs, a small display and some sensors. The data from the sensors goes into the display and the acceleration data is used to detect crashes. If a crash is detected we use the boards WIFI connection to send out UMessages defined by protobuff over MQTT to the backend.

## The Backend

The schema of the backend service can be seen here:

![Description of backend](static/backend.svg)

The backend is written in rust. It provides one thread thats listening to GET or POST requests from the app, and one UTransport client thats listening to MQTT UMessages from the microcontroller and sends UMessages to it.
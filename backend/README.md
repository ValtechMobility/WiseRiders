# The Backend

## Run the backend:

Make sure that you have an available MQTT broker. For testing purposes you can use one provided by Mosquitto under test.mosquitto.org port 1883.
You can change the constants at the beginning of the main function to change them if you want.

To run the backend service just run

```bash
cargo run
```

## What does this backend do?

The backend consists of two threads. One that connects to the MXChip via the UProtocol MQTT transport, and the other one that connects to the app via a very basic TCP connection.
It provides the app with two endpoints via TCP:
- GET checks the status of the global boolean "ACCIDENT_STATUS" variable and returns either with "accident" or "no accident"
- POST triggers a signal to the MXChip which causes it to flash an LED (honk and flash)

The other thread thats connected with the MXChip over UProtocol does two things as well:
- It sends the honk and flash signal to the MXChip if triggered via the POST TCP request
- It listens to UMessages from the MXChip which get sent in the moment that an accident happens and changes the ACCIDENT_STATUS variable accordingly

## Why is the up-transport-mqtt library included explicitly here?

The MQTT library that runs on the MXChip is not capable of sending messages with the MQTT5 and is limited to MQTT3.1. This means that UTransport cant convert its messages out of the box, since a proper UMessage needs to contain information in the Message Properties section of the message which does not exist in MQTT3.1 messages. For this reason we had to include a fallback in the up-transport-mqtt library where the Message Properties can alternatively be provided via the messages payload. The transport library then takes the properties and recovers the actual UMessages that was sent. This addition to the up-transport-mqtt library can become a proper feature in the future but I dont think we can get the PR merged before the hackathon ends :)
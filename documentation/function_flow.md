# Functional Flow

## Flow of the accident detect function

![Flow of the accident detect](static/accident_detect.svg)

The flow of the accident detect function can be traced in these steps:

1. a new acceleration measure is sent by the sensor
2. the value of the acceleration is compared to a threshold
3. if the threshold is passed then a UMessage object is created with the status report "accident"
4. the UMessage is sent via MQTT to the backend
5. the backend changes its internal "accident_status" flag to true
6. the android app periodically polls the accident status by sending a GET request
7. if the flag is set to "true" then the response from the backend is "accident reported"
8. the app flashes red and warns that a crash has happened

## Honk and Flash

![Flow of honk and flash](static/honkandflash.svg)

The flow of the accident detect function can be traced in these steps:

1. a honk and flash request is made by the app by sending a POST to the backend
2. the POST messages triggers the creation of a UMessage
3. the UMessage is sent via MQTT to the microchip
4. the controllers gets the message
5. the controllers LED flashes red for 6 seconds
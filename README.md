# Homebridge MQTT-Thing ESP8266 WS2812 Controller

This program lets you control WS2812 LED stripes connected to an ESP8266 via Homebridge and the Homebridge MQTT-Thing plugin. This way you can control a WS2812 LED stripe via the iOS Home.app or Siri.

## Why this?

I was following [Studio Pieter's HOWTO](https://www.studiopieters.nl/homebridge-homekit-2-mqtt-revised-installation/) for controlling a WS2812 LED stripes, which uses the `homekit2mqtt` Homekit bridge. But I didn't want another Homekit bridge since I already had [Homebridge](https://homebridge.io) installed and in use. So I adjusted the code from [Studio Pieter's NeoPixel-MQTT HOWTO](https://www.studiopieters.nl/homebridge-mqtt-neopixel/) to use the direct HSV values from Homebridge MQTT-Thing plugin and set the LEDs.

## Preconditions

* Platform.io installation
* Homebridge installation
* MQTT Broker installation running in your network
* Homebridge MQTT-Thing plugin installes in Homebridge

For installing Homebridge and MQTT Broker, follow the corresponding parts in [https://www.studiopieters.nl/homebridge-homekit-2-mqtt-revised-installation/](https://www.studiopieters.nl/homebridge-homekit-2-mqtt-revised-installation/). Install Homebridge MQTT-Thing via the Homebridge UI.

## Configuration

### ESP8266 Code

* Adjust the variables at the top of `main.ino` to your needs
* Install the NeoPixel library to the project: `platformio lib install --save "Adafruit NeoPixel"`
* Compile the project: `platformio run`
* Upload the build to your ESP8266

### Homebridge

* You need to configure a Lightbulb - HSV item in the Homebridge MQTT-Thing plugin.

Add this to the `accessories` array on your Homebridge config:

``` json
{
    "type": "lightbulb-HSV",
    "name": "LED Stripe",
    "topics": {
        "getOnline": "led-nodemcu/online",
        "setHSV": "led-nodemcu/hsvcolor",
        "getHSV": "led-nodemcu/hsvcolor",
        "getOn": "led-nodemcu",
        "setOn": "led-nodemcu"
    },
    "confirmationIndicateOffline": true,
    "integerValue": false,
    "accessory": "mqttthing"
}
```

Adjust `led-nodemcu` above if you changed the `host` variable in `main.ino`. Also adjust the name to your needs.

## Acknowledgements and sources

* This is (more or less) heavily based on the code from [https://www.studiopieters.nl/homebridge-mqtt-neopixel/](https://www.studiopieters.nl/homebridge-mqtt-neopixel/), but heavily modified.
* I also used parts from [https://www.studiopieters.nl/homebridge-homekit-2-mqtt-revised-installation/](https://www.studiopieters.nl/homebridge-homekit-2-mqtt-revised-installation/)
* `PubSubClient` library included in this repo by [https://github.com/Imroy/pubsubclient](https://github.com/Imroy/pubsubclient)
* `StringTokenizer` library included in this repo by [https://github.com/syalujjwal/Arduino-StringTokenizer-Library](https://github.com/syalujjwal/Arduino-StringTokenizer-Library)
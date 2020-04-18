/* 
  Project name: homebridge-mqttthing-esp8266-ws2812
  Project URI: https://github.com/deg0nz/
  Description: Control WS2812 LEDs with ESP8266 and Homebridge MQTT-Thing via MQTT
  Version: 1.0.0
  License: MIT
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include <StringTokenizer.h>

#define PIN D2 // The pin where you connect the neopixel dataline
#define NUMPIXELS 60 // How many NeoPixels are attached

const char* ssid = "MYAWESOMEWIFI";
const char* password = "MYSUPERSECRETPASSWORD";
const char* host = "led-nodemcu"; // the name of this device, and the mqtt base channel to listen to
IPAddress MQTTserver(192, 168, 0, 100);

// Comment, if you don't want serial output
#define DEBUG

/* Usually, there is nothing to adjust beneath this line*/
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint16_t hue = 0; 
uint8_t saturation = 0;
uint8_t value = 0; 
uint32_t mappedHSV = 0;
uint8_t lastKnownBrightness = 100; // Start with half of the brightness on boot
bool withHsv = false;

uint16_t pixelCounter;

StringTokenizer tokenizer("foo", ",");
uint8_t tokenCounter;

String myTopic;
String myMessage;

#define BUFFER_SIZE 100

WiFiClient wclient;
PubSubClient client(wclient, MQTTserver, 1883);

void fillLeds(int hue, float saturation, float value) {
  mappedHSV = strip.gamma32(
    strip.ColorHSV(
      map(hue, 0, 360, 0, 65535),
      map(saturation, 0, 100, 0, 255),
      map(value, 0, 100, 0, 255)
    ));

  for (pixelCounter = 0; pixelCounter < strip.numPixels(); pixelCounter++) {
    strip.setPixelColor(pixelCounter, mappedHSV);
  }
  strip.show();
}

void setHsvValuesFromPayload(String payload) {
  tokenCounter = 0;
  tokenizer.setString(payload);
  while(tokenizer.hasNext()) {
    switch (tokenCounter) {
        case 0:
          hue = tokenizer.nextToken().toInt();
          break;
        case 1:
          saturation = tokenizer.nextToken().toInt();
          break;
        case 2:
          value = tokenizer.nextToken().toInt();
          break;
        default:
          return;
    }
    tokenCounter++;
  }
}

void printCurrentValues() {
  Serial.println("");
  Serial.println("Current State");
  Serial.print("Hue (0-360):");
  Serial.println(hue);
  Serial.print("Saturation (0-100):");
  Serial.println(saturation);
  Serial.print("Brightness (0-100):");
  Serial.println(value);
  Serial.println("");
}

void mqttCallback(const MQTT::Publish& pub) {

  myMessage = String(pub.payload_string());
  myTopic = String(pub.topic());

#ifdef DEBUG
  Serial.println("=================================================");
  // handle message arrived
  Serial.print("Incoming message on ");
  Serial.print(pub.topic());
  Serial.print(" => ");
#endif

  if (myTopic == host + (String)"/hsvcolor") 
  {
#ifdef DEBUG
    Serial.println(pub.payload_string());
#endif
    withHsv = true;
    setHsvValuesFromPayload(pub.payload_string());
  }
  else if (myTopic == host)
  {

#ifdef DEBUG
    Serial.print(pub.payload_string());
#endif

    if (pub.payload_string() == "true")
    {
      if(withHsv == true) 
      {
        return;
      }
      else
      {
        value = lastKnownBrightness;
      }
    }
    else
    {
      if(withHsv == true) 
      {
        lastKnownBrightness = value;
      }
      value = 0;
    }

    withHsv = false;
  }

  fillLeds(hue, saturation, value);

#ifdef DEBUG
  printCurrentValues();
#endif
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Wifi connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // MQTT callback
  client.set_callback(mqttCallback);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect("ESP8266: Fountain")) {
        client.publish(host + (String)"/online", (String)"hello world, I'm " + host);
        client.publish(host + (String)"/online", "true");
        client.subscribe(host + (String)"/#");
#ifdef DEBUG
        Serial.println("MQTT client connected and subscribed");
#endif
      }
    }

    if (client.connected())
      client.loop();
  }
}
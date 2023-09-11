/**
 * @file main.cpp
 * 
 * @brief Sensing air temperature, humidity and pressure
 *        by M5Stick-C and Env. Sensor (DHT12 & BMP280).
 *        Send these data to MQTT broker via WiFi network.
 * 
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <M5StickC.h>
#include <Wire.h>
#include <PubSubClient.h>
#include "M5_ENV.h"
#include <Adafruit_BMP280.h>
#include "Adafruit_Sensor.h"
#include "config.h"

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);


void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()/*,mqtt_user,mqtt_pass*/)) {
      Serial.println("connected");
      //client.publish(topic, "START");
      //client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      char buf[256];
      wifiClient.lastError(buf,256);
      Serial.print("SSL error: ");
      Serial.println(buf);
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

// sensor objects
DHT12 dht12;
Adafruit_BMP280 bme;

void setup() {
    M5.begin();             // Init M5StickC.  初始化M5StickC
    M5.Lcd.setRotation(3);  // Rotate the screen.  旋转屏幕
    Wire.begin(32,
               33);  // Wire init, adding the I2C bus.  Wire初始化, 加入i2c总线
    M5.Lcd.println(F("ENV Unit(DHT12 and BMP280) test"));

    M5.Lcd.setCursor(0, 64);
    M5.Lcd.println(F("Sending data to: "));
    M5.Lcd.setCursor(0, 72);
    M5.Lcd.println(F("env.cs.i.nagoya-u.ac.jp"));

    Serial.begin(115200);

    // attempt to connect to WiFi network:
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to WiFi ..");
    while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(500);
    }
    Serial.println(" connected. ");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
 
    M5.Lcd.setTextSize(2);

    M5.Lcd.println(WiFi.localIP());
    M5.Lcd.println("");

    Serial.println("You're connected to the network");
    Serial.println();

    // You can provide a unique client ID, if not set the library uses Arduino-millis()
    // Each client must have a unique client ID
    // mqttClient.setId("clientId");

    // You can provide a username and password for authentication
    // mqttClient.setUsernamePassword("username", "password");

    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(mqtt_server);

    wifiClient.setCACert(root_ca);
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();

    delay(1000);
}

void loop() {
    // WiFi connection check
    if (WiFi.status () != WL_CONNECTED) {
      Serial.println("WiFi connection lost!");
      Serial.println("Reconnecting...");

      WiFi.disconnect();
      WiFi.reconnect();

      Serial.println("WiFi connected.");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }
    // MQTT connection check
    if (!client.connected()) {
      reconnect();
    }
    // sensor check
    while (!bme.begin(0x76)) {
        M5.Lcd.println("Could not find a valid BMP280 sensor, check wiring!");
    }

    // read sensor data
    float tmp = dht12.readTemperature();
    float hum = dht12.readHumidity();
    float pressure = bme.readPressure();
    pressure = pressure / 100.0F;

    // display sensor data
    M5.lcd.fillRect(0, 10, 160, 50,
                    BLACK);
    M5.lcd.setCursor(0, 10);
    M5.Lcd.printf("Temp: %2.1f  \r\nHumi: %2.0f%%  \r\nPres: %4.0fPa\r\n",
                  tmp, hum, pressure);
    Serial.printf("Temperature: %2.1f  \r\nHumidity: %2.0f%%  \r\nPressure:%4.0fPa\r\n",
                  tmp, hum, pressure);

    // send data to MQTT broker
    String message = "T: ";
    message += tmp;
    message += "C H: ";
    message += hum;
    message += "% P: ";
    message += pressure;
    message += "hPa";

    // publish data to MQTT broker
    if (client.publish(topic, message.c_str())) {
      Serial.println("Publish success!");
    } else {
      Serial.println("Publish failed!");
    }

    delay(60 * 1000);
}
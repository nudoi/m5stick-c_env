/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with M5StickC sample source code
*                          配套  M5StickC 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/m5stickc
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/core/m5stickc
*
* Product:  ENI_DH12_BMP280.  环境传感器
* Date: 2022/7/20
*******************************************************************************
  Please connect to Port,Read temperature, humidity and atmospheric pressure and
  display them on the display screen
  请连接端口,读取温度、湿度和大气压强并在显示屏上显示
  Libraries:
  - [Adafruit_BMP280](https://github.com/adafruit/Adafruit_BMP280_Library)
  - [Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor)
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <M5StickC.h>
#include <Wire.h>
#include <PubSubClient.h>
#include "M5_ENV.h"
#include <Adafruit_BMP280.h>
#include "Adafruit_Sensor.h"

#define WIFI_SSID     "********" // replace with your own WiFi SSID
#define WIFI_PASSWORD "********" // replace with your own WiFi password

// CA certificate
const char* root_ca = R"(
-----BEGIN CERTIFICATE-----
MIIFYDCCBEigAwIBAgIQQAF3ITfU6UK47naqPGQKtzANBgkqhkiG9w0BAQsFADA/
MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT
DkRTVCBSb290IENBIFgzMB4XDTIxMDEyMDE5MTQwM1oXDTI0MDkzMDE4MTQwM1ow
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwggIiMA0GCSqGSIb3DQEB
AQUAA4ICDwAwggIKAoICAQCt6CRz9BQ385ueK1coHIe+3LffOJCMbjzmV6B493XC
ov71am72AE8o295ohmxEk7axY/0UEmu/H9LqMZshftEzPLpI9d1537O4/xLxIZpL
wYqGcWlKZmZsj348cL+tKSIG8+TA5oCu4kuPt5l+lAOf00eXfJlII1PoOK5PCm+D
LtFJV4yAdLbaL9A4jXsDcCEbdfIwPPqPrt3aY6vrFk/CjhFLfs8L6P+1dy70sntK
4EwSJQxwjQMpoOFTJOwT2e4ZvxCzSow/iaNhUd6shweU9GNx7C7ib1uYgeGJXDR5
bHbvO5BieebbpJovJsXQEOEO3tkQjhb7t/eo98flAgeYjzYIlefiN5YNNnWe+w5y
sR2bvAP5SQXYgd0FtCrWQemsAXaVCg/Y39W9Eh81LygXbNKYwagJZHduRze6zqxZ
Xmidf3LWicUGQSk+WT7dJvUkyRGnWqNMQB9GoZm1pzpRboY7nn1ypxIFeFntPlF4
FQsDj43QLwWyPntKHEtzBRL8xurgUBN8Q5N0s8p0544fAQjQMNRbcTa0B7rBMDBc
SLeCO5imfWCKoqMpgsy6vYMEG6KDA0Gh1gXxG8K28Kh8hjtGqEgqiNx2mna/H2ql
PRmP6zjzZN7IKw0KKP/32+IVQtQi0Cdd4Xn+GOdwiK1O5tmLOsbdJ1Fu/7xk9TND
TwIDAQABo4IBRjCCAUIwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYw
SwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5pZGVudHJ1
c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTEp7Gkeyxx
+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEEAYLfEwEB
ATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2VuY3J5cHQu
b3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0LmNvbS9E
U1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFHm0WeZ7tuXkAXOACIjIGlj26Ztu
MA0GCSqGSIb3DQEBCwUAA4IBAQAKcwBslm7/DlLQrt2M51oGrS+o44+/yQoDFVDC
5WxCu2+b9LRPwkSICHXM6webFGJueN7sJ7o5XPWioW5WlHAQU7G75K/QosMrAdSW
9MUgNTP52GE24HGNtLi1qoJFlcDyqSMo59ahy2cI2qBDLKobkx/J3vWraV0T9VuG
WCLKTVXkcGdtwlfFRjlBz4pYg1htmf5X6DYO8A4jqv2Il9DjXA6USbW1FzXSLr9O
he8Y4IWS6wY7bCkjCWDcRQJMEhg76fsO3txE+FiYruq9RUWhiF1myv4Q6W+CyBFC
Dfvp7OOGAN6dEOM4+qR9sdjoSYKEBpsr6GtPAQw4dy753ec5
-----END CERTIFICATE-----
)";

#define MQTT_MAX_PACKET_SIZE 1024  // redefine

// MQTT broker settings
const char* mqtt_server = "iot.env.cs.i.nagoya-u.ac.jp"; // MQTT broker
const int mqtt_port = 0; // MQTT broker port, replace with your port
const char* mqtt_user = "";
const char* mqtt_pass = "";

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

// MQTT topic
#define TOPIC_SIZE (64)
char topic[TOPIC_SIZE] = "m5stick-c/env";

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
}

void loop() {
    if (!client.connected()) {
      reconnect();
    }

    while (!bme.begin(0x76)) {
        M5.Lcd.println("Could not find a valid BMP280 sensor, check wiring!");
    }

    float tmp = dht12.readTemperature();  // Store the temperature obtained from
                                          // dht12.  将dht12获取到的温度存储
    float hum = dht12.readHumidity();  // Store the humidity obtained from the
                                       // dht12.  将dht12获取到的湿度存储
    float pressure = bme.readPressure();  // Stores the pressure gained by BMP.
                                          // 存储bmp获取到的压强
    pressure = pressure / 100.0F;         // Convert to hPa.  转换为hPa

    M5.lcd.fillRect(0, 10, 160, 50,
                    BLACK);  // Fill the screen with black (to clear the
                             // screen).  将屏幕填充黑色(用来清屏)
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
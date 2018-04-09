#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#define DHTTYPE DHT11
#include "DHT.h"
#define DHTPIN D2 
DHT dht(DHTPIN, DHTTYPE);

#define ssid "Pitsanu01"             
#define password "0930436150"

void setup() {
  dht.begin();
  //check connect wifi
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print("Connection Failed! Rebooting");
    Serial.println(".");
    delay(500);
    ESP.restart();
  }
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  Serial.print("***Temperature: ");
  Serial.print(t);
  Serial.println(" *C ***");
  delay(250);

}

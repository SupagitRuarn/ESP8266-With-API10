void Line_Notify(String message) ;
#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#define DHTTYPE DHT11
#include "DHT.h"
#define DHTPIN D2 
DHT dht(DHTPIN, DHTTYPE);
#define LED D1
#define Buzzer D3



//Config SSID and Password 
#define ssid "Pitsanu01"             
#define password "0930436150"   

// Line config
#define LINE_TOKEN "ogIUZY2SyjnLc2q1iVKNDibHzP7vFZE0RkmohPX0q6K" // แก้ LINE TOKEN
String message = "Temparature";
String message2 = "Humidity";

// Config Firebase
#define FIREBASE_HOST "dataanalysis-a2e3d.firebaseio.com"
#define FIREBASE_AUTH "kH5bOvkxk1me8pnboF2D1GYDGoBBFUqR0o6MoQza"

void setup() {
  
  dht.begin();
  pinMode(LED,OUTPUT);
  pinMode(Buzzer,OUTPUT);
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

//Set Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  if(t > 28.00)
  {
  Serial.print("***Temperature: ");
  Serial.print(t);
  Serial.println(" *C ***");
  digitalWrite(LED,HIGH);
  digitalWrite(Buzzer,HIGH);
  delay(1250);
  digitalWrite(Buzzer,LOW);
  String msg1 = "Dangerous";
  Line_Notify(msg1 + t + "%C2%B0C");
  delay(200);
  }
   // check firebase ERROR
    if (Firebase.failed()) {
       Serial.print("Cannot connect to firebase");
       Serial.println(Firebase.error());  
       return;
    }
    
    Firebase.setInt("Temparature",t);
    Serial.print("Temparature:");
    Serial.print(t);
    Serial.println(" *C ");
    Firebase.setInt("Humidity",h);
    Serial.print("Humidity:");
    Serial.print(h);
    Serial.println(" % ");
    digitalWrite(LED,LOW);
    Serial.println("Send Data To Firebase");
    Serial.println("******************************");
    delay(500);


}

// Line Notification
void Line_Notify(String message) {
WiFiClientSecure client;

if (!client.connect("notify-api.line.me", 443)) {
Serial.println("connection failed");
return;
}
String req = "";
req += "POST /api/notify HTTP/1.1\r\n";
req += "Host: notify-api.line.me\r\n";
req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
req += "Cache-Control: no-cache\r\n";
req += "User-Agent: ESP8266\r\n";
req += "Content-Type: application/x-www-form-urlencoded\r\n";
req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
req += "\r\n";
req += "message=" + message;
//Serial.println(req);
//Serial.println("Line Send Message");
client.print(req);
delay(20);

//Serial.println("-------------");
while (client.connected()) {
String line = client.readStringUntil('\n');
if (line == "\r") {
break;
}
//Serial.println(line);
}
//Serial.println("-------------");
}



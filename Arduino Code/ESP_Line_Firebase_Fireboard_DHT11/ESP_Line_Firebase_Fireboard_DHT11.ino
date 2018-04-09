void Line_Notify(String message) ;
#include<time.h>
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
#define LED D5
#define Buzzer D3

// Set FireBoard Port
#define DEBUG_WIFICONNECT 14
#define DEBUG_PUTHDATA 5

// Config Firebase
#define FIREBASE_HOST "dataanalysis-a2e3d.firebaseio.com"
#define FIREBASE_AUTH "kH5bOvkxk1me8pnboF2D1GYDGoBBFUqR0o6MoQza"

//Config SSID and Password 
#define ssid "Pitsanu01"             
#define password "0930436150"   

//Set TimeZone
int timezone = 7;
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
int dst = 0;


// Line config
#define LINE_TOKEN "ogIUZY2SyjnLc2q1iVKNDibHzP7vFZE0RkmohPX0q6K" // แก้ LINE TOKEN
String message = "Temparature";
String message2 = "Humidity";


void setup() {
  
  pinMode(DEBUG_WIFICONNECT,OUTPUT);
  pinMode(DEBUG_PUTHDATA,OUTPUT);
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

configTime(timezone * 3600, dst, ntp_server1, ntp_server2, ntp_server3);

  while (!time(nullptr)) 
  {
    Serial.println(".");
    delay(1000);
  }
  Serial.println("Time Ready to work");
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  // check firebase ERROR
    if (Firebase.failed()) 
    {  
       Serial.println("Cannot connect to firebase");
       Serial.println(Firebase.error());
       return;
     } Serial.println("\"Firebase Ready\"");
       delay(2000);

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
  delay(2000);
  }
//Web Show
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = t;
  root["humidity"] = h;
  root["time"] = NowString();
  
  digitalWrite(DEBUG_PUTHDATA, HIGH);
  String name = Firebase.push("logDHT", root);
  Serial.println("Log Data Putted To Firebase");
  digitalWrite(LED,LOW);
  delay(1250); 
  digitalWrite(DEBUG_PUTHDATA, LOW);
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

// Web show function
String NowString() 
  {
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);
  String tmpNow = "";
  tmpNow += String(newtime->tm_hour);
  tmpNow += ":";
  tmpNow += String(newtime->tm_min);
  tmpNow += ":";
  tmpNow += String(newtime->tm_sec);
  return tmpNow;
  }



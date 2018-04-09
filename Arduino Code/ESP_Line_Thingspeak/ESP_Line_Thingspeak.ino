void Line_Notify(String message) ;
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#define DHTTYPE DHT11
#include "DHT.h"
#define DHTPIN D2 
DHT dht(DHTPIN, DHTTYPE);
#include <ThingSpeak.h>
#define LED D1
#define Buzzer D3

//Config SSID and Password 
#define ssid "Pitsanu01"             
#define password "0930436150"   

// Line config
#define LINE_TOKEN "ogIUZY2SyjnLc2q1iVKNDibHzP7vFZE0RkmohPX0q6K" // แก้ LINE TOKEN
String message = "Temparature";
String message2 = "Humidity";

// Config Port Thing Speak
#define DEBUG_WIFICONNECT 14
#define DEBUG_PUTHDATA 5

// Include API form thingspeaks
String apiKey = "4VRE2TCMB5AAKMEM";
const char* server = "api.thingspeak.com";

// Config Timezone
int timezone = 7;
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
int dst = 0;
WiFiClient client;


 void setup() {
  
  pinMode(DEBUG_WIFICONNECT,OUTPUT);
  pinMode(DEBUG_PUTHDATA,   OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(Buzzer,OUTPUT);
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
  if(t > 28.00)
  {
  Serial.print("**Temperature More Than:");
  Serial.print(t);
  Serial.println(" *C **");
  digitalWrite(LED,HIGH);
  digitalWrite(Buzzer,HIGH);
  delay(1250);
  digitalWrite(Buzzer,LOW);
  String msg1 = "Fire Alarm System Work!!!";
  Line_Notify(msg1 + t + "%C2%B0C");
  delay(100);
  }
  if (client.connect(server,80)) // Connect to api.thingspeak.com
{ 
String postStr = apiKey;
postStr +="&field1=";
postStr += String(t);
postStr +="&field2=";
postStr += String(h);
postStr += "\r\n\r\n";
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);
}
client.stop();
Serial.println("Send Data To Thingspeak");
digitalWrite(LED,LOW);
delay(1000); // Minimum Delay of thingspeak 
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



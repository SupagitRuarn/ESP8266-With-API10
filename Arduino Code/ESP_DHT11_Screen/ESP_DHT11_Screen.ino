void Line_Notify(String message) ;
#include<time.h>
#include <ThingSpeak.h>
#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <LiquidCrystal_I2C.h> 
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define DHTTYPE DHT11
#include "DHT.h"
#define DHTPIN D3 
DHT dht(DHTPIN, DHTTYPE);
#define redPin D8
#define greenPin D6
#define bluePin D5
#define Buzzer D7

// Set FireBoard Port
#define DEBUG_WIFICONNECT 14
#define DEBUG_PUTHDATA 5

// Config Firebase
#define FIREBASE_HOST "dataanalysis-a2e3d.firebaseio.com"
#define FIREBASE_AUTH "kH5bOvkxk1me8pnboF2D1GYDGoBBFUqR0o6MoQza"

// Include API form thingspeaks
String apiKey = "4VRE2TCMB5AAKMEM";
const char* server = "api.thingspeak.com";

//Config SSID and Password 
#define ssid "Pitsanu01"             
#define password "0930436150"   

//Set TimeZone
int timezone = 7;
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
int dst = 0;
WiFiClient client;

// Line config
#define LINE_TOKEN "ogIUZY2SyjnLc2q1iVKNDibHzP7vFZE0RkmohPX0q6K" // แก้ LINE TOKEN
String message = "Temparature";
String message2 = "Humidity";


void setup() {
  setColor(0, 0, 0); // Red Color
  pinMode(DEBUG_WIFICONNECT,OUTPUT);
  pinMode(DEBUG_PUTHDATA,OUTPUT);
  dht.begin();
  lcd.begin();
  pinMode(Buzzer, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  
  
for(int i=0;i<2;i++) //option for-loop to blink the display at the beginning. 
  {
    lcd.backlight();
    delay(350);
    lcd.noBacklight();
    delay(350);
  }
  lcd.backlight(); // finish for-loop with backlight on 
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  lcd.setCursor(0,0);
  lcd.print("Connecting");  
  while (WiFi.status() != WL_CONNECTED) {
    //lcd.print(".");
    digitalWrite(DEBUG_WIFICONNECT, !digitalRead(DEBUG_WIFICONNECT));
    delay(500);
  }
   digitalWrite(DEBUG_WIFICONNECT, HIGH);
   lcd.clear();
   lcd.setCursor(3,0);
   lcd.print("\"Connected\"");
   lcd.setCursor(2,1);
   lcd.print(WiFi.localIP());
   delay(2000);
  
  configTime(timezone * 3600, dst, ntp_server1, ntp_server2, ntp_server3);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Waiting for time");
  while (!time(nullptr)) 
  {
    //lcd.print(".");
    delay(2000);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Time Ready");
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //Blink display 
  for(int i = 0; i< 4; i++) 
  {
    lcd.backlight();
    delay(100);
    lcd.noBacklight();
    delay(100);
  }
  lcd.backlight(); 

//   check firebase ERROR
    if (Firebase.failed()) 
    {  lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("firebase fail");
       lcd.print(Firebase.error());
       return;
     }lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("\"Firebase Ready\"");
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
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Alert");
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print((char)223);
  lcd.print("C");
  setColor(255, 0, 0); // Red Color
  digitalWrite(Buzzer,HIGH);
  delay(1250);
  digitalWrite(Buzzer,LOW);
  String msg1 = "Dangerous";
  Line_Notify(msg1 + t + "%C2%B0C");
  delay(5000);
}
  setColor(0, 255, 0); // Green Color
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.print("%");
 delay(1250);
//Web Show
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = t;
  root["humidity"] = h;
  root["time"] = NowString();

//Send Data To Firebase  
  //digitalWrite(DEBUG_PUTHDATA, HIGH);
  String name = Firebase.push("logDHT", root);
  lcd.clear();
  lcd.setCursor(3,0); 
  lcd.print("Firebase");
  delay(10); 
  digitalWrite(DEBUG_PUTHDATA, LOW);

// ThingSpeak 
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
lcd.clear();
lcd.setCursor(3,0); 
lcd.print("ThingSpeak");
delay(1000); // Minimum Delay of thingspeak
}

// Line Notification
void Line_Notify(String message) {
WiFiClientSecure client;

if (!client.connect("notify-api.line.me", 443)) {
lcd.clear();
lcd.setCursor(0,0); 
lcd.print("Line Fail");
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
 void setColor(int redValue, int greenValue, int blueValue)
 {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}



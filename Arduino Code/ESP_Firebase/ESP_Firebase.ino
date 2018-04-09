#include <time.h>
#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "DHT.h"
#define DHTPIN D3 
#define DHTTYPE DHT11
DHT dht(DHTPIN,DHTTYPE);
#include <LiquidCrystal_I2C.h> 
LiquidCrystal_I2C lcd(0x27, 16, 2);


#define DEBUG_WIFICONNECT 14
#define DEBUG_PUTHDATA 5
//Config Firebase
#define FIREBASE_HOST "dataanalysis-a2e3d.firebaseio.com"
#define FIREBASE_AUTH "kH5bOvkxk1me8pnboF2D1GYDGoBBFUqR0o6MoQza"

//Config SSID,Password
#define WIFI_SSID "Pitsanu01"               
#define WIFI_PASSWORD "0930436150"

//Config TimeZone
int timezone = 7;
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
int dst = 0;



void setup()
{
  pinMode(DEBUG_WIFICONNECT,OUTPUT);
  pinMode(DEBUG_PUTHDATA,   OUTPUT);
  lcd.begin();
  dht.begin();
     for(int i=0;i<2;i++) //option for-loop to blink the display at the beginning. 
  {
    lcd.backlight();
    delay(350);
    lcd.noBacklight();
    delay(350);
  }
  lcd.backlight(); // finish for-loop with backlight on
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.setCursor(0,0);
  lcd.print("Connecting");  
  while (WiFi.status() != WL_CONNECTED) {
    lcd.print(".");
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
    lcd.print(".");
    delay(2000);
  }
  
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

  // check firebase ERROR
    if (Firebase.failed()) 
    {  lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Cannot connect to firebase");
       lcd.print(Firebase.error());
       return;
     }lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("\"Firebase Ready\"");
      delay(2000);
}
void loop() 
{
 
  
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
   
  //Web Show
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["temperature"] = t;
  root["humidity"] = h;
  root["time"] = NowString();
  
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
  digitalWrite(DEBUG_PUTHDATA, HIGH);
  String name = Firebase.push("logDHT", root);
  delay(2000); 
  digitalWrite(DEBUG_PUTHDATA, LOW);


}
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



void Line_Notify(String message) ;
#define SW1 D2
#include <ESP8266WiFi.h>

// Config connect WiFi
#define WIFI_SSID "Pitsanu01" // แก้ชื่อ ssid
#define WIFI_PASSWORD "0930436150" // แก้รหัสผ่าน

// Line config
#define LINE_TOKEN "ogIUZY2SyjnLc2q1iVKNDibHzP7vFZE0RkmohPX0q6K" // แก้ LINE TOKEN
String message = "You are pressing switch";
//String message2 = "%E0%B8%AA%E0%B8%A7%E0%B8%B1%E0%B8%AA%E0%B8%94%E0%B8%B5%E0%B8%84%E0%B8%A3%E0%B8%B1%E0%B8%9A%20%22%E0%B8%84%E0%B8%B8%E0%B8%93%E0%B8%A8%E0%B8%B8%E0%B8%A0%E0%B8%81%E0%B8%B4%E0%B8%88%20%E0%B8%A3%E0%B8%B9%E0%B9%89%E0%B8%AD%E0%B9%88%E0%B8%B2%E0%B8%99%22";
//Line Notify ยังไม่รองรับภาษาไทย ดังนั้นเราสามารถแปลงข้อความเป็น utf-8 เพื่อส่งเป็นภาษาไทยได้ จาก http://meyerweb.com/eric/tools/dencoder/

void setup() {
Serial.begin(115200);
pinMode(SW1,INPUT);
WiFi.mode(WIFI_STA);
// connect to wifi.
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("connecting");

while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(500);
}
Serial.println();
Serial.print("connected: ");
Serial.println(WiFi.localIP());
}

void loop() {
  if (digitalRead(SW1) == HIGH) {
    delay(10);
    Serial.println("Enter !");
    Line_Notify(message);    
    delay(10);
  }
  else 
  {
  Serial.println("Ignore");
  delay(20);
  }
  
}

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
Serial.println(req);
client.print(req);
delay(20);

Serial.println("-------------");
while (client.connected()) {
String line = client.readStringUntil('\n');
if (line == "\r") {
break;
}
Serial.println(line);
}
Serial.println("-------------");
}

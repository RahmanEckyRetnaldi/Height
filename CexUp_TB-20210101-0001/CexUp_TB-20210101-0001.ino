#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>

#define trig D6
#define echo D7
#define button D4
#define buzzer D5
#define led D0

LiquidCrystal_I2C lcd(0x27, 16, 2);
//KONEK WIFI        
const char* ssid = "CexupTest";
const char* password = "nabila12";

//SERVER NAME
String serverName = "http://device.cexup.com/api/data-tmp?";

int pb;
int i = 0;
long durasi;
float jarak;

unsigned long lastTime = 0;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  lcd.begin();
  pinMode(button, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  //CEK KONEKSI WIFI
  WiFi.begin(ssid, password);
  lcd.setCursor(0,0);
  lcd.print("Connet to Wifi");
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    i++;
    delay(500);
    lcd.setCursor(i,1);
    lcd.print(".");
    Serial.print(".");
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Wifi Connected");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome to CeXup");
  lcd.setCursor(5,1);
  lcd.print("Height");
}

void loop() {
  String postData;
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Wifi Not Connectd"); 
  }else{
    Serial.println("Wifi Connected");
  }
  // put your main code here, to run repeatedly:
  pb = digitalRead(button);
  Serial.print("Button state : ");
  Serial.println(pb);
  if(pb == LOW){
    digitalWrite(led, HIGH);
    Serial.println("lampu menyala");
    digitalWrite(trig, LOW);
    delayMicroseconds(3);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    durasi = pulseIn(echo, HIGH);
    Serial.println(durasi);
    jarak = durasi*0.0344/2;
//    jarak = (durasi/2)/29.155;
    if(jarak > 450 || jarak <= 2){
      Serial.println("eror");
      delay(500);
    }else{
      Serial.print("jarak : ");
      Serial.print(jarak);
      Serial.println(" cm");
      postData = (String)jarak;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Develop by CeXup");
      lcd.setCursor(0,1);
      lcd.print("Height :");
      lcd.setCursor(8,1);
      lcd.print(jarak);
      lcd.setCursor(13,1);
      lcd.print("CM");
        //Check WiFi connection status
        if(WiFi.status()== WL_CONNECTED){
          HTTPClient http;

          String serverPath = serverName + "device_code=001-20210101-0001&&data=" + jarak+"&&token=qwertyuiop"; 
      
          // Your Domain name with URL path or IP address with path
          http.begin(serverPath.c_str());
      
          // Send HTTP GET request
          int httpResponseCode = http.GET();

      
          if (httpResponseCode == 200 || httpResponseCode == 201) {
            digitalWrite(led, LOW);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Sending Succes...");
            tone(buzzer, 1000,500);
            delay(1000);
            noTone(buzzer);
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);
          }
          else {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("failed Sending...");
            delay(500); 
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
            Serial.println("Server non-Aktif");
            Serial.println("Pengiriman data gagal");
          }
          // Free resources
          http.end();
          postData="";
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Develop by CeXup");
          lcd.setCursor(0,1);
          lcd.print("Height :");
          lcd.setCursor(8,1);
          lcd.print(jarak);
          lcd.setCursor(13,1);
          lcd.print("CM");
        }
      else {
        postData="";
        Serial.println("WiFi Disconnected");
      }
      
    }
  }
  else{
    digitalWrite(led, LOW);
    delay(1000);
  }
}

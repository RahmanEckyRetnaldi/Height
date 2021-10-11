/*
 * HTTPS Secured Client GET Request
 * Copyright (c) 2019, circuits4you.com
 * All rights reserved.
 * https://circuits4you.com 
 * Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

#define trig D6
#define echo D7
#define button D4
#define buzzer D5
#define led D0
 
LiquidCrystal_I2C lcd(0x27, 16, 2);
//KONEK WIFI        
/* Set these to your desired credentials. */
const char *ssid = "CexupTest";  //ENTER YOUR WIFI SETTINGS
const char *password = "nabila12";

//Link to read data from device.cexup.com
//Web/Server address to read/write from 
const char *host = "device.cexup.com";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "2A 8E 5C 95 FE 8C 51 8E 53 0B 20 F6 C0 32 73 90 46 B7 CB 84";

int pb;
int i = 0;
long durasi;
float jarak;

unsigned long lastTime = 0;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;
//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(115200); 
  lcd.begin();
  //pin configure
  pinMode(button, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
  lcd.setCursor(0,0);
  lcd.print("Connet to Wifi");
  Serial.println("Connecting");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    i++;
    delay(500);
    Serial.print(".");
    lcd.setCursor(i,1);
    lcd.print(".");
    Serial.print(".");
    delay(500);
    
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
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

//=======================================================================
//                    Main Program Loop
//=======================================================================
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
          WiFiClientSecure httpsClient;    //Declare object of class WiFiClient
          Serial.println(host);

          Serial.printf("Using fingerprint '%s'\n", fingerprint);
          httpsClient.setFingerprint(fingerprint);
          httpsClient.setTimeout(15000); // 15 Seconds
          delay(1000);
          
          Serial.print("HTTPS Connecting");
          int r=0; //retry counter
          while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
              delay(100);
              Serial.print(".");
              r++;
          }
          if(r==30) {
            Serial.println("Connection failed");
          }
          else {
            Serial.println("Connected to web");
          }
          String Data, getData, Link;
          float value=jarak;  //Read Analog value of LDR
          Data = String(jarak);   //String to interger conversion

          //GET Data
          Link = "/api/data-tmp?device_code=001-20210101-0001&&data=" +Data+"&&token=qwertyuiop" ;

          Serial.print("requesting URL: ");
          Serial.println(host+Link);
        
          httpsClient.print(String("GET ") + Link + " HTTP/1.1\r\n" +
                       "Host: " + host + "\r\n" +               
                       "Connection: close\r\n\r\n");
        
          Serial.println("request sent");
          char status[32] = {0};
          httpsClient.readBytesUntil('\r', status, sizeof(status));
          // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
          if (strcmp(status + 9, "200 OK") != 0) {
            Serial.print(F("Unexpected response: "));
            Serial.println(status);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("failed Sending...");
            httpsClient.stop();
            return;
          }else{
            Serial.println("Sent data Success");
            Serial.println(status);
            digitalWrite(led, LOW);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Sending Succes...");
            tone(buzzer, 1000,500);
            delay(1000);
            noTone(buzzer);
            while (httpsClient.connected()) {
              String line = httpsClient.readStringUntil('\n');
              if (line == "\r") { 
                Serial.println("headers received");
                break;
              }
            }
              Serial.println("Respose was:");
              Serial.println("==========");
              String line;
              while(httpsClient.available()){        
                line = httpsClient.readStringUntil('\n');  //Read Line by Line
                Serial.println(line); //Print response
              }
            Serial.print("HTTP Response code: ");
            Serial.println(status+9);
            Serial.println("==========");
            Serial.println("closing connection");
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Develop by CeXup");
            lcd.setCursor(0,1);
            lcd.print("Height :");
            lcd.setCursor(8,1);
            lcd.print(jarak);
            lcd.setCursor(13,1);
            lcd.print("CM");
            httpsClient.stop();
          }
        }
      else {
        postData="";
        Serial.println("WiFi Disconnected");
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
    }
  }else{
    digitalWrite(led, LOW);
    delay(1000);
  }        

 
}

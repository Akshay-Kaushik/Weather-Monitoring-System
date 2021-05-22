#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <DHT.h>
#include "ThingSpeak.h"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME  "AIO_USERNAME"
#define AIO_KEY  "AIO_KEY"
#define DHTTYPE DHT11
#define dht_dpin 0

DHT dht(dht_dpin, DHTTYPE);
const char* ssid = "SSID";                                  //fill in your wifi name
const char* password = "password";                              //fill in your wifi password
unsigned long myChannelNumber = 1020494;
const char * myWriteAPIKey = "myWriteAPIKey";


WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

boolean MQTT_connect();

boolean MQTT_connect() {  int8_t ret; if (mqtt.connected()) {    return true; }  uint8_t retries = 3;  while ((ret = mqtt.connect()) != 0) { mqtt.disconnect(); delay(2000);  retries--;if (retries == 0) { return false; }} return true;}

Adafruit_MQTT_Publish WeatherMonitoring = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/weathermonitoring");



void setup() {
  Serial.begin(115200);
  ThingSpeak.begin(client);
pinMode(D7,OUTPUT);
dht.begin();

delay(10);


WiFi.begin(ssid, password);

Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
Serial.print("..........");
Serial.println();
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);

}
Serial.println("WiFi connected");
Serial.println();

}


void loop() {
  char value[200]=" ";
  char light[40]=", It is ",rain[40]=", It is ";
  float h=dht.readHumidity();
  delay(500);
  float t=dht.readTemperature();
  ThingSpeak.setField(1,h);
  ThingSpeak.setField(2,t);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  char hum[25]="Humidity:";
  char tem[25]=", Temperature: ";
  char hu[30],te[30];
  dtostrf(h,10,2,hu);
  dtostrf(t,6,2,te);
  strcat(hum,hu);
  strcat(tem,te);
  delay(500);
  Serial.print("Current Humidity: ");
  Serial.print(h);
  Serial.print("%");
  Serial.println();
  Serial.print("Current Temperafure: ");
  Serial.print(t);
  Serial.println();
if(digitalRead(D6)==LOW)
  {
    strcat(light,"Bright Outside");
  }
  else
  {
    strcat(light,"Dark Outside");
    
  }

if(analogRead(A0)<=600)
  {
  Serial.println("Rain detected");
  
  strcat(rain, "Raining");
  
  }
  else
  
  {
  Serial.println("Rain not detected");
  strcat(rain,"Not Raining");
  }

 if (MQTT_connect()) 
 {
      strcat(value,hum);
      strcat(value,tem);
      strcat(value,light);
      strcat(value,rain);
      Serial.println(value);
      WeatherMonitoring.publish(value);
      Serial.println();
      Serial.println("Data Sent");
      
 }
delay(10000);
}

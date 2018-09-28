#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define WIFI_SSID "xxxxxxx"
#define WIFI_PASS "***********"
#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "user_name"
#define MQTT_PASS "AIO KEY"
//Set up MQTT and WiFi clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);
//Set up the feed you're subscribing to
Adafruit_MQTT_Subscribe light = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/light");
const int buzzer=D3;  // on hardware pin D8
const int relay=D2;    // on hardware pin D4
const int power_ind=D4;  //on hardware pin D9
const int trigger_ind=D5; //on hardware pin D5
const int wifi_ind=D6;   //on hardware pin D6
void setup()
{
  Serial.begin(9600);
 pinMode(buzzer,OUTPUT);
 pinMode(relay,OUTPUT);
 pinMode(power_ind,OUTPUT);
 pinMode(trigger_ind,OUTPUT);
 pinMode(wifi_ind,OUTPUT);
 digitalWrite(power_ind, HIGH);
  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  Serial.println("OK!");
  digitalWrite(wifi_ind, HIGH);
  //Subscribe to the Test feed
  mqtt.subscribe(&light);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}
void loop()
{
  MQTT_connect();
  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &light)
    {
      //Print the new value to the serial monitor
      Serial.print("light: ");
      Serial.println((char*) light.lastread); 
      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) light.lastread, "ON"))
      {
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(trigger_ind, HIGH);
        digitalWrite(relay,LOW);
        delay(500);
        digitalWrite(buzzer,HIGH);
        delay(1000);
        digitalWrite(buzzer,LOW);
      }
      else
      {
        digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(trigger_ind, LOW);
        digitalWrite(relay,HIGH);
        delay(500);
        digitalWrite(buzzer,LOW);
      }
    }
  }
  // ping the server to keep the mqtt connection alive
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }
}
void MQTT_connect() 
{
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) 
  {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

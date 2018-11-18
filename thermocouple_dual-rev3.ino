#include <PubSubClient.h>

// ESP8266 
#include <ESP8266WiFi.h> // wifi stuff for 8266


#include "max6675.h"
#include <SPI.h>


/////////////////////////////////////////////////////////////////////////////////////////
// Wifi  - AP definitions
/////////////////////////////////////////////////////////////////////////////////////////
#define AP_SSID "MyWifiNetwork"
#define AP_PASSWORD "MyWifiPassword"


int thermoDO = D6;
int thermoCS = D7;
int thermoCLK = D5;
int thermoCS2 = D8;

float leftTemp[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
String Spinner[4] = { "*" , "|", "/", "-" }; // '\' didnt play well with openhab.
int spinner = 0;
float leftsum = 0.0;
float leftavg = 0.0;
float rightTemp[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float rightsum = 0.0;
float rightavg = 0.0;
MAX6675 thermocouple;
MAX6675 thermocouple2;
int vccPin = 3;
int gndPin = 2;

const char* mqtt_server = "192.168.128.117";

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 wifiConnect();


     //pubsub mqtt
     client.setServer(mqtt_server, 1883);
     client.setCallback(callback);

thermocouple.begin(thermoCLK, thermoCS, thermoDO);
thermocouple2.begin(thermoCLK, thermoCS2, thermoDO);
  // use Arduino pins
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);

  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
delay(500);
    //Serial.println("Looping");
    if (!client.connected()) {
    reconnect();
    }
    client.loop();
    Serial.println("sending");
    client.publish("outTopic", "Switch1");  
    Serial.println("done sending");


  //  Serial.print("C = ");
 //  Serial.println(thermocouple.readCelsius());
   Serial.print("Left F = ");
   Serial.println(thermocouple.readFahrenheit());
   Serial.print("Right F = ");
   Serial.println(thermocouple2.readFahrenheit());
leftsum = 0.0;
rightsum = 0.0;
   for (int ii = 1; ii < 30; ii++) {
 
 leftTemp[ii-1] = leftTemp[ii];
 rightTemp[ii-1] = rightTemp[ii];
 //Serial.println(leftTemp[ii-1]);
 leftsum = leftTemp[ii-1] + leftsum;
 rightsum = rightTemp[ii-1] + rightsum;
}
leftTemp[29] = thermocouple.readFahrenheit();
leftsum = leftTemp[29] + leftsum;
leftavg= leftsum/30;
rightTemp[29] = thermocouple2.readFahrenheit();
rightsum = rightTemp[29] + rightsum;
rightavg= rightsum/30;
Serial.println(Spinner[spinner] );   
   //client.publish("smoker/sensor/left", String(thermocouple.readFahrenheit()).c_str(), true);  
   //client.publish("smoker/sensor/right", String(thermocouple2.readFahrenheit()).c_str(), true);  
   client.publish("smoker/sensor/left", String(leftavg).c_str(), true);  
   client.publish("smoker/sensor/right", String(rightavg).c_str(), true);  
 //client.publish("smoker/sensor/spinner", String(rightavg).c_str(), true);  
 client.publish("smoker/sensor/spinner", String(Spinner[spinner]).c_str(), true);  
  // client.publish("smoker/sensor/spinner", String(Spinner[spinner]).c_str(), true);
   delay(500);
spinner++;
if (spinner == 4) { spinner = 0; }    
}



// Added by danny
void wifiConnect()
{
    delay(1000);
    Serial.print("Connecting to AP");
    WiFi.begin(AP_SSID, AP_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  
}

/////////////////////////////////////////////////////
// callback
/////////////////////////////////////////////////////
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
   
  } else {
   
    }

}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("LED", "New Code");
      // ... and resubscribe
      client.subscribe("LED");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


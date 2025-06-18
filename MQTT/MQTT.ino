#include <ArduinoMqttClient.h>
#include <WiFi101.h>
#include "arduino_secrets.h"
#include "DHT.h"
#define DHTPIN 3   
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int rojo = 4;
int verde = 5;
int azul = 0;

char ssid[] = SECRET_SSID;     
char pass[] = SECRET_PASS;    

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "10.100.3.10";
int        port     = 1883;
const char topic[]  = "real_unique_topic";
const char topic2[]  = "real_unique_topic_2";
const char topic3[]  = "/meca/mkr1000/dht/t";
const char topic4[]  = "/meca/mkr1000/dht/h";


const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    digitalWrite(rojo, HIGH);

    delay(5000);
    
  }

  Serial.println("You're connected to the network");

  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
  encender (100);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    apagar (200);
    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
  digitalWrite(verde, HIGH);

  pinMode(rojo, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(azul, OUTPUT);
  
  Serial.println(F("DHTxx test!"));
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    //record random value from A0, A1 and A2
    int Rvalue = analogRead(A0);
    int Rvalue2 = analogRead(A1);
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.println(Rvalue);

    Serial.print("Sending message to topic: ");
    Serial.println(topic2);
    Serial.println(Rvalue2);

    Serial.print("Sending message to topic: ");
    Serial.println(topic3);
    Serial.println(t);

    Serial.print("Sending message to topic: ");
    Serial.println(topic4);
    Serial.println(h);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    mqttClient.print(Rvalue);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic2);
    mqttClient.print(Rvalue2);
    mqttClient.endMessage();



    Serial.println();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  else{
    mqttClient.beginMessage(topic3);
    mqttClient.print(t);
    mqttClient.endMessage();
    
    mqttClient.beginMessage(topic4);
    mqttClient.print(h);
    mqttClient.endMessage();
  }
  float hic = dht.computeHeatIndex(t, h, false);
  

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.println(F("°F"));
  if 
}
}
  
  void encender (int tiempo){
      digitalWrite(verde, HIGH);
      digitalWrite(rojo, LOW);
      delay(tiempo);
}

void apagar (int tiempo){
      digitalWrite(rojo, HIGH);
      digitalWrite(verde, LOW);
}

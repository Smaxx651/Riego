
#include "DHT.h"
#define DHTPIN 2     
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

int rojo=7;
int verde=8;


void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin();
  pinMode(rojo,OUTPUT);
  pinMode(verde,OUTPUT);
}

void loop() {

  delay(2000);

  float h = dht.readHumidity();
  
  float t = dht.readTemperature();
 
  float f = dht.readTemperature(true);

 
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

 
  float hif = dht.computeHeatIndex(f, h);

  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  int val;
  val = analogRead(0); 
  Serial.println(val); 
  delay(100);
  //Tierra seca = 630
  //Tierra humeda = 503
  //Tierra Mojada = 360

  if(val < 400){
    digitalWrite(rojo, HIGH);
    digitalWrite(verde,LOW);
    Serial.println(" > 400");}

  if (val > 400 && val < 550) {
    digitalWrite(rojo, HIGH);
    digitalWrite(verde,LOW);
    Serial.println("> 400 & <550");}
    
  if(val > 550){
    digitalWrite(rojo, LOW);
    digitalWrite(verde,HIGH);
    Serial.println(" > 550");}

}

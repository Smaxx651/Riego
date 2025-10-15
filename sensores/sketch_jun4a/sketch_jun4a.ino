
#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
int rojo=7;
int verde=8;

DHT dht(DHTPIN, DHTTYPE);
int sensorValue;
long sum = 0;
int vout = 0;
int uv = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  pinMode(A1, INPUT);
  dht.begin();
  pinMode(rojo,OUTPUT);
  pinMode(verde,OUTPUT);
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
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
   
   sensorValue = 0;
  sum = 0;
  for (int i = 0 ; i < 1024 ; i++ ) {
    sensorValue = analogRead(A1);
    sum = sensorValue + sum;
    delay(2);
  }
  vout = sum >> 10;
  vout = vout * 4980.0 / 1024;
  Serial.print("The Photocurrent value:");
  Serial.print(vout);
  Serial.println("mV");

  if (vout < 50) {
    uv = 0;
  }
  else if (vout < 227) {
    uv = 1;
  }
  else if (vout < 318) {
    uv = 2;
  }
  else if (vout < 408) {
    uv = 3;
  }
  else if (vout < 503) {
    uv = 4;
  }
  else if (vout < 606) {
    uv = 5;
  }
  else if (vout < 696) {
    uv = 6;
  }
  else if (vout < 795) {
    uv = 7;
  }
  else if (vout < 881) {
    uv = 8;
  }
  else if (vout < 976) {
    uv = 9;
  }
  else if (vout < 1079) {
    uv = 10;
  }
  else {
    uv = 11;
  }
  delay(20);
  Serial.print("UV Index = ");
  Serial.println(uv);

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



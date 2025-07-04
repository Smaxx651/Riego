#include <HCSR04.h>
int rojo=7;
int verde=8;
int azul=9;

byte triggerPin = 5;
byte echoPin = 12;

void setup () {
  pinMode(rojo,OUTPUT);
  pinMode(verde,OUTPUT);
  pinMode(azul,OUTPUT);
  Serial.begin(9600);
  HCSR04.begin(triggerPin, echoPin);
}

void loop () {
  double* distances = HCSR04.measureDistanceCm();


  
  if(distances[0]> 25){
    digitalWrite(rojo, HIGH);
    digitalWrite(verde,LOW);
    digitalWrite(azul,LOW);
    Serial.println(" > 150");
  

    }

  if(distances[0] > 8 && distances[0] < 25){
    digitalWrite(rojo, LOW);
    digitalWrite(verde,HIGH);
    digitalWrite(azul,LOW);
    Serial.println(" > 75 & <150");
   
  }
  
  if(distances[0] < 8){
    digitalWrite(rojo, LOW);
    digitalWrite(verde,LOW);
    digitalWrite(azul,HIGH);
    Serial.println(" < 75");
    
    }


  Serial.print(distances[0]);
  Serial.println(" cm");
  
  Serial.println("---");
  delay(250);
}
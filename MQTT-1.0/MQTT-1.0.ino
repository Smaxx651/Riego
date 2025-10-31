#include <ArduinoMqttClient.h>
#include <WiFi101.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "arduino_secrets.h"
#include "DHT.h"

#define anolog A1
#define DHTPIN  2          
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int ROJO  = 4;
const int VERDE = 5;
const int AZUL  = 3;

const int relay_1 = 6;
const int relay_2 = 10;
const int relay_3 = 8;
const int relay_4 = 9;

const int boton = 7;      


LiquidCrystal_I2C lcd(0x27, 16, 2); 


char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "10.100.3.11";
int        port     = 1883;
const char topic[]  = "real_unique_topic";
const char topic2[] = "real_unique_topic_2";
const char topic3[] = "/meca/mkr1000/dht/t";
const char topic4[] = "/meca/mkr1000/dht/h";


const long interval = 8000;
unsigned long previousMillis = 0;

bool brokerOK = false;
bool todoOK   = false;

int pantalla = 0;                 
int ultimoEstadoBoton = HIGH;     


void vista1(String IP, int o1, int o2, int o3, int o4, int I1, int I2);
void vista2(int S1, int o1, int o2, int o3, int o4);
void vista3(bool conectado);
void printSpaced(uint8_t col, uint8_t row, const char* txt);

void LedRojo()  { digitalWrite(ROJO, HIGH); digitalWrite(VERDE, LOW);  digitalWrite(AZUL, LOW);  }
void LedVerde() { digitalWrite(ROJO, LOW);  digitalWrite(VERDE, HIGH); digitalWrite(AZUL, LOW);  }
void LedAzul()  { digitalWrite(ROJO, LOW);  digitalWrite(VERDE, LOW);  digitalWrite(AZUL, HIGH); }

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(ROJO, OUTPUT);
  pinMode(VERDE, OUTPUT);
  pinMode(AZUL, OUTPUT);
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
  pinMode(relay_3, OUTPUT);
  pinMode(relay_4, OUTPUT);
  pinMode(boton, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Riego MQTT");
  dht.begin();

  Serial.print("Conectando a: "); Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nWiFi OK");
  brokerOK = true;


  Serial.print("MQTT -> "); Serial.print(broker); Serial.print(":"); Serial.println(port);
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT fallo. Error = ");
    Serial.println(mqttClient.connectError());
    todoOK = false;
  } else {
    Serial.println("MQTT OK");
    todoOK = true;
  }


  if (!brokerOK)      LedRojo();   // No WiFi
  else if (!todoOK)   LedAzul();   // WiFi OK, MQTT NO
  else                LedVerde();  // Todo OK
}

void loop() {

  mqttClient.poll();


  int estadoBoton = digitalRead(boton);
  if (ultimoEstadoBoton == HIGH && estadoBoton == LOW) {
    pantalla = (pantalla + 1) % 4;
    delay(200); 
  }
  ultimoEstadoBoton = estadoBoton;
  


  unsigned long now = millis();
  if (now - previousMillis >= interval) {
    previousMillis = now;

    int Rvalue  = analogRead(A0);
    int Rvalue2 = analogRead(A1); 
    Serial.print("Sensor suelo: ");
Serial.println(Rvalue2);

float humedad = map(Rvalue2, 1023, 450, 0, 100);
Serial.print("Humedad: ");
Serial.print(humedad);
Serial.println("%");


    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(h) || isnan(t)) {
      Serial.println(F("DHT error"));
    } else {
      mqttClient.beginMessage(topic3); mqttClient.print(t); mqttClient.endMessage();
      mqttClient.beginMessage(topic4); mqttClient.print(h); mqttClient.endMessage();
      Serial.print("T="); Serial.print(t); Serial.print("C  H="); Serial.print(h); Serial.println("%");
    }

    mqttClient.beginMessage(topic);  mqttClient.print(Rvalue);  mqttClient.endMessage();
    mqttClient.beginMessage(topic2); mqttClient.print(Rvalue2); mqttClient.endMessage();
  }


  digitalWrite(relay_1, HIGH);
  digitalWrite(relay_2, HIGH);
  digitalWrite(relay_3, HIGH);
  digitalWrite(relay_4, HIGH);


 if (pantalla == 0) {
  vista1(String(broker), 1, 0, 1, 0, DHTPIN, 1);
} else if (pantalla == 1) {
  vista2(123, 1, 5, 6, 7);
} else if (pantalla == 2) {
  vista3(todoOK);
} else if (pantalla == 3) {
  vista4();  
}
}

void vista1(String IP, int o1, int o2, int o3, int o4, int I1, int I2) {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("IP:");
  lcd.setCursor(3,0); lcd.print(IP);

  lcd.setCursor(0,1); lcd.print("O");
  lcd.setCursor(1,1); lcd.print(o1);
  lcd.setCursor(2,1); lcd.print(o2);
  lcd.setCursor(3,1); lcd.print(o3);
  lcd.setCursor(4,1); lcd.print(o4);

  lcd.setCursor(6,1); lcd.print("I");
  lcd.setCursor(7,1); lcd.print(I1);
  lcd.setCursor(8,1); lcd.print(I2);
  delay(300);
}

void vista2(int S1, int o1, int o2, int o3, int o4) {
  lcd.clear();
  lcd.setCursor(0,0);  lcd.print("S1:"); lcd.print(S1);
  lcd.setCursor(7,0);  lcd.print("S3:X");
  lcd.setCursor(14,0); lcd.print("S5:X");

  lcd.setCursor(0,1);  lcd.print("S2:X");
  lcd.setCursor(7,1);  lcd.print("S4:X");
  lcd.setCursor(14,1); lcd.print("S6:X");
  delay(300);
}

void vista3(bool conectado) {
  lcd.clear();
  printSpaced(0, 0, "ESTATUS:");
  lcd.setCursor(0, 1);
  lcd.print(conectado ? "CONECTADO" : "ERROR");
  delay(300);
}
void vista4() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PROYECTO:");
  lcd.setCursor(0, 1);
  lcd.print("Riego MQTT 🌱");
  delay(300);
}



void printSpaced(uint8_t col, uint8_t row, const char* txt) {
  lcd.setCursor(col, row);
  for (const char* p = txt; *p; ++p) { lcd.print(*p); lcd.print(' '); }
}



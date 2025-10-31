#include <ArduinoMqttClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

char ssid[] = "WiFiCJP-Alumnos";     
char pass[] = "cjpalumnos2021";  



#define SDA_PIN 4 
#define SCL_PIN 5  


#define SOIL_PIN A0


#define DHTPIN   12  
#define DHTTYPE  DHT11
DHT dht(DHTPIN, DHTTYPE);


const int relay_1 = 14;
const int relay_2 = 13;
const int relay_3 = 15;
const int relay_4 = 16;


const int boton   = 0;


LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "10.100.3.83";
int        port     = 1883;
const char topicA0[] = "real_unique_topic";
const char topicT[]  = "/meca/mkr1000/dht/t";
const char topicH[]  = "/meca/mkr1000/dht/h";
const char topicSoil[] = "/meca/esp8266/soil/h";


const unsigned long interval = 8000;
unsigned long previousMillis = 0;


bool wifiOK  = false;
bool mqttOK  = false;
int  pantalla = 0;
int  ultimoEstadoBoton = HIGH;


bool todoOK = false;
void vista1(String IP, int o1, int o2, int o3, int o4, int I1, int I2);
void vista2(float temp, float hum, float suelo);
void vista3(bool conectado);
void vista4();
void printSpaced(uint8_t col, uint8_t row, const char* txt);



void setup() {
  Serial.begin(9600);

    // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  while (!Serial) {}

  Wire.begin(SDA_PIN, SCL_PIN);


  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Riego MQTT");
  

  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
  pinMode(relay_3, OUTPUT);
  pinMode(relay_4, OUTPUT);
  pinMode(boton,   INPUT_PULLUP);

  dht.begin();

  Serial.print("Conectando a: "); Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass); 
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
    if (millis() - t0 > 15000) break; 
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi OK");
    wifiOK = true;
  } else {
    Serial.println("\nWiFi FAIL");
  }

  
  if (wifiOK) {
    Serial.print("MQTT -> "); Serial.print(broker); Serial.print(":"); Serial.println(port);
    if (!mqttClient.connect(broker, port)) {
      Serial.print("MQTT fallo. Error = ");
      Serial.println(mqttClient.connectError());
      mqttOK = false;
    } else {
      Serial.println("MQTT OK");
      mqttOK = true;
    }
  }

  digitalWrite(relay_1, LOW);
  digitalWrite(relay_2, LOW);
  digitalWrite(relay_3, LOW);
  digitalWrite(relay_4, LOW);
}

void loop() {

  if (mqttOK) mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    //record random value from A0, A1 and A2
    int Rvalue = analogRead(A0);
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    Serial.print("Sending message to topic: ");
    Serial.println(topicA0);
    Serial.println(Rvalue);

    Serial.print("Sending message to topic: ");
    Serial.println(topicT);
    Serial.println(t);

    Serial.print("Sending message to topic: ");
    Serial.println(topicH);
    Serial.println(h);

       // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topicA0);
    mqttClient.print(Rvalue);
    mqttClient.endMessage();

    Serial.println();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  else{
    mqttClient.beginMessage(topicT);
    mqttClient.print(t);
    mqttClient.endMessage();
    
    mqttClient.beginMessage(topicH);
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
}

  int estadoBoton = digitalRead(boton);
  if (ultimoEstadoBoton == HIGH && estadoBoton == LOW) {
    pantalla = (pantalla + 1) % 4;
    delay(200);
  }
  ultimoEstadoBoton = estadoBoton;


  unsigned long now = millis();
  if (now - previousMillis >= interval) {
    previousMillis = now;


    int soilRaw = analogRead(SOIL_PIN); // 0..1023
    
    float humedadSuelo = map(soilRaw, 900, 300, 0, 100);
    if (humedadSuelo < 0)   humedadSuelo = 0;
    if (humedadSuelo > 100) humedadSuelo = 100;

    float t = dht.readTemperature();
    float h = dht.readHumidity();


    Serial.print("Soil raw="); Serial.print(soilRaw);
    Serial.print("  HumedadSuelo="); Serial.print(humedadSuelo); Serial.println("%");

    if (isnan(h) || isnan(t)) {
      Serial.println(F("DHT error"));
    } else {
      Serial.print("T="); Serial.print(t); Serial.print("C  H="); Serial.print(h); Serial.println("%");
    }

    if (mqttOK) {
 
      mqttClient.beginMessage(topicSoil); mqttClient.print(humedadSuelo); mqttClient.endMessage();

      if (!isnan(h) && !isnan(t)) {
        mqttClient.beginMessage(topicT); mqttClient.print(t); mqttClient.endMessage();
        mqttClient.beginMessage(topicH); mqttClient.print(h); mqttClient.endMessage();
      }
 
      mqttClient.beginMessage(topicA0); mqttClient.print(soilRaw); mqttClient.endMessage();
    }
  }


  digitalWrite(relay_1, HIGH);
  digitalWrite(relay_2, HIGH);
  digitalWrite(relay_3, HIGH);
  digitalWrite(relay_4, HIGH);

  
  float t = dht.readTemperature();
float h = dht.readHumidity();
float humedadSuelo = map(analogRead(A0), 900, 300, 0, 100);

if (humedadSuelo < 0)   humedadSuelo = 0;
if (humedadSuelo > 100) humedadSuelo = 100;

if (pantalla == 0) {
  vista1(String(WiFi.localIP().toString()), 1, 0, 1, 0, DHTPIN, 1);
} else if (pantalla == 1) {
  vista2(t, h, humedadSuelo);
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

void vista2(float temp, float hum, float suelo) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T:"); lcd.print(temp,1); lcd.print("C ");
  lcd.print("H:"); lcd.print(hum,0); lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Suelo:"); lcd.print(suelo,0); lcd.print("%");
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
  lcd.print("Riego MQTT");
  delay(300);
}

void printSpaced(uint8_t col, uint8_t row, const char* txt) {
  lcd.setCursor(col, row);
  for (const char* p = txt; *p; ++p) { lcd.print(*p); lcd.print(' '); }
}


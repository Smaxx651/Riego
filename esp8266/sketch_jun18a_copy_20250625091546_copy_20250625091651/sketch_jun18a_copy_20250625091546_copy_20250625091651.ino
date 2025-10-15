//hola
#include <ArduinoMqttClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "WiFiCJP-Alumnos";
const char* password = "cjpalumnos2021";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "10.100.2.194";
int        port     = 1883;
const char topic[]  = "Esp8266 Meca";

const long interval = 8000;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado al WiFi!");

  Serial.print("Conectando al broker MQTT: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("Fallo la conexión MQTT. Código de error: ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  Serial.println("Conectado al broker MQTT.");
}

void loop() {
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int Rvalue = analogRead(A0);

    Serial.print("Enviando a topic: ");
    Serial.println(topic);
    Serial.println(Rvalue);

    mqttClient.beginMessage(topic);
    mqttClient.print(Rvalue);
    mqttClient.endMessage();

    Serial.println();
  }
}

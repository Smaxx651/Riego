#include <ArduinoMqttClient.h>
#include <ESP8266WiFi.h>

const int rojo = 16;
const int verde = 15;
const int blanco = 4;

const char* ssid = "WiFiCJP-Alumnos";
const char* password = "cjpalumnos2021";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "10.100.2.113";
int port = 1883;
const char topic[] = "Esp8266 Meca";

const long interval = 8000;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);

  pinMode(rojo, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(blanco, OUTPUT);

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

    int rawUV = analogRead(A0); 
    int uvLevel = map(rawUV, 0, 1023, 0, 15); // Normalizar a escala 0-15

    Serial.print("UV Normalizado: ");
    Serial.println(uvLevel);

    // Control de LEDs según el nivel UV
    if (uvLevel <= 4) {
      digitalWrite(rojo, LOW);
      digitalWrite(verde, LOW);
      digitalWrite(blanco, HIGH);
      Serial.println("Nivel UV: Bajo");
    } else if (uvLevel <= 9) {
      digitalWrite(rojo, LOW);
      digitalWrite(verde, HIGH);
      digitalWrite(blanco, LOW);
      Serial.println("Nivel UV: Moderado");
    } else {
      digitalWrite(rojo, HIGH);
      digitalWrite(verde, LOW);
      digitalWrite(blanco, LOW);
      Serial.println("Nivel UV: Alto");
    }

    // Enviar valor al broker MQTT
    mqttClient.beginMessage(topic);
    mqttClient.print(uvLevel);
    mqttClient.endMessage();

    Serial.println("---");
  }
}

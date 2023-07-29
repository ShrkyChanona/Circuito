//sensores
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

//broker
#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid = "MOVISTAR WIFI9624";
const char *pass = "dtzm3157";
char *server = "44.216.167.198";
const int mqttPort = 1883;

char *subscribeTopic = "";
char *publishTopic = "esp32.mqtt";
const char *mqttUser = "guest";
const char *mqttPassword = "guest";

WiFiClient espClient;
PubSubClient client(espClient);

//DATOS DEL SENSOR
#define DHTTYPE DHT11
#define DHTPIN 4

DHT dht(DHTPIN, DHTTYPE);
// Adafruit_BMP085 bmp;

void setup() {
  Serial.begin(9600);
  dht.begin();
  WiFi.begin(ssid, pass);

  // if (!bmp.begin()) {
  //   Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
  //   while (1) {}
  // }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(server, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  float altitude = bmp.readAltitude();
  float pressure = bmp.readSealevelPressure();
  float real_alture = bmp.readAltitude(102000);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("ERROR AL MOMENTO DE LEER LOS DATOS");
    return;
  }

  // Crear un objeto JSON
  DynamicJsonDocument jsonDoc(200);
  jsonDoc["Temperatura"] = temperature;
  jsonDoc["Humedad"] = humidity;
  jsonDoc["altitud"] = altitude;
  jsonDoc["presion"] = pressure;

  // Serializar el objeto JSON a una cadena JSON
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  Serial.print(jsonString);

  if (client.publish(publishTopic, jsonString.c_str())) {
    Serial.println("Msn enviado exitosamente");
  } else {
    Serial.print("Error en el envio");
  }

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("Altitude = ");
  Serial.print(altitude);
  Serial.println(" meters");

  Serial.print("Pressure at sealevel (calculated) = ");
  Serial.print(pressure);
  Serial.println(" Pa");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
  Serial.print("Real altitude = ");
  Serial.print(real_alture);
  Serial.println(" meters");

  Serial.println();

  Serial.println(humidity);
  Serial.println("%");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
  delay(3000);
}

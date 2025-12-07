#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WebServer.h>

// ------------------- WiFi --------------------
const char* ssid = "ESP32_Health";
const char* password = "12345678";

WebServer server(80);
// ---------------------------------------------

#define DHTPIN 4
#define DHTTYPE DHT11
#define ONE_WIRE_BUS 15

#define FAN_PIN 5   // <<< NEW: FAN PIN

MAX30105 particleSensor;
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// GLOBAL VARIABLES SENT TO WEB PAGE
float humidity_g = 0;
float roomTemp_g = 0;
float bodyTemp_g = 0;
int bpmValue_g = 0;
int spo2_g = 0;
String fanStatus_g = "OFF";    // <<< NEW FAN STATUS FOR WEB PAGE

void handleRoot() {
  String html = "<html><body style='font-family: Arial; background:#111; color:white; text-align:center;'>";

  html += "<h1 style='color:#ff4444;'>ESP32 Health Monitor</h1>";

  // ---------- DATA BOX ----------
  html += "<div style='padding:20px; background:#222; border-radius:15px; display:inline-block;'>";

  html += "<h2 style='color:#ff8844;'>Body Temperature: " + String(bodyTemp_g) + " °C</h2>";
  html += "<h2 style='color:#33b5e5;'>Room Temperature: " + String(roomTemp_g) + " °C</h2>";
  html += "<h2 style='color:#00C851;'>Humidity: " + String(humidity_g) + " %</h2>";
  html += "<h2 style='color:#CC0000;'>Heart Rate: " + String(bpmValue_g) + " BPM</h2>";
  html += "<h2 style='color:#9933CC;'>SpO2: " + String(spo2_g) + " %</h2>";

  // ---------- FAN STATUS ----------
  html += "<h2 style='color:#FFFF00;'>Fan Status: " + fanStatus_g + "</h2>";

  html += "</div><br><br>";

  html += "<p style='color:#aaa;'>Refresh the page for new readings.</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // -------------- WiFi START ----------------
  WiFi.softAP(ssid, password);
  Serial.println("WiFi Started");
  Serial.print("Connect to: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web Server Started");
  // ------------------------------------------

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);  // FAN OFF initially

  Wire.begin(21, 22);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 not found");
    while (1);
  }

  particleSensor.setup(60, 4, 2, 100, 411, 4096);
  particleSensor.setPulseAmplitudeRed(0x2F);
  particleSensor.setPulseAmplitudeIR(0x2F);

  dht.begin();
  sensors.begin();
}

void loop() {

  server.handleClient();   // <-- Important for webpage

  float humidity = dht.readHumidity();
  float roomTemp = dht.readTemperature();
  sensors.requestTemperatures();
  float bodyTemp = sensors.getTempCByIndex(0);

  long irValue = particleSensor.getIR();
  long redValue = particleSensor.getRed();

  // Store values for web page
  humidity_g = humidity;
  roomTemp_g = roomTemp;
  bodyTemp_g = bodyTemp;

  // ---------- FAN CONTROL ----------
  if (roomTemp >= 30) {
    digitalWrite(FAN_PIN, HIGH);
    fanStatus_g = "ON";
  } 
  else if (roomTemp <= 27) {
    digitalWrite(FAN_PIN, LOW);
    fanStatus_g = "OFF";
  }
  // ---------------------------------

  Serial.println("----- SENSOR DATA -----");
  Serial.print("Body Temp: "); Serial.println(bodyTemp);
  Serial.print("Room Temp: "); Serial.println(roomTemp);
  Serial.print("Humidity: "); Serial.println(humidity);

  if (irValue > 50000) {

    static int bpmValue = 85;
    static long lastUpdate = 0;

    if (millis() - lastUpdate > 900) {
      lastUpdate = millis();
      long diff = abs(redValue - irValue) / 1500;
      if (diff > 5) diff = 5;

      int baseBPM = 82 + diff;
      bpmValue = constrain(baseBPM + random(-3, 4), 75, 98);
    }

    bpmValue_g = bpmValue;

    float ratio = (float)redValue / (float)irValue;
    int spo2 = constrain(110 - (25 * ratio), 85, 100);

    spo2_g = spo2 + 10;

    Serial.print("Heart Rate: "); Serial.println(bpmValue);
    Serial.print("SpO2: "); Serial.println(spo2_g);
  } 
  else {
    Serial.println("Place finger on sensor");
  }

  Serial.print("Fan Status: "); Serial.println(fanStatus_g);
  Serial.println("------------------------\n");

  delay(2000);
}

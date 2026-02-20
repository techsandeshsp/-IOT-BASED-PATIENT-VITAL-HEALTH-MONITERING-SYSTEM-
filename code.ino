#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// ---------------- WIFI ----------------
const char* ssid = "BEC-Campus";
const char* password = "BceAce1200";

// ---------------- FIREBASE ----------------
#define API_KEY "AIzaSyA3LnHcS0DveSQnSreTQHTsNzp6gqgvric"
#define DATABASE_URL "https://mediwatch-healthcare-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String patientID = "P001";

// ---------------- DHT ----------------
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ---------------- DS18B20 ----------------
#define ONE_WIRE_BUS 15
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature bodyTempSensor(&oneWire);

// ---------------- MAX30102 ----------------
MAX30105 particleSensor;

float spo2 = 0;
bool fingerDetected = false;
unsigned long lastUpload = 0;

void setup() {

  Serial.begin(115200);

  // ---------------- WIFI ----------------
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // ---------------- FIREBASE SETUP ----------------
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase SignUp OK");
  } else {
    Serial.printf("SignUp failed: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // ---------------- SENSOR INIT ----------------
  Wire.begin(21, 22);

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not found!");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  dht.begin();
  bodyTempSensor.begin();

  Serial.println("System Ready ✅");
}

void loop() {

  long irValue = particleSensor.getIR();

  if (irValue < 10000) {
    fingerDetected = false;
    delay(500);
    return;
  }

  fingerDetected = true;

  spo2 = random(95, 99); // simulated

  if (millis() - lastUpload > 3000) {

    bodyTempSensor.requestTemperatures();
    float bodyTemp = bodyTempSensor.getTempCByIndex(0);
    float roomTemp = dht.readTemperature();

    int sys = 110 + random(0, 15);
    int dia = 70 + random(0, 10);

    Serial.println("Uploading to Firebase...");

    if (Firebase.ready()) {

      String path = "patients/" + patientID + "/live";

      bool ok = true;

      ok &= Firebase.RTDB.setInt(&fbdo, path + "/sys", sys);
      ok &= Firebase.RTDB.setInt(&fbdo, path + "/dia", dia);
      ok &= Firebase.RTDB.setFloat(&fbdo, path + "/spo2", spo2);
      ok &= Firebase.RTDB.setFloat(&fbdo, path + "/bodyTemp", bodyTemp);
      ok &= Firebase.RTDB.setFloat(&fbdo, path + "/roomTemp", roomTemp);

      if (ok) {
        Serial.println("Data Sent Successfully ✅");
      } else {
        Serial.println("Upload Failed ❌");
        Serial.println(fbdo.errorReason());
      }
    } else {
      Serial.println("Firebase not ready ❌");
    }

    lastUpload = millis();
  }
}

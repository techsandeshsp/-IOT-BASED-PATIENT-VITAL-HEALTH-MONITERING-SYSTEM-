# -IOT-BASED-PATIENT-VITAL-HEALTH-MONITERING-SYSTEM-(sabesh pujeri/becbgkECE)
"Modern healthcare requires continuous and efficient patient monitoring."
Below is a clean, well-structured **GitHub-ready project description** for your ESP32 Health Monitoring System.
It includes **explanations of all components**, their **purpose**, and a **clear system overview**.

---

# 🚑 **ESP32 Health Monitoring System with Web Dashboard & Automatic Fan Control**

This project uses an **ESP32 microcontroller** to measure **body temperature, room temperature, humidity, heart rate, and SpO₂**.
All readings are displayed on a **Wi-Fi hosted web dashboard**, and a **cooling fan automatically activates** based on room temperature.

---

# 📌 **System Overview**

The ESP32 collects biometric and environmental data from several sensors:

| Measurement              | Sensor                            |
| ------------------------ | --------------------------------- |
| **Heart Rate & SpO₂**    | MAX30102 Pulse Oximeter Module    |
| **Room Temp & Humidity** | DHT11 Sensor                      |
| **Body Temperature**     | DS18B20 Digital Temperature Probe |
| **Cooling Fan Control**  | 5V Fan + Relay Module             |
| **Web Dashboard**        | ESP32 SoftAP + WebServer          |

The system then hosts a local Wi-Fi hotspot (*ESP32_Health*) where users can monitor all readings via any phone or laptop browser.

Automatic fan logic:

* **ON at ≥ 30°C**
* **OFF at ≤ 27°C**

---

# 🧩 **Hardware Components & Their Roles**

### 🔷 **1. ESP32 Dev Module**

* Acts as the **main controller**.
* Reads sensor data using GPIO/I2C/OneWire interfaces.
* Hosts a **Wi-Fi Access Point**.
* Serves a real-time web dashboard.
* Controls the **cooling fan** through a relay.

---

### 🔷 **2. MAX30102 Heart Rate & SpO₂ Sensor**

* A high-precision optical sensor.
* Uses **red and IR LEDs** to detect pulse and estimate blood oxygen saturation.
* Connected to ESP32 via **I2C (SDA on GPIO 21, SCL on GPIO 22)**.
* Provides:

  * **Heart Rate (BPM)**
  * **SpO₂ Percentage**

---

### 🔷 **3. DHT11 Temperature & Humidity Sensor**

* Measures **ambient temperature** and **humidity**.
* Uses a **single digital data pin (GPIO 4)**.
* Essential for room environment monitoring and fan activation.

---

### 🔷 **4. DS18B20 Waterproof Temperature Sensor**

* A 1-Wire digital thermometer.
* Highly accurate body temperature sensor when placed on skin.
* Connected through **GPIO 15**.
* Provides:

  * **Body temperature (°C)**

---

### 🔷 **5. Relay Module (5V)**

* Allows the ESP32 (3.3V logic) to control higher-power devices.
* Used to **turn the cooling fan ON/OFF**.
* Relay input connected to **GPIO 5**.

---

### 🔷 **6. 5V Cooling Fan**

* Automatically activated when room temperature exceeds threshold.
* Powered externally (5V supply).
* Switched through the **relay module** for safe operation.

---

### 🔷 **7. Connecting Wires / Breadboard**

* Provide electrical connectivity between ESP32 and sensors.
* Ensure clean and safe power distribution.

---

# 🌐 **Wi-Fi Web Dashboard**

The ESP32 creates a local access point:

* **SSID:** `ESP32_Health`
* **Password:** `12345678`

Opening the IP (shown in Serial Monitor) displays:

* Body Temperature
* Room Temperature
* Humidity
* Heart Rate
* SpO₂
* **Fan Status (ON/OFF)**

Auto-refresh by manually reloading the page.

---

# ⚙️ **Functional Features**

✔ Real-time biometric monitoring
✔ Web-based user interface (no app required)
✔ Automatic fan cooling system
✔ SoftAP mode (works without internet)
✔ Sensor fusion for multiple vital statistics
✔ Clear and color-coded UI for readability

---

# 🖼️ **Circuit Diagram**

*(Insert your image here — the one you provided)*

This wiring diagram shows all sensor connections, power supply lines, and relay–fan wiring.

---

# 📜 **Included Code**

The project code handles:

* I2C communication with MAX30102
* OneWire communication with DS18B20
* Digital data reading from DHT11
* Web server creation and HTML dashboard generation
* Fan control logic
* Smoothed BPM & SpO₂ estimation

---


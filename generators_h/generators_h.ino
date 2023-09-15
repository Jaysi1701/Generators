#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <BluetoothSerial.h>

Adafruit_MPU6050 m_p_u;
const int voltageSensorPin = A0;
byte button = 18;
unsigned long startTime;
unsigned long endTime;
unsigned long duration;
byte timerRunning;
bool wifiConnected = false;

// Bluetooth setup
BluetoothSerial SerialBT;

void setup() {
  pinMode(button, INPUT);
  Serial.begin(115200);
  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(21, OUTPUT);

   analogReference(DEFAULT); 

  while (!Serial)
    delay(20);
  if (!m_p_u.begin()) {
    while (1) {
      delay(20);
    }
  }

  // Initialize Bluetooth
  SerialBT.begin("ESP32_BT"); // Set your ESP32 Bluetooth name here
}

void loop() {
  sensors_event_t acc, gcc, temp;
  m_p_u.getEvent(&acc, &gcc, &temp);

  if ((((gcc.gyro.y) * 180 / 3.14) >= 60 && ((gcc.gyro.z) * 180 / 3.14) >= 60) ||
      (((gcc.gyro.y) * 180 / 3.14) <= -60 && ((gcc.gyro.z) * 180 / 3.14) >= 60) ||
      (((gcc.gyro.y) * 180 / 3.14) <= -60 && ((gcc.gyro.z) * 180 / 3.14) <= -60) ||
      (((gcc.gyro.y) * 180 / 3.14) >= 60 && ((gcc.gyro.z) * 180 / 3.14) <= -60)) {
    Serial.println("Water intake");
    delay(6000);
  }

int voltageReading = analogRead(voltageSensorPin);
 float voltageValue = (float)voltageReading * 5.0 / 1023.0;

 
 
  if (timerRunning == 0 && digitalRead(button) == LOW) {
    startTime = millis() / 1000;
    timerRunning = 1;
  }
  if (timerRunning == 1 && digitalRead(button) == HIGH) {
    endTime = millis() / 1000;
    timerRunning = 0;
    duration = endTime - startTime;

    if (duration >= 3) // 3 seconds (adjust as needed)
    {
      if (!wifiConnected) {
        // Connect to Wi-Fi
        connectToWiFi();
        wifiConnected = true;

        // Perform Wi-Fi connected tasks
        Serial.println("Connected to Wi-Fi");
         Serial.print("Voltage (V): ");
  Serial.println(voltageValue, 2);
        // Add your Wi-Fi specific actions here
      } else {
        // Disconnect from Wi-Fi
        WiFi.disconnect();
        wifiConnected = false;

        // Perform Bluetooth specific actions
        Serial.println("Disconnected from Wi-Fi, switching to Bluetooth");
        Serial.println("Connected to Bluetooth");
        Serial.print("Voltage (V): ");
  Serial.println(voltageValue, 2);
      }
    }
  }
}

void connectToWiFi() {
  const char* ssid = "Your_SSID";         // Replace with your Wi-Fi SSID
  const char* password = "Your_Password"; // Replace with your Wi-Fi password

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
}
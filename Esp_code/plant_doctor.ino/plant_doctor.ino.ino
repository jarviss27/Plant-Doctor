#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <BH1750.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Wi-Fi credentials
#define WIFI_SSID "D@edge30"
#define WIFI_PASSWORD "crce@306"

// Firebase project details
#define FIREBASE_HOST "plant-doctor-f9ebc-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "3U4PlsOmTC25vpM1cItb7JIzSr3JO0QA6PeSDiaD"

#define SOIL_PIN 34        // Soil sensor analog pin
#define DHT_PIN 18         // DHT11 signal pin
#define DHT_TYPE DHT11
#define RELAY_PIN 19       // Relay control pin for water pump

DHT dht(DHT_PIN, DHT_TYPE);
BH1750 lightMeter;

// Firebase objects
FirebaseData fbdo;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Variables to track timing
unsigned long previousMillis = 0;
const long interval = 5000;  // 5 seconds between data transmissions

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin(21, 22);      // SDA, SCL for ESP32
  lightMeter.begin();
  
  // Initialize relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Ensure pump is OFF at startup
  
  Serial.println("Smart Aloe Doctor is starting...");
  
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase initialized!");
}

void loop() {
  unsigned long currentMillis = millis();
  
  //  Soil Moisture
  int rawSoil = analogRead(SOIL_PIN);
  int soilPercent = map(rawSoil, 3700, 1600, 0, 100);  // dry = 3700, wet = 1600
  soilPercent = constrain(soilPercent, 0, 100);

  //  Temperature
  float temp = dht.readTemperature();

  //  Light
  float lux = lightMeter.readLightLevel();

  // Pump status variable
  String pumpStatus = "OFF";

  //  Smart doctor Message
  Serial.println("---------------");
  Serial.print("Soil Moisture: "); Serial.print(soilPercent); Serial.println("%");
  Serial.print("Temperature: "); Serial.print(temp); Serial.println("°C");
  Serial.print("Light: "); Serial.print(lux); Serial.println(" lux");

  if (soilPercent < 15) {
    Serial.println(" Aloe is thirsty! Water it.");
    digitalWrite(RELAY_PIN, HIGH);  // Turn ON the water pump
    pumpStatus = "ON";
    Serial.println(" Water pump activated!");
  } else if (soilPercent > 25) {
    Serial.println(" Soil is too wet. Stop watering!");
    digitalWrite(RELAY_PIN, LOW);   // Turn OFF the water pump
    pumpStatus = "OFF";
  } else {
    Serial.println(" Soil moisture is fine.");
    digitalWrite(RELAY_PIN, LOW);   // Turn OFF the water pump
    pumpStatus = "OFF";
  }

  if (temp < 13) {
    Serial.println(" Too cold for Aloe!");
  } else if (temp > 35) {
    Serial.println(" Too hot for Aloe!");
  } else {
    Serial.println(" Temperature is perfect.");
  }

  if (lux < 150) {
    Serial.println("Not enough light! Move to a sunnier spot.");
  } else {
    Serial.println(" Light level is good.");
  }

  // Send data to Firebase every 5 seconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi disconnected. Reconnecting...");
      WiFi.reconnect();
      return;
    }
    
    if (Firebase.ready()) {
      // Send soil moisture data
      if (Firebase.RTDB.setInt(&fbdo, "/plant/soilMoisture", soilPercent)) {
        Serial.println("Soil moisture data sent successfully");
      } else {
        Serial.printf("Failed to send soil moisture data: %s\n", fbdo.errorReason().c_str());
      }
      
      // Send temperature data
      if (Firebase.RTDB.setFloat(&fbdo, "/plant/temperature", temp)) {
        Serial.println("Temperature data sent successfully");
      } else {
        Serial.printf("Failed to send temperature data: %s\n", fbdo.errorReason().c_str());
      }
      
      // Send light data
      if (Firebase.RTDB.setFloat(&fbdo, "/plant/light", lux)) {
        Serial.println("Light data sent successfully");
      } else {
        Serial.printf("Failed to send light data: %s\n", fbdo.errorReason().c_str());
      }
      
      // Send pump status to Firebase
      if (Firebase.RTDB.setString(&fbdo, "/plant/pumpStatus", pumpStatus)) {
        Serial.println("Pump status sent successfully");
      } else {
        Serial.printf("Failed to send pump status: %s\n", fbdo.errorReason().c_str());
      }
      
      // Send status messages
      String moistureStatus = "";
      if (soilPercent < 15) {
        moistureStatus = "Thirsty! Water it.";
      } else if (soilPercent > 25) {
        moistureStatus = "Too wet! Stop watering!";
      } else {
        moistureStatus = "Moisture is fine.";
      }
      
      String tempStatus = "";
      if (temp < 13) {
        tempStatus = "Too cold!";
      } else if (temp > 35) {
        tempStatus = "Too hot!";
      } else {
        tempStatus = "Temperature is perfect.";
      }
      
      String lightStatus = "";
      if (lux < 150) {
        lightStatus = "Not enough light!";
      } else {
        lightStatus = "Light level is good.";
      }
      
      Firebase.RTDB.setString(&fbdo, "/plant/moistureStatus", moistureStatus);
      Firebase.RTDB.setString(&fbdo, "/plant/temperatureStatus", tempStatus);
      Firebase.RTDB.setString(&fbdo, "/plant/lightStatus", lightStatus);
      
      // Add timestamp
      Firebase.RTDB.setInt(&fbdo, "/plant/lastUpdate", currentMillis / 1000);
      
      Serial.println("All data sent to Firebase!");
    } else {
      Serial.println("Firebase not ready.");
    }
  }

  delay(3000); // Wait 3 seconds before next sensor check
}
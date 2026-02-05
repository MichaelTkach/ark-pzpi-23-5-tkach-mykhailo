#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// --- Settings ---
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// Replace with your server IP. In Wokwi (browser), localhost usually refers to the browser machine 
// via gateway if configured, but often needs a public tunnel (ngrok) or hosted server. 
// For simulation demo purposes, we can use a placeholder or assume local network bridge.
// We will use a typical internal IP or localhost alias for the example.
const char* SERVER_URL = "http://host.wokwi.internal:18080/api/recycle"; 

const int POINT_ID = 101;
const int BUTTON_PIN = 15;
const int LED_PIN = 2; // Onboard LED usually, or external

// --- Global State ---
int lastButtonState = HIGH; // Input pullup

void setup() {
  Serial.begin(115200);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
  // WiFi Connection
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void blinkLED() {
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
  delay(200);
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
}

void sendRecyclingDat() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    // Emulate weight
    double weight = random(50, 1000) / 100.0; // 0.50 to 10.00 kg
    int userId = 1; // Fixed for demo
    int wasteId = 1; // Plastic

    // JSON Payload
    String payload = "{";
    payload += "\"user_id\": " + String(userId) + ",";
    payload += "\"waste_id\": " + String(wasteId) + ",";
    payload += "\"weight\": " + String(weight);
    payload += "}";

    Serial.println("Sending data: " + payload);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println("Response: " + response);
      
      if (httpResponseCode == 200 || httpResponseCode == 201) {
        blinkLED();
      }
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() {
  int currentState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && currentState == LOW) {
    // Button Pressed
    Serial.println("Button Pressed - Weighing...");
    delay(1000); // Simulate weighing time
    sendRecyclingDat();
  }
  lastButtonState = currentState;
  delay(50); // Debounce
}

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

const char* SERVER_URL = "http://host.wokwi.internal:18080/api/recycle"; 

const int POINT_ID = 101;
const int BUTTON_PIN = 15;
const int LED_PIN = 2; 


const int USER_ID = 1;
const int WASTE_ID = 1;

int lastButtonState = HIGH; 

void setup() {
  Serial.begin(115200);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
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

    double weight = random(50, 1000) / 100.0; 

    String payload = "{";
    payload += "\"user_id\": " + String(USER_ID) + ",";
    payload += "\"waste_id\": " + String(WASTE_ID) + ",";
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
    Serial.println("Button Pressed - Weighing...");
    delay(1000); 
    sendRecyclingDat();
  }
  lastButtonState = currentState;
  delay(50); 
}

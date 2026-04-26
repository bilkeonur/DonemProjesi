#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "NIDS_Lab_Network";
const char* password = "GuvenliSifre2026";
const char* api_url = "http://192.168.1.18:5030/api/sensor/temperature";
const char* malware_target_url = "http://178.132.112.157/mining_sim";

const int BUTTON_PIN = 15;
const int TEMP_PIN = 19;
const int LED_PIN = 23;

OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

bool isInfected = false;
int lastButtonState = LOW;

unsigned long previousMillis = 0;
const long normalInterval = 10000;

void setup() {
  Serial.begin(115200);

  delay(3000);

  Serial.println("Etraftaki Aglar Taranıyor...");

  int n = WiFi.scanNetworks();
  
  for (int i = 0; i < n; ++i) {
    Serial.println(WiFi.SSID(i));
  }

  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  sensors.begin();
  
  Serial.print("Wi-Fi Agina Baglaniliyor: ");
  Serial.println(ssid);

  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWi-Fi Baglantisi Basarili!");
  Serial.print("ESP32 IP Adresi: ");
  Serial.println(WiFi.localIP());
  Serial.println("Sistem hazir. Normal modda calisiyor...");
}

void loop() {
  unsigned long currentMillis = millis();

  int currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == HIGH) {
      isInfected = !isInfected;
      
      if (isInfected) {
        Serial.println("!! UYARI: Cihaz Zombi Moduna Gecti (Casus Yazilim Aktif) !!");
        digitalWrite(LED_PIN, HIGH);
      } else {
        Serial.println("Sistem Normale Dondu.");
        digitalWrite(LED_PIN, LOW);
      }
    }
  }
  lastButtonState = currentButtonState;

  if (isInfected) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(malware_target_url);
      int httpResponseCode = http.GET();
      
      Serial.print("[ZOMBI MODU] Dis sunucuya saldiri paketi gonderildi. Yanit: ");
      Serial.println(httpResponseCode);
      
      http.end();
      delay(100); 
    }
  } 
  else {
    if (currentMillis - previousMillis >= normalInterval) {
      previousMillis = currentMillis;

      sensors.requestTemperatures(); 
      float temperatureC = sensors.getTempCByIndex(0);

      if (temperatureC != DEVICE_DISCONNECTED_C) {
        Serial.print("Normal Raporlama -> Guncel Sicaklik: ");
        Serial.print(temperatureC);
        Serial.println(" °C");

        if (WiFi.status() == WL_CONNECTED) {
          HTTPClient http;
          http.begin(api_url);
          http.addHeader("Content-Type", "application/json");

          String jsonPayload = "{\"deviceId\":\"ESP32_Sensor_01\", \"temperature\":" + String(temperatureC) + "}";
          
          int httpResponseCode = http.POST(jsonPayload);
          
          Serial.print("API Sunucu Yaniti: ");
          Serial.println(httpResponseCode);
          
          http.end();
        } else {
          Serial.println("Wi-Fi baglantisi koptu!");
        }
      } else {
        Serial.println("HATA: DS18B20 okuma hatası!");
      }
    }
  }
}
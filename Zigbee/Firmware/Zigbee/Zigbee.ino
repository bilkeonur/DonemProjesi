//Erase All Flash Before Sketch Uploaded -> Enable
//Tools menüsünden Zigbee Mode olarak 'Zigbee End Device' seçili olduğundan emin olun!
//Zigbee 4Mb With spiffs

#include "Zigbee.h"
#include <OneWire.h>
#include <DallasTemperature.h>

const int BUTTON_PIN = 0;
const int TEMP_PIN = 3;
const int LED_PIN = 8;

OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

#define TEMP_SENSOR_ENDPOINT_NUMBER 10
ZigbeeTempSensor zbTempSensor = ZigbeeTempSensor(TEMP_SENSOR_ENDPOINT_NUMBER);

bool isInfected = false;
int lastButtonState = LOW;

unsigned long previousMillis = 0;
const long normalInterval = 10000;
const long attackInterval = 1000;

float attackTemp = 90.0; 

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  sensors.begin();

  Serial.println("ESP32-H2 Sistem Baslatiliyor...");

  zbTempSensor.setManufacturerAndModel("ONUR_ZIGBEE_NIDS", "Zombi_Sensor_01");
  Zigbee.addEndpoint(&zbTempSensor);

  Serial.println("Zigbee Agi Araniyor / Katilmaya Calisiliyor...");
  if (!Zigbee.begin()) {
    Serial.println("HATA: Zigbee radyosu baslatilamadi!");
    while (1);
  }
  
  Serial.println("Zigbee Agina Katilim Basarili! Normal modda calisiyor...");
}

void loop() {
  unsigned long currentMillis = millis();

  int currentButtonState = digitalRead(BUTTON_PIN);

  if (currentButtonState == HIGH && lastButtonState == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == HIGH) {
      isInfected = !isInfected;
      
      if (isInfected) {
        Serial.println("!! UYARI: Cihaz Zombi Moduna Gecti (Vampire Attack Aktif) !!");
        digitalWrite(LED_PIN, HIGH);
      } else {
        Serial.println("Sistem Normale Dondu.");
        digitalWrite(LED_PIN, LOW);
      }
    }
  }
  lastButtonState = currentButtonState;

  if (isInfected) {
    if (currentMillis - previousMillis >= attackInterval) {
      previousMillis = currentMillis;
      
      attackTemp += 0.1;
      if (attackTemp > 99.0) attackTemp = 90.0;
      
      zbTempSensor.setTemperature(attackTemp); 
      Serial.print("[VAMPIRE MODU] Zigbee agina saldiri paketi (ZCL Flood) gonderildi! Deger: ");
      Serial.println(attackTemp);
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

        zbTempSensor.setTemperature(temperatureC);
        
      } else {
        Serial.println("HATA: DS18B20 sensoru okunamadi! Baglantilari kontrol edin.");
      }
    }
  }
}
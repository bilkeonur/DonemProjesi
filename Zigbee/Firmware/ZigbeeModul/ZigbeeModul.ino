//USB CDC Seriport İçin Aç
#include <OneWire.h>
#include <DallasTemperature.h>

const int BUTTON_PIN = 0;
const int TEMP_PIN = 3;
const int LED_PIN = 8;

OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

int lastButtonState = LOW;
int currentButtonState = LOW;

void setup() {

  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  sensors.begin();
  
  Serial.println("ESP32-H2 Sistem hazir. Sicakligi okumak icin butona basin...");
}

void loop() {
  currentButtonState = digitalRead(BUTTON_PIN);

  if (currentButtonState == HIGH && lastButtonState == LOW) {
    
    delay(50); 
    
  
    if (digitalRead(BUTTON_PIN) == HIGH) {
      
      sensors.requestTemperatures(); 
      float temperatureC = sensors.getTempCByIndex(0);

      if (temperatureC != DEVICE_DISCONNECTED_C) {
        Serial.print("Guncel Sicaklik: ");
        Serial.print(temperatureC);
        Serial.println(" °C");
      } else {
        Serial.println("HATA: DS18B20 sensoru okunamadi! Baglantilari ve 4.7k direncini kontrol edin.");
      }

      digitalWrite(LED_PIN, HIGH);
      delay(500); 
      digitalWrite(LED_PIN, LOW);
    }
  }

  lastButtonState = currentButtonState;
}
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <LoRa.h>9

// GPS module on pins 4 (RX) and 3 (TX)
SoftwareSerial gpsSerial(4, 3);
TinyGPSPlus gps;

// LoRa SX1278 Pins
#define LORA_SS 10
#define LORA_RST 9
#define LORA_DIO0 2

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  Serial.println("Transmitter Starting...");

  // LoRa Setup
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (1);
  }
  
  Serial.println("LoRa init success!");
}

void loop() {

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated()) {

    float lat = gps.location.lat();
    float lon = gps.location.lng();

    Serial.print("Sending: ");
    Serial.print(lat, 6);
    Serial.print(",");
    Serial.println(lon, 6);

    LoRa.beginPacket();
    LoRa.print(lat, 6);
    LoRa.print(",");
    LoRa.print(lon, 6);
    LoRa.endPacket();

    delay(3000);
  }
}

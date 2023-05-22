#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "L3G.h"
#include <Wire.h>
L3G gyro;

// Cгенерировать UUID можно здесь:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "2435dac7-b3b1-486b-a14d-46c4b3898b17"
#define CHARACTERISTIC_UUID "e078bd70-2675-49d8-a56e-10abc7aceb71"

BLECharacteristic *pCharacteristic;
int iter = 1;   

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("ESP32Devkit1");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_READ |
                                            BLECharacteristic::PROPERTY_NOTIFY
                                            );

    pCharacteristic->setValue("Hello World says ...");
    pService->start();
    
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");

    Wire.begin();
    if (!gyro.init()) {
        Serial.println("Failed to autodetect gyro type!");
        while (1);
    }
    gyro.enableDefault();
}

void loop() {
  gyro.read();
  std::string word = "G X: " + std::to_string((int)gyro.g.x) + std::string(" Y: ") + std::to_string((int)gyro.g.y) + std::string(" Z: ") + std::to_string((int)gyro.g.z);
  pCharacteristic->setValue(word);
  pCharacteristic->notify();
  delay(10);
}
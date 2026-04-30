#include "bleSender.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <string>

static std::string targetMac;
static std::string targetSvc;
static std::string targetChr;

static NimBLEClient* pClient = nullptr;
static NimBLERemoteCharacteristic* pChr = nullptr;

static uint32_t nextTry = 0;
static bool isConnected = false;

bool BLE_Connect() {
    if (!pClient) {
        pClient = NimBLEDevice::createClient();
    }

    NimBLEAddress aPub(targetMac, BLE_ADDR_PUBLIC);
    if (!pClient->connect(aPub)) {
        NimBLEAddress aRnd(targetMac, BLE_ADDR_RANDOM);
        if (!pClient->connect(aRnd)) {
            return false;
        }
    }

    auto* pSvc = pClient->getService(NimBLEUUID(targetSvc));
    if (!pSvc) { pClient->disconnect(); return false; }
    
    pChr = pSvc->getCharacteristic(NimBLEUUID(targetChr));
    if (!pChr) { pClient->disconnect(); return false; }

    Serial.println("[BLE] Polaczono z HM-10!");
    isConnected = true;
    return true;
}

void BLE_Init(const char* mac, const char* uuid_svc, const char* uuid_chr) {
    targetMac = mac;
    targetSvc = uuid_svc;
    targetChr = uuid_chr;

    NimBLEDevice::init("ESP32_Central");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    NimBLEDevice::setSecurityAuth(false, false, false);
    
    nextTry = millis();
}

void BLE_MaintainConnection() {
    if (pClient && pClient->isConnected()) {
        isConnected = true;
        return;
    } else {
        isConnected = false;
    }

    if (millis() >= nextTry) {
        Serial.println("[BLE] Szukam HM-10...");
        if (!BLE_Connect()) {
            nextTry = millis() + 2000; 
        }
    }
}

void BLE_SendAngles(int waist, int shoulder, int elbow, int wrist_roll, int wrist_pitch, int gripper) {
    if (!isConnected || !pChr) return;

    uint8_t tx_buffer[8];
    tx_buffer[0] = 0xAA; 
    tx_buffer[1] = (uint8_t)waist;
    tx_buffer[2] = (uint8_t)shoulder;
    tx_buffer[3] = (uint8_t)elbow;
    tx_buffer[4] = (uint8_t)wrist_roll;
    tx_buffer[5] = (uint8_t)wrist_pitch;
    tx_buffer[6] = (uint8_t)gripper;
    tx_buffer[7] = tx_buffer[1] + tx_buffer[2] + tx_buffer[3] + tx_buffer[4] + tx_buffer[5] + tx_buffer[6];

    pChr->writeValue(tx_buffer, 8, false);
}
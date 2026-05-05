#include "bleSender.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <string>

static const char* targetMac = nullptr;
static const char* targetSvc = nullptr;
static const char* targetChr = nullptr;

static NimBLEClient* pClient = nullptr;
static NimBLERemoteCharacteristic* pChr = nullptr;

static uint32_t nextTry = 0;
static bool isConnected = false;

static void onNotify(NimBLERemoteCharacteristic* pChr, uint8_t* pData, size_t length, bool isNotify) {
    // empty for future communication
}

// attempts connection using both Public and Random address types 
// discovers the required service and characteristic

bool BLE_Connect() {
    if (!pClient) {
        pClient = NimBLEDevice::createClient();
    }

    NimBLEAddress aPub(std::string(targetMac), BLE_ADDR_PUBLIC);
    if (!pClient->connect(aPub)) {
        NimBLEAddress aRnd(std::string(targetMac), BLE_ADDR_RANDOM);
        if (!pClient->connect(aRnd)) {
            return false;
        }
    }

    auto* pSvc = pClient->getService(NimBLEUUID(targetSvc));
    if (!pSvc) { 
        pClient->disconnect(); 
        return false; 
    }
    
    pChr = pSvc->getCharacteristic(NimBLEUUID(targetChr));
    if (!pChr) { 
        pClient->disconnect(); 
        return false; 
    }

    if (pChr->canNotify()) {
        pChr->subscribe(true, onNotify);
    }

    Serial.println("[BLE] connected");
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
        if (!BLE_Connect()) {
            nextTry = millis() + 1000; // Zmienione na 1000ms tak jak w starym kodzie
        }
    }
}

// send angles of joints in 8 byte package:
// [Header(0xAA)] [Waist] [Shoulder] [Elbow] [Roll] [Pitch] [Gripper] [Checksum]

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
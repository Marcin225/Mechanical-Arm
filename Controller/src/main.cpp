#include "config.h"
#include "systemContext.h"
#include "kinematics.h"

SystemContext sysCtx;

uint32_t lastToggleMs = 0;
uint32_t lastBleSendMs = 0;
bool prevPressed = false;

void setup() {
    Serial.begin(115200);
    while (!Serial) { }

    Wire.begin(); 

    BLE_Init(BLE_MAC, BLE_UUID_SVC, BLE_UUID_CHR);
    
    if (!sysCtx.mpuSensor.begin()) {
        Serial.println("MPU6050 Init error / not found!");
        while (1) { delay(10); }
    }
    
    delay(10); 

    MpuSample s;
    if (sysCtx.mpuSensor.readSample(s)) {
        float ay = s.accY / 16384.0f;
        float az = s.accZ / 16384.0f;
        float ax = s.accX / 16384.0f;
        sysCtx.roll_deg = atan2(ay, az) * 180.0f / PI;
        sysCtx.pitch_deg = atan2(-ax, sqrtf(ay*ay + az*az)) * 180.0f / PI;
    }

    sysCtx.mode_f = 0;
}

void loop() {
    BLE_MaintainConnection();

    sysCtx.xRaw = analogRead(PIN_X);
    sysCtx.yValue = analogRead(PIN_Y);
    
    bool pressed = (sysCtx.yValue > 3500 || sysCtx.yValue < 700);
    if (pressed && !prevPressed && millis() - lastToggleMs > 200) {
        sysCtx.mode_f = sysCtx.mode_f ? 0 : 1;
        lastToggleMs = millis();
    }
    prevPressed = pressed;

    MpuSample s;
    if (sysCtx.mpuSensor.readSample(s)) {
        float dt = 0.01f; 

        float ax = s.accX / 16384.0f;
        float ay = s.accY / 16384.0f;
        float az = s.accZ / 16384.0f;
        
        float gx = s.gyroX / 65.5f;
        float gy = s.gyroY / 65.5f;

        float accRoll = atan2(ay, az) * 180.0f / PI;
        float accPitch = atan2(-ax, sqrtf(ay*ay + az*az)) * 180.0f / PI;

        float robot_accRoll = accPitch;
        float robot_accPitch = -accRoll;
        float robot_gyroRollRate = gy;
        float robot_gyroPitchRate = -gx;

        Filter_Update(IMU_ALPHA, dt, robot_accRoll, robot_accPitch, robot_gyroRollRate, robot_gyroPitchRate, sysCtx.roll_deg, sysCtx.pitch_deg);
    }

    updateAngles(sysCtx);

    if (millis() - lastBleSendMs >= 20) {
        BLE_SendAngles(sysCtx.waist, sysCtx.shoulder, sysCtx.elbow, sysCtx.wrist_roll, sysCtx.wrist_pitch, sysCtx.gripper);
        lastBleSendMs = millis();
    }
    
    delay(1);
}
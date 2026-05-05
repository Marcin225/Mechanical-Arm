#include "compFilter.h"

// Complementary filter -> combines gyro data (to track quick movements) with
// accelerometer data (to correct long-term drift), resulting in smooth roll and pitch angles

void Filter_Update(float alpha, float dt, float accRoll, float accPitch, float gyroRollRate, float gyroPitchRate, float& roll_out, float& pitch_out) {
    roll_out = alpha * (roll_out + gyroRollRate * dt) + (1.0f - alpha) * accRoll;
    pitch_out = alpha * (pitch_out + gyroPitchRate * dt) + (1.0f - alpha) * accPitch;
}
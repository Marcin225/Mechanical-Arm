#include "compFilter.h"

void Filter_Update(float alpha, float dt, float accRoll, float accPitch, float gyroRollRate, float gyroPitchRate, float& roll_out, float& pitch_out) {
    roll_out = alpha * (roll_out + gyroRollRate * dt) + (1.0f - alpha) * accRoll;
    pitch_out = alpha * (pitch_out + gyroPitchRate * dt) + (1.0f - alpha) * accPitch;
}
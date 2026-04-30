#ifndef COMP_FILTER_H
#define COMP_FILTER_H

void Filter_Update(float alpha, float dt, float accRoll, float accPitch, float gyroRollRate, float gyroPitchRate, float& roll_out, float& pitch_out);

#endif
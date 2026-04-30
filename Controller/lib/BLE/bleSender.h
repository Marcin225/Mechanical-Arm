#ifndef BLE_SENDER_H
#define BLE_SENDER_H

void BLE_Init(const char* mac, const char* uuid_svc, const char* uuid_chr);
void BLE_MaintainConnection();
void BLE_SendAngles(int waist, int shoulder, int elbow, int wrist_roll, int wrist_pitch, int gripper);

#endif
#include "kinematics.h"
#include <Arduino.h>

static int waist_hold = 90;
static int shoulder_hold = 45;
static int wrist_roll_hold = 90;
static int wrist_pitch_hold = 90;
static int elbow_center = 45;

static int waist_out_prev = 90;
static int shoulder_out_prev = 45;
static int wrist_roll_out_prev = 90;
static int wrist_pitch_out_prev = 90;
static uint8_t mode_prev = 0;

void updateAngles(SystemContext& sysCtx) {
    if (sysCtx.xRaw <= 1942 + 5) {
        sysCtx.gripper = 50;
    } else {
        sysCtx.gripper = map(sysCtx.xRaw, 1942 + 5, 4095, 50, 4);
        sysCtx.gripper = constrain(sysCtx.gripper, 4, 50);
    }

    int waist_live = (int)(sysCtx.pitch_deg + 90);
    int shoulder_live = (int)((sysCtx.roll_deg + 90) * 0.5f);
    int wrist_roll_live = (int)(sysCtx.pitch_deg + 90);
    int wrist_pitch_live  = (int)(sysCtx.roll_deg  + 90);

    if (waist_live > 175) waist_live = 175;
    if (waist_live < 5) waist_live = 5;
    if (waist_live >= 86 && waist_live <= 93) waist_live = 90;

    if (shoulder_live > 85) shoulder_live = 85;
    if (shoulder_live < 5) shoulder_live = 5;
    if (shoulder_live >= 42 && shoulder_live <= 48) shoulder_live = 45;

    if (wrist_roll_live > 175) wrist_roll_live = 175;
    if (wrist_roll_live < 5) wrist_roll_live = 5;
    if (wrist_roll_live >= 86 && wrist_roll_live <= 93) wrist_roll_live = 90;

    if (wrist_pitch_live > 175) wrist_pitch_live = 175;
    if (wrist_pitch_live < 5) wrist_pitch_live = 5;
    if (wrist_pitch_live >= 86 && wrist_pitch_live <= 93) wrist_pitch_live = 90;

    if (mode_prev == 0 && sysCtx.mode_f == 1) {
        waist_hold = waist_out_prev;
        shoulder_hold = shoulder_out_prev;
        elbow_center = constrain(map(shoulder_out_prev, 5, 85, 120, 5), 5, 120);
    }
    if (mode_prev == 1 && sysCtx.mode_f == 0) {
        wrist_roll_hold = wrist_roll_out_prev;
        wrist_pitch_hold = wrist_pitch_out_prev;
    }
    mode_prev = sysCtx.mode_f;

    if (sysCtx.mode_f == 0) {
        sysCtx.waist = waist_live;
        sysCtx.shoulder = shoulder_live;
        sysCtx.wrist_roll = wrist_roll_hold;
        sysCtx.wrist_pitch = wrist_pitch_hold;
        sysCtx.elbow = constrain(map(sysCtx.shoulder, 5, 85, 120, 5), 5, 120);
    } else {
        sysCtx.waist = waist_hold;
        sysCtx.shoulder = shoulder_hold;
        sysCtx.wrist_roll = wrist_roll_live;
        sysCtx.wrist_pitch = wrist_pitch_live;

        int mapped = map(sysCtx.wrist_pitch, 5, 175, 5, 120);
        int corrMin = elbow_center - 7; if (corrMin < 5) corrMin = 5;
        int corrMax = elbow_center + 7; if (corrMax > 120) corrMax = 120;
        sysCtx.elbow = constrain(mapped, corrMin, corrMax);
    }

    waist_out_prev = sysCtx.waist;
    shoulder_out_prev = sysCtx.shoulder;
    wrist_roll_out_prev = sysCtx.wrist_roll;
    wrist_pitch_out_prev = sysCtx.wrist_pitch;
}
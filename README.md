# Mechanical Arm Project

A remote-controlled **6-servo mechanical arm** operated by a motion-based controller.

The project consists of two main parts:

- **MechanicalArm** — STM32-based receiver controlling the servos,
- **Controller** — ESP32-based handheld controller with IMU and joystick.

The arm is controlled using hand movements detected by the **MPU6050** sensor. An analog joystick is used to change control modes and operate the gripper.

The project uses **BLE communication**, a **PCA9685 PWM servo driver**, and custom firmware for both the controller and the robotic arm.

---

## Demo

[Watch the demo video](https://www.youtube.com/shorts/1-4nBzwr-3U)

---

## Project Overview

The mechanical arm uses:

- 3 × **MG996R** servos,
- 3 × **SG90** servos,
- all servos with 180° movement range,
- 3D printed mechanical arm parts,
- **NUCLEO-L476RG** as the arm controller,
- **PCA9685** for PWM servo control,
- **HM-10 BLE module** for wireless communication,
- **ESP32-S3 WROOM N8R8** as the handheld controller,
- **MPU6050** for motion sensing,
- analog joystick for mode switching and gripper control.

The printed arm parts are based on commonly available 3D printable mechanical arm designs.

---


## Control Modes

The controller supports two main operating modes.

### Mode 1 — Main Arm Control

Used to control the lower part of the arm:

- waist / base rotation,
- shoulder,
- elbow.

In this mode, the **IMU controls the waist and shoulder axes**.  
The **elbow position is linked to the shoulder movement**, so the arm can move in a more natural and coordinated way.

### Mode 2 — Wrist and Gripper Control

Used to control the upper/end part of the arm:

- wrist pitch,
- wrist roll,
- elbow fine correction,
- gripper.

In this mode, the **IMU controls wrist pitch and wrist roll using two motion axes**.  
The IMU also provides a small correction to the **elbow position**.

The **analog joystick** is used to control the gripper in this mode.

---

## Implemented

- control of 6 servos using **PCA9685**,
- STM32 firmware for the mechanical arm,
- ESP32 firmware for the handheld controller,
- MPU6050 data reading,
- complementary filter for smoother IMU readings,
- basic kinematics and angle mapping,
- BLE communication between controller and arm.


---

## Circuit Diagram

![Circuit diagram](docs/schematic.jpg)

---

## Project Structure

```
MECHANICALARMPROJECT/
├── Controller/                 # ESP32 handheld controller
│   ├── include/
│   │   ├── config.h
│   │   ├── kinematics.h
│   │   └── systemContext.h
│   │
│   ├── lib/
│   │   ├── BLE/
│   │   │   ├── bleSender.cpp
│   │   │   └── bleSender.h
│   │   │
│   │   ├── IMU_Filters/
│   │   │   ├── compFilter.cpp
│   │   │   └── compFilter.h
│   │   │
│   │   └── mpu6050/
│   │       ├── mpu6050_driver.cpp
│   │       └── mpu6050_driver.h
│   │
│   ├── src/
│   │   ├── kinematics.cpp
│   │   └── main.cpp
│   │
│   └── platformio.ini
│
└── MechanicalArm/              # STM32 servo receiver
    ├── Core/
    │   ├── Inc/
    │   │   ├── helpers.h
    │   │   ├── main.h
    │   │   ├── PCA9685.h
    │   │   ├── stm32l4xx_hal_conf.h
    │   │   └── stm32l4xx_it.h
    │   │
    │   └── Src/
    │       ├── helpers.c
    │       ├── main.c
    │       ├── PCA9685.c
    │       ├── stm32l4xx_hal_msp.c
    │       ├── stm32l4xx_it.c
    │       ├── syscalls.c
    │       ├── sysmem.c
    │       └── system_stm32l4xx.c
    │
    ├── CMakeLists.txt
    └── MechanicalArm.ioc
  ```
  ---
  How to Run
  ---
  Clone the repository:
  ```
  git clone https://github.com/Marcin225/Mechanical-Arm
  cd MechanicalArmProject
  ```
  
  ### Controller

1.  Open the `Controller` folder in **VS Code + PlatformIO**.
2.  Build the project:

```
pio run
```

3.  Upload firmware to the ESP32-S3:

```
pio run --target upload
```

4.  Open the serial monitor:

```
pio device monitor
```

----------

### Mechanical Arm

1.  Open the `MechanicalArm` project in **STM32CubeIDE** or **VS Code with STM32 CMake tools**.
2.  Connect the NUCLEO-L476RG board.
3.  Build the firmware.
4.  Flash the firmware to the STM32 board.
5.  Power the servos from an external power supply.

> Do not power all servos directly from the STM32 board.
    
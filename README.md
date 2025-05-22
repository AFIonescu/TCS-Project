# TCS-Project

**Traction Control System for an Electric Vehicle**

**Author:** Alexandru-Florin Ionescu
**Date:** Feb 2021 – Feb 2022

---

## 🚀 Project Overview

The Traction Control System (TCS) monitors wheel speeds and automatically adjusts motor outputs to prevent wheel slip. When a speed differential is detected between the front and rear wheels, the system commands the motor controller to reduce power on the faster wheel, improving vehicle stability and safety.

---

## 🔧 Hardware Components

| Component                              | Purpose                                                 |
| -------------------------------------- | ------------------------------------------------------- |
| Arduino UNO (ATmega328P)               | Main control unit for rear-wheel logic                  |
| Arduino Nano (ATmega328P)              | Front-wheel control and CAN communication               |
| MMA8452 Accelerometer (I²C, 0x1C)      | Measures vehicle pitch/roll for dynamic control         |
| Wheel Speed Sensors (digital inputs)   | Counts pulses to determine wheel revolutions per second |
| L298N Dual H-Bridge Motor Driver       | Drives the DC motors based on control signals           |
| 12 V → 5 V DC/DC Converter             | Powers the microcontrollers and sensors                 |
| 1 MΩ Linear Potentiometer              | Acts as the accelerator pedal input                     |
| CAN Transceiver Module                 | Sends speed & sensor data between UNOs via CAN bus      |
| Misc. wiring, breadboard, power supply | Prototyping and power distribution                      |

---

## 🖥️ Software & Libraries

* **IDE:** Arduino IDE
* **Language:** C++ (Arduino framework)
* **Libraries:**

  * `CAN.h` — MCP CAN bus communication
  * `Wire.h` — I²C communication for MMA8452 accelerometer

---

## ⚙️ Features

* **Wheel Speed Monitoring**
  Reads pulses from two hall-effect sensors to compute revolutions per second (RPS).
* **CAN Bus Messaging**
  Transmits RPS and accelerometer readings between front/rear controllers.
* **Traction Adjustment Logic**
  Compares front vs. rear wheel RPS; reduces PWM duty on the faster wheel by a fixed offset when difference > 1 RPS.
* **Vehicle Velocity Estimation**
  Calculates vehicle linear speed from wheel RPS and radius.
* **Dynamic Feedback**
  Uses MMA8452 accelerometer data to refine control under varying conditions.
* **Serial Telemetry**
  Prints real-time sensor values and control actions for debugging.

---

## 📦 Setup & Wiring

1. **Upload “Master” sketch** to the Arduino UNO (rear controller):

   * Connect accelerometer to I²C (SDA/SCL) pins.
   * Wire speed sensor 1 to digital pin 7; speed sensor 2 to pin 8.
   * Connect CAN transceiver to SPI pins (MOSI/MISO/SCK) and CS.
2. **Upload “Slave” sketch** to the Arduino Nano (front controller):

   * Wire PWM outputs to L298N driver inputs (pins 3,9 for PWM; 4,5 & 6,7 for direction).
   * Connect CAN transceiver similarly.
3. **Power** both boards via the 12 V → 5 V converter.
4. **Open Serial Monitor** at 9600 baud to view telemetry.

---

## 🏃 Usage

Turn the potentiometer (accelerator) to drive the motors. Observe the Serial Monitor; you should see output like:

```
Potentiometer: 75%
rear_modifier: -25
front_modifier: 0
rear_rps: 12.3
front_rps: 10.8
vehicle_speed: 2.44 m/s
X       Y       Z
0.12    -0.05   0.98
```

When one wheel spins faster, its PWM duty is automatically reduced by the modifier value.

---

## 🗂️ Repository Structure

```
TCS-Project/
├─ LICENSE
├─ README.md
├─ MasterController/          # Arduino UNO code (rear wheel + CAN recv)
│  └─ master_controller.ino
└─ FrontController/           # Arduino Nano code (front wheel + CAN send)
   └─ front_controller.ino
```

---

## 📜 License

This project is released under the **MIT License**. See [LICENSE.txt](./LICENSE.txt) for details.

---

## 🎓 Acknowledgments

Developed as part of the **Electronic Systems Lab** at University POLITEHNICA of Bucharest.

# Embedded Blood Pressure Monitor

A real-time, non-invasive blood pressure monitoring system utilizing an embedded microcontroller and a custom-designed analog front-end for high-precision sensor data acquisition.

## 🛠️ Hardware & System Design
- **Microcontroller:** Arduino Nano (ATmega328P) with 16x2 LCD UI.
- **Pressure Sensor:** MPS20N0040D (0-40kPa).
- **Custom Analog Front-End (Signal Conditioning):** - High-precision instrumentation amplification using **AD620**.
  - Multi-stage active filtering using **LM741** Op-Amps.
- **Power Management:** Dual power supply design using **LM7805** (+5V) and **ICL7660S** (-5V for Op-Amp symmetrical rails).
- **Power Drivers:** Transistor-based driving circuits (**TIP41C**, **2N2222**) with flyback protection for actuators.
- **Actuators:** Mini air pump, solenoid release valve, and speaker.

## 📂 Repository Structure
- `/hardware`: Contains schematic snippets demonstrating the custom signal conditioning and power distribution networks.
- `/firmware`: Embedded C/C++ source code for sensor calibration, UI control, and actuator management.

## 🚀 Key Signal Routing
- **MPS Sensor Output** -> AD620 Instrumentation Amp -> LM741 Active Filters -> **Arduino ADC0**
- **Motor/Valve Control** -> Arduino D2, D3 -> TIP41C Driver Network

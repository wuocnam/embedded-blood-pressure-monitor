<img width="1080" height="1920" alt="z7873015730073_17597a3d94fae7621c3b929b31e877bd" src="https://github.com/user-attachments/assets/785e6e4c-9bd3-4356-b463-56de8dffb591" /># Embedded Blood Pressure Monitor

A real-time, non-invasive blood pressure monitoring system utilizing an embedded microcontroller and a custom-designed analog front-end for high-precision sensor data acquisition.

## 📸 Product Showcase
<img width="1706" height="2560" alt="z7873014103155_68d4da9852d3bea1b45f12b7498baded" src="https://github.com/user-attachments/assets/eba61e89-448e-466b-ae82-f454ee49021a" />
<img width="2090" height="2065" alt="z7873015730836_ad06e7b14a9b0673478aa2cc813ee6c5" src="https://github.com/user-attachments/assets/3d077eb9-b64d-4088-9ef2-ac18e7a849df" />

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

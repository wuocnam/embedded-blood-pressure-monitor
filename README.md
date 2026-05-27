# Embedded Blood Pressure Monitor

A real-time, non-invasive blood pressure monitoring system utilizing an embedded microcontroller and specialized pressure sensors.

## 🛠️ Hardware Components
- **Microcontroller:** Arduino Nano (ATmega328P)
- **Pressure Sensor:** MPS20N0040D (0-40kPa)
- **Signal Conditioning:** Internal ADC with Op-Amp amplification
- **Actuators:** Mini air pump (3V/6V) and solenoid release valve

## 📂 Repository Structure
- `/hardware`: Schematic design and PCB layout files (Altium Designer).
- `/firmware`: Embedded C/C++ source code for sensor calibration and data processing.

## 🚀 Pin Configuration (MPS20N0040D to Arduino Nano)
- **VCC** -> 5V
- **GND** -> GND
- **OUT+ / OUT-** -> Connected via amplifier module to Analog Pins (A0/A1)

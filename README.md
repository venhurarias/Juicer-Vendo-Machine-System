# Juice Vendo Machine

A standalone embedded vending machine project designed to automatically prepare and dispense juice. The system supports juice mixing and cooling, similar to an ordinary juicer, but built as an automated vendo machine.

This project focuses on hardware automation and does not include a mobile app or web dashboard.

---

## 🚀 Features

- Automatic juice vending operation
- Juice mixing process
- Cooling process for cold juice
- Motor or actuator control
- Timed dispensing cycle
- Standalone machine operation
- Simple embedded control logic
- Can be expanded with coin slot, keypad, LCD, or sensors

---

## 🧰 Hardware Requirements

- ESP32 / Arduino-compatible controller
- Juice mixing motor
- Cooling system or cooling module
- Solenoid valve or dispensing pump
- Relay module or motor driver
- Power supply
- Container or juice tank
- Tubing / dispenser nozzle
- Optional coin slot or payment module
- Optional LCD display
- Optional push button or keypad

---

## ⚙️ System Overview

The Juice Vendo Machine automates the preparation and dispensing of juice. Once triggered, the system can start the mixing process, activate the cooling mechanism, and dispense the drink through a pump or valve.

The cooling and mixing process may not be fully included in the firmware if those parts are handled mechanically or through separate hardware, but they are part of the complete vending machine design.

---

## 🔁 Basic Workflow

1. Machine powers on
2. User starts the vending process
3. Juice mixing begins
4. Cooling system keeps the juice cold
5. Pump or valve dispenses juice
6. System stops after the configured amount/time
7. Machine waits for the next transaction

---

## 🧠 Control Logic

- Mixing motor prepares the juice
- Cooling system maintains cold temperature
- Pump or solenoid valve controls dispensing
- Timing controls the amount of juice dispensed
- Optional payment input can trigger the process

---

## 🔌 Possible Components

| Component | Purpose |
|----------|---------|
| Controller | Main system control |
| Mixing Motor | Mixes the juice |
| Cooling Module | Keeps juice cold |
| Pump / Solenoid Valve | Dispenses juice |
| Relay Module | Controls high-power devices |
| Coin Slot / Button | Starts vending cycle |
| LCD Display | Shows machine status |
| Power Supply | Powers the system |

---

## 🛠 Setup Instructions

1. Connect the controller to the relay or motor driver modules
2. Connect the mixing motor
3. Connect the cooling system
4. Connect the pump or solenoid valve
5. Upload the firmware to the controller
6. Power the machine
7. Test dispensing, mixing, and cooling operation

---

## 🔐 Safety Notes

- Do not connect motors, pumps, or cooling devices directly to the controller
- Use proper relays or motor drivers
- Use a separate power supply when needed
- Ensure wiring is insulated and protected from liquid spills
- Keep electronics away from juice or water
- Use food-safe tubes and containers

---

## 📌 Notes

- This is a separate juice vending machine project
- Mixing and cooling may be handled by hardware even if not directly included in the code
- The system can be upgraded with payment, billing, display, or IoT monitoring features



Juice Vendo Machine Project

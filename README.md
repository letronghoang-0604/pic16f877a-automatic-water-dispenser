# pic16f877a-automatic-water-dispenser
Automatic water volume dispenser using PIC16F877A, flow sensor and IR cup detection (CCS C)

# Automatic Water Volume Dispenser using PIC16F877A (CCS C)

This is a small embedded project using a **PIC16F877A** microcontroller and **CCS C** compiler to automatically dispense a specified volume of water in milliliters.  
The system measures flow using a **YF-S402 flow sensor**, detects cup presence with an **IR sensor**, displays information on a **16x2 LCD**, and controls a pump/valve through a **relay**. Push buttons are used to set the desired volume, and a **buzzer** plus **LEDs** provide user feedback.

## Features

- Set the desired water volume in **milliliters (ml)** using **UP** and **DOWN** buttons (e.g. step of 10 ml).
- Detect cup presence using an **IR sensor** so the system only starts when a cup is in place.
- Measure dispensed volume using a **YF-S402 flow sensor** connected to an external interrupt pin.
- Display the **target volume** and **current dispensed volume** on a **16x2 character LCD**.
- Control a pump or valve using a **relay**:
  - Pump OFF when there is no cup or the target volume is reached.
  - Pump ON while dispensing until the requested volume is delivered.
- Use **LEDs** and a **buzzer** to indicate system status:
  - Beep at start/end of dispensing.
  - Different LED colors for idle/dispensing state.

## Hardware

- Microcontroller: **PIC16F877A** (20 MHz external crystal)
- Flow sensor: **YF-S402** (or similar Hall-effect water flow sensor)
- IR cup detection sensor (IR LED + photodiode module)
- 16x2 character LCD (4-bit mode)
- Relay module to drive the water pump or solenoid valve
- Buzzer for audible feedback
- Status LEDs (e.g. RED and GREEN)
- Push buttons for **UP** and **DOWN** volume adjustment
- Power supply (5 V for logic, separate supply for pump if needed)
- Miscellaneous: resistors, wires, breadboard or PCB

## Software & Tools

- **Compiler**: CCS C for PIC (`#include <16F877A.h>`)  
- Clock: 20 MHz (`#use delay(clock=20000000)`)
- LCD library: `lcd.c` (standard CCS LCD driver)
- IDE: CCS IDE or MPLAB X with CCS plug-in (or any environment that supports CCS C)
- Programmer: PICkit or compatible programmer for PIC16F877A
- Optional: Proteus for circuit simulation and testing

## Code Overview

Main source file: `water_dispenser.c`

Key points:
- The **flow sensor** output is connected to the external interrupt pin (`RB0/INT`); each pulse from the sensor increments a global pulse counter.
- A conversion function translates the number of pulses into **milliliters** using a calibration factor (ml per pulse).
- An **IR sensor** input is used to detect whether a cup is present under the outlet; if no cup is detected, the system does not start dispensing.
- **UP** and **DOWN** buttons adjust the target volume variable (e.g. `set_ml`) in steps (e.g. 10 ml), with simple debounce delays.
- The **LCD** shows:
  - Target volume (SET) on one line.
  - Current dispensed volume or status messages on the other line.
- The **relay** control pin turns the pump/valve ON while dispensing and OFF when:
  - The desired volume is reached, or
  - The cup is removed.
- The **buzzer** and **LEDs** give feedback at the start and end of dispensing or when there is an error/no cup.

## How It Works (High-Level)

1. **Initialize:**
   - Configure I/O directions for buttons, IR sensor, LEDs, relay and buzzer.
   - Initialize the LCD in 4-bit mode.
   - Configure external interrupt on `RB0` for the flow sensor pulses.
   - Initialize global variables for pulse count, target volume and current volume.

2. **In the main loop:**
   - Read the **UP** and **DOWN** buttons to increase or decrease the target volume (in ml) within allowed limits.
   - Continuously check the **IR sensor**:
     - If no cup is detected, show a “no cup” / “place cup” message, keep the relay OFF and the system in idle state.
     - If a cup is detected and the system is not currently dispensing, prepare to start a new dispensing cycle.
   - When a valid target volume is set and a cup is present:
     - Reset the pulse counter and current volume.
     - Turn ON the relay to start the pump/valve.
     - Optionally beep the buzzer and change LED state to “dispensing”.
   - While dispensing:
     - Update the current volume by converting the pulse count from the flow sensor into ml.
     - Refresh the LCD display with the target and current volume.
     - If the cup is removed, immediately stop dispensing for safety.
     - If the current volume reaches or exceeds the target volume, turn OFF the relay, beep the buzzer, set a “done” status and wait for the next cycle.

# pic16f877a-automatic-water-dispenser
Automatic water volume dispenser using PIC16F877A, flow sensor and IR cup detection (CCS C)

# Automatic Water Volume Dispenser using PIC16F877A (CCS C)

This project implements an automatic water volume dispenser using a **PIC16F877A** microcontroller and **CCS C** compiler.  
The system uses a **flow sensor** (YF-S402), an **IR sensor** to detect the cup, push buttons to set the target volume in milliliters, a **relay** to control the pump/valve, a **buzzer** for feedback and **LEDs** plus an **LCD** for user interface. :contentReference[oaicite:1]{index=1}

---

## Features

- Set the desired water volume in **milliliters (ml)** using **UP** and **DOWN** buttons (step of 10 ml).
- Detect cup presence using an **IR sensor**:
  - If no cup is present, the system shows a message and does not start dispensing.
- Measure the dispensed volume using a **YF-S402** flow sensor:
  - Flow pulses are counted via external interrupt on `RB0/INT`.
  - Pulses are converted to ml using a calibration factor.
- Control a **relay** to turn the pump/valve ON/OFF.
- Provide visual and sound feedback:
  - **RED / GREEN LEDs** indicate status.
  - **Buzzer** beeps at start and end of dispensing.
- Information display on a **16x2 character LCD**:
  - Target volume (SET)
  - Current dispensed volume (ml)
  - Status messages (cup present / no cup / done).

---

## Hardware

- Microcontroller: **PIC16F877A** (20 MHz crystal)
- Flow sensor: **YF-S402** (or similar Hall-effect flow sensor)
- IR sensor module for cup detection
- 16x2 character LCD (in 4-bit mode)
- Relay module to drive pump or solenoid valve
- Buzzer
- LEDs: RED and GREEN
- Push buttons: `UP`, `DOWN`
- Power supply

Pin mapping (as used in the code):

- **LCD**: control pins on PORTB/PORTD (see `lcd.c` and defines in `water_dispenser.c`)
- **IR sensor**: `PIN_A0`
- **UP button**: `PIN_A1`
- **DOWN button**: `PIN_A2`
- **FLOW sensor (YF-S402)**: `PIN_B0` (external interrupt `INT_EXT`)
- **RELAY**: `PIN_C3`
- **BUZZER**: `PIN_C0`
- **LED_RED**: `PIN_D1`
- **LED_GR**: `PIN_D0`

---

## Software & Tools

- **Compiler**: CCS C for PIC (`#include <16F877A.h>`)
- **Clock**: 20 MHz (`#use delay(clock=20000000)`)
- **LCD driver**: `lcd.c` (standard CCS LCD library)
- **IDE**: CCS IDE or MPLAB X with CCS plugin
- **Programmer**: PICkit or compatible PIC programmer

---

## How It Works

### 1. Pulse counting and volume calculation

- The flow sensor (YF-S402) generates pulses proportional to the flow rate.
- Each pulse triggers the external interrupt `INT_EXT` on `RB0`, and the interrupt service routine increments `pulse_count`.
- A helper function converts pulses to ml:
float pulses_to_ml(unsigned int16 pulses) {
   return (float)pulses / 4.5;   // calibration factor (adjust as needed)
}
The current dispensed volume is computed as: current_ml = (unsigned int16)pulses_to_ml(pulse_count);

### 2. User interface

set_ml holds the target volume (default 100 ml, adjustable from 10 to 9990 ml).
UP / DOWN buttons increase or decrease set_ml by 10 ml with a simple debounce delay.
The LCD displays:
  - Line 1: SET: xxxx ml
  - Line 2: current volume or status messages.
  
### 3. Dispensing logic

Continuously read buttons and update set_ml.
Check IR sensor:

If no cup is present:
  - Show "NO CUP" message (in the original code, Vietnamese text is used).
  - Relay is OFF, LEDs indicate safe/idle state.

If a cup is detected and the system is not in done state:
  - Turn on RED LED, turn off GREEN LED.
  - Beep the buzzer briefly to indicate start.
  - Reset pulse_count and current_ml.
  - Turn ON the relay to start water flow.

While the cup is present and current_ml < set_ml:
  - Continuously update current_ml from pulse_count.
  - Update the LCD with the current volume.
  - If the cup is removed mid-process, stop immediately.

When current_ml >= set_ml or the cup is removed:
  - Turn OFF the relay.
  -Beep the buzzer to indicate completion.
  -Set done = 1 until the next cycle.

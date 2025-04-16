# The-Fidget-Reactor

Porting of The-Reactor to hardware.

---

## 🎛️ What is The Fidget Reactor?

**The Fidget Reactor** is a tactile control panel simulation project designed to emulate a richly interactive industrial interface. The system mimics a cryptic, playful control environment of lights, buttons, meters, gauges, and switches — all controlled by state machines simulating plausible physical behavior.

---

## 🧠 Project Purpose

The goal is to **fully simulate hardware behavior in software first**, using modular state logic and bus messaging to build a testbed for embedded control panel development. This architecture allows easy transition from simulated to real-world components.

Key goals include:

- C++ state machine-driven **reactor control logic**
- Event-based **message bus system** between subsystems
- Virtual React UI mirroring real hardware layout
- Seamless migration path to ESP32 + ATtiny microcontrollers

---

## 🧪 Current Phase: Simulation with Real Control Logic

✅ **Power Cycle State Machine is Working**  
Simulates transitions: `OFF → INIT → STARTUP → ON → SHUTDOWN → OFF`

✅ **Controller tick engine** drives per-frame logic  
✅ **MessageBus** routes all input and internal messages  
✅ **Compiled via `make` using g++`**  
🧪 Subsystem logic (SCRAM, core/cooling/gen) in development  
🔜 Peripheral communication (ATtiny/I2C/SPI) upcoming

---

## 🔩 Planned Hardware Targets

- **ESP32 main controller** (WiFi, OTA, I2C/SPI master)
- **ATtiny-based peripheral boards** (buttons, LEDs, knobs)
- **Multiplexed IO layers** modeled directly from the simulation layout

All hardware design is informed by simulation output.

---

## 🔧 Tech Stack

- **C++17** core logic (portable to embedded targets)
- **Custom message bus architecture**
- **React** frontend (mirrors control layout)
- **Makefile** build system

---

## 🚀 Getting Started

To build the C++ backend:

```bash
cd simulation/
make
./controller_runner
```

You’ll see the simulation tick through power states based on virtual button input.

---

## 🗺️ Status

| Feature             | Status     |
|---------------------|------------|
| C++ controller core | ✅ Working |
| Tick engine         | ✅ Working |
| Power state logic   | ✅ Working |
| Simulation UI       | ✅ Functional |
| Hardware mapping    | 🧪 In progress |
| Peripheral I/O      | 📝 Pending |
| BOM + schematic     | 📅 Planned |

---

_This README will expand as new subsystems, hardware targets, and firmware layers are added._

---

# Next Steps — Fidget Reactor Controller

> Last session success: The C++ tick engine is compiling and running successfully!  
> Let’s build on that with messaging and subsystems.

---

## ✅ Environment Confirmed
- g++ working (v14.2.0)
- VS Code ready
- Tick loop outputs and runs independently

---

## 🔧 Immediate Next Tasks

### 1. `MessageBus.h / MessageBus.cpp`
- Create a simple pub/sub or `emit()` interface
- Accept hardcoded messages like:
  - `ScramEvent`
  - `FuelRodPressedEvent { int x, int y }`
- Start with `emit()` logging to stdout

### 2. Create `subsystems/`
- Folder: `simulation/subsystems/`
- File: `coreSystem.h / coreSystem.cpp`
- Implement:
  - `initialize()`
  - `onTick()`
  - `handleMessage(const Message&)`
- Log internal state (even if dummy for now)

### 3. Connect to Tick Loop
In `tickEngine.cpp`:
- Call `coreSystem::onTick()` each cycle
- Simulate a `FuelRodPressedEvent` every few ticks for testing

---

## 🧠 Stretch Goal (Optional Next Session)
- Accept basic input via `std::getline()` in `controller_runner`
- Parse and emit real messages
  - `press 3 4`
  - `scram`

---

## 📁 Suggested Structure

```
simulation/
├── controller_runner.cpp
├── tickEngine.cpp/h
├── MessageBus.cpp/h
└── subsystems/
    └── coreSystem.cpp/h
```


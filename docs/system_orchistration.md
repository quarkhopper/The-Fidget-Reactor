# System Orchestration - Fidget Reactor

## 🧠 Central MCU (ESP32)
The ESP32 acts as the single master controller for the entire system. It runs all logic and orchestrates system behavior.

### Responsibilities:
- Executes the full state machine
- Hosts all subsystems in shared memory:
  - CoreSystem
  - CtrlSystem
  - GenSystem
  - CoolSystem (etc.)
- Polls and interprets protobuf messages from peripheral controllers
- Sends updates to the debug UI (in simulation only)

## 🧸 Peripheral Controllers (ATtiny)
These microcontrollers are used for interfacing physical components like buttons, sliders, and switches.

### Responsibilities:
- Monitor physical pin states (digital/analog inputs)
- Translate changes into serialized protobuf messages
- Send messages to ESP32 via simple serial protocol (e.g., UART)
- No internal logic or system awareness
- In simulation: runs as its own process

## 🧪 Debug UI (reactor_ui.exe)
A temporary, software-only interface that serves as both the simulated front panel and a surrogate boot ROM for launching the simulation environment.

### Responsibilities:
- Display state feedback from ESP32 during simulation
- Simulate button presses and gauge input for testing
- Sends **raw high/low pin signals** to the ATtiny sim via pipe (not protobuf)
- Communicates with ATtiny simulator only
- Is implemented as `reactor_ui.exe`
- **Responsible for launching and monitoring the ESP32 and ATtiny simulation processes**
- **Reads a configuration file that defines which simulation processes to launch and their parameters**
- Has no hardcoded knowledge of components or system layout—this is defined in static config files and loaded dynamically
- Will be replaced entirely with physical hardware in production

## 🔀 Bus Router (Optional for Simulation)
A fourth process may be required to route and multiplex messages between components via named pipes.
This is especially useful if more than one process needs access to a shared bus or communication line.

## 💬 Communication Overview

| From          | To           | Method | Format    | Purpose                        |
|---------------|--------------|--------|-----------|---------------------------------|
| Debug UI      | ATtiny Sim   | Pipe   | Raw (hi/lo)| Simulated pin signals           |
| ATtiny Sim    | ESP32 Sim    | Pipe   | Protobuf  | Translated pin messages         |
| ESP32 Sim     | Debug UI     | Pipe   | Protobuf  | System state monitoring         |

---

## Notes
- No subsystems should be spun out into separate processes within the ESP32 sim.
- `BusClient` abstraction should be retained **only** for routing external messages (UI + ATtiny), not internal subsystem communication.
- All simulation code must stay cleanly separated from production logic to avoid creep.
- Debug UI will eventually be replaced by real gauges, buttons, indicator lights, etc.
- Only the ATtiny peripheral controllers are external in hardware.
- The simulation consists of **three key processes**:
  1. ESP32 simulation (core logic and state machine)
  2. ATtiny simulation (raw pin input to protobuf)
  3. Debug UI (reactor_ui.exe): raw pin signal sender and process launcher/monitor
- A fourth process (bus router) may be introduced to route pipes cleanly between the three.
- The existing `config/` directory reflects this architecture.

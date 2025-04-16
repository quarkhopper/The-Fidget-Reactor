# Next Steps — Fidget Reactor Process Managers

### ✅ Completed
- All subsystems scaffolded and registered in `ControllerManager`
- `tickEngine` runs and logs tick events for each subsystem
- Process managers created:
  - `InitManager`
  - `StartupManager`
  - `TestManager`
  - `ShutdownManager`
- `ControllerManager` wires up `InitManager` and kicks off `begin()` with `on_complete` and `on_fault` logic

---

### ⏭️ Next Steps

#### 1. Implement `init_manager.cpp`
- Add `begin()` implementation:
  - Emit `Message` with `.type = PROCESS_BEGIN`, `.strValue = "init"`
  - Set `pending_acks` to expected count (e.g., number of peripherals)
- Implement `handle_message()`:
  - Count `ACK_INIT` or similar messages
  - Call `on_complete()` or `on_fault()` as appropriate

#### 2. Hook up `handle_message()` routing
- Either:
  - Let each manager register with the `MessageBus` (if passive dispatching is desired), or
  - Route all `MessageBus.getInbound()` messages manually in `ControllerManager::tick()` to relevant `*_manager.handle_message()`

#### 3. Repeat same structure for:
- `StartupManager`
- `TestManager`
- `ShutdownManager`

Each one should emit its own `PROCESS_BEGIN`, track replies, and trigger callbacks.

---

### 🧠 Optional Enhancements
- Simulate peripherals sending replies to complete each manager’s task
- Add mock failure cases to test `on_fault()` paths
- Extend `ControllerState` or state machine to reflect phase transitions triggered by managers

---

This setup keeps internal coordination in-memory while routing external peripheral logic through the `MessageBus`, matching the architectural goal.

# Checkpoint: VS Code Integration and Next Steps

## ✅ Current State (as of April 18, 2025)

This document is a technical checkpoint written for ChatGPT (Chichi) and for future reference. The goal is to clearly articulate where the project stands and what needs to happen next.

### 🧱 Build Environment
- The project is now fully integrated with **Visual Studio Code**.
- CMake is working end-to-end using the **Visual Studio 2019 Build Tools (x64)**.
- The vcpkg toolchain is successfully hooked into VS Code via `.vscode/settings.json`.
- CMake uses `C:/vcpkg/scripts/buildsystems/vcpkg.cmake` for library resolution.
- Protobuf code generation (`reactor.pb.h/.cc`) is now synced and version-matched with vcpkg's `libprotoc 29.3`.
- Manual build via `cmake --build .` and VS Code GUI build both succeed.
- The executable `reactor_ui.exe` is successfully generated in `build/Debug/`.

### 📦 Protobuf
- `reactor.proto` lives in `proto/`
- The generated code lives in `generated/`
- We have verified that the generated files are built cleanly against the expected `libprotobuf`
- **BUT:** There is currently no working messaging bus that uses protobuf for communication

---

## 🔧 Next Steps (in priority order)

### 1. **Implement a Protobuf-based Message Bus**
- The current message bus logic is not yet wired up to use Protobuf messages
- We need to define the schema of all messages in `reactor.proto`
- The controller and the thin UI will need to agree on message framing, serialization, and transport format

### 2. **Split Application Into Two Processes**
- Right now, everything is monolithic
- We want to isolate the debug UI into a **thin client**
- Everything else (controller logic, state machine, simulation) should run in a **separate process**
- Communication between processes should happen over **pipes** (or another IPC mechanism, TBD)

### 3. **Wire Pipes or IPC Layer**
- Evaluate named pipes (Windows), sockets, or stdio redirection
- Ensure that communication is cross-platform friendly if possible (but Windows is the main concern for now)
- All UI interaction should happen through serialized messages passed over this layer

### 4. **Thin UI Refactor**
- The UI should:
  - Only render the current state
  - Send input events (button presses, slider changes) to the backend
  - Subscribe to state updates (light changes, gauge updates, etc.)
- No logic should run inside the UI; it should be reactive only

### 5. **Process Launcher for Dev Workflow**
- Eventually, consider a small helper that launches both the controller and the UI process for easier debugging
- Could be a Python script or a C++ supervisor

---

## 🧠 Notes to Chichi
- Assume you are helping implement each layer — design with clarity and traceability in mind
- Keep the protobuf schema minimal to start (e.g. basic message types like `ButtonPress`, `StateUpdate`)
- Prioritize making the message bus and UI process boundary feel clean and testable
- Once this pipeline works, everything else will follow much more easily


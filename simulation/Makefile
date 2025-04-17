# === CONFIG ===
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra \
  -IC:/msys64/mingw64/include/SDL2 \
  -LC:/msys64/mingw64/lib
TARGET = reactor_ui
SRC = main_window.cpp power_button.cpp pin_sim.cpp debug_console.cpp process_managers/init_manager.cpp
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

# === BUILD RULES ===
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	del /f /q $(TARGET).exe 2>nul || exit 0

.PHONY: all clean
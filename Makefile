CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra \
  -IC:/msys64/mingw64/include/SDL2 \
  -LC:/msys64/mingw64/lib \
  -Igenerated  # ⬅️ Add Protobuf headers

SRC = main_window.cpp power_button.cpp pin_sim.cpp debug_console.cpp controller_manager.cpp \
  process_managers/init_manager.cpp config/simulator_config.cpp peripheral_controller.cpp \
  generated/reactor.pb.cc  # ⬅️ Add Protobuf source

TARGET = reactor_ui
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lprotobuf  # ⬅️ Add protobuf to link

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f reactor_ui.exe

.PHONY: all clean

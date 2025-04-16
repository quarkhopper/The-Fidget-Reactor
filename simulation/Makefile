# === CONFIG ===
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = controller_runner
SRC = controller_runner.cpp

# === BUILD RULES ===
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	del /f /q $(TARGET).exe 2>nul || exit 0

.PHONY: all clean

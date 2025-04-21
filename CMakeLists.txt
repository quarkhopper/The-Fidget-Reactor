# Updated to only build the debug_ui target
cmake_minimum_required(VERSION 3.20)
project(FidgetReactor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# --- Debug UI Target ---
add_executable(debug_ui
    ui/debug_console.cpp
    ui/main_window.cpp
    ui/power_button.cpp
    bus/pin_sim.cpp
)

# --- Link Libraries ---
target_link_libraries(debug_ui
    PRIVATE
        protobuf::libprotobuf
        nlohmann_json::nlohmann_json
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)

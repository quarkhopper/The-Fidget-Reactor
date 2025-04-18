cmake_minimum_required(VERSION 3.20)
project(FidgetReactor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find dependencies ---
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# --- List your source files ---
set(SOURCES
    main_window.cpp
    power_button.cpp
    pin_sim.cpp
    debug_console.cpp
    controller_manager.cpp
    init_manager.cpp
    simulator_config.cpp
    peripheral_controller.cpp
    generated/reactor.pb.cc
)

# --- Create the executable ---
add_executable(reactor_ui ${SOURCES})

# --- Include protobuf headers ---
target_include_directories(reactor_ui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)

# --- Link all required libraries ---
target_link_libraries(reactor_ui
    PRIVATE
        protobuf::libprotobuf
        nlohmann_json::nlohmann_json
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)

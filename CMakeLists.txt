cmake_minimum_required(VERSION 3.20)
project(FidgetReactor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# --- Common includes ---
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)

# --- Shared sources ---
# --- Simplify reactor_ui sources ---
# Updated to reflect the merged `PipeBusClient` class
# Add Protobuf generation for reactor.proto
set(PROTO_SRC proto/reactor.proto)
set(GENERATED_SRC generated/reactor.pb.cc)
set(GENERATED_HDR generated/reactor.pb.h)

find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC})

# Ensure generated files are included in the build
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(generate_proto ALL DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})

# Add generated files to the reactor_ui target
set(REACTOR_UI_SOURCES
    ui/main_window.cpp
    ui/power_button.cpp
    ui/debug_console.cpp
    bus/pin_sim.cpp
    ${PROTO_SRCS}
    bus/pipe_bus_client.cpp
)

# --- Executables ---
# --- Update reactor_ui executable ---
add_executable(debug_ui ${REACTOR_UI_SOURCES})
add_executable(main_controller
    main_controller/main_controller.cpp
    bus/pipe_bus_client.cpp
)
# --- Remove unused files from phc ---
add_executable(phc
    peripheral_controllers/phc.cpp
    interfaces/base_controller.cpp
    config/config_helper.cpp
)

# --- Link libraries ---
target_link_libraries(debug_ui
    PRIVATE
        protobuf::libprotobuf
        nlohmann_json::nlohmann_json
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)

target_link_libraries(phc
    PRIVATE
        nlohmann_json::nlohmann_json
        SDL2::SDL2
        SDL2::SDL2main
)

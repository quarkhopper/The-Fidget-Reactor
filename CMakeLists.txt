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
set(REACTOR_UI_SOURCES
    main_window.cpp
    power_button.cpp
    pin_sim.cpp
    controller_manager.cpp
    init_manager.cpp
    generated/reactor.pb.cc
    pipe_bus_client.cpp
)

# --- Executables ---
# --- Update reactor_ui executable ---
add_executable(reactor_ui ${REACTOR_UI_SOURCES})
add_executable(bus_router bus_router.cpp)
add_executable(controller_runner
    controller_runner.cpp
    pipe_bus_client.cpp
    init_manager.cpp
)
# --- Remove unused files from phc ---
add_executable(phc
    phc.cpp
    base_controller.cpp
    config_helper.cpp
)

# --- Link libraries ---
target_link_libraries(reactor_ui
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

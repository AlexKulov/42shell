# Minimum version CMake
cmake_minimum_required(VERSION 3.10)

# Pro name & set std language
project(42twin LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
# Path for exec file generation
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../42)

# Include path for headers
include_directories(
    ../42/Include
    ../42change/Include
    ../42/Kit/Include
)

# Src file list
set(SOURCES
    ../42/Source/42main.c
    ../42/Kit/Source/dcmkit.c
    ../42/Kit/Source/envkit.c
    ../42/Kit/Source/fswkit.c
    ../42/Kit/Source/geomkit.c
    ../42/Kit/Source/iokit.c
    ../42/Kit/Source/mathkit.c
    ../42/Kit/Source/msis86kit.c
    ../42/Kit/Source/nrlmsise00kit.c
    ../42/Kit/Source/orbkit.c
    ../42/Kit/Source/sigkit.c
    ../42/Kit/Source/sphkit.c
    ../42/Kit/Source/timekit.c
    ../42/Source/42actuators.c
    ../42/Source/42environs.c
    ../42/Source/42ephem.c
    ../42/Source/42ipc.c
    ../42/Source/42perturb.c
    ../42/Source/42sensors.c
    ../42/Source/42jitter.c
    ../42/Source/42joints.c
    ../42/Source/42optics.c
    ../42/Source/AcApp.c
    ../42/Source/42dynamics.c
    ../42/Source/42fsw.c
    ../42/Source/42cmd.c
    ../42change/Source/42init.c
    ../42change/Source/shIokit.c
    ../42change/Source/42exec.c
    ../42change/Source/shFsw.c
    ../cAlgorithms/serviceAlg.c
    ../cAlgorithms/simplestMode.c
#IPC_OVERHAUL - 42 version after 27.03.25
    ../42/Source/AutoCode/TxRxIPC.c
    ../42/Source/AutoCode/WriteAcToCsv.c
    ../42/Source/AutoCode/WriteScToCsv.c
)

# Exec file create
add_executable(${PROJECT_NAME} ${SOURCES})

# Setting for Windows
if(WIN32)
    target_link_libraries(42twin PRIVATE ws2_32)
endif()

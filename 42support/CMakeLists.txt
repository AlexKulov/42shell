# Minimum version CMake
cmake_minimum_required(VERSION 3.10)

# Pro name & set std language
project(42twin LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
# Path for exec file generation
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../42core)

# Include path for headers
include_directories(
    ../42core/Include
    ../42core/Kit/Include
    ../42dev
)

# Src file list
set(SOURCES
    ../42core/Source/42main.c
    ../42core/Kit/Source/dcmkit.c
    ../42core/Kit/Source/envkit.c
    ../42core/Kit/Source/fswkit.c
    ../42core/Kit/Source/meshkit.c
    ../42core/Kit/Source/iokit.c
    ../42core/Kit/Source/mathkit.c
    ../42core/Kit/Source/msis86kit.c
    ../42core/Kit/Source/nrlmsise00kit.c
    ../42core/Kit/Source/orbkit.c
    ../42core/Kit/Source/sigkit.c
    ../42core/Kit/Source/sphkit.c
    ../42core/Kit/Source/timekit.c
    ../42core/Source/42actuators.c
    ../42core/Source/42environs.c
    ../42core/Source/42ephem.c
    ../42core/Source/42ipc.c
    ../42core/Source/42perturb.c
    ../42core/Source/42sensors.c
    ../42core/Source/42jitter.c
    ../42core/Source/42joints.c
    ../42core/Source/42optics.c
    ../42core/Source/AcApp.c
    ../42core/Source/42dynamics.c
    ../42core/Source/42fsw.c
    ../42core/Source/42cmd.c
    ../42core/Source/AutoCode/TxRxIPC.c
    ../42core/Source/AutoCode/WriteAcToCsv.c
    ../42core/Source/AutoCode/WriteScToCsv.c
    ../42dev/PSModel.c
    ../42dev/SPSModel.c
    ../42dev/fswAlg.c
    ../42dev/fswMode.c
    ../42dev/orbControl.c
    ../42change/Source/42init.c
    ../42change/Source/shActuators.c
    ../42change/Source/shFsw.c
    ../42change/Source/shIokit.c
    ../42change/Source/shExec.c
)

# Exec file create
add_executable(${PROJECT_NAME} ${SOURCES})

# Setting for Windows
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()

#target_link_libraries(${PROJECT_NAME} m)

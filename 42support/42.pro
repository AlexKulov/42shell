QT -= gui

CONFIG += console
CONFIG -= app_bundle

CONFIG += 42GUI
42GUI {
    DEFINES += _ENABLE_GUI_
    DEFINES += _USE_SHADERS_
    DEFINES += _USE_GLUT_
    INCLUDEPATH += glew/include/GL \
                   freeglut/include/GL

    SOURCES += \
        ../42/Kit/Source/glkit.c \
        ../42/Source/42gl.c \
        ../42/Source/42glut.c \
        ../42change/Source/42gpgpu.c
    # GLWF not use in origin 42/Makefile
    #   ../42/Source/42glfw.c

    LIBS += -lopengl32 -lfreeglut -lglu32 -lglew32
}


INCLUDEPATH +=../42/Include \
              ../42/Kit/Include

SOURCES += \
        ../42/Kit/Source/dcmkit.c \
        ../42/Kit/Source/envkit.c \
        ../42/Kit/Source/fswkit.c \
        ../42/Kit/Source/geomkit.c \
        ../42/Kit/Source/iokit.c \
        ../42/Kit/Source/mathkit.c \
        ../42/Kit/Source/msis86kit.c \
        ../42/Kit/Source/nrlmsise00kit.c \
        ../42/Kit/Source/orbkit.c \
        ../42/Kit/Source/sigkit.c \
        ../42/Kit/Source/sphkit.c \
        ../42/Kit/Source/timekit.c \
        ../42/Source/42actuators.c \
        ../42/Source/42cmd.c \
        ../42/Source/42dynamics.c \
        ../42/Source/42environs.c \
        ../42/Source/42ephem.c \
        ../42change/Source/42exec.c \
        ../42/Source/42fsw.c \
        ../42/Source/42init.c \
        ../42/Source/42ipc.c \
        ../42/Source/42main.c \
        ../42/Source/42perturb.c \
        ../42/Source/42report.c \
        ../42/Source/42sensors.c \
        ../42/Source/42jitter.c \
        ../42/Source/42joints.c \
        ../42/Source/42optics.c \
        ../42/Source/AcApp.c \
        ../42/Source/IPC/SimReadFromFile.c \
        ../42/Source/IPC/SimReadFromSocket.c \
        ../42/Source/IPC/SimWriteToFile.c \
        ../42/Source/IPC/SimWriteToSocket.c

TEMPLATE = app
TARGET = my42

OBJECTS_DIR = ./debug
DESTDIR = ./../42

LIBS += -lws2_32

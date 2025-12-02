#Test
QT -= gui

CONFIG += console
CONFIG -= app_bundle

CONFIG += QTPLOT
QTPLOT {
    DEFINES += _USE_QTPLOT_
    QT += core gui widgets
    QT += network printsupport #for customplot
    INCLUDEPATH +=QtSource \

    SOURCES += \
    QtSource/mainwindow.cpp \
    QtSource/main.cpp \
    QtSource/qcustomplot.cpp \

    HEADERS += \
    QtSource/mainwindow.h \
    QtSource/qcustomplot.h

    FORMS += \
    QtSource/mainwindow.ui
}
else {
    SOURCES += \
    ../42/Source/42main.c \
}

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
        ../42change/Source/42glut.c \
        ../42change/Source/42gpgpu.c
    # GLWF not use in origin 42/Makefile
    #   ../42/Source/42glfw.c

    win32:{
        LIBS += ./glew/lib/libglew32.a
        LIBS += ./glew/lib/libglew32.dll.a
        LIBS += ./freeglut/lib/libfreeglut.a
        LIBS += ./freeglut/lib/libfreeglut_static.a
        LIBS += -lopengl32 -lglu32
    }
    unix: {
        LIBS += -lGL -lglut -lGLEW -lGLU
    }
}

INCLUDEPATH +=../42/Include \
              ../42change/Include \
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
        ../42/Source/42environs.c \
        ../42/Source/42ephem.c \
        ../42/Source/42ipc.c \
        ../42/Source/42perturb.c \
        ../42/Source/42sensors.c \
        ../42/Source/42jitter.c \
        ../42/Source/42joints.c \
        ../42/Source/42optics.c \
        ../42/Source/AcApp.c \
        ../42/Source/42dynamics.c \
        #../42/Source/42init.c \
        ../42/Source/42fsw.c \
        ../42/Source/42cmd.c \
        ../42change/Source/42init.c \
        ../42change/Source/shFsw.c \
        ../42change/Source/shIokit.c \
        ../42change/Source/42exec.c \
        ../42change/Source/thrModel.c \
        ../cAlgorithms/serviceAlg.c \
        ../cAlgorithms/simplestMode.c

#42 version after 27.03.25
CONFIG += IPC_OVERHAUL
IPC_OVERHAUL {
    SOURCES += \
        ../42/Source/AutoCode/TxRxIPC.c \
        ../42/Source/AutoCode/WriteAcToCsv.c \
        ../42/Source/AutoCode/WriteScToCsv.c
}
else{
   SOURCES += \
        ../42/Source/IPC/SimReadFromFile.c \
        ../42/Source/IPC/SimReadFromSocket.c \
        ../42/Source/IPC/SimWriteToFile.c \
        ../42/Source/IPC/SimWriteToSocket.c
}

TEMPLATE = app
TARGET = 42

OBJECTS_DIR = ./debug
DESTDIR = ./../42

win32: LIBS += -lws2_32

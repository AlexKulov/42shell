QT -= gui

CONFIG += console
CONFIG -= app_bundle

CONFIG -= QTPLOT
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

CONFIG += IMPLOT
IMPLOT {
    DEFINES += _USE_IMPLOT_
    INCLUDEPATH +=../42add/plot \
                  ../42add/plot/imgui \
                  ../42add/plot/imgui/backends \
                  ../42add/plot/implot
    SOURCES += \
        ../42add/plot/plotDraw.cpp

    SOURCES += \
        ../42add/plot/imgui/imgui.cpp \
        ../42add/plot/imgui/imgui_draw.cpp \
        ../42add/plot/imgui/imgui_widgets.cpp \
        ../42add/plot/imgui/imgui_tables.cpp \
        ../42add/plot/imgui/backends/imgui_impl_glut.cpp \
        ../42add/plot/imgui/backends/imgui_impl_opengl2.cpp

    SOURCES += \
        ../42add/plot/implot/implot.cpp \
        ../42add/plot/implot/implot_items.cpp \
}

CONFIG += 42GUI
42GUI {
    DEFINES += _ENABLE_GUI_
    DEFINES += _USE_SHADERS_
    DEFINES += _USE_GLUT_
    INCLUDEPATH += glew/include/GL \
                   freeglut/include/GL

    SOURCES += \
        ../42core/Kit/Source/glkit.c \
        ../42core/Source/42gl.c \
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

INCLUDEPATH +=../42core/Include \
              ../42core/Kit/Include \
              ../42add

SOURCES += \
        ../42core/Kit/Source/dcmkit.c \
        ../42core/Kit/Source/envkit.c \
        ../42core/Kit/Source/fswkit.c \
        #../42/Kit/Source/geomkit.c \
        ../42core/Kit/Source/meshkit.c \
        ../42core/Kit/Source/iokit.c \
        ../42core/Kit/Source/mathkit.c \
        ../42core/Kit/Source/msis86kit.c \
        ../42core/Kit/Source/nrlmsise00kit.c \
        ../42core/Kit/Source/orbkit.c \
        ../42core/Kit/Source/sigkit.c \
        ../42core/Kit/Source/sphkit.c \
        ../42core/Kit/Source/timekit.c \
        ../42core/Source/42actuators.c \
        ../42core/Source/42environs.c \
        ../42core/Source/42ephem.c \
        ../42core/Source/42ipc.c \
        ../42core/Source/42perturb.c \
        ../42core/Source/42sensors.c \
        ../42core/Source/42jitter.c \
        ../42core/Source/42joints.c \
        ../42core/Source/42optics.c \
        ../42core/Source/AcApp.c \
        ../42core/Source/42dynamics.c \
        ../42/Source/42fsw.c \
        ../42/Source/42cmd.c \
    ../42add/PSModel.c \
    ../42add/SPSModel.c \
    ../42add/fswAlg.c \
    ../42add/fswMode.c \
        ../42change/Source/42init.c \
    ../42change/Source/shActuators.c \
        ../42change/Source/shFsw.c \
        ../42change/Source/shIokit.c \
    ../42change/Source/shExec.c \
    ../42add/orbControl.c

#42 version after 27.03.25
CONFIG += IPC_OVERHAUL
IPC_OVERHAUL {
    SOURCES += \
        ../42core/Source/AutoCode/TxRxIPC.c \
        ../42core/Source/AutoCode/WriteAcToCsv.c \
        ../42core/Source/AutoCode/WriteScToCsv.c
}
else{
   SOURCES += \
        ../42core/Source/IPC/SimReadFromFile.c \
        ../42core/Source/IPC/SimReadFromSocket.c \
        ../42core/Source/IPC/SimWriteToFile.c \
        ../42core/Source/IPC/SimWriteToSocket.c
}

TEMPLATE = app
TARGET = 42

OBJECTS_DIR = ./debug
DESTDIR = ./../42core

win32: LIBS += -lws2_32

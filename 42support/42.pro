QT -= gui

CONFIG += console
CONFIG -= app_bundle

CONFIG += QTPLOT
QTPLOT {
    DEFINES += _USE_QTPLOT_
    QT += core gui widgets
    QT += network printsupport #for customplot
    INCLUDEPATH +=../42dev/qtPlotGui

    SOURCES += \
    ../42dev/qtPlotGui/mainwindow.cpp \
    ../42dev/qtPlotGui/main.cpp \
    ../42dev/qtPlotGui/qcustomplot.cpp

    HEADERS += \
    ../42dev/qtPlotGui/mainwindow.h \
    ../42dev/qtPlotGui/qcustomplot.h

    FORMS += \
    ../42dev/qtPlotGui/mainwindow.ui
}
else {
    SOURCES += \
    ../42core/Source/42main.c
}

CONFIG += IMPLOT
IMPLOT {
    DEFINES += _USE_IMPLOT_
    INCLUDEPATH +=../42dev/imPlotGui \
                  ../42dev/imPlotGui/imgui \
                  ../42dev/imPlotGui/imgui/backends \
                  ../42dev/imPlotGui/implot
    SOURCES += \
        ../42dev/imPlotGui/plotDraw.cpp

    SOURCES += \
        ../42dev/imPlotGui/imgui/imgui.cpp \
        ../42dev/imPlotGui/imgui/imgui_draw.cpp \
        ../42dev/imPlotGui/imgui/imgui_widgets.cpp \
        ../42dev/imPlotGui/imgui/imgui_tables.cpp \
        ../42dev/imPlotGui/imgui/backends/imgui_impl_glut.cpp \
        ../42dev/imPlotGui/imgui/backends/imgui_impl_opengl2.cpp

    SOURCES += \
        ../42dev/imPlotGui/implot/implot.cpp \
        ../42dev/imPlotGui/implot/implot_items.cpp \
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
              ../42dev

SOURCES += \
        ../42core/Kit/Source/dcmkit.c \
        ../42core/Kit/Source/envkit.c \
        ../42core/Kit/Source/fswkit.c \
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
        ../42core/Source/AutoCode/TxRxIPC.c \
        ../42core/Source/AutoCode/WriteAcToCsv.c \
        ../42core/Source/AutoCode/WriteScToCsv.c \
        ../42core/Source/42fsw.c \
        ../42core/Source/42cmd.c \
        ../42dev/PSModel.c \
        ../42dev/SPSModel.c \
        ../42dev/fswAlg.c \
        ../42dev/fswMode.c \
        ../42dev/orbControl.c \
        ../42change/Source/42init.c \
        ../42change/Source/shActuators.c \
        ../42change/Source/shFsw.c \
        ../42change/Source/shIokit.c \
        ../42change/Source/shExec.c

TEMPLATE = app
TARGET = 42

OBJECTS_DIR = ./debug
DESTDIR = ./../42core

win32: LIBS += -lws2_32

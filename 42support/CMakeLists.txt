# Minimum version CMake
cmake_minimum_required(VERSION 3.10)

# Pro name & set std language
project(42sh LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
# Path for exec file generation
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../42core)

# --------------------------------------------------------------
# Build options
option(QTPLOT "Enable Qt-based plotting (replaces console main)" OFF)
option(IMPLOT "Enable ImPlot plotting (requires 42GUI)" OFF)
option(42GUI "Enable User interface by 42 origin" OFF)
option(cJSON+ "Enable cJSON output for Report+" OFF)
# --------------------------------------------------------------

# Include path for headers
include_directories(
    ../42core/Include
    ../42core/Kit/Include
    ../42dev
)

# Src file list
set(BASE_SOURCES
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
    ../42core/Source/42fsw.c
    ../42core/Source/42ipc.c
    ../42core/Source/42perturb.c
    ../42core/Source/42sensors.c
    ../42core/Source/42jitter.c
    ../42core/Source/42joints.c
    ../42core/Source/42optics.c
    ../42core/Source/42report.c
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
    ../42change/Source/42exec.c
    ../42dev/report+/Interface.c
    ../42dev/report+/ViewWindow.c
    ../42dev/report+/FlightData.c
    ../42dev/report+/Common.c
)

# --------------------------------------------------------------
# choice of input point depend of QTPLOT
if(QTPLOT)
    set(MAIN_SOURCE ../42dev/qtPlotGui/main.cpp)
else()
    set(MAIN_SOURCE ../42core/Source/42main.c)
endif()
# --------------------------------------------------------------
set(SOURCES ${BASE_SOURCES} ${MAIN_SOURCE})

# --------------------------------------------------------------
if(QTPLOT)
    # 1. Include Qt5
    find_package(Qt5 COMPONENTS Core Gui Widgets Network PrintSupport REQUIRED)
    # 2. Add source Qt-gui
    list(APPEND SOURCES
        ../42dev/qtPlotGui/mainwindow.cpp
        ../42dev/qtPlotGui/qcustomplot.cpp
    )
    # 3. .ui file processing (ui_mainwindow.h generate)
    set(UI_FILE ../42dev/qtPlotGui/mainwindow.ui)
    qt5_wrap_ui(UI_HEADERS ${UI_FILE})
    list(APPEND SOURCES ${UI_HEADERS})
    # 4. MOC processing
    set(QTPLOT_HEADERS
        ../42dev/qtPlotGui/qcustomplot.h
        ../42dev/qtPlotGui/mainwindow.h
    )
    qt5_wrap_cpp(QTPLOT_MOC_SOURCES ${QTPLOT_HEADERS})
    list(APPEND SOURCES ${QTPLOT_MOC_SOURCES})

    include_directories(${CMAKE_CURRENT_BINARY_DIR}) #for ui_mainwindows.h
    include_directories(../42dev/qtPlotGui)

endif()
# --------------------------------------------------------------
if(IMPLOT)
    # 1. Path for ImGui/ImPlot
    include_directories(
        ../42dev/imPlotGui
        ../42dev/imPlotGui/imgui
        ../42dev/imPlotGui/imgui/backends
        ../42dev/imPlotGui/implot
    )
    # 2. Source for ImPlot и ImGui
    list(APPEND SOURCES
        ../42dev/imPlotGui/plotDraw.cpp
        ../42dev/imPlotGui/imgui/imgui.cpp
        ../42dev/imPlotGui/imgui/imgui_draw.cpp
        ../42dev/imPlotGui/imgui/imgui_widgets.cpp
        ../42dev/imPlotGui/imgui/imgui_tables.cpp
        ../42dev/imPlotGui/imgui/backends/imgui_impl_glut.cpp
        ../42dev/imPlotGui/imgui/backends/imgui_impl_opengl2.cpp
        ../42dev/imPlotGui/implot/implot.cpp
        ../42dev/imPlotGui/implot/implot_items.cpp
    )
endif()
# --------------------------------------------------------------
if(42GUI)
    # Source for 42GUI
    list(APPEND SOURCES
        ../42core/Kit/Source/glkit.c
        ../42core/Source/42gl.c
        ../42change/Source/42glut.c
        ../42change/Source/42gpgpu.c
    )
endif()
# --------------------------------------------------------------
if(cJSON+)
    # Support json output
    list(APPEND SOURCES
        ../42dev/report+/cJSON/cJSON.c
    )

    include_directories(../42dev/report+/cJSON/)
endif()
# --------------------------------------------------------------
if(IMPLOT OR 42GUI)# include GLEW и freeglut
    include_directories(
        glew/include/GL
        freeglut/include/GL
        glew/include
        freeglut/include
    )
endif()
# --------------------------------------------------------------
# MAKE EXE-FILE
add_executable(${PROJECT_NAME} ${SOURCES})
# --------------------------------------------------------------
if(QTPLOT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_QTPLOT_)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        Qt5::PrintSupport
    )
endif()

if(IMPLOT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_IMPLOT_)
endif()

if(42GUI)# include GLEW и freeglut
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _ENABLE_GUI_
        _USE_SHADERS_
        _USE_GLUT_
    )
endif()

if(cJSON+)# support cjson output file
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CJSON_PLUS_
    )
endif()

if(IMPLOT OR 42GUI)# include GLEW и freeglut
    if(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/glew/lib/libglew32.a
            ${CMAKE_SOURCE_DIR}/glew/lib/libglew32.dll.a
            ${CMAKE_SOURCE_DIR}/freeglut/lib/libfreeglut.a
            ${CMAKE_SOURCE_DIR}/freeglut/lib/libfreeglut_static.a
            opengl32
            glu32
        )
    else() # Unix/Linux
        target_link_libraries(${PROJECT_NAME} PRIVATE
            GL
            glut
            GLEW
            GLU
        )
    endif()
endif()

# Setting socket lib for Windows
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()

# Setting math lib for Linux
if(UNIX OR 42GUI)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

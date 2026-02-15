#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <GL/glut.h>
#include "freeglut_ext.h"

#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include "implot.h"

/* Интерфейс */
#ifdef __cplusplus
extern "C" {
#endif
    #include "42.h"
    int HandoffToPlotGui(int argc, char **argv);
#ifdef __cplusplus
}
#endif

/* Глобальные переменные для управления графиками */
static int plot_width = 1200;
static int plot_height = 800;
static GLuint plot_window;
static int plot_data_buffer_size = 2000; /* Количество точек на графике */

/* Структуры для хранения данных графиков */
typedef struct {
    double *data;        /* Массив данных */
    int size;           /* Текущий размер */
    int capacity;       /* Вместимость буфера */
    int offset;         /* Смещение для кольцевого буфера */
    char label[64];     /* Метка графика */
    ImVec4 color;       /* Цвет графика */
} PlotBuffer;

/* Список переменных для отображения */
typedef enum {
    PLOT_WB_X,      /* Угловая скорость X */
    PLOT_WB_Y,      /* Угловая скорость Y */
    PLOT_WB_Z,      /* Угловая скорость Z */
    PLOT_RW1,       /* Скорость маховика 1 */
    PLOT_RW2,       /* Скорость маховика 2 */
    PLOT_RW3,       /* Скорость маховика 3 */
    PLOT_RW4,       /* Скорость маховика 4 */
    PLOT_NUM_VARS   /* Количество переменных */
} PlotVariable;

static PlotBuffer plot_buffers[PLOT_NUM_VARS];
static double *time_buffer = NULL; /* Буфер времени */
static int time_buffer_size = 0;
static int time_buffer_offset = 0;

/* Флаги для управления */
static int plot_paused = 0;
static bool plot_show_grid = 1;
static bool plot_show_legend = 1;
static bool plot_auto_scale = 1;
static float plot_history_sec = 60.0f; /* История в секундах */
static float plot_refresh_rate = 30.0f; /* Частота обновления Гц */

/* Прототипы функций */
static void PlotRenderExec(void);
static void PlotReshapeHandler(int width, int height);
static void PlotIdle(void);
static void PlotKeyboardHandler(unsigned char key, int x, int y);
static void PlotSpecialKeyHandler(int key, int x, int y);
static void InitPlotBuffers(void);
static void UpdatePlotData(void);
static void DrawPlotWindow(void);
static void CleanupPlotGui(void);
static double* PrepareTimeData(int count);
static double* PreparePlotData(PlotBuffer* buffer, int count);
static void GetBufferMinMax(PlotBuffer* buffer, double* min_val, double* max_val);

/* Вспомогательные функции для подготовки данных */
static double* PrepareTimeData(int count) {
    if (count <= 0) return NULL;

    double* result = (double*)malloc(count * sizeof(double));
    if (!result) return NULL;

    for (int i = 0; i < count; i++) {
        int idx = (time_buffer_offset - count + i + time_buffer_size) % time_buffer_size;
        result[i] = time_buffer[idx];
    }

    return result;
}

static double* PreparePlotData(PlotBuffer* buffer, int count) {
    if (count <= 0) return NULL;

    double* result = (double*)malloc(count * sizeof(double));
    if (!result) return NULL;

    for (int i = 0; i < count; i++) {
        int idx = (buffer->offset - count + i + buffer->capacity) % buffer->capacity;
        result[i] = buffer->data[idx];
    }

    return result;
}

static void GetBufferMinMax(PlotBuffer* buffer, double* min_val, double* max_val) {
    if (buffer->size == 0) {
        *min_val = 0.0;
        *max_val = 0.0;
        return;
    }

    int first_idx = (buffer->offset - buffer->size + buffer->capacity) % buffer->capacity;
    *min_val = buffer->data[first_idx];
    *max_val = buffer->data[first_idx];

    for (int i = 1; i < buffer->size; i++) {
        int idx = (buffer->offset - buffer->size + i + buffer->capacity) % buffer->capacity;
        if (buffer->data[idx] < *min_val) *min_val = buffer->data[idx];
        if (buffer->data[idx] > *max_val) *max_val = buffer->data[idx];
    }
}

static void InitPlotBuffers(void) {
    /* Инициализация буфера времени */
    time_buffer_size = plot_data_buffer_size;
    time_buffer = (double*)malloc(time_buffer_size * sizeof(double));
    memset(time_buffer, 0, time_buffer_size * sizeof(double));

    /* Только необходимые переменные */
    const char* labels[PLOT_NUM_VARS] = {
        "Angular Velocity X (rad/s)",
        "Angular Velocity Y (rad/s)",
        "Angular Velocity Z (rad/s)",
        "Reaction Wheel 1 (rad/s)",
        "Reaction Wheel 2 (rad/s)",
        "Reaction Wheel 3 (rad/s)",
        "Reaction Wheel 4 (rad/s)"
    };

    const ImVec4 colors[PLOT_NUM_VARS] = {
        ImVec4(1.0f, 0.0f, 0.0f, 1.0f), /* Красный - ω_x */
        ImVec4(0.0f, 1.0f, 0.0f, 1.0f), /* Зеленый - ω_y */
        ImVec4(0.0f, 0.5f, 1.0f, 1.0f), /* Синий - ω_z */
        ImVec4(1.0f, 1.0f, 0.0f, 1.0f), /* Желтый - RW1 */
        ImVec4(1.0f, 0.0f, 1.0f, 1.0f), /* Пурпурный - RW2 */
        ImVec4(0.0f, 1.0f, 1.0f, 1.0f), /* Голубой - RW3 */
        ImVec4(1.0f, 0.5f, 0.0f, 1.0f)  /* Оранжевый - RW4 */
    };

    for (int i = 0; i < PLOT_NUM_VARS; i++) {
        plot_buffers[i].capacity = plot_data_buffer_size;
        plot_buffers[i].data = (double*)malloc(plot_buffers[i].capacity * sizeof(double));
        memset(plot_buffers[i].data, 0, plot_buffers[i].capacity * sizeof(double));
        plot_buffers[i].size = 0;
        plot_buffers[i].offset = 0;
        strncpy(plot_buffers[i].label, labels[i], sizeof(plot_buffers[i].label) - 1);
        plot_buffers[i].color = colors[i];
    }
}

static void UpdatePlotData(void) {
    static double last_update_time = 0.0;

    /* Проверяем, нужно ли обновлять данные (по частоте обновления) */
    if (plot_paused || (SimTime - last_update_time) < (1.0 / plot_refresh_rate)) {
        return;
    }

    last_update_time = SimTime;

    /* Обновляем буфер времени (кольцевой буфер) */
    time_buffer[time_buffer_offset] = SimTime;
    time_buffer_offset = (time_buffer_offset + 1) % time_buffer_size;

    /* Получаем данные из симуляции для первого космического аппарата */
    if (Nsc > 0 && SC[0].Exists) {
        struct SCType *S = &SC[0];

        /* Угловые скорости (wn - угловая скорость в инерциальной системе, wb - в связанной) */
        /* Скорее всего вам нужно wb, но в вашей структуре я вижу только wn (в N) */
        /* Если нужно wb, возможно оно вычисляется или хранится в другом месте */
        plot_buffers[PLOT_WB_X].data[plot_buffers[PLOT_WB_X].offset] = S->B[0].wn[0]; /* или wb? */
        plot_buffers[PLOT_WB_Y].data[plot_buffers[PLOT_WB_Y].offset] = S->B[0].wn[1];
        plot_buffers[PLOT_WB_Z].data[plot_buffers[PLOT_WB_Z].offset] = S->B[0].wn[2];

        /* Скорости маховиков - через структуру Whl */
        if (S->Nw > 0 && S->Whl) {
            plot_buffers[PLOT_RW1].data[plot_buffers[PLOT_RW1].offset] = S->Whl[0].H;
            if (S->Nw > 1) plot_buffers[PLOT_RW2].data[plot_buffers[PLOT_RW2].offset] = S->Whl[1].H;
            if (S->Nw > 2) plot_buffers[PLOT_RW3].data[plot_buffers[PLOT_RW3].offset] = S->Whl[2].H;
            if (S->Nw > 3) plot_buffers[PLOT_RW4].data[plot_buffers[PLOT_RW4].offset] = S->Whl[3].H;
        } else {
            /* Если маховиков нет, установим 0 */
            plot_buffers[PLOT_RW1].data[plot_buffers[PLOT_RW1].offset] = 0.0;
            plot_buffers[PLOT_RW2].data[plot_buffers[PLOT_RW2].offset] = 0.0;
            plot_buffers[PLOT_RW3].data[plot_buffers[PLOT_RW3].offset] = 0.0;
            plot_buffers[PLOT_RW4].data[plot_buffers[PLOT_RW4].offset] = 0.0;
        }

        /* Обновляем смещения для всех буферов */
        for (int i = 0; i < PLOT_NUM_VARS; i++) {
            plot_buffers[i].offset = (plot_buffers[i].offset + 1) % plot_buffers[i].capacity;
            if (plot_buffers[i].size < plot_buffers[i].capacity) {
                plot_buffers[i].size++;
            }
        }
    }
}

static int plot_current_tab = 0;
static const int plot_tab_count = 2;

static void DrawPlotWindow(void) {
    /* Главное окно управления */
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize); // Используем актуальный размер

    ImGui::Begin("Spacecraft Telemetry", NULL,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_MenuBar);

    /* Меню-бар - упрощенная версия */
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit", "Esc")) {
                glutLeaveMainLoop();
            }
            ImGui::EndMenu();
        }

        /* Статус симуляции */
        ImGui::SameLine(ImGui::GetWindowWidth() - 250);
        ImGui::TextColored(plot_paused ? ImVec4(1, 0.5, 0, 1) : ImVec4(0, 1, 0, 1),
                          "%s", plot_paused ? "PAUSED" : "RUNNING");
        ImGui::SameLine();
        ImGui::Text(" | Time: %.2fs", SimTime);

        ImGui::EndMenuBar();
    }

    /* Панель управления */
    ImGui::BeginChild("ControlPanel", ImVec2(200, 0), true);

    ImGui::Text("Controls:");
    if (ImGui::Button(plot_paused ? "Resume" : "Pause")) {
        plot_paused = !plot_paused;
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        for (int i = 0; i < PLOT_NUM_VARS; i++) {
            plot_buffers[i].size = 0;
            plot_buffers[i].offset = 0;
            memset(plot_buffers[i].data, 0, plot_buffers[i].capacity * sizeof(double));
        }
        time_buffer_offset = 0;
        memset(time_buffer, 0, time_buffer_size * sizeof(double));
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Display:");
    ImGui::Checkbox("Legend", &plot_show_legend);
    ImGui::Checkbox("Grid", &plot_show_grid);
    ImGui::Checkbox("Auto Scale", &plot_auto_scale);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Parameters:");
    ImGui::SliderFloat("History (s)", &plot_history_sec, 1.0f, 300.0f, "%.0f s");
    ImGui::SliderFloat("Refresh (Hz)", &plot_refresh_rate, 1.0f, 60.0f, "%.0f Hz");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    /* Информация о текущих значениях */
    ImGui::Text("Current Values:");
    if (Nsc > 0 && SC[0].Exists) {
        struct SCType *S = &SC[0];
        ImGui::TextColored(ImVec4(1, 0.0, 0, 1), "w_x: %.6f", S->B[0].wn[0]);
        ImGui::TextColored(ImVec4(0, 1.0, 0, 1), "w_y: %.6f", S->B[0].wn[1]);
        ImGui::TextColored(ImVec4(0, 0.5, 1, 1), "w_z: %.6f", S->B[0].wn[2]);

        /* Данные маховиков */
        if (S->Nw > 0 && S->Whl) { 
            ImGui::TextColored(ImVec4(1, 1.0, 0, 1), "RW1: %.1f", S->Whl[0].H);
            if (S->Nw >= 1)
                ImGui::TextColored(ImVec4(1, 0.0, 1, 1), "RW2: %.1f", S->Whl[1].H);
            if (S->Nw >= 2)
                ImGui::TextColored(ImVec4(0, 1.0, 1, 1), "RW3: %.1f", S->Whl[2].H);
            if (S->Nw >= 3)
                ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "RW4: %.1f", S->Whl[3].H);
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();

    /* Графики */
    ImGui::BeginChild("PlotArea", ImVec2(0, 0), true);

    if (ImGui::BeginTabBar("PlotTabs", ImGuiTabBarFlags_None)) {
        /* Вкладка 1: Угловые скорости */
        ImGuiTabItemFlags tab1_flags = ImGuiTabItemFlags_None;
        if (plot_current_tab == 0) {
            tab1_flags |= ImGuiTabItemFlags_SetSelected;
        }

        if (ImGui::BeginTabItem("Angular Velocity", NULL, tab1_flags)) {
            /* Флаги для графика */
            ImPlotFlags flags = ImPlotFlags_None;
            if (!plot_show_legend) {
                flags |= ImPlotFlags_NoLegend;
            }
            if (plot_show_grid) {
                flags |= ImPlotFlags_Crosshairs;
            }

            if (ImPlot::BeginPlot("Angular Velocity", ImVec2(-1, -1), flags)) {

                /* Настройка осей */
                ImPlot::SetupAxes("Time (s)", "Angular Velocity (rad/s)",
                                 ImPlotAxisFlags_None, ImPlotAxisFlags_None);

                /* Настройка сетки */
                if (plot_show_grid) {
                    // Сетка отображается по умолчанию, флаги не нужны
                    ImPlot::SetupAxis(ImAxis_X1, NULL, ImPlotAxisFlags_None);
                    ImPlot::SetupAxis(ImAxis_Y1, NULL, ImPlotAxisFlags_None);
                } else {
                    // Явно отключаем сетку и декорации тиков
                    ImPlot::SetupAxis(ImAxis_X1, NULL, ImPlotAxisFlags_NoGridLines);
                    ImPlot::SetupAxis(ImAxis_Y1, NULL, ImPlotAxisFlags_NoGridLines);
                }

                double x_min = SimTime - plot_history_sec;
                double x_max = SimTime;
                ImPlot::SetupAxisLimits(ImAxis_X1, x_min, x_max, ImPlotCond_Always);
                /* Автомасштабирование */
                if (plot_auto_scale) {
                    /* Автоматический масштаб по Y на основе данных */
                    double y_min = 0, y_max = 0;
                    for (int i = PLOT_WB_X; i <= PLOT_WB_Z; i++) {
                        if (plot_buffers[i].size > 0) {
                            double min_val, max_val;
                            GetBufferMinMax(&plot_buffers[i], &min_val, &max_val);
                            y_min = fmin(y_min, min_val);
                            y_max = fmax(y_max, max_val);
                        }
                    }
                    /* Добавляем 10% от диапазона для запаса */
                    double range = fmax(fabs(y_max - y_min), 0.001);
                    ImPlot::SetupAxisLimits(ImAxis_Y1,
                                           y_min - 0.1 * range,
                                           y_max + 0.1 * range,
                                           ImPlotCond_Always);
                }

                /* Отображение данных */
                int count = plot_buffers[PLOT_WB_X].size;
                if (count > 0) {
                    /* Подготавливаем данные для отображения */
                    double* time_data = PrepareTimeData(count);
                    double* wb_x_data = PreparePlotData(&plot_buffers[PLOT_WB_X], count);
                    double* wb_y_data = PreparePlotData(&plot_buffers[PLOT_WB_Y], count);
                    double* wb_z_data = PreparePlotData(&plot_buffers[PLOT_WB_Z], count);

                    if (time_data && wb_x_data && wb_y_data && wb_z_data) {
                        /* Рисуем линии с разными цветами */
                        ImPlotSpec spec;
                        spec.LineColor = plot_buffers[PLOT_WB_X].color;
                        spec.LineWeight = 2.0f;
                        ImPlot::PlotLine("w_x", time_data, wb_x_data, count, spec);

                        //ImPlot::SetNextLineStyle(plot_buffers[PLOT_WB_X].color, 2.0f);
                        //ImPlot::PlotLine("w_x", time_data, wb_x_data, count);

                        //ImPlot::SetNextLineStyle(plot_buffers[PLOT_WB_Y].color, 2.0f);
                        spec.LineColor = plot_buffers[PLOT_WB_Y].color;
                        ImPlot::PlotLine("w_y", time_data, wb_y_data, count, spec);

                        //ImPlot::SetNextLineStyle(plot_buffers[PLOT_WB_Z].color, 2.0f);
                        spec.LineColor = plot_buffers[PLOT_WB_Z].color;
                        ImPlot::PlotLine("w_z", time_data, wb_z_data, count, spec);
                    }

                    /* Освобождаем временные массивы */
                    if (time_data) free(time_data);
                    if (wb_x_data) free(wb_x_data);
                    if (wb_y_data) free(wb_y_data);
                    if (wb_z_data) free(wb_z_data);
                }

                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
        }

        /* Вкладка 2: Маховики */
        ImGuiTabItemFlags tab2_flags = ImGuiTabItemFlags_None;
        if (plot_current_tab == 1) {
            tab2_flags |= ImGuiTabItemFlags_SetSelected;
        }

        if (ImGui::BeginTabItem("Reaction Wheels", NULL, tab2_flags)) {
            /* Флаги для графика */
            ImPlotFlags flags = ImPlotFlags_None;
            if (!plot_show_legend) {
                flags |= ImPlotFlags_NoLegend;
            }
            if (plot_show_grid) {
                flags |= ImPlotFlags_Crosshairs;
            }

            if (ImPlot::BeginPlot("Reaction Wheel Speeds", ImVec2(-1, -1), flags)) {

                /* Настройка осей */
                ImPlot::SetupAxes("Time (s)", "Speed (rad/s)",
                                 ImPlotAxisFlags_None, ImPlotAxisFlags_None);

                /* Настройка сетки */
                if (plot_show_grid) {
                    // Сетка отображается по умолчанию, флаги не нужны
                    ImPlot::SetupAxis(ImAxis_X1, NULL, ImPlotAxisFlags_None);
                    ImPlot::SetupAxis(ImAxis_Y1, NULL, ImPlotAxisFlags_None);
                } else {
                    // Явно отключаем сетку и декорации тиков
                    ImPlot::SetupAxis(ImAxis_X1, NULL, ImPlotAxisFlags_NoGridLines);
                    ImPlot::SetupAxis(ImAxis_Y1, NULL, ImPlotAxisFlags_NoGridLines);
                }

                double x_min = SimTime - plot_history_sec;
                double x_max = SimTime;
                ImPlot::SetupAxisLimits(ImAxis_X1, x_min, x_max, ImPlotCond_Always);
                /* Автомасштабирование */
                if (plot_auto_scale) {
                    /* Автоматический масштаб по Y на основе данных */
                    double y_min = 0, y_max = 0;
                    for (int i = PLOT_RW1; i <= PLOT_RW4; i++) {
                        if (plot_buffers[i].size > 0) {
                            double min_val, max_val;
                            GetBufferMinMax(&plot_buffers[i], &min_val, &max_val);
                            y_min = fmin(y_min, min_val);
                            y_max = fmax(y_max, max_val);
                        }
                    }
                    /* Добавляем 10% от диапазона для запаса */
                    double range = fmax(fabs(y_max - y_min), 0.001);
                    ImPlot::SetupAxisLimits(ImAxis_Y1,
                                           y_min - 0.1 * range,
                                           y_max + 0.1 * range,
                                           ImPlotCond_Always);
                }

                /* Отображение данных */
                int count = plot_buffers[PLOT_RW1].size;
                if (count > 0) {
                    /* Подготавливаем данные для отображения */
                    double* time_data = PrepareTimeData(count);
                    double* rw1_data = PreparePlotData(&plot_buffers[PLOT_RW1], count);
                    double* rw2_data = PreparePlotData(&plot_buffers[PLOT_RW2], count);
                    double* rw3_data = PreparePlotData(&plot_buffers[PLOT_RW3], count);
                    double* rw4_data = PreparePlotData(&plot_buffers[PLOT_RW4], count);

                    if (time_data && rw1_data && rw2_data && rw3_data && rw4_data) {
                        /* Рисуем линии с разными цветами */
                        ImPlotSpec spec;
                        spec.LineColor = plot_buffers[PLOT_RW1].color;
                        spec.LineWeight = 2.0f;

                        //ImPlot::SetNextLineStyle(plot_buffers[PLOT_RW1].color, 1.5f);
                        ImPlot::PlotLine("RW1", time_data, rw1_data, count, spec);

                        //ImPlot::SetNextLineStyle(plot_buffers[PLOT_RW2].color, 1.5f);
                        spec.LineColor = plot_buffers[PLOT_RW2].color;
                        ImPlot::PlotLine("RW2", time_data, rw2_data, count, spec);

                        //ImPlot::SetNextLineStyle(plot_buffers[PLOT_RW3].color, 1.5f);
                        spec.LineColor = plot_buffers[PLOT_RW3].color;
                        ImPlot::PlotLine("RW3", time_data, rw3_data, count, spec);

                        //ImPlot::SetNextLineStyle(plot_buffers[PLOT_RW4].color, 1.5f);
                        spec.LineColor = plot_buffers[PLOT_RW4].color;
                        ImPlot::PlotLine("RW4", time_data, rw4_data, count, spec);
                    }

                    /* Освобождаем временные массивы */
                    if (time_data) free(time_data);
                    if (rw1_data) free(rw1_data);
                    if (rw2_data) free(rw2_data);
                    if (rw3_data) free(rw3_data);
                    if (rw4_data) free(rw4_data);
                }

                ImPlot::EndPlot();
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndChild();

    ImGui::End();

    /* Рендеринг ImGui */
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

static void PlotRenderExec(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // получаем реальный размер текущего окна GLUT
    int winWidth = glutGet(GLUT_WINDOW_WIDTH);
    int winHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // Обновляем размер дисплея ImGui
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)winWidth, (float)winHeight);

    /* Начинаем новый ImGui фрейм */
    ImGui_ImplGLUT_NewFrame(); // !!!
    ImGui_ImplOpenGL2_NewFrame();
    ImGui::NewFrame();

    /* Обновляем данные графиков */
    UpdatePlotData();

    /* Отрисовываем окно с графиками */
    DrawPlotWindow(); // Убедитесь, что здесь используется всё доступное пространство

    /* Рендеринг ImGui уже выполняется в DrawPlotWindow() через ImGui::Render() */
    glutSwapBuffers();
}

static void PlotReshapeHandler(int width, int height) {
    // 1. Стандартный OpenGL viewport
    glViewport(0, 0, width, height);

    // 2. Обновляем размер для ImGui
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);

    // 3. Запрашиваем перерисовку
    glutPostRedisplay();
}

static void PlotIdle(void) {
    /* Если не на паузе, выполняем шаг симуляции */
    if (!plot_paused) {
        // Выполняем шаг симуляции
        /*int done = SimStep();
        // Если симуляция завершена
        if (done) {
            printf("Simulation completed\n");
            glutLeaveMainLoop();
            return;
        }
        glutPostRedisplay();*/

        static int timer_initialized = 0;
        static struct timespec last_time;
        struct timespec current_time;
        long nanosec_per_frame;

        if (!timer_initialized) {
            clock_gettime(CLOCK_MONOTONIC, &last_time);
            timer_initialized = 1;
        }

        // Ограничение частоты обновления
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        nanosec_per_frame = (long)(1e9 / plot_refresh_rate);

        long elapsed_ns = (current_time.tv_sec - last_time.tv_sec) * 1e9 +
                         (current_time.tv_nsec - last_time.tv_nsec);

        if (elapsed_ns >= nanosec_per_frame) {
            int done = SimStep();
            if (done) {
                printf("Simulation completed\n");
                glutLeaveMainLoop();
                return;
            }
            last_time = current_time;
            glutPostRedisplay();
        }
    }
    else {
        // На паузе, просто перерисовываем
        glutPostRedisplay();
    }
}

static void PlotKeyboardHandler(unsigned char key, int x, int y) {
    /* Передаем ввод в ImGui */
    ImGui_ImplGLUT_KeyboardFunc(key, x, y);

    /* Дополнительные горячие клавиши */
    switch (key) {
        case ' ':
            plot_paused = !plot_paused;
            break;
        case 'q':
        case 27: /* ESC */
            glutLeaveMainLoop();
            break;
        case 'r':
            /* Сброс данных */
            for (int i = 0; i < PLOT_NUM_VARS; i++) {
                plot_buffers[i].size = 0;
                plot_buffers[i].offset = 0;
                memset(plot_buffers[i].data, 0, plot_buffers[i].capacity * sizeof(double));
            }
            time_buffer_offset = 0;
            memset(time_buffer, 0, time_buffer_size * sizeof(double));
            break;
        case 9: /* TAB key (ASCII code for Tab) */
        /* Переключаем вкладку */
            plot_current_tab = (plot_current_tab + 1) % plot_tab_count;
            break;
        case 'l': // Устанавливаем легенду
            plot_show_legend = !plot_show_legend;
            break;
        case 'g': // Устанавливаем сетку
            plot_show_grid = !plot_show_grid;
            break;
        case 's':
            plot_auto_scale = !plot_auto_scale;
            break;
    }

    glutPostRedisplay();
}

static void PlotSpecialKeyHandler(int key, int x, int y) {
    /* Передаем специальные клавиши в ImGui */
    ImGui_ImplGLUT_SpecialFunc(key, x, y);
    glutPostRedisplay();
}

/* Функция для очистки ресурсов */
static void CleanupPlotGui(void) {
    /* Очищаем ImGui */
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    /* Очищаем буферы данных */
    if (time_buffer) {
        free(time_buffer);
        time_buffer = NULL;
    }

    for (int i = 0; i < PLOT_NUM_VARS; i++) {
        if (plot_buffers[i].data) {
            free(plot_buffers[i].data);
            plot_buffers[i].data = NULL;
        }
    }
}

int HandoffToPlotGui(int argc, char **argv) {
    printf("Initializing Plot GUI with ImGui/ImPlot\n");

    /* Инициализация GLUT */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(plot_width, plot_height);

    /* Создание окна */
    plot_window = glutCreateWindow("Spacecraft Simulation - Real-time Plots");
    glutSetWindow(plot_window);
    glutPositionWindow(100, 100);

    /* Настройка OpenGL */
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    /* Инициализация ImGui */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    /* Настройка стиля ImGui */
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;

    /* Инициализация ImGui для GLUT/OpenGL2 */
    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL2_Init();

    /* Настройка callback функций */
    glutDisplayFunc(PlotRenderExec);
    glutIdleFunc(PlotIdle);
    glutReshapeFunc(PlotReshapeHandler);
    glutKeyboardFunc(PlotKeyboardHandler);
    glutSpecialFunc(PlotSpecialKeyHandler);

    /* Инициализация буферов для графиков */
    InitPlotBuffers();

    printf("Plot GUI initialized successfully\n");

    /* Запуск главного цикла GLUT */
    glutMainLoop();

    /* Очистка при выходе */
    CleanupPlotGui();

    return 0;
}

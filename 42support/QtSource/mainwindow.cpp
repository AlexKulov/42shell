#include "mainwindow.h"
#include "ui_mainwindow.h"

Ui::MainWindow * extUi;
double mSimTime;  //Для передачи параметра в методы класса из внешней функции CppToPlot

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Simulation spacecraft");
    connect (ui->pushButtonSimulation,&QAbstractButton::clicked,this,&MainWindow::StartSimulation);
    setupPlots();

    ui->Plot11->addGraph();
    ui->Plot11->graph(0)->setPen(QColor(Qt::green));
    ui->Plot11->addGraph();
    ui->Plot11->graph(1)->setPen(QColor(Qt::red));
    ui->Plot11->addGraph();
    ui->Plot11->graph(2)->setPen(QColor(Qt::blue));


    ui->Plot12->addGraph();
    ui->Plot12->graph(0)->setPen(QColor(Qt::green));
    ui->Plot12->addGraph();
    ui->Plot12->graph(1)->setPen(QColor(Qt::red));


    ui->Plot13->addGraph();
    ui->Plot13->graph(0)->setPen(QColor(Qt::green));
    ui->Plot13->addGraph();
    ui->Plot13->graph(1)->setPen(QColor(Qt::red));
    ui->Plot13->addGraph();


    ui->Plot21->addGraph();
    ui->Plot21->graph(0)->setPen(QColor(Qt::red));
    ui->Plot21->addGraph();
    ui->Plot21->graph(1)->setPen(QColor(Qt::green));
    ui->Plot21->addGraph();
    ui->Plot21->graph(2)->setPen(QColor(Qt::blue));


    ui->Plot22->addGraph();
    ui->Plot22->graph(0)->setPen(QColor(Qt::red));
    ui->Plot22->addGraph();
    ui->Plot22->graph(1)->setPen(QColor(Qt::green));
    ui->Plot22->addGraph();
    ui->Plot22->graph(2)->setPen(QColor(Qt::blue));


    ui->Plot31->addGraph(); //plotKm
    ui->Plot31->graph(0)->setPen(QColor(Qt::red));
    ui->Plot31->addGraph(); //plotKm
    ui->Plot31->graph(1)->setPen(QColor(Qt::green));
    ui->Plot31->addGraph(); //plotKm
    ui->Plot31->graph(2)->setPen(QColor(Qt::blue));


    ui->Plot32->addGraph();
    ui->Plot32->graph(0)->setPen(QColor(Qt::red));
    ui->Plot32->addGraph();
    ui->Plot32->graph(1)->setPen(QColor(Qt::green));
    ui->Plot32->addGraph();
    ui->Plot32->graph(2)->setPen(QColor(Qt::blue));

    uiPlotSampleCounter=0;
    uiPlotMaxCounter = 10;
    extUi = ui;
}

MainWindow::~MainWindow()
{
    delete ui;
}

#ifdef __cplusplus
extern "C"{
#endif
#include "42.h"
#include "../cAlgorithms/serviceAlg.h"
extern int exec(int argc,char **argv);

void ToPlot(double Time);
#ifdef __cplusplus
}
#endif

extern double modeAng[3];
void ToPlot(double SimTime){
    mSimTime = SimTime;
    static unsigned int PlotSampleCounter=0;
    static unsigned int PlotMaxCounter = 10;

    PlotSampleCounter++;
    if(PlotSampleCounter>PlotMaxCounter){
        PlotSampleCounter = 0;
        if(true){
            extUi->Plot11->graph(0)->addData(SimTime,SC[0].Thr[0].F);
            extUi->Plot11->graph(1)->addData(SimTime,SC[0].Thr[1].F);
            extUi->Plot11->rescaleAxes();
            extUi->Plot11->replot();
        }

        if(true){
            extUi->Plot12->graph(0)->addData(SimTime,SC[0].Thr[2].F);
            extUi->Plot12->graph(1)->addData(SimTime,SC[0].Thr[3].F);
            extUi->Plot12->rescaleAxes();
            extUi->Plot12->replot();
        }

        if(true){
            extUi->Plot13->graph(0)->addData(SimTime,SC[0].Thr[4].F);
            extUi->Plot13->graph(1)->addData(SimTime,SC[0].Thr[5].F);
            extUi->Plot13->rescaleAxes();
            extUi->Plot13->replot();
        }

        if(true){
            extUi->Plot21->graph(0)->addData(SimTime,SC[0].B[0].Trq[0]);
            extUi->Plot21->graph(1)->addData(SimTime,SC[0].B[0].Trq[1]);
            extUi->Plot21->graph(2)->addData(SimTime,SC[0].B[0].Trq[2]); //SC[0].B[0].wn[2]
            extUi->Plot21->rescaleAxes();
            extUi->Plot21->replot();
        }

        if(true){
            extUi->Plot22->graph(0)->addData(SimTime,SC[0].B[0].wn[0]); //SC[0].VelR[0]
            extUi->Plot22->graph(1)->addData(SimTime,SC[0].B[0].wn[1]);
            extUi->Plot22->graph(2)->addData(SimTime,SC[0].B[0].wn[2]);
            extUi->Plot22->rescaleAxes();
            extUi->Plot22->replot();
        }

        if(true){
            double magPosN = MAGV(SC[0].PosN);
            extUi->Plot31->graph(0)->addData(SimTime,SC[0].B->Fuel);
            //extUi->Plot31->graph(1)->addData(SimTime,SC[0].PosN[1]);
            //extUi->Plot31->graph(2)->addData(SimTime,SC[0].PosN[2]);
            extUi->Plot31->rescaleAxes();
            extUi->Plot31->replot();
        }

        if(true){
            extUi->Plot32->graph(1)->addData(SimTime,SC[0].VelR[0]);
            extUi->Plot32->graph(2)->addData(SimTime,SC[0].VelEH[0]);
            extUi->Plot32->rescaleAxes();
            extUi->Plot32->replot();
        }
        QApplication::processEvents();
    }
}

void MainWindow::StartSimulation(){
    exec(argc, argv);
}

void MainWindow::setupPlots()
{
    //Обработка двойных нажатий на графики
    connect(ui->Plot11, &QCustomPlot::mouseDoubleClick,     this,       &MainWindow::doubleClickSlot);
    connect(ui->Plot12, &QCustomPlot::mouseDoubleClick,     this,       &MainWindow::doubleClickSlot);
    connect(ui->Plot13, &QCustomPlot::mouseDoubleClick,     this,       &MainWindow::doubleClickSlot);
    connect(ui->Plot21, &QCustomPlot::mouseDoubleClick,     this,       &MainWindow::doubleClickSlot);
    connect(ui->Plot31, &QCustomPlot::mouseDoubleClick,     this,       &MainWindow::doubleClickSlot);
    connect(ui->Plot22, &QCustomPlot::mouseDoubleClick,     this,       &MainWindow::doubleClickSlot);
    connect(ui->Plot32, &QCustomPlot::mouseDoubleClick,     this,       &MainWindow::doubleClickSlot);

    //Обработка одиночных нажатий на графики
    connect(ui->Plot11, &QCustomPlot::mousePress,           this,       &MainWindow::pressEventSlot);
    connect(ui->Plot12, &QCustomPlot::mousePress,           this,       &MainWindow::pressEventSlot);
    connect(ui->Plot13, &QCustomPlot::mousePress,           this,       &MainWindow::pressEventSlot);
    connect(ui->Plot21, &QCustomPlot::mousePress,           this,       &MainWindow::pressEventSlot);
    connect(ui->Plot31, &QCustomPlot::mousePress,           this,       &MainWindow::pressEventSlot);
    connect(ui->Plot22, &QCustomPlot::mousePress,           this,       &MainWindow::pressEventSlot);
    connect(ui->Plot32, &QCustomPlot::mousePress,           this,       &MainWindow::pressEventSlot);

    //Обработка отпускания кнопки мыши над графиками
    connect(ui->Plot11, &QCustomPlot::mouseRelease,         this,       &MainWindow::releaseEventSlot);
    connect(ui->Plot12, &QCustomPlot::mouseRelease,         this,       &MainWindow::releaseEventSlot);
    connect(ui->Plot13, &QCustomPlot::mouseRelease,         this,       &MainWindow::releaseEventSlot);
    connect(ui->Plot21, &QCustomPlot::mouseRelease,         this,       &MainWindow::releaseEventSlot);
    connect(ui->Plot31, &QCustomPlot::mouseRelease,         this,       &MainWindow::releaseEventSlot);
    connect(ui->Plot22, &QCustomPlot::mouseRelease,         this,       &MainWindow::releaseEventSlot);
    connect(ui->Plot32, &QCustomPlot::mouseRelease,         this,       &MainWindow::releaseEventSlot);
}

void MainWindow::doubleClickSlot(QMouseEvent *event)
{
    Q_UNUSED(event)
    QCustomPlot* plot = qobject_cast<QCustomPlot*>(sender());
    if(!plot) return;
    plot->xAxis->setRange(0, mSimTime);
//    plot->yAxis->setRange(plot->yAxis->range().lower, plot->yAxis->range().upper);
    plot->replot();
}

void MainWindow::pressEventSlot(QMouseEvent *event)
{
    QCustomPlot * plot = qobject_cast<QCustomPlot*>(sender());
    if(!plot) return;
    if(event->button() & Qt::LeftButton) {
        plot->setSelectionRectMode(QCP::srmZoom);
    }
    if(event->button() & Qt::RightButton) {
        plot->setSelectionRectMode(QCP::srmNone);
        this->setCursor(Qt::ClosedHandCursor);
    }
}

void MainWindow::releaseEventSlot(QMouseEvent *event)
{
    if(event->button() & Qt::RightButton) {
        this->setCursor(Qt::ArrowCursor);
    }
}

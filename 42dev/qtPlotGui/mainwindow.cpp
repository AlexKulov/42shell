#include "mainwindow.h"
#include "ui_mainwindow.h"

Ui::MainWindow * extUi;
double mSimTime;  //Для передачи параметра в методы класса из внешней функции CppToPlot

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("42shell");
    connect (ui->pushButtonSimulation,&QAbstractButton::clicked,this,&MainWindow::StartSimulation);
    setupPlots();

    ui->Plot11->addGraph();
    ui->Plot11->graph(0)->setPen(QColor(Qt::red));
    ui->Plot11->addGraph();
    ui->Plot11->graph(1)->setPen(QColor(Qt::green));
    ui->Plot11->addGraph();
    ui->Plot11->graph(2)->setPen(QColor(Qt::blue));

    ui->Plot12->addGraph();
    ui->Plot12->graph(0)->setPen(QColor(Qt::red));
    ui->Plot12->addGraph();
    ui->Plot12->graph(1)->setPen(QColor(Qt::green));
    ui->Plot12->addGraph();
    ui->Plot12->graph(2)->setPen(QColor(Qt::blue));

    ui->Plot13->addGraph();
    ui->Plot13->graph(0)->setPen(QColor(Qt::red));
    ui->Plot13->addGraph();
    ui->Plot13->graph(1)->setPen(QColor(Qt::green));
    ui->Plot13->addGraph();
    ui->Plot13->graph(2)->setPen(QColor(Qt::blue));
    ui->Plot13->addGraph();
    ui->Plot13->graph(3)->setPen(QColor(Qt::yellow));

    ui->Plot21->addGraph(); //plotKm
    ui->Plot21->graph(0)->setPen(QColor(Qt::red));
    ui->Plot21->addGraph(); //plotKm
    ui->Plot21->graph(1)->setPen(QColor(Qt::blue));
    ui->Plot22->addGraph();//plotWheelPower
    ui->Plot22->graph(0)->setPen(QColor(Qt::red));
    ui->Plot22->addGraph(); //plotKm
    ui->Plot22->graph(1)->setPen(QColor(Qt::blue));

    ui->Plot31->addGraph();//plotScMode
    ui->Plot31->graph(0)->setPen(QColor(Qt::red));
    ui->Plot32->addGraph();//plotWorkAndZRV
    ui->Plot32->graph(0)->setPen(QColor(Qt::red));

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
#include "SPSModel.h"
#include "PSModel.h"
#include "../42dev/serviceAlg.h"
extern int exec(int argc,char **argv);
extern long getSpsSoc(struct SCType *S, float * batSOC);

void ToPlot(double Time);
#ifdef __cplusplus
}
#endif

extern double modeAng[3];
void ToPlot(double SimTime){
    mSimTime = SimTime;
    static unsigned int PlotSampleCounter=0;
    static unsigned int PlotMaxCounter = 25;

    PlotSampleCounter++;
    if(PlotSampleCounter>PlotMaxCounter){
        PlotSampleCounter = 0;
        if(true){
            static double angLimit = 180;
            modeAng[0] = Limit(modeAng[0], -angLimit, angLimit);
            extUi->Plot11->graph(0)->addData(SimTime,modeAng[0]);
            modeAng[1] = Limit(modeAng[1], -angLimit, angLimit);
            extUi->Plot11->graph(1)->addData(SimTime,modeAng[1]);
            modeAng[2] = Limit(modeAng[2], -angLimit, angLimit);
            extUi->Plot11->graph(2)->addData(SimTime,modeAng[2]);
            extUi->Plot11->rescaleAxes();
            extUi->Plot11->replot();
        }

        if(true){
            extUi->Plot12->graph(0)->addData(SimTime,SC[0].Thr[0].PulseWidthCmd);
            //extUi->Plot12->graph(1)->addData(SimTime,SC[0].B[0].wn[1]);
            //extUi->Plot12->graph(2)->addData(SimTime,SC[0].B[0].wn[2]);
            extUi->Plot12->rescaleAxes();
            extUi->Plot12->replot();
        }

        if(true){
            if(false){
                if(SC[0].Nw > 0)
                    extUi->Plot13->graph(0)->addData(SimTime,SC[0].Whl[0].Trq);
                if(SC[0].Nw > 1)
                    extUi->Plot13->graph(1)->addData(SimTime,SC[0].Whl[1].Trq);
                if(SC[0].Nw > 2)
                    extUi->Plot13->graph(2)->addData(SimTime,SC[0].Whl[2].Trq);
                if(SC[0].Nw > 3)
                    extUi->Plot13->graph(3)->addData(SimTime,SC[0].Whl[3].Trq);
            }
            else if(true){
                if(SC[0].Nthr > 0)
                    extUi->Plot13->graph(0)->addData(SimTime,SC[0].Thr[0].F);
                if(SC[0].Nthr > 1)
                    extUi->Plot13->graph(1)->addData(SimTime,SC[0].Thr[1].F);
                if(SC[0].Nthr > 2)
                    extUi->Plot13->graph(2)->addData(SimTime,SC[0].Thr[2].F);
                if(SC[0].Nthr > 4)
                    extUi->Plot13->graph(3)->addData(SimTime,SC[0].Thr[4].F);
            }

            extUi->Plot13->rescaleAxes();
            extUi->Plot13->replot();
        }

        if(true){
            double posN0 = 0;
            double posN1 = 0;
            posN0 = MAGV(SC[0].PosN) - World[EARTH].rad;
            if(Nsc>1)
                posN1 = MAGV(SC[1].PosN) - World[EARTH].rad;
            extUi->Plot21->graph(0)->addData(SimTime, posN1/1000);
            extUi->Plot21->graph(1)->addData(SimTime, posN0/1000);
            extUi->Plot21->rescaleAxes();
            extUi->Plot21->replot();
        }

        if(true){
            double magVelN0 = 0;
            double magVelN1 = 0;
            magVelN0 = MAGV(SC[0].VelN)/1000;
            if(Nsc>1)
                magVelN1 = MAGV(SC[1].VelN)/1000;
            extUi->Plot22->graph(0)->addData(SimTime, magVelN1);
            extUi->Plot22->graph(1)->addData(SimTime, magVelN0);
            extUi->Plot22->rescaleAxes();
            extUi->Plot22->replot();
        }

        if(true){
            double fuelMassa = getThrsFuel(&SC[0],0);
            extUi->Plot31->graph(0)->addData(SimTime, fuelMassa);
            extUi->Plot31->rescaleAxes();
            extUi->Plot31->replot();
        }

        if(true){
            float batSOC[4];
            long nBatSoc = getSpsSoc(&SC[0], batSOC);
            if(nBatSoc>0)
                extUi->Plot32->graph(0)->addData(SimTime, batSOC[0]);
            else
                extUi->Plot32->graph(0)->addData(SimTime, 0);
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

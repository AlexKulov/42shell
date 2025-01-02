#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <QLineEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QMouseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
Q_OBJECT

QTimer timerPlotPaint;
//----------------------------------------------------------
public:
    int argc;
    char ** argv;
explicit MainWindow(QWidget *parent = nullptr);
~MainWindow();

public:
void StartSimulation();

private:
Ui::MainWindow *ui;

void setupPlots();

private slots:
void doubleClickSlot(QMouseEvent *event);
void pressEventSlot(QMouseEvent *event);
void releaseEventSlot(QMouseEvent *event);

private:
    unsigned int uiPlotSampleCounter;
    unsigned int uiPlotMaxCounter;
};

#endif // MAINWINDOW_H

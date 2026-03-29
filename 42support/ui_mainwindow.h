/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionsave;
    QAction *actionread;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_2;
    QCustomPlot *Plot12;
    QCustomPlot *Plot11;
    QCustomPlot *Plot13;
    QWidget *tab_spec;
    QGridLayout *gridLayout_8;
    QLabel *labelWheelPower;
    QLabel *labelKM;
    QCustomPlot *Plot21;
    QCustomPlot *Plot22;
    QWidget *tab_4;
    QGridLayout *gridLayout_5;
    QLabel *label_8;
    QLabel *label_9;
    QCustomPlot *Plot31;
    QCustomPlot *Plot32;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_4;
    QProgressBar *pbModeling;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButtonSimulation;
    QPushButton *pushButtonSO;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1023, 737);
        actionsave = new QAction(MainWindow);
        actionsave->setObjectName(QString::fromUtf8("actionsave"));
        actionread = new QAction(MainWindow);
        actionread->setObjectName(QString::fromUtf8("actionread"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setEnabled(true);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_2 = new QGridLayout(tab);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(tab);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        label_3 = new QLabel(tab);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 2, 0, 1, 1);

        label_2 = new QLabel(tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

        Plot12 = new QCustomPlot(tab);
        Plot12->setObjectName(QString::fromUtf8("Plot12"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(Plot12->sizePolicy().hasHeightForWidth());
        Plot12->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(Plot12, 1, 1, 1, 1);

        Plot11 = new QCustomPlot(tab);
        Plot11->setObjectName(QString::fromUtf8("Plot11"));
        sizePolicy2.setHeightForWidth(Plot11->sizePolicy().hasHeightForWidth());
        Plot11->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(Plot11, 0, 1, 1, 1);

        Plot13 = new QCustomPlot(tab);
        Plot13->setObjectName(QString::fromUtf8("Plot13"));
        sizePolicy2.setHeightForWidth(Plot13->sizePolicy().hasHeightForWidth());
        Plot13->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(Plot13, 2, 1, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_spec = new QWidget();
        tab_spec->setObjectName(QString::fromUtf8("tab_spec"));
        gridLayout_8 = new QGridLayout(tab_spec);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        labelWheelPower = new QLabel(tab_spec);
        labelWheelPower->setObjectName(QString::fromUtf8("labelWheelPower"));

        gridLayout_8->addWidget(labelWheelPower, 1, 0, 1, 1);

        labelKM = new QLabel(tab_spec);
        labelKM->setObjectName(QString::fromUtf8("labelKM"));
        sizePolicy1.setHeightForWidth(labelKM->sizePolicy().hasHeightForWidth());
        labelKM->setSizePolicy(sizePolicy1);

        gridLayout_8->addWidget(labelKM, 0, 0, 1, 1);

        Plot21 = new QCustomPlot(tab_spec);
        Plot21->setObjectName(QString::fromUtf8("Plot21"));
        sizePolicy2.setHeightForWidth(Plot21->sizePolicy().hasHeightForWidth());
        Plot21->setSizePolicy(sizePolicy2);

        gridLayout_8->addWidget(Plot21, 0, 3, 1, 1);

        Plot22 = new QCustomPlot(tab_spec);
        Plot22->setObjectName(QString::fromUtf8("Plot22"));

        gridLayout_8->addWidget(Plot22, 1, 3, 1, 1);

        tabWidget->addTab(tab_spec, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        gridLayout_5 = new QGridLayout(tab_4);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        label_8 = new QLabel(tab_4);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_5->addWidget(label_8, 0, 0, 1, 1);

        label_9 = new QLabel(tab_4);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_5->addWidget(label_9, 1, 0, 1, 1);

        Plot31 = new QCustomPlot(tab_4);
        Plot31->setObjectName(QString::fromUtf8("Plot31"));
        sizePolicy2.setHeightForWidth(Plot31->sizePolicy().hasHeightForWidth());
        Plot31->setSizePolicy(sizePolicy2);

        gridLayout_5->addWidget(Plot31, 0, 1, 1, 1);

        Plot32 = new QCustomPlot(tab_4);
        Plot32->setObjectName(QString::fromUtf8("Plot32"));
        sizePolicy2.setHeightForWidth(Plot32->sizePolicy().hasHeightForWidth());
        Plot32->setSizePolicy(sizePolicy2);

        gridLayout_5->addWidget(Plot32, 1, 1, 1, 1);

        tabWidget->addTab(tab_4, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 3, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setSizeConstraint(QLayout::SetMinimumSize);
        pbModeling = new QProgressBar(centralwidget);
        pbModeling->setObjectName(QString::fromUtf8("pbModeling"));
        pbModeling->setValue(0);

        verticalLayout_4->addWidget(pbModeling);


        horizontalLayout->addLayout(verticalLayout_4);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        pushButtonSimulation = new QPushButton(centralwidget);
        pushButtonSimulation->setObjectName(QString::fromUtf8("pushButtonSimulation"));

        verticalLayout->addWidget(pushButtonSimulation);

        pushButtonSO = new QPushButton(centralwidget);
        pushButtonSO->setObjectName(QString::fromUtf8("pushButtonSO"));
        pushButtonSO->setEnabled(true);

        verticalLayout->addWidget(pushButtonSO);


        horizontalLayout->addLayout(verticalLayout);


        gridLayout->addLayout(horizontalLayout, 3, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionsave->setText(QCoreApplication::translate("MainWindow", "save", nullptr));
        actionread->setText(QCoreApplication::translate("MainWindow", "read", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\204\320\270\320\272 11", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\204\320\270\320\272 13", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\204\320\270\320\272 12", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\204\320\270\320\272\320\270 1", nullptr));
        labelWheelPower->setText(QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\204\320\270\320\272 22", nullptr));
        labelKM->setText(QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\204\320\270\320\272 21", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_spec), QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\204\320\270\320\272\320\270 2", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\204\320\270\320\272 31", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\204\320\270\320\272 32", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QCoreApplication::translate("MainWindow", "\320\223\321\200\320\260\321\204\320\270\320\272\320\270 3", nullptr));
        pushButtonSimulation->setText(QCoreApplication::translate("MainWindow", "\320\241\321\202\320\260\321\200\321\202", nullptr));
        pushButtonSO->setText(QCoreApplication::translate("MainWindow", "\320\241\321\202\320\276\320\277", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

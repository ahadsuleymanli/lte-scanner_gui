#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"
#include "qcustomplot.h"
#include <boost/thread/scoped_thread.hpp>

#include <LTE-Tracker_adapter.h>
#include "lte_cell_info_container.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QThread* thread = new QThread;
    Worker* worker = new Worker(ui->customPlot);
    worker->moveToThread(thread);
    //connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    QCustomPlot* customPlot = ui->customPlot;
    customPlot->addGraph();
    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(-1, 1);
    customPlot->yAxis->setRange(0, 1);
    customPlot->replot();


    //thread->start();


}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::showEvent(QShowEvent *ev)
{
    QMainWindow::showEvent(ev);
    showEventHelper();
}

void  MainWindow::processOneThing(){

    qDebug() <<"twork?";
}

void MainWindow::showEventHelper()
{


       //creating a pointer customPlot to the one from ui

    // generate some data:
    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i]; // let's plot a quadratic function
    }
    // create graph and assign data to it:


/*


        customPlot->addGraph();
        customPlot->graph(0)->setData(x, y);
        // give the axes some labels:
        customPlot->xAxis->setLabel("x");
        customPlot->yAxis->setLabel("y");
        // set axes ranges, so we see all data:
        //customPlot->xAxis->setRange(-1, i);
        customPlot->yAxis->setRange(0, 1);
        customPlot->replot();

*/



}

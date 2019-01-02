#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"
#include "qcustomplot.h"
#include <boost/thread/scoped_thread.hpp>

#include <LTE-Tracker_adapter.h>
#include "lte_cell_info_container.h"
#include "mousereactiveqlabel.h"
#include "mousereactiveqwidget.h"

PlotDrawer *plotDrawer;

void PlotDrawer::drawPlot(){
    if (cellsListNode==nullptr)
        this->cellsListNode = ui->cellsList->getHead();

    if(true && cellsListNode!=nullptr){
        customPlot->yAxis->setLabel( QString::fromStdString(cellsListNode->cellID) );
        vector<double> vect;
        for (int i=0; i<cellsListNode->plot.size(); ++i)
        {
          vect.push_back((cellsListNode->plot)[i]);
        }

        QVector<double> y = QVector<double>::fromStdVector(vect);
        QVector<double> x;
        for (int i=0; i<y.size(); ++i)
        {
          x.push_back(i);
        }
        //customPlot->graph(0)->setData(x, y);
        customPlot->clearGraphs();

        QVector<double> amplitudes;
        QVector<double> keys;
        for( int i = 0 ; i<y.size() ; i++){
            customPlot->addGraph();
            QVector<double> amplitudes;
            amplitudes.push_back(y[i]);
            amplitudes.push_back(yMin);
            QVector<double> keys;
            keys.push_back(i);
            keys.push_back(i);
            customPlot->graph(i)->addData(keys,amplitudes);
        }
        customPlot->replot();
    }

}
void PlotDrawer::setNode(Node *cellsListNode){
    this->cellsListNode=cellsListNode;

}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //initiationg customPlot to the one in the UI:
    customPlot = ui->customPlot;
    plotDrawer = new PlotDrawer(this);



    //1 sec interval timer and it's thread
    QThread* thread = new QThread;
    QTimer *timer = new QTimer();
    timer->setInterval(1000);
    timer->moveToThread(thread);
    connect(thread, SIGNAL(started()), timer, SLOT(start()));
    connect(thread, SIGNAL(finished()), timer, SLOT(deleteLater()));
    connect(timer, SIGNAL(timeout()), this, SLOT(upDateCellsInfo()));

    //faster timer and it's thread
    QThread *plotTimerThread = new QThread;
    QTimer *plotTimer = new QTimer();
    plotTimer->setInterval(200);
    plotTimer->moveToThread(plotTimerThread);
    connect(plotTimerThread, SIGNAL(started()), plotTimer, SLOT(start()));
    connect(plotTimerThread, SIGNAL(finished()), plotTimer, SLOT(deleteLater()));
    connect(plotTimer, SIGNAL(timeout()), this, SLOT(upDatePlot()));

    scrollWidget = new QWidget;
    scroll = ui->scrollArea;
    scrollLayout = new QFormLayout(scrollWidget);
    scroll->setWidget(scrollWidget);

    thread->start();
    plotTimerThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCellsList(Cell_info_LL *cellsList){
    this->cellsList = cellsList;
}


void MainWindow::showEvent(QShowEvent *ev)
{
    QMainWindow::showEvent(ev);
    showEventHelper();
}

//for clearing QLayouts and whatever may lay in them. clears recursively.
void clearLayout(QLayout *layout){
    QLayoutItem *item;
    while( /*layout->count()>0 &&*/ (item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
            //item->widget()->deleteLater();
            delete item->widget();
        }
        delete item;
        //Error: free(): invalid next size (fast):
    }
}

//populates a Qlayout within the scroll area with labels displaying cell info.
void  MainWindow::upDateCellsInfo(){

    qDebug().nospace() << qPrintable( QString::fromStdString(cellsList->display()) );
    clearLayout(scrollLayout);
    Node *cellsListNode = cellsList->getHead();
    while( cellsListNode != nullptr ){
        MousereactiveQWidget *widget = new MousereactiveQWidget("");

        QFormLayout *PCILayout = new QFormLayout((QWidget*)widget);
        QString str = QString::fromStdString( "  PCI: " + cellsListNode->cellID);
//        MousereactiveQLabel *PCI_label = new MousereactiveQLabel(str, widget,"rgb(255, 100, 80)","grey");
        MousereactiveQLabel *PCI_label = new MousereactiveQLabel(str, widget,"rgb(255, 100, 80)","grey", cellsListNode, plotDrawer);
        PCI_label->setTextColor("blue");
        PCI_label->setMargin(1);
        PCILayout->addWidget(PCI_label);

        for(int i = 0 ; i < cellsListNode->port_list.size() ; i++){
            string portInfo = cellsListNode->port_list[i].portSNR_string;
            MousereactiveQLabel *port_label = new MousereactiveQLabel("   "+QString::fromStdString(
                                            portInfo), widget,"rgb(255, 000, 80)","grey");
            port_label->setMargin(1);
            PCILayout->addWidget(port_label);
        }
        scrollLayout->addWidget(widget);
        cellsListNode = cellsList->getNext(cellsListNode);
    }


}




void  MainWindow::upDatePlot(){
    plotDrawer->drawPlot();
}

//this runs as soon as the window is displayed
void MainWindow::showEventHelper()
{

}

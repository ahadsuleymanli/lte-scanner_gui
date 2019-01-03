#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"
#include "qcustomplot.h"
#include <boost/thread/scoped_thread.hpp>

#include <LTE-Tracker_adapter.h>
#include "lte_cell_info_container.h"
#include "mousereactiveqlabel.h"
#include "mousereactiveqwidget.h"

bool draw_usrp = true;
bool graphStyle = false;

PlotDrawer *plotDrawer;

void PlotDrawer::drawPlot(){
    customPlot->xAxis->setRange(0,xMax);
    customPlot->yAxis->setRange(yMin,yMax);
    if (cellsListNode==nullptr)
        this->cellsListNode = ui->cellsList->getHead();

    if(!draw_usrp && cellsListNode!=nullptr){
        yMax = -10;
        xMax = 72;
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


        customPlot->addGraph();
        for( int i = 0 ; i<y.size() ; i++){
            QVector<double> amplitudes;
            QVector<double> keys;
            amplitudes.push_back( yMin );
            keys.push_back(i);
            amplitudes.push_back( y[i] );
            keys.push_back(i);
            amplitudes.push_back( yMin );
            keys.push_back(i);
            customPlot->graph(0)->addData(keys,amplitudes);
        }
        customPlot->replot();
    }
    else if(draw_usrp){
        yMax = -80;
        vector<float> *vect = ui->lpdftVect;
        if((*vect).size())
        {
            customPlot->clearGraphs();

            if (this->xMax<vect->size()){
                this->xMax = vect->size();
            }
            if(graphStyle){

                customPlot->addGraph();
                customPlot->graph(0)->setLineStyle((QCPGraph::LineStyle)1);
                customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
                for( int i = 0 ; i<vect->size() ; i++){
                    double amplitude = (*vect)[i];
                    if((*vect)[i]>yMax){
                        yMax = amplitude + 5;
                    }
                    QVector<double> amplitudes;
                    QVector<double> keys;

                    amplitudes.push_back( yMin );
                    keys.push_back(i);
                    amplitudes.push_back( amplitude );
                    keys.push_back(i);
                    amplitudes.push_back( yMin );
                    keys.push_back(i);
                    customPlot->graph(0)->addData(keys,amplitudes);

                }

            }
            else{
                QVector<double> x, y;
                for (int j=0; j<vect->size(); ++j)
                {
                  //x.push_back( j/15.0 * 5*3.14 + 0.01);
                  //y.push_back(7*qSin(x[j])/x[j] - (i-QCPGraph::lsNone)*5 + (QCPGraph::lsImpulse)*5 + 2)  ;
                  x.push_back( j);
                  y.push_back((*vect)[j])  ;

                  if((*vect)[j]>yMax){
                      yMax = (*vect)[j] + 5;
                  }
                }

                customPlot->addGraph();
                customPlot->graph(0)->setData(x,y);
                customPlot->graph(0)->setLineStyle((QCPGraph::LineStyle)1);
                customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
            }
            customPlot->replot();
        }
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

    QPushButton *switchBtn = ui->switchGraph;
    QPushButton *graphStyleBtn = ui->graphStyleBtn;

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
    plotTimer->setInterval(60);
    plotTimer->moveToThread(plotTimerThread);
    connect(plotTimerThread, SIGNAL(started()), plotTimer, SLOT(start()));
    connect(plotTimerThread, SIGNAL(finished()), plotTimer, SLOT(deleteLater()));
    connect(plotTimer, SIGNAL(timeout()), this, SLOT(upDatePlot()));

    scrollWidget = new QWidget;
    scroll = ui->scrollArea;
    scrollLayout = new QFormLayout(scrollWidget);
    scroll->setWidget(scrollWidget);

    connect(switchBtn, SIGNAL (released()),this, SLOT (handleSwitchBtn()));
    connect(graphStyleBtn, SIGNAL (released()),this, SLOT (handleGraphStyle()));

    thread->start();
    plotTimerThread->start();
}

void  MainWindow::handleSwitchBtn(){
    draw_usrp = !draw_usrp;
}
void  MainWindow::handleGraphStyle(){
    graphStyle = !graphStyle;
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

    //qDebug().nospace() << qPrintable( QString::fromStdString(cellsList->display()) );
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

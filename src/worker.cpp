#include "worker.h"
#include <QString>
#include <QTimer>
#include "LTE-Tracker_adapter.h"


// --- CONSTRUCTOR ---
Worker::Worker(Cell_info_LL *cellsList, QCustomPlot *p) {
    this->p=p;
    m_value = 0;
    this->cellsList = cellsList;
    // you could copy data from constructor arguments to internal variables here.

}
void Worker::setCellsList(Cell_info_LL *cellsList){
    this->cellsList = cellsList;
}
// --- DECONSTRUCTOR ---
Worker::~Worker() {
    // free resources
}

// --- PROCESS ---
// Start processing data.
void Worker::initialize(Cell_info_LL *cellsList) {
    // allocate resources using new here
    qDebug("Hello World!");

    this->cellsList = cellsList;
    qDebug("Goodbye cruel World!");
    //emit finished();
}

void Worker::dostuff() {
    // allocate resources using new here

    qDebug()<<"what "<< m_value <<endl;
    //cellsList->print();
    m_value++;/*
    if (cellsList->getHead()->next!=NULL){
        //string str = cellsList->display();
        QString printStr ="";
        //printStr = QString::fromStdString("str" + cellsList->getHead()->cellID );
        //qDebug("" + printStr.toLatin1());
        qDebug().nospace() << qPrintable(printStr);
    }
*/
    //emit finished();
}






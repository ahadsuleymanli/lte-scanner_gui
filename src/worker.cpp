#include "worker.h"
#include <QString>
#include <QTimer>
#include "LTE-Tracker_adapter.h"


// --- CONSTRUCTOR ---
Worker::Worker(QCustomPlot *p) {
    this->p=p;
    // you could copy data from constructor arguments to internal variables here.
}

// --- DECONSTRUCTOR ---
Worker::~Worker() {
    // free resources
}

// --- PROCESS ---
// Start processing data.
void Worker::process() {
    // allocate resources using new here
    qDebug("Hello World!");
    double d = 0;
    while(1){
        d+=0.001;

        p->xAxis->setRange(-1, d);
        if (d>10000.0)
            break;
    }
    qDebug("Goodbye cruel World!");
    emit finished();
}





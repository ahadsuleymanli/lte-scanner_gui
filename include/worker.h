    #ifndef WORKER_H
    #define WORKER_H

    #include <QThread>
    #include <QMutex>
    #include "qcustomplot.h"
     
class Worker : public QObject {
    Q_OBJECT

public:
    Worker(QCustomPlot *p);
    ~Worker();

public slots:
    void process();

signals:
    void finished();
    void error(QString err);

private:
    // add your variables here
    QCustomPlot *p;
};

    #endif // WORKER_H

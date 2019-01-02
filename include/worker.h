    #ifndef WORKER_H
    #define WORKER_H

    #include "qcustomplot.h"
    #include "lte_cell_info_container.h"
     
class Worker : public QObject {
    Q_OBJECT

public:
    Worker(Cell_info_LL *cellsList, QCustomPlot *p);
    void setCellsList(Cell_info_LL * cellsList);
    ~Worker();

public slots:
    void initialize(Cell_info_LL *cellsList);
    void dostuff();

signals:
    void finished();
    void error(QString err);

private:
    // add your variables here
    QCustomPlot *p;
    int m_value;
    QString str;
    QTimer m_timer;
    Cell_info_LL *cellsList;
};

    #endif // WORKER_H

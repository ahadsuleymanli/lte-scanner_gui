#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "qcustomplot.h"
#include <QMainWindow>
#include "lte_cell_info_container.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void setCellsList(Cell_info_LL * cellsList);
    void setUsrpGetterParams(double *freq, bool *stopFlag, std::vector<float> *lpdftVect){
        this->freq=freq;
        this->stopFlag=stopFlag;
        this->lpdftVect = lpdftVect;
    }
    ~MainWindow();


public slots:
    void  upDateCellsInfo();
    void  handleSwitchBtn();
    void  handleGraphStyle();
    void  upDatePlot();

protected:
      void showEvent(QShowEvent *ev);

private:
    void showEventHelper();
    Ui::MainWindow *ui;

public:
    Cell_info_LL * cellsList;
    QWidget *scrollWidget;
    QScrollArea *scroll;
    QFormLayout *scrollLayout;
    QCustomPlot* customPlot;

    double *freq;
    bool *stopFlag;
    vector<float> *lpdftVect;

};

class PlotDrawer
{
    private:
    MainWindow * ui;
    QCustomPlot* customPlot;
    Node * cellsListNode = nullptr; //node where plot is stored
    int portNo;           //port whose plot to display

    int yMin = -140;
    int yMax = -90;
    int xMax = 72;

    public:

    PlotDrawer(MainWindow * ui){
        this->ui = ui;
        this->customPlot = ui->customPlot;


        // give the axes some labels:
        //customPlot->xAxis->setLabel("x");
        //customPlot->yAxis->setLabel("y");
        // set axes ranges, so we see all data:
        customPlot->xAxis->setRange(0, xMax);
        customPlot->yAxis->setRange(yMin, yMax);
        customPlot->xAxis->setVisible(false);
    }
    ~PlotDrawer();

    void drawPlot();
    void setNode(Node *cellsListNode);

};


#endif // MAINWINDOW_H

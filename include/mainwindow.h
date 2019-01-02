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
    ~MainWindow();


public slots:
    void  upDateCellsInfo();
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

};



#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "qcustomplot.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void  processOneThing();


protected:
      void showEvent(QShowEvent *ev);

private:
    void showEventHelper();
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H

#ifndef DYNAMICGUIELEMENTS_H
#define DYNAMICGUIELEMENTS_H
#include <QObject>
#include "mainwindow.h"

class DynamicGuiElements : public QObject {
    Q_OBJECT

public:
    DynamicGuiElements(MainWindow *ui){

    }
    void setCellsList();
    ~DynamicGuiElements();

public slots:
    void dostuff();

signals:
    void finished();
    void error(QString err);

private:
    MainWindow *ui;

};

#endif // DYNAMICGUIELEMENTS_H

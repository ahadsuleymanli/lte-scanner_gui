#ifndef PID_LABEL_H
#define PID_LABEL_H
#include <QLabel>
#include <QPushButton>
#include <mousereactiveqwidget.h>
#include "lte_cell_info_container.h"
#include "mainwindow.h"
/*
This Qlabel sets self to a specific color on mouseover. as well as the widget it is housed in.
*/

class MousereactiveQLabel : public QLabel
{
    Q_OBJECT
public:
    MousereactiveQLabel(QWidget* parent = nullptr) : QLabel(parent){ }
    MousereactiveQLabel(QString str) : QLabel(str){
    }
    MousereactiveQLabel(QString str, QWidget *widget, QString selfColor, QString widgetColor) : QLabel(str){
        this->widget = widget;
        this->reactiveQWidget = NULL;
        this->selfColor = selfColor;
        this->widgetColor = widgetColor;
        this->cellsListNode = nullptr;
        this->plotDrawer = nullptr;
    }
    MousereactiveQLabel(QString str, MousereactiveQWidget *reactiveQWidget, QString selfColor, QString widgetColor, Node *cellsListNode, PlotDrawer * plotDrawer ) : QLabel(str){
        this->widget = NULL;
        this->reactiveQWidget = reactiveQWidget;
        this->selfColor = selfColor;
        this->widgetColor = widgetColor;
        this->cellsListNode = cellsListNode;
        this->plotDrawer = plotDrawer;
    }
    MousereactiveQLabel(QString str, MousereactiveQWidget *reactiveQWidget, QString selfColor, QString widgetColor) : QLabel(str){
        this->widget = NULL;
        this->reactiveQWidget = reactiveQWidget;
        this->selfColor = selfColor;
        this->widgetColor = widgetColor;
        reactiveQWidget->setReactionColors(widgetColor);
        this->cellsListNode = nullptr;
        this->plotDrawer = nullptr;
    }
    void setTextColor(QString textColor){
        this->textColor = textColor;
        setStyleSheet("QLabel { color : " + textColor + "; }");
    }

signals:
    void clicked(); //not used

protected:

    void mousePressEvent(QMouseEvent* event){
        if(cellsListNode!=nullptr)
            plotDrawer->setNode(cellsListNode);
        //emit clicked();
    }

    void enterEvent(QEvent *ev) override
    {
        setStyleSheet("QLabel { background-color : " + selfColor + "; color : " + textColor + "; }");
        if(widget!=NULL)
            widget->setStyleSheet("QWidget { background-color : " + widgetColor + "; }");
/*
        if(reactiveQWidget!=NULL)
            reactiveQWidget->setStyleSheet("QWidget { background-color : " + widgetColor + "; }");
*/
    }

    void leaveEvent(QEvent *ev) override
    {
        setStyleSheet("QLabel { color : " + textColor + "; }");
        if(widget!=NULL)
            widget->setStyleSheet("QWidget { }");
        /*
        if(reactiveQWidget!=NULL)
            reactiveQWidget->setStyleSheet("QWidget { }");
        */
    }

private:
    QWidget *widget;
    MousereactiveQWidget *reactiveQWidget;
    QString selfColor,textColor, widgetColor;
    Node *cellsListNode;
    PlotDrawer *plotDrawer;
};
#endif // PID_LABEL_H

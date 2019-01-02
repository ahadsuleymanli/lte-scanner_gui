#ifndef PORTINFO_LABEL_H
#define PORTINFO_LABEL_H
#include <QLabel>

class PORTINFOLabel : public QLabel
{
    Q_OBJECT
public:
    PORTINFOLabel(QWidget* parent = nullptr) : QLabel(parent){ }
    PORTINFOLabel(QString str) : QLabel(str){
    }
    PORTINFOLabel(QString str, QWidget *PCIWidget) : QLabel(str){
        this->PCIWidget = PCIWidget;
    }

protected:
    void enterEvent(QEvent *ev) override
    {
        setStyleSheet("QLabel { background-color : blue; }");
        PCIWidget->setStyleSheet("QWidget { background-color : orange; }");
    }

    void leaveEvent(QEvent *ev) override
    {
        //setStyleSheet("QLabel { background-color : green; }");
        setStyleSheet("QLabel { }");
        PCIWidget->setStyleSheet("QWidget {  }");
    }
private:
    QWidget *PCIWidget;
};
#endif // PORTINFO_LABEL_H

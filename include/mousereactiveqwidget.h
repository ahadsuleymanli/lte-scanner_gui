#ifndef MOUSEREACTIVEQWIDGET_H
#define MOUSEREACTIVEQWIDGET_H
#include <QWidget>
#include <QStyleOption>
#include <QPainter>
class MousereactiveQWidget : public QWidget
{
    Q_OBJECT
public:
    MousereactiveQWidget(QWidget* parent = nullptr) : QWidget(parent){ }
    MousereactiveQWidget() : QWidget(){
    }
    MousereactiveQWidget( QString selfColor) : QWidget(){
        this->selfColor = selfColor;
    }
    void setReactionColors(QString selfColor){
        this->selfColor = selfColor;
    }


protected:
    void enterEvent(QEvent *ev) override
    {
        setStyleSheet("QWidget { background-color : " + selfColor + "; }");
    }

    void leaveEvent(QEvent *ev) override
    {
        setStyleSheet("QWidget { }");
    }
    void paintEvent(QPaintEvent* event)
    {
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
     QWidget::paintEvent(event);
    }

private:
    QString selfColor, widgetColor;
};
#endif // MOUSEREACTIVEQWIDGET_H

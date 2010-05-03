#ifndef WIDGETSEARCHMONITOR_H
#define WIDGETSEARCHMONITOR_H

#include <QMainWindow>

namespace Ui {
    class WidgetSearchMonitor;
}

class WidgetSearchMonitor : public QMainWindow {
    Q_OBJECT
public:
    WidgetSearchMonitor(QWidget *parent = 0);
    ~WidgetSearchMonitor();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::WidgetSearchMonitor *ui;
};

#endif // WIDGETSEARCHMONITOR_H

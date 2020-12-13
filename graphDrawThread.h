#ifndef FINAL_GRAPHDRAWTHREAD_H
#define FINAL_GRAPHDRAWTHREAD_H
#include <QChart>
#include <QChartView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QThread>
#include <QPointF>
#include "mainwindow.h"
#include <vector>
using namespace std;
QT_CHARTS_USE_NAMESPACE

class graphDrawThread: public QThread {
    Q_OBJECT
public:
    void run();
    graphDrawThread(class MainWindow* window,int period=3600);
private:
    class MainWindow *window;
    int period;
signals:
    void success();
};


#endif //FINAL_GRAPHDRAWTHREAD_H

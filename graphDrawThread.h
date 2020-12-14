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
#include "constant.h"

QT_CHARTS_USE_NAMESPACE

class graphDrawThread: public QThread {
    Q_OBJECT
public:
    void run();
    graphDrawThread(class MainWindow* window,int period=3600, int function=0);
private:
    class MainWindow *window;
    int period;
    int function;
    void spatial_temporal();
    void fee_distribution();
    void traveltime_distribution();
signals:
    void success(graphDrawThread *thread);
    void function_fail(graphDrawThread *thread, int function);

};


#endif //FINAL_GRAPHDRAWTHREAD_H

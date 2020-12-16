//
// Created by MacBook Pro on 12/14/20.
//

#ifndef FINAL_PREDICTIONTHREAD_H
#define FINAL_PREDICTIONTHREAD_H

#include <QThread>
#include <QSqlDatabase>
#include "mainwindow.h"

class predictionThread:public QThread {
    Q_OBJECT
public:
    predictionThread(MainWindow *window):window(window){}
    void run();

private:
    MainWindow *window;
    QSqlDatabase db;
    void predictTravelTime();
    void predictDestination();
signals:
    void success(predictionThread *thread);
    void fail(predictionThread *thread, int function);
};


#endif //FINAL_PREDICTIONTHREAD_H

//
// Created by MacBook Pro on 12/14/20.
//

#ifndef FINAL_FREEQUERYTHREAD_H
#define FINAL_FREEQUERYTHREAD_H
#include <QThread>
#include <QString>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QWidgetItem>
#include "mainwindow.h"

class freeQueryThread: public QThread {
    Q_OBJECT
public:
    freeQueryThread(class MainWindow *window):window(window){}

private:
    void run();
    class MainWindow *window;
    bool parse(QString querystr,QStringList &header);
signals:
    void success(freeQueryThread *thread);
    void fail(freeQueryThread *thread);
};


#endif //FINAL_FREEQUERYTHREAD_H

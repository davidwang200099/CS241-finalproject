#ifndef FILEREADTHREAD_H
#define FILEREADTHREAD_H
#include <QThread>
#include <QTextStream>
#include <QFile>
#include <QSemaphore>
#include <QDebug>
#include "structures.h"
#include <cstdlib>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include "mainwindow.h"
#define NUMBER QString::number
class fileReadThread : public QThread{
    Q_OBJECT
public:
    fileReadThread(const QString date, class MainWindow *window,
            QSemaphore *mutex_db, int threadRank);
    /*~fileReadThread(){qDebug()<<"Destructor of fileReadThread "<<this<<" called!";}*/
    QString date;
private:
    void run();
    QString datasetFolder;
    QSemaphore *mutex_db;
    int threadRank;
    MainWindow *window;
signals:
    void success(fileReadThread *thread);
    void suc(fileReadThread *thread, class MainWindow *window);
    void fail(fileReadThread *thread);
};

#endif // FILEREADTHREAD_H

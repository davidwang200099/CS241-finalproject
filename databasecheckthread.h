#ifndef FINAL_DATABASECHECKTHREAD_H
#define FINAL_DATABASECHECKTHREAD_H
#include <QThread>
#include <QSemaphore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#define NUMBER QString::number
class databaseCheckThread:public QThread{
    Q_OBJECT
public:
    databaseCheckThread(QSemaphore *mutex_cntfinish, QSemaphore *mutex_db, QSqlDatabase *db,QStringList *import_dates, int *cnt_finish):
                       mutex_cntfinish(mutex_cntfinish),mutex_db(mutex_db),import_dates(import_dates),cnt_finish(cnt_finish),db(db){}
    /*~databaseCheckThread(){qDebug()<<"Destructor of databaseCheckThread called!";}*/
private:
    void run();

    QSemaphore *mutex_cntfinish;
    QSemaphore *mutex_db;
    QSqlDatabase *db;
    int *cnt_finish;
    QStringList *import_dates;
signals:
    void success(databaseCheckThread *thread);
    void fail(databaseCheckThread *thread);
};


#endif //FINAL_DATABASECHECKTHREAD_H

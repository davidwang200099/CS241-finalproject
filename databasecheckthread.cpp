#include "databasecheckthread.h"

void databaseCheckThread::run() {
    mutex_cntfinish->acquire();
    qDebug()<<cnt_finish<<" "<<import_dates->size();
    if(*cnt_finish!=import_dates->size()) {mutex_cntfinish->release();qDebug()<<"Load not finished";return;}
    mutex_cntfinish->release();
    qDebug()<<"Check!";
    db->open();
    QSqlQuery query(*db);

    mutex_db->acquire();
    qint64 num_rec=0;
    for(QString &dates:*import_dates){
        query.exec(QString("SELECT * FROM order_%1").arg(dates));
        while(query.next()) num_rec++;
        qDebug()<<QString("Table order_%1 has %2 records").arg(dates,NUMBER(num_rec));
        num_rec=0;
        query.clear();
    }
    mutex_db->release();
    qDebug()<<"Check finished!";
    emit success(this);
}

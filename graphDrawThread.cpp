
#include "graphDrawThread.h"
#include <QLineSeries>
#include <QValueAxis>
graphDrawThread::graphDrawThread(class MainWindow *window, int period) :window(window), period(period){}

int getPeriod(int target, QDateTime start, int step){
    int from=start.toTime_t();
    return (target-from)/step;
}

void graphDrawThread::run() {
    QSqlDatabase db=window->db;
    window->order_by_period.clear();
    /*if(QSqlDatabase::contains("graphdrawthread")){
        db=QSqlDatabase::database("graphdrawthread");
        qDebug()<<"graphDrawThread established a connection";
    }else{
        db=QSqlDatabase::addDatabase("QSQLITE","graphdrawthread");
        db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
        db.setDatabaseName("file::memory:");
        qDebug()<<"graphDrawThread connected.";
    }*/
    db.open();
    QSqlQuery query(db);

    QDate startdate=QDate::fromString(window->from_date,"yyyyMMdd");
    QDate enddate=QDate::fromString(window->to_date,"yyyyMMdd");

    QDateTime startdatetime=QDateTime::fromString(window->from_date+"000000","yyyyMMddhhmmss");
    QDateTime enddatetime=QDateTime::fromString(window->to_date+"235959","yyyyMMddhhmmss");
    int start=startdatetime.toTime_t();
    int end=enddatetime.toTime_t();

    int ssize;

    for(int i=0;i<(ssize=(end-start)/period+1);i++) window->order_by_period.push_back(0);

    qDebug()<<(window->order_by_period.size()==ssize);
    //window->order_by_period=QVector<int>(ssize=,0);
    qDebug()<<"Loop begin!";
    for(auto i=startdate;i<=enddate;i=i.addDays(1)){
        QString select_query=QString("SELECT departure_time FROM order_%1 ORDER BY departure_time").arg(i.toString("yyyyMMdd"));
        qDebug()<<select_query;
        auto r=query.exec(select_query);
        qDebug()<<r;
        if(!r) qDebug()<<query.lastError();
        while(query.next()) {
            int unix_stamp=query.value(0).toInt();
            //qDebug()<<unix_stamp;
            int rank=getPeriod(unix_stamp,startdatetime,period);
            if(rank>=ssize) {qDebug()<<"out of range! "<<rank<<">"<<ssize;return;}
            window->order_by_period[rank]++;
        }
    }
    qDebug()<<"Finished!";
    //window->view=new QChartView(window);
    db.close();
    //for(int i=0;i<order_by_period.size();i++) qDebug()<<order_by_period[i];
    emit success();
}
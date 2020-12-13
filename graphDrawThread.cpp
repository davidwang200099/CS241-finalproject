
#include "graphDrawThread.h"
#include <QValueAxis>

#define DATE QDate::fromString
#define DT QDateTime::fromString


graphDrawThread::graphDrawThread(class MainWindow *window, int period) :window(window), period(period){}

int getPeriod(int target, QDateTime start, int step){
    int from=start.toTime_t();
    return (target-from)/step;
}

void graphDrawThread::run() {
    QSqlDatabase db=window->db;
    window->order_by_period.clear();
    db.open();
    QSqlQuery query(db);

    //QDate startdate=QDate::fromString(window->from_date,"yyyyMMdd");
    //QDate enddate=QDate::fromString(window->to_date,"yyyyMMdd");

    //QDateTime startdatetime=QDateTime::fromString(window->from_date+"000000","yyyyMMddhhmmss");
    //QDateTime enddatetime=QDateTime::fromString(window->to_date+"235959","yyyyMMddhhmmss");
    QDate startdate=DATE(window->boxes[0]->currentText(),"yyyy-MM-dd");
    QDate enddate=DATE(window->boxes[2]->currentText(),"yyyy-MM-dd");

    QDateTime startdatetime=DT(window->boxes[0]->currentText()+"-"+window->boxes[1]->currentText()+":00","yyyy-MM-dd-hh:mm:ss");
    QDateTime enddatetime=DT(window->boxes[2]->currentText()+"-"+window->boxes[3]->currentText()+":00","yyyy-MM-dd-hh:mm:ss");

    qDebug()<<startdatetime;
    qDebug()<<enddatetime;

    int start=startdatetime.toTime_t();
    int end=enddatetime.toTime_t();

    int ssize;

    for(int i=0;i<(ssize=(end-start)/period+1);i++) window->order_by_period.push_back(0);

    qDebug()<<(window->order_by_period.size()==ssize);
    //window->order_by_period=QVector<int>(ssize=,0);
    qDebug()<<"Loop begin!";
    if(window->fields_onehot[3]&&window->fields_onehot[4]){
        QPointF lefttop=window->boxes[]
        for(auto i=startdate;i<=enddate;i=i.addDays(1)){
            QString select_query= QString("SELECT departure_time FROM order_%1 ORDER BY departure_time"
                                          "WHERE orig_lng >= %2 and orig_lng < %3 and orig_lat >= %4 and orig_lat < %5 ").arg(
                                                  i.toString("yyyyMMdd"),
                                                  )
        }
    }
    else {
        for (auto i = startdate; i <= enddate; i = i.addDays(1)) {
            QString select_query = QString("SELECT departure_time FROM order_%1 ORDER BY departure_time").arg(
                    i.toString("yyyyMMdd"));
            qDebug() << select_query;
            auto r = query.exec(select_query);
            qDebug() << r;
            if (!r) qDebug() << query.lastError();
            while (query.next()) {
                int unix_stamp = query.value(0).toInt();
                //qDebug()<<unix_stamp;
                int rank = getPeriod(unix_stamp, startdatetime, period);
                if (rank >= ssize) continue;
                window->order_by_period[rank]++;
            }
        }
    }
    qDebug()<<"Finished!";
    //window->view=new QChartView(window);
    db.close();
    //for(int i=0;i<order_by_period.size();i++) qDebug()<<order_by_period[i];
    emit success();
}
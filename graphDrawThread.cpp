
#include "graphDrawThread.h"
#include <QValueAxis>




graphDrawThread::graphDrawThread(class MainWindow *window, int period, int function) :window(window), period(period), function(function){}

int getPeriod(int target, QDateTime start, int step){
    int from=start.toTime_t();
    return (target-from)/step;
}

void graphDrawThread::spatial_temporal() {
    QSqlDatabase db=window->db;
    window->order_by_period.clear();
    db.open();
    QSqlQuery query(db);
    QDate startdate=DATE(window->boxes[0]->currentText(),"yyyy-MM-dd");
    QDate enddate=DATE(window->boxes[2]->currentText(),"yyyy-MM-dd");

    QDateTime startdatetime=DT(window->boxes[0]->currentText()+"-"+window->boxes[1]->currentText()+":00","yyyy-MM-dd-hh:mm:ss");
    QDateTime enddatetime=DT(window->boxes[2]->currentText()+"-"+window->boxes[3]->currentText()+":00","yyyy-MM-dd-hh:mm:ss");

    qDebug()<<startdatetime;
    qDebug()<<enddatetime;

    int start=startdatetime.toTime_t();
    int end=enddatetime.toTime_t();

    int ssize;
    qDebug()<<"In graphDrawThread, period="<<period;
    for(int i=0;i<(ssize=(end-start)/period+1);i++) window->order_by_period.push_back(0);

    qDebug()<<(window->order_by_period.size()==ssize);
    //window->order_by_period=QVector<int>(ssize=,0);
    qDebug()<<"Loop begin!";

    auto i1=window->boxes[LONG]->currentIndex()-1;
    auto i2=window->boxes[LAT]->currentIndex()-1;

    if(window->fields_onehot[3]&&window->fields_onehot[4]&&i1>=0&&i2>=0){
        auto gridrank=i1*i2;
        qDebug()<<gridrank;
        QPointF lefttop=window->grids[gridrank].first;
        QPointF rightbottom=window->grids[gridrank].second;
        auto rst=0;
        for(auto i=startdate;i<=enddate;i=i.addDays(1)){
            QString select_query= QString("SELECT departure_time FROM order_%1 "
                                          "WHERE orig_lng >= %2 and orig_lng < %3"
                                          " and orig_lat >= %4 and orig_lat < %5 ").arg(
                    i.toString("yyyyMMdd"),
                    NUMBER(lefttop.x()),NUMBER(rightbottom.x()),
                    NUMBER(rightbottom.y()),NUMBER(lefttop.y())
            );
            qDebug()<<select_query;
            auto r=query.exec(select_query);
            qDebug()<<r;
            if(!r) qDebug()<<query.lastError();
            while (query.next()) {
                int unix_stamp = query.value(0).toInt();
                rst++;
                //qDebug()<<unix_stamp;
                int rank = getPeriod(unix_stamp, startdatetime, period);
                if (rank >= ssize|| rank<0) continue;
                window->order_by_period[rank]++;
            }
            qDebug()<<rst;
        }
    }
    else {
        for (auto i = startdate; i <= enddate; i = i.addDays(1)) {
            QString select_query = QString("SELECT departure_time FROM order_%1 ").arg(
                    i.toString("yyyyMMdd"));
            qDebug() << select_query;
            auto r = query.exec(select_query);
            qDebug() << r;
            if (!r) qDebug() << query.lastError();
            while (query.next()) {
                int unix_stamp = query.value(0).toInt();
                //qDebug()<<unix_stamp;
                int rank = getPeriod(unix_stamp, startdatetime, period);
                if (rank >= ssize || rank<0) continue;
                window->order_by_period[rank]++;
            }
        }
    }
    qDebug()<<"Finished!";

    db.close();
    emit success(this);
}

void graphDrawThread::fee_distribution() {
    if(!window->fields_onehot[7]) {emit function_fail(this, function);return;}
    QSqlDatabase db=window->db;
    window->order_by_period.clear();
    db.open();
    QSqlQuery query(db);
    QDate startdate=DATE(window->boxes[0]->currentText(),"yyyy-MM-dd");
    QDate enddate=DATE(window->boxes[2]->currentText(),"yyyy-MM-dd");

    QDateTime startdatetime=DT(window->boxes[0]->currentText()+"-"+window->boxes[1]->currentText()+":00","yyyy-MM-dd-hh:mm:ss");
    QDateTime enddatetime=DT(window->boxes[2]->currentText()+"-"+window->boxes[3]->currentText()+":00","yyyy-MM-dd-hh:mm:ss");

    window->order_by_fee.clear();

    int ssize=35;
    for(int i=0;i<ssize;i++) window->order_by_fee.push_back(0);

    auto i1=window->boxes[LONG]->currentIndex()-1;
    auto i2=window->boxes[LAT]->currentIndex()-1;
    if(window->fields_onehot[3]&&window->fields_onehot[4]&&i1>=0&&i2>=0){
        auto gridrank=i1*i2;
        qDebug()<<gridrank;
        QPointF lefttop=window->grids[gridrank].first;
        QPointF rightbottom=window->grids[gridrank].second;
        auto rst=0;
        for(auto i=startdate;i<=enddate;i=i.addDays(1)){
            QString select_query= QString("SELECT fee FROM order_%1 "
                                          "WHERE orig_lng >= %2 and orig_lng < %3"
                                          " and orig_lat >= %4 and orig_lat < %5"
                                          " and departure_time >= %6 and departure_time < %7 ").arg(
                    i.toString("yyyyMMdd"),
                    NUMBER(lefttop.x()),NUMBER(rightbottom.x()),
                    NUMBER(rightbottom.y()),NUMBER(lefttop.y()),
                    NUMBER(startdatetime.toTime_t()),NUMBER(enddatetime.toTime_t())
            );
            qDebug()<<select_query;
            auto r=query.exec(select_query);
            qDebug()<<r;
            if(!r) qDebug()<<query.lastError();
            while (query.next()) {
                rst++;
                //qDebug()<<unix_stamp;
                int rank = int(query.value(0).toFloat());
                if (rank >= ssize|| rank<0) continue;
                window->order_by_fee[rank]++;
            }
            qDebug()<<rst;
        }
    }else{
        for (auto i = startdate; i <= enddate; i = i.addDays(1)) {
            QString select_query = QString("SELECT fee FROM order_%1 "
                                               "WHERE departure_time >= %2 and departure_time < %3").arg(
                    i.toString("yyyyMMdd"), NUMBER(startdatetime.toTime_t()), NUMBER(enddatetime.toTime_t()));
            qDebug() << select_query;
            auto r = query.exec(select_query);
            qDebug() << r;
            if (!r) qDebug() << query.lastError();
            while (query.next()) {
                //qDebug()<<unix_stamp;
                int rank = int(query.value(0).toFloat());
                if (rank >= ssize || rank<0) continue;
                window->order_by_fee[rank]++;
            }
        }
    }
    emit success(this);
}

void graphDrawThread::traveltime_distribution() {
    if(!window->fields_onehot[2]||!window->fields_onehot[1]) {emit function_fail(this, function);return;}
    QSqlDatabase db=window->db;
    window->order_by_traveltime.clear();
    db.open();
    QSqlQuery query(db);
    QDate startdate=DATE(window->boxes[0]->currentText(),"yyyy-MM-dd");
    QDate enddate=DATE(window->boxes[2]->currentText(),"yyyy-MM-dd");

    QDateTime startdatetime=DT(window->boxes[0]->currentText()+"-"+window->boxes[1]->currentText()+":00","yyyy-MM-dd-hh:mm:ss");
    QDateTime enddatetime=DT(window->boxes[2]->currentText()+"-"+window->boxes[3]->currentText()+":00","yyyy-MM-dd-hh:mm:ss");

    int ssize=36;
    for(int i=0;i<ssize;i++) window->order_by_traveltime.push_back(0);

    auto i1=window->boxes[LONG]->currentIndex()-1;
    auto i2=window->boxes[LAT]->currentIndex()-1;
    if(window->fields_onehot[3]&&window->fields_onehot[4]&&i1>=0&&i2>=0){
        auto gridrank=i1*i2;
        qDebug()<<gridrank;
        QPointF lefttop=window->grids[gridrank].first;
        QPointF rightbottom=window->grids[gridrank].second;
        auto rst=0;
        for(auto i=startdate;i<=enddate;i=i.addDays(1)){
            QString select_query= QString("SELECT departure_time,end_time FROM order_%1 "
                                          "WHERE orig_lng >= %2 and orig_lng < %3"
                                          " and orig_lat >= %4 and orig_lat < %5"
                                          " and departure_time >= %6 and departure_time < %7 ").arg(
                    i.toString("yyyyMMdd"),
                    NUMBER(lefttop.x()),NUMBER(rightbottom.x()),
                    NUMBER(rightbottom.y()),NUMBER(lefttop.y()),
                    NUMBER(startdatetime.toTime_t()),NUMBER(enddatetime.toTime_t())
            );
            qDebug()<<select_query;
            auto r=query.exec(select_query);
            qDebug()<<r;
            if(!r) qDebug()<<query.lastError();
            while (query.next()) {
                rst++;
                //qDebug()<<unix_stamp;
                int rank = (query.value(1).toInt()-query.value(0).toInt())/300;
                if (rank >= ssize|| rank<0) continue;
                window->order_by_traveltime[rank]++;
            }
            qDebug()<<rst;
        }
    }else{
        for (auto i = startdate; i <= enddate; i = i.addDays(1)) {
            QString select_query = QString("SELECT departure_time,end_time FROM order_%1 "
                                           "WHERE departure_time >= %2 and departure_time < %3").arg(
                    i.toString("yyyyMMdd"), NUMBER(startdatetime.toTime_t()), NUMBER(enddatetime.toTime_t()));
            qDebug() << select_query;
            auto r = query.exec(select_query);
            qDebug() << r;
            if (!r) qDebug() << query.lastError();
            while (query.next()) {
                //qDebug()<<unix_stamp;
                int rank = (query.value(1).toInt()-query.value(0).toInt())/300;
                if (rank >= ssize || rank<0) continue;
                window->order_by_traveltime[rank]++;
            }
        }
    }
    emit success(this);
}

void graphDrawThread::run() {
    switch(function){
        case 0:
            qDebug()<<"0!";
            spatial_temporal();
            break;
        case 1:
            traveltime_distribution();
        case 2:
            fee_distribution();
            break;
        default:
            spatial_temporal();
    }
}
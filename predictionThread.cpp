//
// Created by MacBook Pro on 12/14/20.
//

#include "predictionThread.h"
typedef struct{
    int gridrank;
    int num_order;
} order_t;

bool operator<(const order_t &order1, const order_t &order2){
    return order1.num_order<order2.num_order;
}

void predictionThread::run() {

    db=this->window->db;
    window->mutex_db.acquire();
    qDebug()<<"Begin!";
    //QSqlQuery query(db);
    db.open();

    switch(window->predictboxes[4]->currentIndex()){
        case 0:
            predictDestination();
            break;
        case 1:
            predictTravelTime();
            break;
        default:
            emit fail(this);
    }
    window->mutex_db.release();
}

pair<int, int> getGridID(float north, float south, float west, float east, float lng, float lat){
    int col=9-(lng-west)/((east-west)/10);
    int row=9-(lat-south)/((north-south)/10);
    if(col<0) col=0;
    if(col>9) col=9;
    if(row<0) row=0;
    if(row>9) row=9;
    return pair<int,int>(row,col);
}

void predictionThread::predictDestination() {
    //window->predictedit->clear();
    QSqlQuery query(db);
    db.open();
    //QTime time=QTime::fromString(window->timeedit->text()+":00","hh:mm:ss");
    QString from_date=window->from_date;
    QString to_date=window->to_date;
    QString time_point=window->timeedit->text()+":00";

    int row=window->predictboxes[1]->currentIndex();
    int col=window->predictboxes[0]->currentIndex();

    qDebug()<<row;
    qDebug()<<col;
    QPointF topleft=window->grids[row*10+col].first;
    QPointF bottomright=window->grids[row*10+col].second;



    QDate fromdate=QDate::fromString(from_date,"yyyyMMdd");
    QDate todate=QDate::fromString(to_date,"yyyyMMdd");
    qDebug()<<fromdate;
    qDebug()<<todate;

    QVector<order_t> orders(100);
    for(int i=0;i<100;i++) {orders[i].gridrank=i;orders[i].num_order=0;}

    pair<int, int> coordinate;
    int cnt=0;
    for(auto i=fromdate;i<=todate;i=i.addDays(1)){
        auto datestring=i.toString("yyyyMMdd");
        qDebug()<<datestring;
        QDateTime timepoint=QDateTime::fromString(datestring+"-"+time_point,"yyyyMMdd-hh:mm:ss");
        auto lowerbound=timepoint.toTime_t()-1800;
        auto upperbound=timepoint.toTime_t()+1800;
        QString querystr=QString("SELECT dest_lng,dest_lat FROM order_%1 WHERE "
                                 "departure_time>=%2 and departure_time<=%3 "
                                 "and orig_lng >= %4 and orig_lng <%5 "
                                 "and orig_lat >= %6 and orig_lat <%7 ").arg(
                datestring,NUMBER(lowerbound),NUMBER(upperbound),
                NUMBER(topleft.x()),NUMBER(bottomright.x()),
                NUMBER(bottomright.y()),NUMBER(topleft.y())
        );
        qDebug()<<querystr;
        auto r=query.exec(querystr);
        if(!r) qDebug()<<query.lastError();
        while(query.next()){
            cnt++;
            coordinate=getGridID(window->grid_north,window->grid_south,window->grid_west,window->grid_east,
                    query.value(0).toFloat(),query.value(1).toFloat());
            qDebug()<<coordinate;

            orders[coordinate.first*10+coordinate.second].num_order++;
        }
        //emit success(this);
        //return;
    }
    qDebug()<<cnt;
    sort(orders.begin(),orders.end());
    window->predictedit->append(QString("From Grid No.%1(%2 °E ~ %3 °E, %4 °N ~ %5 °N), the most popular 5 destinations are:\n").arg(
            NUMBER(row+col*10),
                    NUMBER(topleft.x()),NUMBER(bottomright.x()),NUMBER(bottomright.y()),NUMBER(topleft.y())));

    for(int i=0;i<5;i++){
        QPointF topleft=window->grids[orders[orders.size()-1-i].gridrank].first;
        QPointF bottomright=window->grids[orders[orders.size()-1-i].gridrank].second;
        window->predictedit->append(QString("Grid No.%1(%2 °E ~ %3 °E, %4 °N ~ %5 °N) : %6 orders").arg(
                NUMBER(orders[orders.size()-1-i].gridrank),
                NUMBER(topleft.x()),NUMBER(bottomright.x()),NUMBER(bottomright.y()),NUMBER(topleft.y()),
                NUMBER(orders[orders.size()-1-i].num_order)
                ));
    }
    emit success(this);
}


void predictionThread::predictTravelTime() {
    QSqlQuery query(db);
    db.open();
    //QTime time=QTime::fromString(window->timeedit->text()+":00","hh:mm:ss");
    QString from_date=window->from_date;
    QString to_date=window->to_date;
    QString time_point=window->timeedit->text()+":00";

    int fromrow=window->predictboxes[1]->currentIndex();
    int fromcol=window->predictboxes[0]->currentIndex();

    int torow=window->predictboxes[3]->currentIndex();
    int tocol=window->predictboxes[2]->currentIndex();

    qDebug()<<fromrow;
    qDebug()<<fromcol;
    qDebug()<<torow;
    qDebug()<<tocol;

    QPointF fromtopleft=window->grids[fromrow*10+fromcol].first;
    QPointF frombottomright=window->grids[fromrow*10+fromcol].second;
    QPointF totopleft=window->grids[torow*10+tocol].first;
    QPointF tobottomright=window->grids[torow*10+tocol].second;

    QDate fromdate=QDate::fromString(from_date,"yyyyMMdd");
    QDate todate=QDate::fromString(to_date,"yyyyMMdd");
    qDebug()<<fromdate;
    qDebug()<<todate;

    QVector<int> periods;

    for(auto i=fromdate;i<=todate;i=i.addDays(1)){
        auto datestring=i.toString("yyyyMMdd");
        qDebug()<<datestring;
        QDateTime timepoint=QDateTime::fromString(datestring+"-"+time_point,"yyyyMMdd-hh:mm:ss");
        auto lowerbound=timepoint.toTime_t()-1800;
        auto upperbound=timepoint.toTime_t()+1800;
        QString querystr=QString("SELECT departure_time,end_time FROM order_%1 WHERE departure_time>=%2 and departure_time<=%3 ").arg(
                datestring,NUMBER(lowerbound),NUMBER(upperbound)
                )+
                QString(
                        "and orig_lng >= %1 and orig_lng <%2 "
                        "and orig_lat >= %3 and orig_lat <%4 "
                        "and dest_lng >= %5 and dest_lng <%6 "
                        "and dest_lat >= %7 and dest_lat <%8 ").arg(
                NUMBER(fromtopleft.x()),NUMBER(frombottomright.x()),
                NUMBER(frombottomright.y()),NUMBER(fromtopleft.y()),
                NUMBER(totopleft.x()),NUMBER(tobottomright.x()),
                NUMBER(tobottomright.y()),NUMBER(totopleft.y())
        );
        qDebug()<<querystr;
        auto r=query.exec(querystr);
        if(!r) qDebug()<<query.lastError();

        while(query.next()){
            int t=query.value(1).toInt()-query.value(0).toInt();
            periods.push_back(t);
        }

    }
    sort(periods.begin(),periods.end());
    QString ans=QString("\nFrom Grid No.%1(%2 °E ~ %3 °E, %4 °N ~ %5 °N) ").arg(
            NUMBER(fromrow*10+fromcol),
            NUMBER(fromtopleft.x()),NUMBER(frombottomright.x()),NUMBER(frombottomright.y()),NUMBER(fromtopleft.y())
    )+QString("to Grid No.%1(%2 °E ~ %3 °E, %4 °N ~ %5 °N)").arg(
            NUMBER(torow*10+tocol),
            NUMBER(totopleft.x()),NUMBER(tobottomright.x()),NUMBER(tobottomright.y()),NUMBER(totopleft.y())
    );
    window->predictedit->append(ans);
    window->predictedit->append(QString("Number of records: %1\n").arg(periods.size()));
    qDebug()<<periods.size();
    if(periods.size()==0) {emit success(this);return;}

    if(periods[periods.size()-1]<=60)
        window->predictedit->append(QString("Maximum: %1 s.\n").arg(NUMBER(periods[periods.size()-1])));
    else{
        int seconds=periods[periods.size()-1];
        if(seconds<=3600)
            window->predictedit->append(QString("Maximum: %1 min, %2 s.\n").arg(
                    NUMBER(seconds/60),NUMBER(seconds-60*(seconds/60))));
        else{
            window->predictedit->append(QString("Maximum: %1 h, %2 min, %3 s.\n").arg(
                    NUMBER(seconds/3600),
                    NUMBER((seconds-3600*(seconds/3600))/60),
                            NUMBER(seconds-60*(seconds/60))));
        }
    }

    if(periods[0]<=60)
        window->predictedit->append(QString("Minimum: %1 s.\n").arg(NUMBER(periods[periods.size()-1])));
    else{
        int seconds=periods[0];
        if(periods[periods.size()-1<=3600])
            window->predictedit->append(QString("Minimum: %1 min, %2 s.\n").arg(
                    NUMBER(seconds/60),NUMBER(seconds-60*(seconds/60))));
        else{
            window->predictedit->append(QString("Minimum: %1 h, %2 min, %3 s.\n").arg(
                    NUMBER(seconds/3600),
                    NUMBER((seconds-3600*(seconds/3600))/60),
                    NUMBER(seconds-60*(seconds/60))));
        }
    }

    if(periods[periods.size()/2]<=60)
        window->predictedit->append(QString("Midst: %1 s.\n").arg(NUMBER(periods[periods.size()-1])));
    else{
        int seconds=periods[periods.size()/2];
        if(seconds<=3600)
            window->predictedit->append(QString("Midst: %1 min, %2 s.\n").arg(
                    NUMBER(seconds/60),NUMBER(seconds-60*(seconds/60))));
        else{
            window->predictedit->append(QString("Midst: %1 h, %2 min, %3 s.\n").arg(
                    NUMBER(seconds/3600),
                    NUMBER((seconds-3600*(seconds/3600))/60),
                    NUMBER(seconds-60*(seconds/60))));
        }
    }

    //window->predictedit->append(QString("Minimum: %1\n").arg(NUMBER(periods[0])));
    //window->predictedit->append(QString("Midst: %1\n").arg(NUMBER(periods[periods.size()/2])));
    emit success(this);
}


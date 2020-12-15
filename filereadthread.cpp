#include "filereadthread.h"

fileReadThread::fileReadThread(const QString date,class MainWindow *w,
        QSemaphore *mutex_db,int threadRank):
        date(date),window(w),mutex_db(mutex_db),threadRank(threadRank){}

void fileReadThread::run(){
    //qDebug()<<currentThreadId();
    qDebug()<<threadRank<<" reads "<<date;
    QSqlDatabase db;

    mutex_db->acquire();
    if(QSqlDatabase::contains("dataset_"+NUMBER(threadRank))){
        db=QSqlDatabase::database("dataset");
        //qDebug()<<threadRank<<" established a connection";
    }else{
        db=QSqlDatabase::addDatabase("QSQLITE","dataset_"+NUMBER(threadRank));
        db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");
        db.setDatabaseName("file::memory:");
        //qDebug()<<threadRank<<" created a connection";

    }
    mutex_db->release();

    if(!db.open()) qDebug()<<"Open failed!";
    QSqlQuery query(db);

    QString create_sql=QString("CREATE TABLE order_%1 (id VARCHAR(50) PRIMARY KEY").arg(date);
    QStringList query_list=QStringList{"departure_time INT","end_time INT",
                                       "orig_lng FLOAT","orig_lat FLOAT",
                                       "dest_lng FLOAT","dest_lat FLOAT","fee FLOAT" };
    auto _size=window->import_fields.size();
    for(int i=1;i<_size;i++) {
        if(window->fields_onehot[i]) create_sql+=","+query_list[i-1];
        qDebug()<<query_list[i-1];
    }
    create_sql+=")";
    qDebug()<<create_sql;

    mutex_db->acquire();


    /*if(!query.exec(QString("CREATE TABLE order_%1 (id VARCHAR(50) PRIMARY KEY,"
                           "departure_time INT, end_time INT, "
                           "orig_lng INT, orig_lat INT, "
                           "dest_lng FLOAT, dest_lat FLOAT,"
                           "fee FLOAT)").arg(date)))*/
    if(!query.exec(create_sql))
        qDebug()<<QString("Thread %1 reads %2 create failed!").arg(NUMBER(threadRank),date);
    mutex_db->release();

    QDir dir(window->dirName);
    QStringList filterlist;
    QString re="order_"+date+"_part*.csv";
    filterlist.push_back(re);
    dir.setNameFilters(filterlist);
    dir.setSorting(QDir::Name);
    QStringList files=dir.entryList();

    qDebug()<<files.size();

    if(files.isEmpty()) {qDebug()<<re;return;}
    for(auto &filename:files) qDebug()<<filename;

    QString insert_sql=QString("INSERT INTO order_%1 VALUES(").arg(date);
    for(auto &filename:files){
        QFile file(dir.absoluteFilePath(filename));
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        in.readLine();
        int num_item=0;
        while(!in.atEnd()){
            QString line=in.readLine().trimmed();
            QStringList l=line.split(',');
            num_item++;

            insert_sql+=QString("'%2'").arg(l[0]);

            for(int i=1;i<_size;i++) {
                if(window->fields_onehot[i]) insert_sql += QString(",%1").arg(l[i]);
            }
            insert_sql += ")";
            //qDebug()<<insert_sql;
            //mutex_db->acquire();
            /*if(!query.exec(QString("INSERT INTO order_%1 VALUES('%2',%3,%4,%5,%6,%7,%8,%9)").arg(
                    date,l[0],l[1],l[2],l[3],l[4],l[5],l[6],l[7]
            )))*/
            if(!query.exec(insert_sql)){
                qDebug()<<QString("Thread %1 reads %2 insert failed").arg(NUMBER(threadRank),filename);
                qDebug()<<query.lastError().text();
                mutex_db->release();
                return;
            }
            //mutex_db->release();
            insert_sql=QString("INSERT INTO order_%1 VALUES(").arg(date);
        }
        qDebug()<<QString("Thread %1 has just read %2").arg(NUMBER(threadRank),filename);
    }


    mutex_db->acquire();
    if(!query.exec(QString("SELECT * FROM order_%1").arg(date))) {
        qDebug() << QString("Thread %1 reads %2 select failed").arg(NUMBER(threadRank), date);
        mutex_db->release();
        return;
    }
    //while(query.next()) qDebug()<<query.value(0);
    mutex_db->release();
    qDebug()<<date<<" finished!";
    //db.close();
    emit success(this);
}

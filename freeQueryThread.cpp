//
// Created by MacBook Pro on 12/14/20.
//

#include "freeQueryThread.h"
#include <QSqlQueryModel>
void freeQueryThread::run() {

    window->datatable->clear();
    QString querystr=this->window->queryedit->text().simplified();
    qDebug()<<querystr;

    QSqlDatabase db=window->db;

    QSqlQuery query(db);
    db.open();

    QVector<QStringList> records;

    QStringList header;

    /*if(!parse(querystr,header)) {
        emit fail(this);
        return;
    }

    for(auto i=0;i<header.size();i++) qDebug()<<header[i];

    auto _size=header.size();*/

    query.exec(querystr);

    QSqlQueryModel model;
    model.setQuery(query);

    int num_column=model.columnCount();

    for(int i=0;i<window->fields_onehot.size();i++)
        if(window->fields_onehot[i]) header.push_back(window->import_fields[i]);

    while(query.next()){
        QStringList l;
        for(int i=0;i<num_column;i++) l.append(query.value(i).toString());
        records.push_back(l);
    }

    window->datatable->setColumnCount(num_column);
    window->datatable->setRowCount(records.size()+1);
    window->datatable->setHorizontalHeaderLabels(header);
    for(auto i=0;i<records.size();i++){
        for(auto j=0;j<header.size();j++) window->datatable->setItem(i,j,new QTableWidgetItem(records[i][j]));
    }
    /*while(query.next()){
        QStringList l;
        for(int i=0;i<_size;i++)
            l.append(query.value(i).toString());
        records.push_back(l);
    }

    qDebug()<<records.size();

    window->datatable->setColumnCount(header.size());
    window->datatable->setRowCount(records.size()+1);
    window->datatable->setHorizontalHeaderLabels(header);
    for(auto i=0;i<records.size();i++){
        for(auto j=0;j<header.size();j++) window->datatable->setItem(i,j,new QTableWidgetItem(records[i][j]));
    }*/

    //db.close();
    //query.exec(querystr);

    /*while(query.next()){

    }*/
}

bool isValidArgNameStart(QChar ch){
    return ch.isLetter()||ch==QChar('_');
}

bool isValidArgName(QChar ch){
    return ch.isDigit()||ch.isLetter()||ch==QChar('_');
}

bool freeQueryThread::parse(QString querystr, QStringList &header){
    //qDebug()<<QString("\t\t    a        b\t\t\t\t\t\t\t\tc\t  ").simplified();
    //qDebug()<<isValidArgName(QChar(' '));
    if(!querystr.startsWith("SELECT",Qt::CaseInsensitive)) {
        //qDebug()<<"Not a valid query order!";
        return false;
    }
    auto _size=querystr.size();

    QString argname="";

    int end=querystr.indexOf("FROM");
    //qDebug()<<end;
    if(querystr.size()<=7||end==-1) return false;

    //qDebug()<<"Begin to parse...";
    //qDebug()<<querystr[7];
    for(int i=7;i<end;){
        while(querystr[i].isSpace()) i++;
        //qDebug()<<querystr[i];
        if(!isValidArgNameStart(querystr[i])) {
            return header.isEmpty();
        }
        for(;i<end;) {
            if(isValidArgName(querystr[i]))
                argname+=querystr[i++];
            else{
                if(querystr[i]==QChar(','))
                {i++;break;}
                else{
                    if(querystr[i].isSpace())
                        i++;
                    else return false;
                }
            }
        }
        header.push_back(argname);
        //qDebug()<<argname;
        argname="";
    }
    qDebug()<<"Finished!";
    return true;
}


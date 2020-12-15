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

    //QVector<QStringList> records;

    //QStringList header;

    query.exec(querystr);

    QSqlQueryModel model;
    model.setQuery(query);

    int num_column=model.columnCount();

    for(int i=0;i<window->fields_onehot.size();i++)
        if(window->fields_onehot[i]) window->tableheader.push_back(window->import_fields[i]);


    while(query.next()){
        QStringList l;
        for(int i=0;i<num_column;i++) l.push_back(query.value(i).toString());
        window->records.push_back(l);
    }

    window->datatable->setColumnCount(num_column);
    window->datatable->setRowCount(window->records.size()+1);

    window->datatable->setHorizontalHeaderLabels(window->tableheader);
    /*for(auto i=0;i<window->records.size();i++){
        for(auto j=0;j<window->tableheader.size();j++) window->datatable->setItem(i,j,new QTableWidgetItem(window->records[i][j]));
    }*/
    emit success(this);
}

/*bool isValidArgNameStart(QChar ch){
    return ch.isLetter()||ch==QChar('_');
}

bool isValidArgName(QChar ch){
    return ch.isDigit()||ch.isLetter()||ch==QChar('_');
}*/

/*bool freeQueryThread::parse(QString querystr, QStringList &header){
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
}*/



#include "freeQueryThread.h"
#include <QSqlQueryModel>
void freeQueryThread::run() {


    QString querystr=this->window->queryedit->text().simplified();
    qDebug()<<querystr;

    if(!querystr.startsWith("SELECT",Qt::CaseInsensitive)) {emit fail(this,READONLY_ERROR);return;}
    QSqlDatabase db=window->db;

    QSqlQuery query(db);
    db.open();

    if(!query.exec(querystr)) {emit fail(this,SQL_EXEC_ERROR);return;}

    QSqlQueryModel model;
    model.setQuery(query);

    int num_column=model.columnCount();

    for(int i=0;i<num_column;i++) window->tableheader.push_back("");

    for(int i=0;i<window->fields_onehot.size();i++) {
        auto index=query.record().indexOf(window->import_fields[i]);
        if (index>=0 && index<num_column)
            window->tableheader[index]=window->import_fields[i];
    }
    qDebug()<<query.lastError();
    int cnt=0;
    while(query.next()){
        cnt++;
        vector<QString> l;
        for(int i=0;i<num_column;i++) l.push_back(QString(query.value(i).toString()));
        window->records.push_back(l);
    }
    qDebug()<<cnt;
    window->datatable->setColumnCount(num_column);
    window->datatable->setRowCount(window->records.size()+1);

    window->datatable->setHorizontalHeaderLabels(window->tableheader);
    emit success(this);
    quit();
}


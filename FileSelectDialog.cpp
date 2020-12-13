#include "FileSelectDialog.h"


FileSelectDialog::FileSelectDialog(MainWindow *window,QWidget *parent):
                                  QDialog(parent),window(window){
    setWindowTitle("Data Visualization System");
    resize(400,300);

    layout=new QVBoxLayout;
    fieldboxlayout=new QVBoxLayout();
    dateboxlayout=new QVBoxLayout();
    checkboxlayout=new QHBoxLayout();
    QFrame *framecheckbox=new QFrame;
    framecheckbox->setLayout(checkboxlayout);



    this->setLayout(layout);
    QHBoxLayout *buttonlayout=new QHBoxLayout;
    QFrame *framebutton=new QFrame;
    framebutton->setLayout(buttonlayout);

    layout->addWidget(framebutton,0);
    layout->addWidget(framecheckbox,1);
    //layout->addLayout(checkboxlayout,1);
    QPushButton *button_file=new QPushButton();
    buttonlayout->addWidget(button_file,0);

    button_file->setText("Select file");
    connect(button_file, &QPushButton::clicked, this, &FileSelectDialog::selectFile);
    QPushButton *button_ok=new QPushButton();
    button_ok->setText("OK");

    connect(button_ok, &QPushButton::clicked,  [=](){
        if(daterangeIsValid()) {
            window->from_date=QDate::fromString(from->currentText(),"yyyy-MM-dd").toString("yyyyMMdd");
            window->to_date=QDate::fromString(to->currentText(),"yyyy-MM-dd").toString("yyyyMMdd");
            this->accept();
        }
        else {
            QMessageBox msg;
            msg.setText("To date must be after From date!");
            msg.exec();
        }
    });
    buttonlayout->addWidget(button_ok,0);
    checkboxlayout->addLayout(fieldboxlayout,0);
    checkboxlayout->addLayout(dateboxlayout,0);

}

void FileSelectDialog::selectFile() {
    QString dirname=QFileDialog::getExistingDirectory();
    QString absoluteFileName;
    if(!folderIsValid(dirname, absoluteFileName)) {qDebug()<<"Dataset directory is invalid!";return;}
    window->dirName=dirname;
    QDir dir(dirname);
    QFile file(absoluteFileName);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QStringList dataFields=in.readLine().trimmed().split(',');
    auto n_fields=dataFields.size();
    QCheckBox **fieldboxes=new QCheckBox *[n_fields];

    for(int i=0;i<n_fields;i++) {
        window->import_fields.push_back(dataFields[i]);
        window->fields_onehot.push_back(false);
        fieldboxes[i]=new QCheckBox(dataFields[i],this);
        fieldboxlayout->addWidget(fieldboxes[i],0);
        connect(fieldboxes[i], &QCheckBox::stateChanged,[=](int){
            window->fields_onehot[i]=fieldboxes[i]->isChecked();
        });

    }
    //checkboxlayout->addLayout(fieldboxlayout,0);
    //checkboxlayout->addLayout(dateboxlayout,0);
    QStringList date_list;

    QString re=QString("order_")+QString("*_part*.csv");
    QStringList filterlist;
    filterlist.push_back(re);
    dir.setNameFilters(filterlist);
    auto entrylist=dir.entryList();
    qDebug()<<entrylist.size();

    for(auto filename:entrylist){
        QString date=filename.split('_')[1];
        if(!date_list.contains(date)) date_list.push_back(date);
    }
    auto n_date=date_list.size();

    from=new QComboBox;
    to=new QComboBox;
    QLabel *labelfrom=new QLabel("From:");
    QLabel *labelto=new QLabel("To:");

    dateboxlayout->addWidget(labelfrom,0);
    dateboxlayout->addWidget(from,0);
    dateboxlayout->addWidget(labelto,0);
    dateboxlayout->addWidget(to,0);

    for(int i=0;i<n_date;i++){
        //window->import_dates.push_back(date_list[i]);
        //window->dates_onehot.push_back(false);
        from->addItem(QDate::fromString(date_list[i],"yyyyMMdd").toString("yyyy-MM-dd"));
        to->addItem(QDate::fromString(date_list[i],"yyyyMMdd").toString("yyyy-MM-dd"));
    }

    //checkboxlayout->addLayout(fieldboxlayout,0);
    //checkboxlayout->addLayout(dateboxlayout,0);
    //this->setLayout(checkboxlayout);
}

bool FileSelectDialog::folderIsValid(QString dirname, QString &filename) {
    QDir dir(dirname);

    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    QStringList filelist=dir.entryList();
    qDebug()<<filelist[0];
    if(filelist[0].contains("order")) {
        qDebug() << "Success!";
        filename = dir.absoluteFilePath(filelist[0]);
        qDebug() << filename;
        return true;
    }else {
        filename="";
        return false;
    }
}


bool FileSelectDialog::daterangeIsValid() {
    int s=from->currentIndex();
    int t=to->currentIndex();
    return s<=t;
}
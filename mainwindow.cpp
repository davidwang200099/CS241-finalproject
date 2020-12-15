#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QChart>
#include <QChartView>
#include <QValueAxis>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),mutex_db(1),mutex_cntfinish(1)
{
     initBasicVisTab();
     initAdvancedTab();
     initPredictionTab();
}

void MainWindow::initPredictionTab() {
    //QVBoxLayout *layout=new QVBoxLayout;

    //layout->addWidget(new QChartView());

    QVBoxLayout *predictlayout=new QVBoxLayout;

    QHBoxLayout *fromplacelayout=new QHBoxLayout;
    QHBoxLayout *toplacelayout=new QHBoxLayout;

    predictboxes=new QComboBox *[5];
    for(int i=0;i<5;i++) predictboxes[i]=new QComboBox;
    fromplacelayout->addWidget(new QLabel("From:"),0);
    fromplacelayout->addWidget(predictboxes[0],1);
    fromplacelayout->addWidget(predictboxes[1],1);

    QHBoxLayout *totimelayout=new QHBoxLayout;
    toplacelayout->addWidget(new QLabel("To:  "),0);
    toplacelayout->addWidget(predictboxes[2],1);
    toplacelayout->addWidget(predictboxes[3],1);

    for(int i=0;i<4;i++){
        qDebug()<<i;
        for(int j=0;j<10;j++) predictboxes[i]->addItem(NUMBER(j));
    }

    predictlayout->addWidget(predictedit=new QTextEdit);
    predictlayout->addLayout(fromplacelayout,0);
    predictlayout->addLayout(toplacelayout,0);

    QHBoxLayout *timelayout=new QHBoxLayout;
    timelayout->addWidget(timeedit=new QLineEdit,1);

    timelayout->addWidget(predictboxes[4],1);
    predictboxes[4]->addItems(PREDICT_LIST);

    QPushButton *button=new QPushButton;
    timelayout->addWidget(button,0);
    predictlayout->addLayout(timelayout,0);
    connect(button,&QPushButton::clicked,[=](){
        qDebug()<<button;
        //button->setEnabled(false);
        predictionThread *thread=new predictionThread(this);
        thread->start();
        qDebug()<<"create!";
        connect(thread,&predictionThread::success,[=](predictionThread *thread){
            qDebug()<<button;

            button->setEnabled(true);
            thread->exit(0);
            thread->deleteLater();
        });
    });
    ui->tab_predict->setLayout(predictlayout);
}

void MainWindow::initAdvancedTab() {
    QVBoxLayout *layout=new QVBoxLayout;

    QHBoxLayout *querylayout=new QHBoxLayout;
    queryedit=new QLineEdit;
    querylayout->addWidget(queryedit,1);
    QPushButton *querybutton=new QPushButton;
    querybutton->setText("Query!");
    querylayout->addWidget(querybutton,0);
    connect(querybutton,&QPushButton::clicked,[=](){
        freeQueryThread *thread=new freeQueryThread(this);
        thread->start();
        querybutton->setEnabled(false);
        records.clear();
        tableheader.clear();
        connect(thread,&freeQueryThread::success,[=](freeQueryThread *thread){
            //querybutton->setEnabled(false);
            querybutton->setEnabled(true);
            thread->exit(0);
            thread->deleteLater();
            for(auto i=0;i<records.size();i++){
                for(auto j=0;j<tableheader.size();j++) datatable->setItem(i,j,new QTableWidgetItem(records[i][j]));
            }

        });
    });
    layout->addLayout(querylayout,0);
    datatable=new QTableWidget;
    layout->addWidget(datatable,1);

    ui->tab_advanced->setLayout(layout);
}

void MainWindow::initBasicVisTab() {
    ui->setupUi(this);
    this->view=NULL;
    this->chart=NULL;
    boxes=new QComboBox*[N_BOX];
    for(int i=0;i<N_BOX;i++) boxes[i]=new QComboBox;

    QVBoxLayout *layout=new QVBoxLayout;

    QHBoxLayout *buttonlayout=new QHBoxLayout;

    QHBoxLayout *viewlayout=new QHBoxLayout;
    viewlayout->addWidget(view=new QChartView);

    buttonlayout->addWidget(button_file=new QPushButton("Import"));
    buttonlayout->addWidget(button_draw=new QPushButton("Draw"));
    buttonlayout->addWidget(boxes[DRAW_FUNC]);
    boxes[DRAW_FUNC]->addItems(FUNCTION_LIST);

    button_draw->setEnabled(false);
    layout->addLayout(buttonlayout,1);
    layout->addLayout(viewlayout,1);

    QHBoxLayout *fromtimelayout=new QHBoxLayout;

    fromtimelayout->addWidget(new QLabel("From:"),0);
    fromtimelayout->addWidget(boxes[0],1);
    fromtimelayout->addWidget(boxes[1],1);

    QHBoxLayout *totimelayout=new QHBoxLayout;
    totimelayout->addWidget(new QLabel("To:  "),0);
    totimelayout->addWidget(boxes[2],1);
    totimelayout->addWidget(boxes[3],1);

    layout->addLayout(fromtimelayout,0);
    layout->addLayout(totimelayout,0);

    QHBoxLayout *steplayout=new QHBoxLayout;
    steplayout->addWidget(new QLabel("Step:"),0);
    steplayout->addWidget(edit=new QLineEdit("30"),1);
    steplayout->addWidget(boxes[4],1);
    boxes[4]->addItem("s");
    boxes[4]->addItem("min");
    boxes[4]->addItem("h");
    boxes[4]->addItem("d");
    boxes[4]->setCurrentIndex(1);
    layout->addLayout(steplayout,0);
    initProgressBar();

    QHBoxLayout *citygridlayout=new QHBoxLayout;
    citygridlayout->addWidget(new QLabel("Grid: "),0);
    citygridlayout->addWidget(new QLabel("Longtitude NO: "),1);
    citygridlayout->addWidget(boxes[5],1);
    citygridlayout->addWidget(new QLabel("Latitude NO: "),1);
    citygridlayout->addWidget(boxes[6],1);
    boxes[5]->addItem("All");
    boxes[6]->addItem("All");

    for(int i=0;i<N_GRID;i++) {
        boxes[5]->addItem(NUMBER(i));
        boxes[6]->addItem(NUMBER(i));
    }

    layout->addLayout(citygridlayout,0);
    ui->tab_basic->setLayout(layout);



    connect(button_file,&QPushButton::clicked,this,&MainWindow::selectfile);
    connect(button_draw,&QPushButton::clicked,this,&MainWindow::draw);
    cnt_finish=0;
    cnt_create=0;

    qDebug()<<"centralwidget: "<<ui->centralwidget->size();
    qDebug()<<"tabwidget: "<<ui->tabwidget->size();
    qDebug()<<"tab_basic: "<<ui->tab_basic->size();
}

void MainWindow::initProgressBar() {
    ui->statusbar->setSizeGripEnabled(true);
    bar=new QProgressBar(this);
    bar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    ui->statusbar->addPermanentWidget(bar,1);

    bar->setMinimum(0);
    bar->setMaximum(100);
    bar->setValue(100);
    bar->setVisible(true);


    progress=new QLabel(this);
    progress->setText("Loading dataset...");
    ui->statusbar->addPermanentWidget(progress,0);
    progress->show();
}

MainWindow::~MainWindow()
{
    delete [] boxes;
    delete ui;
}

void MainWindow::selectfile(){
    button_file->setEnabled(false);
    //dirName="/Users/macbookpro/Downloads/Dataset-CS241-2020";
    QDir dir(dirName);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    QStringList filelist=dir.entryList();

    db=QSqlDatabase::addDatabase("QSQLITE","main");
    db.setDatabaseName("file::memory:");
    db.setConnectOptions("QSQLITE_OPEN_URI;QSQLITE_ENABLE_SHARED_CACHE");

    if(db.open()) qDebug()<<"Open database succeed!";
    else qDebug()<<"Open database failed!";

    auto from=QDate::fromString(from_date,"yyyyMMdd");
    auto to=QDate::fromString(to_date,"yyyyMMdd");

    int threadrank=0;

    for(int i=0;i<N_HOUR;i++){
        QString str=(i<10)?"0"+NUMBER(i)+":00":NUMBER(i)+":00";
        boxes[1]->addItem(str);
        boxes[3]->addItem(str);
    }

    for(auto i=from;i<=to;i=i.addDays(1)) {
        auto thread = new fileReadThread(i.toString("yyyyMMdd"), this, &mutex_db, threadrank++);
        auto str=i.toString("yyyy-MM-dd");
        boxes[0]->addItem(str);
        boxes[2]->addItem(str);
        thread->start();
        cnt_create++;
        connect(thread, &fileReadThread::success, [=](fileReadThread *thread){
            //qDebug()<<QThread::currentThread();
            //qDebug()<<thread->currentThread();
            bool flag=false;
            mutex_cntfinish.acquire();
            cnt_finish++;
            //qDebug()<<(&threadrank);
            //qDebug()<<cnt_finish<<" "<<threadrank;
            if(cnt_finish==cnt_create) flag=true;
            mutex_cntfinish.release();
            if(flag) {
                qDebug()<<"All finished!";
                button_draw->setEnabled(true);
            }
            //thread->deleteLater();
        });
    }

    QFile gridfile(dirName+"/rectangle_grid_table.csv");
    gridfile.open(QIODevice::ReadOnly);

    QTextStream in(&gridfile);

    in.readLine();

    grid_north=0.0;
    grid_south=90.0;
    grid_east=0.0;
    grid_west=180.0;

    while(!in.atEnd()){
        float a,b,c,d;
        QStringList l=in.readLine().trimmed().split(',');
        QPointF lefttop(a=l[1].toFloat(),b=l[2].toFloat());
        QPointF rightbottom(c=l[5].toFloat(),d=l[6].toFloat());
        grids.push_back(pair<QPointF, QPointF>(lefttop,rightbottom));
        if(grid_north<b) grid_north=b;
        if(grid_south>d) grid_south=d;
        if(grid_east<c) grid_east=c;
        if(grid_west>a) grid_west=a;
    }
}

void MainWindow::on_draw_success(class graphDrawThread *thread) {
    qDebug()<<"Finished drawing graph!";
    thread->deleteLater();
}


int MainWindow::askUser() {
    FileSelectDialog window(this);
    int r=window.exec();
    qDebug()<<r;
    for(auto field:import_fields) qDebug()<<field;
    for(auto i:fields_onehot) qDebug()<<i;
    return r;
}

void MainWindow::draw() {
    //if(view) delete view;
    //view=new QChartView(this);
    button_draw->setEnabled(false);
    if(boxes[FROM_DATE]->currentIndex()==boxes[TO_DATE]->currentIndex() && boxes[FROM_HOUR]->currentIndex()==boxes[TO_HOUR]->currentIndex()){
        QMessageBox msg;
        msg.setText("FROM equals TO! Please choose a period!");
        msg.exec();
        return;
    }

    int a=edit->text().toInt();
    int b;
    switch(boxes[4]->currentIndex()){
        case 0:
            b=1;break;
        case 1:
            b=60;break;
        case 2:
            b=3600;break;
        case 3:
            b=86400;break;
        default:
            b=60;
    }
    auto thread=new graphDrawThread(this,period=a*b, boxes[DRAW_FUNC]->currentIndex());
    connect(thread,&graphDrawThread::success,this,&MainWindow::plot);
    thread->start();
}

void MainWindow::plot() {
    button_draw->setEnabled(true);
    QChart *old=NULL;
    if(chart) {
        qDebug()<<"delete chart!";
        QChart *old=chart;
        //view->close();
        //delete view;
    }

    switch(boxes[DRAW_FUNC]->currentIndex()) {
        case 0:
            plot_spatial_temporal();
            break;
        case 1:
            plot_traveltime();
            break;
        case 2:
            plot_fee();
            break;
    }
}

void MainWindow::plot_traveltime() {
    QChart *old=NULL;
    if(chart) {
        qDebug()<<"delete chart!";
        QChart *old=chart;
    }
    chart = new QChart;
    QSplineSeries *series = new QSplineSeries();

    QDateTime startdatetime = DT(boxes[0]->currentText() + "-" + boxes[1]->currentText() + ":00",
                                 "yyyy-MM-dd-hh:mm:ss");
    QDateTime enddatetime = DT(boxes[2]->currentText() + "-" + boxes[3]->currentText() + ":00",
                               "yyyy-MM-dd-hh:mm:ss");

    int max = -1;
    for(int i=0;i<order_by_fee.size();i++) {
        qDebug() << order_by_traveltime[i];
        if (order_by_fee[i] > max) max = order_by_traveltime[i];

        series->append(i, order_by_traveltime[i]);
    }

    QValueAxis *x=new QValueAxis;

    QValueAxis *y = new QValueAxis;
    //y->setRange(0, max);
    y->setLabelFormat("%d");

    series->setName("fee of order");
    chart->addSeries(series);
    chart->setAxisX(x, series);
    chart->setAxisY(y, series);

    view->setChart(chart);
    view->show();
    if (old) delete old;

}

void MainWindow::plot_spatial_temporal() {
    QChart *old=NULL;
    if(chart) {
        qDebug()<<"delete chart!";
        QChart *old=chart;
    }
    chart = new QChart;
    QSplineSeries *series = new QSplineSeries();

    QDateTime startdatetime = DT(boxes[0]->currentText() + "-" + boxes[1]->currentText() + ":00",
                                 "yyyy-MM-dd-hh:mm:ss");
    QDateTime enddatetime = DT(boxes[2]->currentText() + "-" + boxes[3]->currentText() + ":00",
                               "yyyy-MM-dd-hh:mm:ss");

    int max = -1;
    qDebug() << "In MainWindow, period=" << period;
    for (int i = 0; i < order_by_period.size(); i++) {
        qDebug() << order_by_period[i];
        if (order_by_period[i] > max) max = order_by_period[i];
        series->append(startdatetime.addSecs(i * period).toMSecsSinceEpoch(), order_by_period[i]);
    }
    QDateTimeAxis *x = new QDateTimeAxis;
    x->setTickCount(24);
    x->setLabelsAngle(-90);
    x->setFormat("yyyy-MM-dd hh:mm");

    QValueAxis *y = new QValueAxis;
    //y->setRange(0, max);
    y->setLabelFormat("%d");

    series->setName("num. of order");
    chart->addSeries(series);
    chart->setAxisX(x, series);
    chart->setAxisY(y, series);

    view->setChart(chart);
    view->show();
    if (old) delete old;
}

void MainWindow::plot_fee() {
    QChart *old=NULL;
    if(chart) {
        qDebug()<<"delete chart!";
        QChart *old=chart;
        //view->close();
        //delete view;
    }
    chart = new QChart;
    QSplineSeries *series = new QSplineSeries();

    QDateTime startdatetime = DT(boxes[0]->currentText() + "-" + boxes[1]->currentText() + ":00",
                                 "yyyy-MM-dd-hh:mm:ss");
    QDateTime enddatetime = DT(boxes[2]->currentText() + "-" + boxes[3]->currentText() + ":00",
                               "yyyy-MM-dd-hh:mm:ss");

    int max = -1;

    for(int i=0;i<order_by_fee.size();i++) {
        qDebug() << order_by_fee[i];
        if (order_by_fee[i] > max) max = order_by_fee[i];

        series->append(i, order_by_fee[i]);
    }
    QValueAxis *x=new QValueAxis;

    QValueAxis *y = new QValueAxis;
    //y->setRange(0, max);
    y->setLabelFormat("%d");

    series->setName("fee of order");
    chart->addSeries(series);
    chart->setAxisX(x, series);
    chart->setAxisY(y, series);

    view->setChart(chart);
    view->show();
    if (old) delete old;
}

/*void MainWindow::freequery() {
    freeQueryThread * thread=new freeQueryThread(this);
    thread->start();
    connect(thread, &freeQueryThread::success,this,&MainWindow::on_query_success);
}

void MainWindow::predict() {
    //predictedit->clear();
    predictionThread *thread=new predictionThread(this);
    thread->start();
    connect(thread,&predictionThread::success,this,&MainWindow::on_predict_success);

}

void MainWindow::on_predict_success(class predictionThread *thread) {
    thread->deleteLater();
}

void MainWindow::on_query_success(class freeQueryThread *thread) {
    thread->deleteLater();
}*/
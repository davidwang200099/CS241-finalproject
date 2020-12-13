#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#define N_FIELD 7

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),mutex_db(1),mutex_cntfinish(1)
{
    ui->setupUi(this);
    this->view=NULL;
    this->chart=NULL;
    QVBoxLayout *layout=new QVBoxLayout;

    QHBoxLayout *buttonlayout=new QHBoxLayout;
    QPushButton *button_file;
    QPushButton *button_draw;
    QHBoxLayout *viewlayout=new QHBoxLayout;
    viewlayout->addWidget(view=new QChartView);

    buttonlayout->addWidget(button_file=new QPushButton("Import",this));
    buttonlayout->addWidget(button_draw=new QPushButton("Draw",this));

    layout->addLayout(buttonlayout,1);
    layout->addLayout(viewlayout,1);

    QHBoxLayout *fromtimelayout=new QHBoxLayout;

    boxes=new QComboBox[4];

    fromtimelayout->addWidget(boxes);
    fromtimelayout->addWidget(boxes+1);

    QHBoxLayout *totimelayout=new QHBoxLayout;

    totimelayout->addWidget(boxes+2);
    totimelayout->addWidget(boxes+3);
    layout->addLayout(fromtimelayout,0);
    layout->addLayout(totimelayout,0);

    ui->centralwidget->setLayout(layout);

    connect(button_file,&QPushButton::clicked,this,&MainWindow::selectfile);
    connect(button_draw,&QPushButton::clicked,this,&MainWindow::draw);
    cnt_finish=0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectfile(){

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

    for(auto i=from;i<=to;i=i.addDays(1)) {
        auto thread = new fileReadThread(i.toString("yyyyMMdd"), this, &mutex_db, threadrank++);
        thread->start();
        connect(thread, &fileReadThread::success, [=](fileReadThread *thread){
            bool flag=false;
            mutex_cntfinish.acquire();
            cnt_finish++;
            if(cnt_finish==threadrank) flag=true;
            mutex_cntfinish.release();
            if(flag) qDebug()<<"All finished!";
        });
    }
    ui->statusbar->setSizeGripEnabled(true);
    bar=new QProgressBar(this);
    bar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    ui->statusbar->addPermanentWidget(bar,1);

    bar->setMinimum(0);
    bar->setMaximum(100);
    bar->setValue(100);
    bar->setVisible(true);
    qDebug()<<"size of bar: "<<bar->size();

    progress=new QLabel(this);
    progress->setText("Loading dataset...");
    ui->statusbar->addPermanentWidget(progress,0);
    progress->show();
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
    auto thread=new graphDrawThread(this);
    connect(thread,&graphDrawThread::success,this,&MainWindow::plot);
    thread->start();
}

void MainWindow::plot() {
    QChart *old=NULL;
    if(chart) {
        qDebug()<<"delete chart!";
        QChart *old=chart;
        //view->close();
        //delete view;
    }


    chart=new QChart;
    QLineSeries *series=new QLineSeries(chart);

    int max=-1;
    for(int i=0;i<order_by_period.size();i++) {
        if(order_by_period[i]>max) max=order_by_period[i];
        series->append(i, order_by_period[i]);
    }
    int period=order_by_period.size();
    QValueAxis *x=new QValueAxis;
    x->setRange(0, period);
    x->setLabelFormat("%d");
    x->setTickCount(period+1);

    QValueAxis *y=new QValueAxis;
    y->setRange(0, max);
    y->setLabelFormat("%d");


    chart->addSeries(series);
    chart->setAxisX(x,series);
    chart->setAxisY(y,series);
    //window->view->setChart(chart);
    //window->view->show();

    view->setChart(chart);
    view->show();
    if(old) delete old;

    /*qDebug()<<"view = "<<view;
    qDebug()<<"chart = "<<chart;
    qDebug()<<"chart+16= "<<((char *)chart+0x10);
    qDebug()<<"series->parent() = "<<series->parent();
    qDebug()<<"series->parent()->parent() = "<<series->parent()->parent();
    qDebug()<<"x->parent() = "<<x->parent();
    qDebug()<<"x->parent()->parent() = "<<x->parent()->parent();*/
}
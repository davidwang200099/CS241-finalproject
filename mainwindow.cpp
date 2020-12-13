#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include <QValueAxis>

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

    boxes=new QComboBox*[N_BOX];

    for(int i=0;i<N_BOX;i++) boxes[i]=new QComboBox;

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

    ui->centralwidget->setLayout(layout);

    connect(button_file,&QPushButton::clicked,this,&MainWindow::selectfile);
    connect(button_draw,&QPushButton::clicked,this,&MainWindow::draw);
    cnt_finish=0;
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
    qDebug()<<"size of bar: "<<bar->size();

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

    /*auto endofday=QTime::fromString("23:59","hh:mm");
    qDebug()<<endofday;
    for(auto time=QTime::fromString("00:00","hh:mm");time<=endofday;time=time.addSecs(3600)){
        boxes[1]->addItem(time.toString("hh:mm"));
        boxes[3]->addItem(time.toString("hh:mm"));
        qDebug()<<time;
    }*/
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
        connect(thread, &fileReadThread::success, [=](fileReadThread *thread){
            bool flag=false;
            mutex_cntfinish.acquire();
            cnt_finish++;
            if(cnt_finish==threadrank) flag=true;
            mutex_cntfinish.release();
            if(flag) qDebug()<<"All finished!";
        });
    }

    QFile gridfile(dirName+"/rectangle_grid_table.csv");
    gridfile.open(QIODevice::ReadOnly);

    QTextStream in(&gridfile);

    in.readLine();

    while(!in.atEnd()){
        QStringList l=in.readLine().trimmed().split(',');
        QPointF lefttop(l[1].toFloat(),l[2].toFloat());
        QPointF rightbottom(l[7].toFloat(),l[8].toFloat());
        grids.push_back(pair<QPointF, QPointF>(lefttop,rightbottom));
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
    auto thread=new graphDrawThread(this,a*b);
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
    QSplineSeries *series=new QSplineSeries();

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
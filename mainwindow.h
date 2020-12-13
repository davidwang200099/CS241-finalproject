#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QSemaphore>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QProgressBar>
#include <QSizePolicy>
#include <QLayout>
#include <QFrame>
#include <QSpacerItem>
#include <QChart>
#include <QChartView>
#include <QLineEdit>
#include <QPointF>
#include <QTime>
#include <QSplineSeries>
#include <QComboBox>
#include "filereadthread.h"
#include "databasecheckthread.h"
#include "filereadthread.h"
#include "FileSelectDialog.h"
#include "graphDrawThread.h"
#include "constant.h"
QT_CHARTS_USE_NAMESPACE
using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class FileSelectDialog;
    friend class fileReadThread;
    friend class graphDrawThread;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int askUser();
private:
    void checkDatabase();
    void selectfile();
    void on_read_success(class fileReadThread *thread);
    void draw();
    void plot();
    void on_draw_success(class graphDrawThread *thread);
    void initProgressBar();
    Ui::MainWindow *ui;
    //QPushButton *button;
    QString dirName;
    QSemaphore mutex_db;
    QProgressBar *bar;
    QLabel *progress;
    QChartView *view;
    QChart *chart;
    QComboBox **boxes;
    QLineEdit *edit;
    //vector<fileReadThread *> threads;

    QSqlDatabase db;
    QVector<int> order_by_period;
    //QStringList import_dates;
    //QVector<bool> dates_onehot;

    QString from_date;
    QString to_date;

    QSemaphore mutex_cntfinish;
    int cnt_finish;
    QStringList import_fields;
    QVector<bool> fields_onehot;

    QVector<pair<QPointF, QPointF> > grids;

};


#endif // MAINWINDOW_H

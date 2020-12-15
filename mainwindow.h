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
#include <QDateTimeAxis>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include "filereadthread.h"
#include "databasecheckthread.h"
#include "filereadthread.h"
#include "FileSelectDialog.h"
#include "graphDrawThread.h"
#include "constant.h"
#include "freeQueryThread.h"
#include "predictionThread.h"
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
    friend class freeQueryThread;
    friend class predictionThread;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int askUser();
private:
    void checkDatabase();
    void selectfile();
    void freequery();
    void predict();
    void on_read_success(class fileReadThread *thread);
    void on_predict_success(class predictionThread *thread);
    void on_query_success(class freeQueryThread *thread);
    void draw();
    void plot();
    void plot_spatial_temporal();
    void plot_fee();
    void plot_traveltime();
    void on_draw_success(class graphDrawThread *thread);
    void initProgressBar();
    void initBasicVisTab();
    void initAdvancedTab();
    void initPredictionTab();
    Ui::MainWindow *ui;
    //QPushButton *button;
    QString dirName;
    QSemaphore mutex_db;
    QProgressBar *bar;
    QLabel *progress;
    QChartView *view;
    QChart *chart;
    QComboBox **boxes;
    QComboBox **predictboxes;
    QLineEdit *edit;
    QPushButton *button_file;
    QPushButton *button_draw;
    QTableWidget *datatable;
    QTextEdit *predictedit;
    QLineEdit *queryedit;
    QLineEdit *timeedit;
    QSqlDatabase db;
    QVector<int> order_by_period;
    QVector<int> order_by_fee;
    QVector<int> order_by_traveltime;
    int period;

    QString from_date;
    QString to_date;

    QSemaphore mutex_cntfinish;
    int cnt_finish;
    int cnt_create;
    QStringList import_fields;
    QVector<bool> fields_onehot;

    QVector<pair<QPointF, QPointF> > grids;
    float grid_north;
    float grid_south;
    float grid_west;
    float grid_east;

};


#endif // MAINWINDOW_H

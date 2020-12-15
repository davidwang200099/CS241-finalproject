#include "mainwindow.h"

#include <QApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //QQmlApplicationEngine engine;
    //engine.load(QStringLiteral("main.qml"));
    if(!w.askUser()) return 0;
    w.show();
    return a.exec();
}

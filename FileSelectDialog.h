//
// Created by MacBook Pro on 12/11/20.
//

#ifndef FINAL_FILESELECTDIALOG_H
#define FINAL_FILESELECTDIALOG_H
#include <QDialog>
#include <QString>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QPushButton>
#include <QCheckBox>
#include <QDate>
#include <QDateTime>
#include <QLayout>
#include "mainwindow.h"

class FileSelectDialog: public QDialog {
    Q_OBJECT
public:
    FileSelectDialog(class MainWindow *window,QWidget *parent= nullptr);
    void selectFile();
    bool folderIsValid(const QString dirname, QString &filename);
    bool daterangeIsValid();
    QVBoxLayout *layout;
    QHBoxLayout *checkboxlayout;
    QVBoxLayout *fieldboxlayout;
    QVBoxLayout *dateboxlayout;
    QComboBox *from;
    QComboBox *to;
    MainWindow *window;
    QCheckBox **fieldboxes;
};


#endif //FINAL_FILESELECTDIALOG_H

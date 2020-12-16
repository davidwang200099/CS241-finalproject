//
// Created by MacBook Pro on 12/13/20.
//

#ifndef FINAL_CONSTANT_H
#define FINAL_CONSTANT_H

#include <QStringList>
#define N_FIELD 7
#define N_BOX 8
#define N_GRID 10
#define N_HOUR 24

#define FROM_DATE 0
#define FROM_HOUR 1
#define TO_DATE 2
#define TO_HOUR 3
#define STEP 4
#define LONG 5
#define LAT 6
#define DRAW_FUNC 7
#define DATE QDate::fromString
#define DT QDateTime::fromString

#define FUNCTION_LIST QStringList{"Spatial-Temporal Data","Distribution of Travel Time","Distribution of Fee"}
#define PREDICT_LIST QStringList{"Destination Prediction","Travel Time Prediction"}

#define READONLY_ERROR 0
#define SQL_EXEC_ERROR 1

#endif //FINAL_CONSTANT_H

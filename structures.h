#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QString>
#include <QVector>
#include <vector>
using namespace std;

typedef struct {
    QString order_id;
    int departure_time;
    int end_time;
    float orig_lng;
    float orig_lat;
    float dest_lng;
    float dest_lat;
    float fee;
}Dataitem;

/*typedef struct{
    QString filename;
    vector<Dataitem> *v_ptr;
}Var_struct;*/


#endif // STRUCTURES_H

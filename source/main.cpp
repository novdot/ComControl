#include "comdispatcher.h"

#include <QApplication>
#include <QDebug>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

double decode_ieee_single( void *v, int natural_order)
{
    unsigned char *data;
    int s, e;
    unsigned long src;
    long f;
    double value;

    data = (unsigned char *)v;

    if (natural_order) {
        src =
                ((unsigned long)data[2] << 24) |
                ((unsigned long)data[2] << 16) |
                ((unsigned long)data[2] << 8) |
                ((unsigned long)data[3]);
    } else {
        src =
                ((unsigned long)data[1] << 24) |
                ((unsigned long)data[1] << 16) |
                ((unsigned long)data[1] << 8) |
                ((unsigned long)data[0]);
    }

    s = (src & 0x80000000UL) >> 31;
    e = (src & 0x7F800000UL) >> 23;
    f = (src & 0x007FFFFFUL);

    if (e == 255 && f != 0) {
        /* NaN (Not a Number) */
        value = DBL_MAX;
    }
    else if (e == 255 && f == 0 && s == 1) {
        /* Negative infinity */
        value = -DBL_MAX;
    }
    else if (e == 255 && f == 0 && s == 0) {
        /* Positive infinity */
        value = DBL_MAX;
    }
    else if (e > 0 && e < 255) {
        /* Normal number */
        f += 0x00800000UL;
        if (s) f = -f;
        value = ldexp(f, e - 150);
    }
    else if (e == 0 && f != 0) {
        /* Denormal number */
        if (s) f = -f;
        value = ldexp(f, -149);
    }
    else if (e == 0 && f == 0 && s == 1) {
        /* Negative zero */
        value = 0;
    }
    else if (e == 0 && f == 0 && s == 0) {
        /* Positive zero */
        value = 0;
    }
    else {
    /* Never happens */
    //printf("s = %d, e = %d, f = %lu\n", s, e, f);
    //assert(!"Woops, unhandled case in decode_ieee_single()");
    }
    return value;
}
union Convert{
    float f;
    int i;
} ;
void float_dbg()
{
    //APPEND_FDATA(g_accel.accelout.fTemper,COMPLETE_IND_F_16_32(0,0,0));
    float fl = -9.02;
    unsigned char *ptr;
    Convert conv;
    unsigned char ptr2[4];
    float fl2 = 0.0;

    ptr = (unsigned char *) &(fl);//g_accel.accelout.fTemper
    qDebug()<<  *(ptr);
    qDebug()<<  *(ptr+1);
    qDebug()<<  *(ptr+2);
    qDebug()<<  *(ptr+3);

    ptr2[0] = 236;
    ptr2[1] = 81;
    ptr2[2] = 16;
    ptr2[3] = 0x41;//193;
    memcpy(&fl2, ptr2, sizeof(float));

    qDebug()<< fl2;//conv.f;
}

int main(int argc, char *argv[])
{
    //float_dbg();
    QApplication a(argc, argv);
    ComDispatcher w;
    w.show();
    return a.exec();
}

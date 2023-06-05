#ifndef SINGULAR_H
#define SINGULAR_H

enum filter_type
{
    EQ = 0,
    NEQ = 1,
    GEQ = 2,
    LEQ = 3,
    LESS = 4,
    GREATER = 5
};
void print(double a[], int size);
void shift(double a[], int size, double by);
int filter(double a[], int count, enum filter_type t, double threshold);

#endif
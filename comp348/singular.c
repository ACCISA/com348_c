#include <stdio.h>
#include <stdbool.h>
#include <math.h>

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
size_t filter(double a[], size_t count, enum filter_type t, double threshold);

extern int prec;
extern bool hasPrec;
extern int *lineSizes;
extern int *lineSizesFormat;

int cusPow(int base, int prec);
double cusRound(double x);

void print(double a[], int size)
{
    if (size == 0)
        return;
    // printf("%d \n", size);
    for (int i = 0; i < size; i++)
    {
        if (prec == -1)
        {
            printf("%.4lf ", a[i]);
            continue;
        }
        int power = cusPow(10, prec);
        double rounded = cusRound(a[i] * power) / power;
        printf("%.*lf ", prec, rounded);
    }
    printf("\n");
}

void shift(double a[], int size, double by)
{
    for (int i = 0; i < size; i++)
    {
        a[i] += by;
    }
}

size_t filter(double a[], size_t count, enum filter_type t, double threshold)
{
    int newSize = 0;
    for (int i = 0; i < count; i++)
    {
        if (t == EQ)
        {
            if (a[i] == threshold)
                newSize += 1;
            continue;
        }
        if (t == NEQ)
        {
            if (a[i] != threshold)
                newSize += 1;
            continue;
        }
        if (t == GEQ)
        {
            if (a[i] >= threshold)
                newSize += 1;
            continue;
        }
        if (t == LEQ)
        {
            if (a[i] <= threshold)
                newSize += 1;
            continue;
        }
        if (t == LESS)
        {
            if (a[i] < threshold)
                newSize += 1;
            continue;
        }
        if (t == GREATER)
        {
            if (a[i] > threshold)
                newSize += 1;
            continue;
        }
    }
    double tempA[newSize];

    int index = 0;
    for (int i = 0; i < count; i++)
    {
        if (t == EQ)
        {
            if (a[i] == threshold)
            {
                tempA[index] = a[i];
                index += 1;
            }
            continue;
        }
        if (t == NEQ)
        {

            if (a[i] != threshold)
            {
                tempA[index] = a[i];
                index += 1;
            }
            continue;
        }
        if (t == GEQ)
        {
            if (a[i] >= threshold)
            {
                tempA[index] = a[i];
                index += 1;
            }
            continue;
        }
        if (t == LEQ)
        {

            if (a[i] <= threshold)
            {
                tempA[index] = a[i];
                index += 1;
            }
            continue;
        }
        if (t == LESS)
        {

            if (a[i] < threshold)
            {
                tempA[index] = a[i];
                index += 1;
            }
            continue;
        }
        if (t == GREATER)
        {

            if (a[i] > threshold)
            {
                tempA[index] = a[i];
                index += 1;
            }
            continue;
        }
    }
    a = tempA;
    return newSize;
}
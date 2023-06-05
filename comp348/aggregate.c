#include <stdio.h>
#include <strings.h>

typedef const char *AggregateFunction;
typedef double (*AggregateFunctionPointer)(double *, int);

static double COUNT(double *arr, int size);
static double MIN(double *arr, int size);
static double MAX(double *arr, int size);
static double SUM(double *arr, int size);
static double AVG(double *arr, int size);
static double PAVG(double *arr, int size);
void printSmt();
int numFunctions = 6;

static AggregateFunction functionsStr[] = {"COUNT", "MIN", "MAX", "SUM", "AVG", "PAVG"};
static AggregateFunctionPointer functionPointers[] = {&COUNT, &MIN, &MAX, &SUM, &AVG, &PAVG};

void printSmt()
{
    printf("test\n");
}

double aggregate(const char *func, double *arr, int size)
{
    for (int i = 0; i < numFunctions; i++)
    {
        if (strcasecmp(func, functionsStr[i]) == 0)
        {
            return functionPointers[i](arr, size);
        }
    }
    fprintf(stderr, "Invalid function name: %s\n", func);
    return -1;
}

static double MAX(double *arr, int size)
{
    double max = 0.0;
    for (int i = 0; i < size; i++)
    {
        if (max > arr[i])
            continue;
        max = arr[i];
    }
    return max;
}

static double MIN(double *arr, int size)
{
    double min = arr[0];
    for (int i = 0; i < size; i++)
    {
        if (min < arr[i])
            continue;
        min = arr[i];
    }
    return min;
}

static double AVG(double *arr, int size)
{
    return SUM(arr, size) / size;
}

static double PAVG(double *arr, int size)
{
    return (MIN(arr, size) + MAX(arr, size)) / 2;
}

static double COUNT(double *arr, int size)
{
    printf("%d\n", size);
    return 0.0;
}

static double SUM(double *arr, int size)
{
    double sum = 0;
    for (int i = 0; i < size; i++)
        sum = sum + arr[i];
    return sum;
}
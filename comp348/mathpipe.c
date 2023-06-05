#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include "aggregate.h"
#include "singular.h"

#define BUFFER_SIZE 20
#define MAX_SIZE 1000

int lineCount = 1; // asume last line doesnt have \n
int lineCountFormat = 0;
double **jagged;
double **jaggedIfSize;
int *lineSizes;
int *lineSizesFormat;
char strData[1024] = "";
int indexSize = 0;
int size;
int prec = -1;
bool hasSize = false;
bool hasPrec = false;
bool isSingular = false;
bool isFilter = false;
char *command;
double threshold;
enum filter_type type;
double shiftBu;
int *newLines;

bool isAVG = false;
bool isPAVG = false;
bool isMIN = false;
bool isMAX = false;
bool isCOUNT = false;
bool isSUM = false;
bool isPRINT = false;
/*
to implement

 If an input row has more columns than the default size, the exceeding values are
automatically considered to be in a new row.

*/
int main(int argc, char *argv[])
{
    void retrieveDoubles(const char *str);
    void readStringLineByLine(const char *str);
    int createJaggedArray(const char *str);
    void formatJaggedArray();
    enum filter_type strToEnum(const char *str);
    int isDouble(const char *str);
    int cusPow(int base, int prec);
    double cusRound(double x);
    char buffer[BUFFER_SIZE];
    for (int i = 0; i < argc; i++)
    {
        sscanf(argv[i], "-size=%d", &size);
        sscanf(argv[i], "-prec=%d", &prec);
        if (prec < -1)
        {
            fprintf(stderr, "%s", "Error, negative precision argument\n");
            exit(1);
        }
        if (strcasecmp(argv[i], "print") == 0 || strcasecmp(argv[i], "filter") == 0 || strcasecmp(argv[i], "shift") == 0)
        {
            isSingular = true;
            command = (char *)malloc(sizeof(char) * 10);
            command = argv[i];
            if (strcasecmp(argv[i], "filter") == 0)
            {

                if (i + 3 > argc)
                {
                    fprintf(stderr, "%s", "FILTER called with missing arguments \nUsage: FILTER <TYPE> <threshold> \n");
                    exit(1);
                }
                isFilter = true;
                if (strcmp(argv[i + 1], "NEQ") != 0 && strcmp(argv[i + 1], "EQ") != 0 && strcmp(argv[i + 1], "GEQ") != 0 && strcmp(argv[i + 1], "LEQ") != 0 && strcmp(argv[i + 1], "LESS") != 0 && strcmp(argv[i + 1], "GREATER") != 0)
                {
                    printf("%s", argv[i + 1]);
                    printf("%d", strcmp(argv[i + 1], "EQ"));
                    fprintf(stderr, "%s", "FILTER called with no/invalid <TYPE> argument\n");
                    exit(1);
                }
                const char *strDouble = argv[i + 2];
                if (isDouble(strDouble) == 0)
                {
                    fprintf(stderr, "%s", "FILTER called with no/invalid <threshold> argument\n");
                    exit(1);
                }
                char *ptr;
                threshold = strtod(argv[i + 2], &ptr);
                char *strEnum = argv[i + 1];
                type = strToEnum(strEnum);
            }
            // TODO
            if (strcasecmp(argv[i], "shift") == 0)
            {
                if (i + 2 > argc)
                {
                    fprintf(stderr, "%s", "SHIFT called with no <SHIFT BY> value\nUsage: SHIFT <TYPE> <SHIFT BY>\n");
                }
                const char *strDouble = argv[i + 1];
                if (isDouble(strDouble) == 0)
                {
                    fprintf(stderr, "%s", "SHIFT called with no/invalid <threshold> argument\n");
                    exit(1);
                }
            }
            // printf("command: %s \n", command);
        }
        if (strcasecmp(argv[i], "avg") == 0)
        {
            isAVG = true;
        }
        else if (strcasecmp(argv[i], "pavg") == 0)
        {
            isPAVG = true;
        }
        else if (strcasecmp(argv[i], "sum") == 0)
        {
            isSUM = true;
        }
        else if (strcasecmp(argv[i], "count") == 0)
        {
            isCOUNT = true;
        }
        else if (strcasecmp(argv[i], "min") == 0)
        {
            printf("ye\n ");
            isMIN = true;
        }
        else if (strcasecmp(argv[i], "max") == 0)
        {
            isMAX = true;
        }
        else if (strcasecmp(argv[i], "print") == 0)
        {
            isPRINT = true;
            isSingular = true;
        }

        if (hasSize == false && size != 0)
            hasSize = true;
        if (hasPrec == false && prec != -1)
            hasPrec = true;
    }

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL)
    {
        const char *str = buffer;
        // printf("%s s",buffer);
        if (strcmp(buffer, "\n") == 0)
        {
            printf("ad\n");
            continue;
        }
        strcat(strData, str);
        char *newline = strchr(buffer, '\n');
        if (newline != NULL)
        {

            lineCount = lineCount + 1;
            *newline = '\0';
        }
    }
    const char *strDataRead = strData;
    lineSizes = (int *)malloc(sizeof(int) * lineCount);

    readStringLineByLine(strDataRead);
    createJaggedArray(strDataRead);
    if (hasSize)
    {

        formatJaggedArray();

        for (int i = 0; i < lineCountFormat && hasSize; i++)
        {
            if (isSingular)
            {
                if (isFilter)
                {
                    size_t newSize = filter(jaggedIfSize[i], lineSizesFormat[i], type, threshold);
                    print(jaggedIfSize[i], newSize);
                    continue;
                }
                if (isPRINT)
                {
                    print(jaggedIfSize[i], lineSizesFormat[i]);
                }
            }
            if (isMAX)
            {
                double returnVal = aggregate("MAX", jaggedIfSize[i], lineSizesFormat[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                    continue;
                }
                printf("%lf ", returnVal);
            }
            else if (isMIN)
            {
                double returnVal = aggregate("MIN", jaggedIfSize[i], lineSizesFormat[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                    continue;
                }
                printf("%lf ", returnVal);
            }
            else if (isAVG)
            {
                double returnVal = aggregate("AVG", jaggedIfSize[i], lineSizesFormat[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                    continue;
                }
                printf("%lf ", returnVal);
            }
            else if (isPAVG)
            {
                double returnVal = aggregate("PAVG", jaggedIfSize[i], lineSizesFormat[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                    continue;
                }
                printf("%lf ", returnVal);
            }
            else if (isCOUNT)
            {
                double returnVal = aggregate("COUNT", jaggedIfSize[i], lineSizesFormat[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                }
                printf("%lf ", returnVal);
            }
            else if (isSUM)
            {
                double returnVal = aggregate("SUM", jaggedIfSize[i], lineSizesFormat[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                    continue;
                }
                printf("%lf ", returnVal);
            }
        }
    }
    else
    {
        for (int i = 0; i < lineCount; i++)
        {
            if (isSingular)
            {
                if (isFilter)
                {
                    size_t newSize = filter(jagged[i], lineSizes[i], type, threshold);
                    print(jagged[i], newSize);
                }
            }
            if (isSingular)
            {
                if (isPRINT)
                {
                    print(jagged[i], lineSizes[i]);
                }
            }
            if (isMAX)
            {
                double returnVal = aggregate("MAX", jagged[i], lineSizes[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                    continue;
                }
                printf("%lf ", returnVal);
            }
            else if (isMIN)
            {
                double returnVal = aggregate("MIN", jagged[i], lineSizes[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                    continue;
                }
                printf("%lf ", returnVal);
            }
            else if (isAVG)
            {
                double returnVal = aggregate("AVG", jagged[i], lineSizes[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                    continue;
                }
                printf("%lf ", returnVal);
            }
            else if (isPAVG)
            {
                double returnVal = aggregate("PAVG", jagged[i], lineSizes[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                    continue;
                }
                printf("%lf ", returnVal);
            }
            else if (isCOUNT)
            {
                double returnVal = aggregate("COUNT", jagged[i], lineSizes[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                }
                printf("%lf ", returnVal);
            }
            else if (isSUM)
            {
                double returnVal = aggregate("SUM", jagged[i], lineSizes[i]);
                if (hasPrec)
                {
                    int power = cusPow(10, prec);
                    double rounded = cusRound(returnVal * power) / power;
                    printf("%.*lf ", prec, rounded);
                    continue;
                }
                printf("%lf ", returnVal);
            }
        }
        if (!isPRINT)
            printf("\n");
    }
    free(jagged);
    free(jaggedIfSize);
    free(lineSizes);
    free(lineSizesFormat);
    return 0;
}

int cusPow(int base, int prec)
{
    if (prec == 0)
    {
        return 1.0;
    }

    double result = base;
    int i;
    for (i = 1; i < prec; i++)
    {
        result *= base;
    }

    return result;
}

double cusRound(double x)
{
    if (x >= 0.0)
    {
        return (int)(x + 0.5);
    }
    else
    {
        return (int)(x - 0.5);
    }
}

int isDouble(const char *str)
{
    char *endptr;
    strtod(str, &endptr);
    if (*endptr == '\0')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

enum filter_type strToEnum(const char *str)
{
    if (strcmp(str, "EQ") == 0)
    {
        return EQ;
    }
    else if (strcmp(str, "NEQ") == 0)
    {
        return NEQ;
    }
    else if (strcmp(str, "GEQ") == 0)
    {
        return GEQ;
    }
    else if (strcmp(str, "LEQ") == 0)
    {
        return LEQ;
    }
    else if (strcmp(str, "LESS") == 0)
    {
        return LESS;
    }
    else if (strcmp(str, "GREATER") == 0)
    {
        return GREATER;
    }
    return EQ; // should never reach this point
}

void formatJaggedArray()
{
    free(jagged);
    int createJaggedArrayFormat();
    lineSizesFormat = (int *)malloc(sizeof(int) * MAX_SIZE);
    int lineSizesFormatIndex = 0;
    for (int i = 0; i < lineCount; i++)
    {

        int newLineSize;
        if (lineSizes[i] % size == 0)
        {
            newLineSize = lineSizes[i] / size;
            for (int j = 0; j < newLineSize; j++)
            {
                lineSizesFormat[lineSizesFormatIndex] = size;
                lineSizesFormatIndex = lineSizesFormatIndex + 1;
            }
        }
        else
        {
            newLineSize = (lineSizes[i] / size) + 1;
            for (int j = 0; j < newLineSize; j++)
            {
                if (j + 1 == newLineSize)
                {

                    int lastArrLen = lineSizes[i] - (size * (newLineSize - 1));
                    lineSizesFormat[lineSizesFormatIndex] = lastArrLen;
                    lineSizesFormatIndex = lineSizesFormatIndex + 1;
                    continue;
                }
                lineSizesFormat[lineSizesFormatIndex] = size;
                lineSizesFormatIndex = lineSizesFormatIndex + 1;
            }
        }
        lineCountFormat = lineCountFormat + newLineSize;
    }

    if (lineCount == lineCountFormat)
    {
        hasSize = false;
        return;
    }
    free(lineSizes);
    createJaggedArrayFormat();
}

int createJaggedArrayFormat()
{
    double getDoubles(const char *str, int count);
    int tempIndex = 0;
    jaggedIfSize = malloc(lineCountFormat * sizeof(double *));
    if (jaggedIfSize == NULL)
    {
        printf("memory allocation err for new format array");
        return 1;
    }
    for (int i = 0; i < lineCountFormat; i++)
    {
        if (lineSizesFormat[i] == 0)
        {
            printf("%d\n", size);
            jaggedIfSize[i] = malloc(size * sizeof(double));
            continue;
        }

        jaggedIfSize[i] = malloc(lineSizesFormat[i] * sizeof(double));
    }
    for (int i = 0; i < lineCountFormat; i++)
    {
        for (int j = 0; j < lineSizesFormat[i]; j++)
        {
            jaggedIfSize[i][j] = getDoubles(strData, tempIndex);
            tempIndex = tempIndex + 1;
        }
    }

    return 0;
}

int createJaggedArray(const char *str)
{
    double getDoubles(const char *str, int count);
    int countVal = 0;

    jagged = malloc(lineCount * sizeof(double *));
    if (jagged == NULL)
    {
        printf("memory allocation err");
        return 1;
    }
    for (int i = 0; i < lineCount; i++)
    {
        jagged[i] = malloc(lineSizes[i] * sizeof(double));
    }
    for (int i = 0; i < lineCount; i++)
    {
        for (int j = 0; j < lineSizes[i]; j++)
        {
            jagged[i][j] = getDoubles(strData, countVal);
            // printf("%lf x\n",jagged[i][j]);
            countVal = countVal + 1;
        }
    }
    return 0;
}

void retrieveDoubles(const char *str)
{
    // printf("------ \n");

    int lineSize = 0;
    char *endptr;
    double number;
    while (*str != '\0')
    {
        // if (*str == '\n')
        //     printf("new line \n");
        // Skip non-numeric characters
        while (*str != '\0' && !isdigit(*str) && *str != '.' && *str != '-')
        {
            str++;
        }

        // Convert the next double in the string
        number = strtod(str, &endptr);

        // If conversion succeeded, print the double
        if (str != endptr)
        {
            lineSize = lineSize + 1;
        }

        // Move to the next potential double in the string
        str = endptr;
        if (*str == '\0')
        {
            lineSizes[indexSize] = lineSize;
        }
    }
}

void readStringLineByLine(const char *str)
{

    char buffer[1024];
    int i = 0;
    int j = 0;

    while (str[i] != '\0')
    {
        if (str[i] == '\n')
        {
            buffer[j] = '\0';
            retrieveDoubles(buffer);
            indexSize = indexSize + 1;
            j = 0;
        }
        else
        {
            buffer[j] = str[i];
            j++;
        }
        i++;
    }
    if (j > 0)
    {
        buffer[j] = '\0';
        retrieveDoubles(buffer);
    }
}

double getDoubles(const char *str, int count)
{
    // printf("------ \n");
    int countCompare = 0;
    int lineSize = 0;
    char *endptr;
    double number;
    while (*str != '\0')
    {
        // if (*str == '\n')
        //     printf("new line \n");
        // Skip non-numeric characters
        while (*str != '\0' && !isdigit(*str) && *str != '.' && *str != '-')
        {
            str++;
        }

        // Convert the next double in the string
        number = strtod(str, &endptr);

        // If conversion succeeded, print the double
        if (str != endptr)
        {
            if (count == countCompare)
                return number;
            countCompare = countCompare + 1;
            // printf("%f\n", number);
            lineSize = lineSize + 1;
        }

        // Move to the next potential double in the string
        str = endptr;
    }
    printf("ERRRRRROR \n");
    return -1;
}

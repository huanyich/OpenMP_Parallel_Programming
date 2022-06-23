// MyOwnArray.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>


#define SIZE 1000000
#define NUMTRIES 10

float a[SIZE];
float b[SIZE];
float c[SIZE];
float sum;

int main()
{
    sum = 0.;
    for (int i = 0; i < SIZE; i++) {
        a[i] = i;
        b[i] = i;
    }

    double maxMegaMults = 0.;

    for (int t = 0; t < NUMTRIES; t++) {
        double time0 = omp_get_wtime();

        #pragma omp simd
        for (int i = 0; i < SIZE; i++) {
            sum = sum + a[i] * b[i];
        }

        double time1 = omp_get_wtime();
        double megaMults = (double)SIZE / (time1 - time0) / 1000000.;
        if (megaMults > maxMegaMults)
            maxMegaMults = megaMults;
    }

    printf("Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults);

    return 0;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


// print debugging messages?
#ifndef DEBUG
#define DEBUG	false
#endif

// setting the number of threads:
#ifndef NUMT
#define NUMT		    8
#endif

// setting the number of nodes
#ifndef NUMNODES
#define NUMNODES	3000
#endif

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES	10
#endif


#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

#define N	0.70

float Height(int, int);	// function prototype

float
Height(int iu, int iv)	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1. + 2. * (float)iu / (float)(NUMNODES - 1);	// -1. to +1.
	float y = -1. + 2. * (float)iv / (float)(NUMNODES - 1);	// -1. to +1.

	float xn = pow(fabs(x), (double)N);
	float yn = pow(fabs(y), (double)N);
	float r = 1. - xn - yn;
	if (r <= 0.)
		return 0.;
	float height = pow(r, 1. / (float)N);
	return height;
}

int main(int argc, char* argv[])
{

#ifndef _OPENMP
	fprintf(stderr, "No OpenMP support!\n");
	return 1;
#endif
	//. . .
	omp_set_num_threads(NUMT);
	double maxPerformance = 0;
	
		// the area of a single full-sized tile:
		// (not all tiles are full-sized, though)
	
		float fullTileArea = (((XMAX - XMIN) / (float)(NUMNODES - 1)) *
			((YMAX - YMIN) / (float)(NUMNODES - 1)));
		//printf("fullTileArea is %f\n", fullTileArea);
	
	/*
		float dx = ((XMAX - XMIN) / (float)(NUMNODES - 1));
		float dy = ((YMAX - YMIN) / (float)(NUMNODES - 1));
		float fullTileArea = dx * dy;
		printf("fullTileArea is %10.8lf:\n", fullTileArea);
	*/
		float HalfTileArea = fullTileArea / 2.;
		//printf("HalfTileArea is %f\n", HalfTileArea);

		float QuaterTileArea = fullTileArea / 4.;
		//printf("QuaterTileArea is %f\n", QuaterTileArea);
		float sum;
	for (int tries = 0; tries < NUMTRIES; tries++)
	{
			sum = 0;
			double time0 = omp_get_wtime();
#pragma omp parallel for collapse(2), default(none), reduction(+:sum)
			for (int iv = 0; iv < NUMNODES; iv++)
			{
				for (int iu = 0; iu < NUMNODES; iu++)
				{
					float z = Height(iu, iv);
					float volume;

					if (iu == 0 || iv == 0 || iu == NUMNODES - 1 || iv == NUMNODES - 1) {
						if ((iu == 0 && iv == 0) || (iu == 0 && iv == NUMNODES - 1) || (iu == NUMNODES - 1 && iv == 0) || (iu == NUMNODES - 1 && iv == NUMNODES - 1)) {

							//printf("it's QuaterTileArea,iu = %d, iv = %d, z=%f\n", iu, iv, z);
							volume = z * QuaterTileArea;
						}
						else {

							//printf("it's HalfTileArea,iu = %d, iv = %d, z=%f\n", iu, iv, z);
							volume = z * HalfTileArea;
						}

					}
					else {

						//printf("it's FullTileArea,iu = %d, iv = %d, z=%f\n", iu, iv, z);
						volume = z * fullTileArea;
					}

					sum += volume;



				}
			}

			double time1 = omp_get_wtime();
			float megaNumNodesPersecond = (double)(NUMNODES*NUMNODES) / (time1 - time0) / 1000000.;
			//printf("time1-time0 = %f\n", time1 - time0);
			//printf("megaTrialsPerSecond = %f\n", (double)NUMNODES / (time1 - time0)/1000000.);


			if (megaNumNodesPersecond > maxPerformance)
				maxPerformance = megaNumNodesPersecond;
	} // for ( # of timing tries )
		sum = sum * 2;
		fprintf(stderr, "%2d threads : %8d numnodes ; sum = %6.2f% ; meganodes/sec = %6.2f\n",
			NUMT, NUMNODES, sum, maxPerformance);
		
		//printf("it's sum, sum=%f\n", sum);
	// sum up the weighted heights into the variable "volume"
	// using an OpenMP for loop and a reduction:
		return 0;
}

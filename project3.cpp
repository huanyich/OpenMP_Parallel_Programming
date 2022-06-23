#include<iostream>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
using namespace std;

int	NowYear;		// 2021 - 2026
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
int NumDinosaur;

const float GRAIN_GROWS_PER_MONTH = 9.0;
const float ONE_DEER_EATS_PER_MONTH = 1.0;

const float AVG_PRECIP_PER_MONTH = 7.0;	// average
const float AMP_PRECIP_PER_MONTH = 6.0;	// plus or minus
const float RANDOM_PRECIP = 2.0;	// plus or minus noise

const float AVG_TEMP = 60.0;	// average
const float AMP_TEMP = 20.0;	// plus or minus
const float RANDOM_TEMP = 10.0;	// plus or minus noise

const float MIDTEMP = 40.0;
const float MIDPRECIP = 10.0;

unsigned int seed = 0;

omp_lock_t	Lock;
int		NumInThreadTeam;
int		NumAtBarrier;
int		NumGone;


//lock

//float x = Ranf(&seed, -1.f, 1.f);

float
Ranf(unsigned int* seedp, float low, float high)
{
	//float r = (float) rand_r(seedp);              // 0 - RAND_MAX
	float r = (float)rand();
	return(low + r * (high - low) / (float)RAND_MAX);
}


int
Ranf(unsigned int* seedp, int ilow, int ihigh)
{
	float low = (float)ilow;
	float high = (float)ihigh + 0.9999f;

	return (int)(Ranf(seedp, low, high));
}


void
InitBarrier(int n)
{
	NumInThreadTeam = n;
	NumAtBarrier = 0;
	omp_init_lock(&Lock);
}

void
WaitBarrier()
{
	omp_set_lock(&Lock);
	{
		NumAtBarrier++;
		if (NumAtBarrier == NumInThreadTeam)
		{
			NumGone = 0;
			NumAtBarrier = 0;
			// let all other threads get back to what they were doing
// before this one unlocks, knowing that they might immediately
// call WaitBarrier( ) again:
			while (NumGone != NumInThreadTeam - 1);
			omp_unset_lock(&Lock);
			return;
		}
	}
	omp_unset_lock(&Lock);

	while (NumAtBarrier != 0);	// this waits for the nth thread to arrive

#pragma omp atomic
	NumGone++;			// this flags how many threads have returned
}
//



float
SQR(float x)
{
	return x * x;
}
void Grain() {
	while (NowYear < 2027)
	{
		float tempFactor = exp(-SQR((NowTemp - MIDTEMP) / 10.));
		float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP) / 10.));



		float nextHeight = NowHeight;
		nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
		nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

		if (nextHeight < 0.) nextHeight = 0.;
		


		WaitBarrier();
			NowHeight = nextHeight;
		WaitBarrier();

		WaitBarrier();
	}
}

void Deer() {

	while (NowYear < 2027)
	{
		int nextNumDeer = NowNumDeer;
		int carryingCapacity = (int)(NowHeight);
		if (nextNumDeer < carryingCapacity)
			nextNumDeer++;
		else
			if (nextNumDeer > carryingCapacity)
				nextNumDeer--;

		if (nextNumDeer < 0)
			nextNumDeer = 0;

			// DoneComputing barrier:
		WaitBarrier();
		NowNumDeer = nextNumDeer;

			// DoneAssigning barrier:
		WaitBarrier();

			// DonePrinting barrier:
		WaitBarrier();
	}
}

void Watcher(){
	while (NowYear < 2027)
	{
		WaitBarrier();
		WaitBarrier();
		

		

		
		float ang = (30. * (float)NowMonth + 15.) * (M_PI / 180.);

		float temp = AVG_TEMP - AMP_TEMP * cos(ang);
		NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
		NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);
		if (NowPrecip < 0.)
			NowPrecip = 0.;

		NowMonth++;
		if (NowMonth == 12) {
			NowMonth = 0;
			NowYear++;
		}

		float C_temperature = (5. / 9.) * (NowTemp - 32);
		float P_Height = NowPrecip * 2.54;
		float G_Height = NowHeight * 2.54;
		cout << endl << "Month:" << NowMonth << endl << "Year:" << NowYear << endl << "Dinosaur:"<<NumDinosaur <<endl<<"Deer:" << NowNumDeer << endl << "Grain Height:" << G_Height << endl << "Temp:" << C_temperature << endl << "Precip:" << P_Height << endl<<endl;

		
		WaitBarrier();

		
	}

}

void Dinosaur() {
	while (NowYear < 2027)
	{	
		int tmpDinosaur = NumDinosaur;

		if (tmpDinosaur >= (NowNumDeer / 3)) {
			tmpDinosaur--;
		}
		else if (tmpDinosaur < (NowNumDeer / 3)) {
			tmpDinosaur++;
		}

		if (tmpDinosaur < 0) {
			tmpDinosaur = 0;
		}

		
		WaitBarrier();
		
		NumDinosaur = tmpDinosaur;
		WaitBarrier();
		WaitBarrier();
			
	}
}

int main() {

	

	// starting date and time:
	NowMonth = 0;
	NowYear = 2021;

	// starting state (feel free to change this if you want):
	NowNumDeer = 3;
	NowHeight = 1.;
	NumDinosaur = 4;

	float ang = (30. * (float)NowMonth + 15.) * (M_PI / 180.);

	float temp = AVG_TEMP - AMP_TEMP * cos(ang);
	NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
	NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);
	if (NowPrecip < 0.)
		NowPrecip = 0.;


	omp_set_num_threads(4);	// same as # of sections
	InitBarrier(4);
#pragma omp parallel sections
	{
#pragma omp section
		{
			Deer();
		}

#pragma omp section
		{
			Grain();
		}

#pragma omp section
		{
			Watcher();
		}

#pragma omp section
		{
			Dinosaur();	// your own
		}
		
	}       // implied barrier -- all functions must return in order
		// to allow any of them to get past here

	return 0;
}
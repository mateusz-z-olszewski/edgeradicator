#pragma once

#include <time.h>


static clock_t starttime, endtime;

void Timer() {
	starttime = clock();
}

double Measure() {
	endtime = clock();

	return ((double)(endtime - starttime)) / CLOCKS_PER_SEC;
}
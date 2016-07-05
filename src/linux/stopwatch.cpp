#include "stopwatch.h"



void StopWatch::resume() {
	runningQ_=true;

	struct timeval tv;
	gettimeofday(&tv, 0);

	last_start_ = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}



void StopWatch::pause() {
	runningQ_=false;

	struct timeval tv;
	gettimeofday(&tv, 0);

	t_ += ((tv.tv_sec * 1000 + tv.tv_usec / 1000) - last_start_);
}



unsigned long long StopWatch::ms() {
	return t_;
}


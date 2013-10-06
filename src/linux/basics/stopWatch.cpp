#include "stopWatch.h"


void StopWatch::resume()
{
	runningQ_=true;

        time_val tv;
        gettimeofday(&tv, NULL);

	last_start_ = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void StopWatch::pause()
{
	runningQ_=false;

        time_val tv;
        gettimeofday(&tv, NULL);

	t_ += ((tv.tv_sec * 1000 + tv.tv_usec / 1000) - last_start_);
}

unsigned long long StopWatch::ms()
{
	return t_;
}

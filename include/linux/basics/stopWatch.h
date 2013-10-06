#ifndef STOPWATCHH
#define STOPWATCHH

#include <sys/time.h>

// stop watch function for time keeping
class StopWatch {

	unsigned long long t_;
	unsigned long long last_start_;
	bool runningQ_;
public:
	StopWatch() {reset();};

	void reset() { t_=0; runningQ_ = false;};
	void resume();
	void pause();
	unsigned long long ms();
};


#endif

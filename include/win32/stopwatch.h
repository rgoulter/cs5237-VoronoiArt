#ifndef STOPWATCHH
#define STOPWATCHH

// We want to use std::min, std::max
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>



// stop watch function for time keeping
class StopWatch {

	ULONGLONG t_;
	ULONGLONG last_start_;
	bool runningQ_;
public:
	StopWatch() {reset();};

	void reset() { t_=0; runningQ_ = false;};
	void resume();
	void pause();
	ULONGLONG ms();
};


#endif

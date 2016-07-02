#ifndef STOPWATCHH
#define STOPWATCHH

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

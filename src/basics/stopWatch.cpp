#include "stopWatch.h"


void StopWatch::resume()
{
	runningQ_=true;
    SYSTEMTIME st;
    
    GetLocalTime(&st);
	last_start_ = (((st.wHour*60+st.wMinute)*60)+st.wSecond)*1000+st.wMilliseconds;
}

void StopWatch::pause()
{
	runningQ_=false;
    SYSTEMTIME st;
    
    GetLocalTime(&st);
	t_ += ((((st.wHour*60+st.wMinute)*60)+st.wSecond)*1000+st.wMilliseconds - last_start_);
}

ULONGLONG StopWatch::ms()
{
	return t_;
}
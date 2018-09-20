#include "Event.h"


Event::Event(bool isManual /* = true */)
{
	
	hEvent = CreateEvent(NULL, (isManual ? TRUE: FALSE), FALSE, NULL);
}

void Event::setEvent()
{
	::SetEvent(hEvent);
}

void Event::resetEvent()
{
	::ResetEvent(hEvent);
}

Event::~Event(void)
{
	CloseHandle(hEvent);
}

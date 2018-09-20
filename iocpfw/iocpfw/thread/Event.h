#ifndef _EVENT_H_
#define _EVENT_H_


#include <Windows.h>
#include "../common/Common.h"

		class Event
		{
		public:
			Event(bool isManual = false);
			~Event(void);
			void setEvent();
			void resetEvent();
			HANDLE getHandle() {return hEvent;}

		private:
			HANDLE hEvent;
		};




#endif


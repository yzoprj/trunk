
#ifndef _ANONYMOUS_PIPE_H_
#define _ANONYMOUS_PIPE_H_

#include <Winodws.h>

		class AnonymousPipe
		{
		public:
			AnonymousPipe(void);
			~AnonymousPipe(void);
			int read(){}
		private:
			HANDLE hAnonymousPipe;
		};



#endif





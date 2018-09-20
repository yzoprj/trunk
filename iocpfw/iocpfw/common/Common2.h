#ifndef COMMON2_H
#define COMMON2_H

#include <string>
namespace JKProtocol
{
	namespace Common
	{
		class RawBuffer;
	}
}

void postCommandMessage(const RawBuffer &rb);

void postRoadConnectedStatusMessage(unsigned int ip, int roadStatus);

#endif // COMMON2_H

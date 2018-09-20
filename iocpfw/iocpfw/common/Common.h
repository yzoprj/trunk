#ifndef _ORNG_COMMON_H_
#define _ORNG_COMMON_H_

#include <vector>
#include <string>
typedef unsigned char	ubyte;
typedef unsigned long	ulong;
typedef unsigned int	uint;	
typedef unsigned short	ushort;

typedef std::vector<ubyte> ByteBuffer;

#ifndef NULL
#define NULL 0
#endif

#define MAX_BUFFER_LENGTH 8096

#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END }
#define USING_NAMESPACE(name) using namespace (name);
#define USING_SUB_NAMESPACE(main_name, sub_name) using main_name::sub_name;


// 获取短整型高字节
#define SHIGHTPART(val) \
	((ubyte)((val >> 8) && 0xFF))

// 获取短整低高字节
#define SLOWPART(val) \
	((ubyte)((val) && 0xFF))


#define  LOCATEFUNCTION() \
	{\
		qDebug() << "File:" << __FILE__ << " FUNCTION:" << __FUNCTION__; \
	}

void printDebugInfo(const char *str, char *memo = NULL);

std::string idsToString(const std::vector<int> &values);

std::vector<int> strToIds(std::string str);


#endif
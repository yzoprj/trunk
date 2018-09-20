#include "Common.h"

#include <string>
#include <ctime>

using std::string;

void printDebugInfo(const char *str, char *memo)
{
	time_t tt;
	time(&tt);
	struct tm currentTime;
	localtime_s(&currentTime, &tt);
	int size = strlen(str); 
	char buffer[8096] = {0};
	if (memo == NULL)
	{
		memo = "Info";
	}
	sprintf_s(buffer, 8096, "[%04d-%02d-%02d %02d:%02d:%02d]:[%8s][%05d][%s]", currentTime.tm_year + 1900,
													currentTime.tm_mon + 1,
													currentTime.tm_mday,
													currentTime.tm_hour,
													currentTime.tm_min,
													currentTime.tm_sec,
													memo,
													size,
													str);

	//delete buffer;
}


string idsToString(const std::vector<int> &values)
{
	char buffer[32] = {0};
	std::string result;

	for(int i = 0; i < values.size(); i++)
	{
		sprintf(buffer, "%02X", values[i]);
		result += buffer;
	}

	return result;
}

std::vector<int> strToIds(string str)
{
	std::vector<int> vec;
	bool ok;
	if (str.size() % 2 != 0)
	{
		return std::vector<int>();
	}
	
	string strTemp;
	for (int i = 0; i < str.size() / 2 ; i++)
	{
		strTemp = str.substr(i * 2, 2);
		vec.push_back(strtol(strTemp.c_str(), NULL, 16));
	}

	return vec;
}

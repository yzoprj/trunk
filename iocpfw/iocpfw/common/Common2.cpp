//#include "../form/JKEInstructionDebugForm.h"
//#include "../../MainWindow.h"
//#include "../../App.h"
//#include <WinSock2.h>
//#include <Windows.h>
//void postCommandMessage(const JKProtocol::Common::RawBuffer &rb)
//{
//	if (JKEInstructionDebugForm::isInstanceExisted() == true)
//	{
//		JKEInstructionDebugForm::genInstance()->postCommnandMessage(rb);
//	}
//}
//
//
//void postRoadConnectedStatusMessage(unsigned int ip, int roadStatus)
//{
//
//	if (MainWindow::mainWinPtr == NULL)
//	{
//		return;
//	}
//
//	std::string strAddr = inet_ntoa(*(in_addr *)&ip);
//
//
//
//	std::map<std::string, RoadParameter>::iterator iter = App::mapRoads.begin();
//
//
//	while (iter != App::mapRoads.end())
//	{
//		if (iter->second.communicateParam.ipAddress == strAddr)
//		{
//			break;
//		}
//		iter++;
//	}
//
//	if (iter == App::mapRoads.end())
//	{
//		return;
//	}
//
//	MainWindow::mainWinPtr->postRoadConnectedStatus(iter->second.name, roadStatus);
//}
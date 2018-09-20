#ifndef SerialPort_h__
#define SerialPort_h__
#include "CommunicationInformation.h"

		class SerialPort
		{
		public:
			SerialPort(void);
			SerialPort(const SerialPort &sp);
			~SerialPort(void);
			bool init(int commName);
			unsigned int getHandle() { return _hComm;}
			//char* getCommName() const { return (char *)_commName;}
			CommunicatioInformation getCommunicationInformation() const;
			void setCommunicationInfomation(const CommunicatioInformation &ci);
			void close();
		private:
			unsigned int _hComm;
			//char _commName[32];
			CommunicatioInformation _ci;
		};



#endif // SerialPort_h__

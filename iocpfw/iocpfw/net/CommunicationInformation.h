#ifndef _COMMUNICATION_INFORMATION_H_
#define _COMMUNICATION_INFORMATION_H_
#include <cstring>
#include <string>

using std::string;


	enum SocketMode
	{
		SM_Null = 0,
		SM_TCP = 1,
		SM_UDP = 2,
	};

	enum ConnectionType
	{
		CT_Null,
		CT_Socket,
		CT_SerialPort
	};
	struct CommunicatioInformationBase
	{

	public:

		CommunicatioInformationBase() {}
		~CommunicatioInformationBase(){}
		ConnectionType getConnectType() {return connType;}
		void setConnectionType(ConnectionType connType)
		{
			connType = connType;
		}
	
		ConnectionType connType;
	};
	
	struct CommunicatioInformation : public   CommunicatioInformationBase
	{

		CommunicatioInformation(ConnectionType ct = CT_Socket)
		{
			this->connType = ct;
			sockMode = SM_Null; 
		}

		CommunicatioInformation(int address, ConnectionType connType, SocketMode sockMode, int port = 10018)
		{
			this->hostAddress = address;
			this->connType = connType;
			this->sockMode = sockMode;
			this->port = port;
		}

		void setHostAddress(int hostAddrsss)
		{
			this->hostAddress = hostAddress;
		}


		void setSocketMode(SocketMode sm)
		{
			this->sockMode = sm;
		}

		void setHostAddress(const char *hostAddress);
		void setPort(int port);
		//{
		//	this->port = port;
		//}

		static string generateKeyForSocket(int ip, int port, int sockMode)
		{
			char key[128] = {0};
			sprintf(key, "%08X%04X%02X", ip, port, sockMode);

			return key;
		}

		static string generateKeyForCommunication(int ip,  int sockMode, int connType)
		{
			char key[128] = {0};
			sprintf(key, "%08X%02X%02X", ip, sockMode, connType);

			return key;
		}
		

		string generateKeyForSocket()
		{
			return generateKeyForSocket(hostAddress, port, sockMode);
		}
			

		string generateKeyForCommunication()
		{
			return generateKeyForCommunication(hostAddress, sockMode, connType);
		}
		int hostAddress;
		int port;
		SocketMode sockMode;

	};

	//struct SocketInformation : public CommunicatioInformationBase
	//{
	//	SocketInformation(ConnectionType ct = CT_Socket)
	//	{
	//		this->connType = ct;
	//		memset(hostAddress, 0, 32);
	//	}
	//	
	//	void setHostAddress(char *address)
	//	{
	//		strcpy(hostAddress, address);
	//	}

	//	void setPort(int port)
	//	{
	//		this->port = port;
	//	}
	//private:

	//	char hostAddress[32];
	//	int port;
	//};

	//struct SerialPortInformation : public CommunicatioInformationBase
	//{
	//	SerialPortInformation()
	//	{
	//		connType = CT_SerialPort;
	//	}

	//	char comName[32];
	//};



#endif
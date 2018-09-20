#ifndef _SYNC_SOCKET_H_
#define _SYNC_SOCKET_H_
#include "CommunicationInformation.h"

	class AsyncSocket 
	{
	public:


		AsyncSocket(void);
		AsyncSocket(unsigned int handle, SocketMode sockMode = SM_TCP);
		~AsyncSocket(void);
		
		
		bool init(SocketMode sm,int timeout = 3, bool isAsync = true, bool isRemote = true);
		bool bindPort(int port);
		int	 getPort() const {return _ci.port;}
		void close();
		int connectRemote(const CommunicatioInformation &ci, bool isAsync = false);
		//char *getIPAddress() const;
		CommunicatioInformation getCommunicationInformation() const;
		void setCommunicationInfomation(const CommunicatioInformation &ci);
		void setSocketMode(SocketMode sm) { _ci.sockMode = sm;}
		SocketMode getSocketMode() const {return _ci.sockMode;}

		void setHandle(unsigned int handle);

		unsigned int getHandle() {return _handle;}
	protected:
		void setTimeout(int timeout);
		void getSockInfo(bool isRemote = true);
		
	private:
		unsigned int _handle;
		//char _ipAddr[32];
		//int hostAddress;
		//int _port;
		//SocketMode _sm;
		CommunicatioInformation _ci;

	};


#endif



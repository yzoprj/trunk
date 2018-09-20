#ifndef CommunicationSession_h__
#define CommunicationSession_h__
#include "ASyncSocket.h"
#include "SerialPort.h"
#include <cstring>
#include "CommunicationInformation.h"


		struct CommunicationSession
		{



			CommunicationSession(int count = 5, ConnectionType connType = CT_Null)
			{
				ct = connType;
				timeoutCount = count;

			}


			CommunicationSession(const CommunicationSession &src)
			{
				ct = src.ct;
				timeoutCount = timeoutCount;
			}

			virtual ~CommunicationSession() {}


			int getConnectionType() const { return (int)ct;}

			virtual CommunicatioInformation getCommunicatioInformation() const = 0;
			

			//static CommunicationSession NullCommunicationSession;
			ConnectionType ct;
			int timeoutCount;
		};

		struct SocketSession : public CommunicationSession
		{

	

			enum SocketSessionState
			{
				SS_NULL = 0,
				SS_Connected = 1,
				SS_Listen = 2,
				SS_DisConnect = -1,
				SS_Error = -2
				
			}  ;


			SocketSession()
			{
				sessionState = SS_NULL;				
			}

			SocketSession(const AsyncSocket &as)
			{
				
				session = as;
				ct = as.getCommunicationInformation().getConnectType();

				if (session.getHandle() != -1)
				{
					sessionState = SS_Connected;
				}else
				{
					sessionState = SS_NULL;
				}
			}

			SocketSession(const SocketSession &ss)
			{
				sessionState = ss.sessionState;
				session = ss.session;
				ct = ss.ct;
				timeoutCount = ss.timeoutCount;
			}

			~SocketSession(){}

			bool operator==(const SocketSession &ss)
			{
				CommunicatioInformation srcCi = session.getCommunicationInformation();
				CommunicatioInformation dstCi = ss.session.getCommunicationInformation();
				if ((srcCi.hostAddress == dstCi.hostAddress) 
					&& srcCi.port == dstCi.port
					&& srcCi.sockMode == dstCi.sockMode)
				{
					return true;
				}

				return false;
			}

			



			//char *getName() const { return this->getIPAddress();}

			//int getMode() const { return (int)session.getSocketMode();}

			CommunicatioInformation getCommunicatioInformation() const
			{
				return session.getCommunicationInformation();
			}
			AsyncSocket getSession() { return session;}
			//int getPort() const
			//{
			//	return session.getPort();
			//}
			//char* getIPAddress() const
			//{
			//	return session.getIPAddress();
			//}

			AsyncSocket session;
			SocketSessionState sessionState;
			//CommunicatioInformation remoteHostInfo;


			static SocketSession NullSession;
		};


		struct SerialPortSession : CommunicationSession
		{

			SerialPortSession(){}
			SerialPortSession(const SerialPort &sp)
			{
				session = sp;
			}

			SerialPortSession(const SerialPortSession &sps)
			{
				session = sps.session;
				ct = sps.ct;
				timeoutCount = sps.timeoutCount;
			}

			~SerialPortSession(){}

			bool operator==(const SerialPortSession &sps)
			{
				if (session.getCommunicationInformation().hostAddress == sps.session.getCommunicationInformation().hostAddress)
				{
					return true;
				}

				return false;
			}
			
			CommunicatioInformation getCommunicatioInformation() const
			{
				return session.getCommunicationInformation();
			}
			int getMode() const { return 0;}

			SerialPort getSession()
			{
				return session;
			}

			SerialPort session;
			static SerialPortSession NullSession;
		};


#endif // CommunicationSession_h__

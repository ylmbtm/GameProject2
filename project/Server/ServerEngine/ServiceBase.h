#ifndef __SERVICE_BASE_H__
#define __SERVICE_BASE_H__

#include "IBufferHandler.h"
#include "Connection.h"
#include "EventFuncManager.h"
#include "Utility/LockFreeQueue.h"
#include "Utility/IniFile/ConfigFile.h"

struct NetPacket
{
	NetPacket( CConnection *pConnect = NULL, IDataBuffer *pBuffer = NULL)
	{
		m_pConnect = pConnect;

		m_pDataBuffer = pBuffer;
	}

	CConnection *m_pConnect;

	IDataBuffer *m_pDataBuffer;
};

class ServiceBase : public IDataHandler, public CEventFuncManager
{
public:
	ServiceBase(void);
	virtual ~ServiceBase(void);
public:
	static ServiceBase* GetInstancePtr();
	
    BOOL            StartNetwork(UINT16 nPortNum, UINT32 nMaxConn);

    BOOL            StopNetwork();

	BOOL			OnDataHandle(IDataBuffer *pDataBuffer , CConnection *pConnection);

	BOOL			OnCloseConnect(CConnection *pConnection);

	BOOL			OnNewConnect(CConnection *pConnection);
public:
	BOOL			ConnectToOtherSvr(std::string strIpAddr, UINT16 sPort);

	BOOL			SendCmdToConnection(UINT64 u64ConnID, IDataBuffer *pSrcBuffer);

	BOOL			SendCmdToConnection(UINT64 u64ConnID, UINT64 u64CharID, UINT32 dwSceneID, IDataBuffer *pSrcBuffer );

	template<typename T>
	BOOL			SendCmdToConnection(UINT16 uCmdID, T &Data, UINT64 u64ConnID, UINT64 uCharID = 0,UINT32 dwSceneID = 0);

	CConnection*    GetConnectionByID(UINT64 u64ConnID);

	BOOL			Update();

	ArrayLockFreeQueue<NetPacket>      m_DataQueue;
	ArrayLockFreeQueue<CConnection*>   m_NewConList;
	ArrayLockFreeQueue<CConnection*>   m_CloseConList;
};


#endif /*__SERVICE_BASE_H__*/
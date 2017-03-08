#ifndef _GAME_SERVICE_H_
#define _GAME_SERVICE_H_
#include "ServiceBase.h"
#include "CharWillEnterList.h"
#include "ProxyCmdHandler.h"
class  CConnection;

class CGameService 
{
private:
	CGameService(void);
	virtual ~CGameService(void);

public:
	static CGameService* GetInstancePtr();

	BOOL		Init();

	BOOL		Uninit();

	BOOL		Run();

	BOOL		OnNewConnection(NetPacket *pPacket);

	BOOL		OnCloseConnection(NetPacket *pPacket);

	BOOL		SetWorldServerID(UINT32 dwSvrID);

public:
	BOOL		OnCmdGMCommand(UINT16 wCommandID, UINT64 u64ConnID, CBufferHelper *pBufferHelper);

	BOOL		OnCmdHeartBeatReq(UINT16 wCommandID, UINT64 u64ConnID, CBufferHelper *pBufferHelper);


public:
	//处理普通的网络连接

	CProxyCmdHandler	m_ProxyCmdHandler;

	CWillEnterNodeMgr   m_WillEnterNodeMgr;

	UINT32				m_dwWorldServerID;
};

#endif

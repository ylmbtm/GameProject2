#ifndef _GAME_SERVICE_H_
#define _GAME_SERVICE_H_
#include "ServiceBase.h"
#include "LoginCmdHandler.h"

class CGameService  : public IPacketDispatcher
{
private:
	CGameService(void);
	virtual ~CGameService(void);

public:
	static CGameService* GetInstancePtr();

public:

	BOOL		Init();

	BOOL		Uninit();

	BOOL		Run();

	BOOL		OnNewConnect(CConnection *pConn);

	BOOL		OnCloseConnect(CConnection *pConn);

	BOOL		DispatchPacket( NetPacket *pNetPacket);

	BOOL        SendCmdToDBConnection(IDataBuffer *pBuffer);

public:
	CLoginCmdHandler	m_LoginCmdHandler;


	CConnection        *m_pStatSvrConn;
	CConnection        *m_pDbSvrConn;
};

#endif

#ifndef _GAME_SERVICE_H_
#define _GAME_SERVICE_H_
#include "ServiceBase.h"
#include "LoginCmdHandler.h"

class CGameService 
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

	BOOL		OnNewConnection(NetPacket *pPacket);

	BOOL		OnCloseConnection(NetPacket *pPacket);

	BOOL        SendCmdToDBConnection(IDataBuffer *pBuffer);

public:
	CLoginCmdHandler	m_LoginCmdHandler;
};

#endif

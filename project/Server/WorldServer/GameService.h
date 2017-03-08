#ifndef _GAME_SERVICE_H_
#define _GAME_SERVICE_H_
#include "ServiceBase.h"
#include "WorldCmdHandler.h"

class CGameService :public ServiceBase
{
private:
	CGameService(void);
	virtual ~CGameService(void);

public:
	static CGameService* GetInstancePtr();

	BOOL		Init();

	BOOL		Run();


public:
	BOOL       SendCmdToDBConnection(IDataBuffer *pBuffer);
public:
	CWorldCmdHandler		m_WorldCmdHandler;
};

#endif

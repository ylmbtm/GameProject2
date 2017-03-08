#include "stdafx.h"
#include "GameService.h"
#include "CommandDef.h"
#include "Utility/Log/Log.h"
#include "Utility/CommonFunc.h"
#include "Utility/CommonEvent.h"
#include "PacketDef/ClientPacket.h"
#include "DataBuffer/BufferHelper.h"

CGameService::CGameService(void)
{

}

CGameService::~CGameService(void)
{
}

CGameService* CGameService::GetInstancePtr()
{
	static CGameService _GameService;

	return &_GameService;
}

BOOL CGameService::Init()
{
	CommonFunc::SetCurrentWorkPath("");

	if(!CLog::GetInstancePtr()->StartLog("WorldServer"))
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	if(!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
	{
		ASSERT_FAIELD;
		CLog::GetInstancePtr()->AddLog("配制文件加载失败!");
		return FALSE;
	}

	UINT16 nPort = CConfigFile::GetInstancePtr()->GetIntValue("world_svr_port");
	INT32  nMaxConn = CConfigFile::GetInstancePtr()->GetIntValue("world_svr_max_con");
	if(!ServiceBase::GetInstancePtr()->StartNetwork(nPort, nMaxConn))
	{
		ASSERT_FAIELD;
		CLog::GetInstancePtr()->AddLog("启动服务失败!");

		return FALSE;
	}

	UINT32 nStatPort = CConfigFile::GetInstancePtr()->GetIntValue("stat_svr_port");
	std::string strStatIp = CConfigFile::GetInstancePtr()->GetStringValue("stat_svr_ip");
	ServiceBase::GetInstancePtr()->ConnectToOtherSvr(strStatIp, nStatPort);


	UINT32 nDBPort = CConfigFile::GetInstancePtr()->GetIntValue("stat_svr_port");
	std::string strDBIp = CConfigFile::GetInstancePtr()->GetStringValue("db_svr_ip");
	ServiceBase::GetInstancePtr()->ConnectToOtherSvr(strDBIp, nDBPort);

	m_WorldCmdHandler.Init(0);

	return TRUE;
}

BOOL CGameService::Run()
{
    while(TRUE)
    {
        char sz[100];
        gets(sz);

        if(strcmp(sz,"exit") == 0)
        {
            CGameService::GetInstancePtr()->StopNetwork();
            break;
        }
    }

	return TRUE;
}

BOOL CGameService::SendCmdToDBConnection(IDataBuffer *pBuffer)
{
	return TRUE;
}

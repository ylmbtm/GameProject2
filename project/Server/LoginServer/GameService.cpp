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

	if(!CLog::GetInstancePtr()->StartLog("LoginServer","log"))
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	CLog::GetInstancePtr()->AddLog("---------服务器开始启动-----------");

	if(!CConfigFile::GetInstancePtr()->Load("servercfg.ini"))
	{
		ASSERT_FAIELD;
		CLog::GetInstancePtr()->AddLog("配制文件加载失败!");
		return FALSE;
	}

	UINT16 nPort = CConfigFile::GetInstancePtr()->GetIntValue("login_svr_port");
	INT32  nMaxConn = CConfigFile::GetInstancePtr()->GetIntValue("login_svr_max_con");
    if(!ServiceBase::GetInstancePtr()->StartNetwork(nPort, nMaxConn))
    {
        ASSERT_FAIELD;
        CLog::GetInstancePtr()->AddLog("启动服务失败!");

		return FALSE;
	}

	UINT32 nStatPort = CConfigFile::GetInstancePtr()->GetIntValue("stat_svr_port");
	std::string strStatIp = CConfigFile::GetInstancePtr()->GetStringValue("stat_svr_ip");
	ServiceBase::GetInstancePtr()->ConnectToOtherSvr(strStatIp, nStatPort);

	m_LoginCmdHandler.Init();

	ServiceBase::GetInstancePtr()->RegisterMessageHandle(CMD_NEW_CONNECTION, &CGameService::OnNewConnection, this);
	ServiceBase::GetInstancePtr()->RegisterMessageHandle(CMD_CLOSE_CONNECTION, &CGameService::OnCloseConnection, this);

	return TRUE;
}


BOOL CGameService::Uninit()
{
	return TRUE;
}

BOOL CGameService::Run()
{
	while(TRUE)
	{
		ServiceBase::GetInstancePtr()->Update();

		Sleep(1);
	}

	return TRUE;
}


BOOL CGameService::SendCmdToDBConnection(IDataBuffer *pBuffer)
{
	return TRUE;
}

BOOL CGameService::OnNewConnection(NetPacket *pPacket)
{
	CLog::GetInstancePtr()->AddLog("新连接来到!");
	return TRUE;
}

BOOL CGameService::OnCloseConnection(NetPacket *pPacket)
{
	CLog::GetInstancePtr()->AddLog("断开连接!");
	return TRUE;
}
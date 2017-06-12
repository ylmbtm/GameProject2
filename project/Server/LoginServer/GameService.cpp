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
    if(!ServiceBase::GetInstancePtr()->StartNetwork(nPort, nMaxConn,this))
    {
        ASSERT_FAIELD;
        CLog::GetInstancePtr()->AddLog("启动服务失败!");

		return FALSE;
	}

	UINT32 nStatPort = CConfigFile::GetInstancePtr()->GetIntValue("stat_svr_port");
	std::string strStatIp = CConfigFile::GetInstancePtr()->GetStringValue("stat_svr_ip");
	m_pStatSvrConn = ServiceBase::GetInstancePtr()->ConnectToOtherSvr(strStatIp, nStatPort);
	if(m_pStatSvrConn == NULL)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	UINT32 nAccountPort = CConfigFile::GetInstancePtr()->GetIntValue("account_svr_port");
	std::string strAccountIp = CConfigFile::GetInstancePtr()->GetStringValue("account_svr_ip");
	m_pAccountSvrConn = ServiceBase::GetInstancePtr()->ConnectToOtherSvr(strAccountIp, nAccountPort);
	if(m_pAccountSvrConn == NULL)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	m_LoginCmdHandler.Init();

	

	return TRUE;
}


BOOL CGameService::Uninit()
{
	ServiceBase::GetInstancePtr()->StopNetwork();
	return TRUE;
}

BOOL CGameService::Run()
{
	while(TRUE)
	{
		ServiceBase::GetInstancePtr()->Update();

		CommonThreadFunc::Sleep(1);
	}

	return TRUE;
}


BOOL CGameService::SendCmdToAccountConnection(IDataBuffer *pBuffer)
{
	if(m_pAccountSvrConn == NULL || !m_pAccountSvrConn->IsConnectionOK())
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	m_pAccountSvrConn->SendBuffer(pBuffer);

	return TRUE;
}

BOOL CGameService::OnNewConnect(CConnection *pConn)
{
	CLog::GetInstancePtr()->AddLog("新连接来到!");
	return TRUE;
}

BOOL CGameService::OnCloseConnect(CConnection *pConn)
{
	CLog::GetInstancePtr()->AddLog("断开连接!");
	return TRUE;
}

BOOL CGameService::DispatchPacket(NetPacket *pNetPacket)
{
	switch(pNetPacket->m_dwCmdID)
	{
	default:
		{

		}
		break;
	}

	return TRUE;
}
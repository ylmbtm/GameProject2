#include "stdafx.h"
#include "CommandDef.h"
#include "GameDefine.h"
#include "Error.h"
#include "StatCmdHandler.h"
#include "Utility/Log/Log.h"
#include "Utility/CommonFunc.h"
#include "Utility/CommonEvent.h"
#include "PacketDef/ClientPacket.h"
#include "GameService.h"
#include "PacketDef/PacketHeader.h"
#include "PacketDef/DBPacket.h"
#include "Utility/CommonSocket.h"
#include "DataBuffer/BufferHelper.h"
#include "DataBuffer/DataBuffer.h"
#include "PacketDef/SvrReportPacket.h"

CStatCmdHandler::CStatCmdHandler()
{

}

CStatCmdHandler::~CStatCmdHandler()
{

}

BOOL CStatCmdHandler::Init(UINT32 dwReserved)
{
	return TRUE;
}

BOOL CStatCmdHandler::Uninit()
{
	return TRUE;
}



BOOL CStatCmdHandler::DispatchPacket(NetPacket *pNetPacket)
{
	switch(pNetPacket->m_dwCmdID)
	{
		PROCESS_COMMAND_ITEM(CMD_CHAR_ENTER_GAME_REQ,		OnCmdSvrRunningStateReq);
	default:
		{
			
		}
		break;
	}

	return TRUE;
}



BOOL CStatCmdHandler::OnCmdSvrRunningStateReq(NetPacket *pPacket)
{
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);

	UINT32 dwReportID = 0;
	bh.Read(dwReportID);
	bh.Seek(0 - sizeof(dwReportID));

	switch(dwReportID)
	{
	case SVR_REPROT_NORMAL:
		{
			OnSvrReport_NormalLog(&bh);
		}
		break;
	default:
		{

		}
		break;
	}


	return TRUE;
}

BOOL CStatCmdHandler::OnSvrReport_NormalLog(CBufferHelper *pBufferHelper)
{
	StNormalReport NormalReport;

	pBufferHelper->Read(NormalReport);

	printf("SVRID:%d, SVRTYPE:%d, REPORTID:%d, CONTENT:%s", NormalReport.dwSvrID, NormalReport.dwSvrType, NormalReport.dwReportID, NormalReport.szLogContent);

	return TRUE;
}




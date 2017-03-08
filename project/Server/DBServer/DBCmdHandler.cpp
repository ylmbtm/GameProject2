#include "stdafx.h"
#include "CommandDef.h"
#include "GameDefine.h"
#include "Error.h"
#include "DBCmdHandler.h"
#include "Utility/Log/Log.h"
#include "Utility/CommonFunc.h"
#include "Utility/CommonEvent.h"
#include "PacketDef/PacketHeader.h"
#include "PacketDef/ClientPacket.h"
#include "GameService.h"
#include "Utility/CommonSocket.h"

#include "DataBuffer/BufferHelper.h"
#include "DataBuffer/DataBuffer.h"
#include "PacketDef/DBPacket.h"




CDBCmdHandler::CDBCmdHandler()
{

}

CDBCmdHandler::~CDBCmdHandler()
{

}

BOOL CDBCmdHandler::Init(UINT32 dwReserved)
{
	if(!m_DBProcManager.Init())
	{
		return FALSE;
	}

	

	return TRUE;
}

BOOL CDBCmdHandler::Uninit()
{
	m_DBProcManager.Uninit();

	return TRUE;
}


BOOL CDBCmdHandler::DispatchPacket(NetPacket *pNetPacket)
{
	switch(pNetPacket->m_dwCmdID)
	{
		PROCESS_COMMAND_ITEM(CMD_DB_NEW_ACCOUNT_REQ,	OnCmdDBNewAccountReq);
		PROCESS_COMMAND_ITEM(CMD_DB_NEW_CHAR_REQ,		OnCmdDBNewCharReq);
		PROCESS_COMMAND_ITEM(CMD_DB_DEL_CHAR_REQ,		OnCmdDBDelCharReq);
		PROCESS_COMMAND_ITEM(CMD_DB_PICK_CHAR_REQ,		OnCmdDBPickCharReq);
		PROCESS_COMMAND_ITEM(CMD_DB_LOGIN_REQ,			OnCmdDBLoginReq);
		PROCESS_COMMAND_ITEM(CMD_DB_LOAD_CHAR_REQ,		OnCmdDBLoadCharReq);
		PROCESS_COMMAND_ITEM(CMD_DB_SAVE_CHAR_REQ,		OnCmdDBSaveCharReq);
	default:
		{

		}
		break;
	}


	return TRUE;
}

BOOL CDBCmdHandler::OnUpdate(UINT32 dwTick)
{
	return TRUE;
}

BOOL CDBCmdHandler::OnCmdDBNewAccountReq(NetPacket *pPacket)
{
	StDBNewAccountReq DBNewAccountReq;

	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(DBNewAccountReq);

	ASSERT(DBNewAccountReq.u64ConnID != 0);

	StDBNewAccountAck DBNewAccountAck;

	if(m_DBProcManager.CreateAccount(DBNewAccountReq.CharNewAccountReq.szAccountName, DBNewAccountReq.CharNewAccountReq.szPassword))
	{
		DBNewAccountAck.CharNewAccountAck.nRetCode = E_SUCCESSED;
	}
	else
	{
		DBNewAccountAck.CharNewAccountAck.nRetCode = E_FAILED;
	}

	DBNewAccountAck.u64ConnID = DBNewAccountReq.u64ConnID;

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_DB_NEW_ACCOUNT_ACK, 0, 0);
	WriteHelper.Write(DBNewAccountAck);
	WriteHelper.EndWrite();
	ServiceBase::GetInstancePtr()->SendCmdToConnection(pPacket->m_pConnect->GetConnectionID(), WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CDBCmdHandler::OnCmdDBNewCharReq(NetPacket *pPacket)
{
	StDBNewCharReq DBNewCharReq;
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(DBNewCharReq);

	StDBCharNewCharAck DBCharNewCharAck;
	DBCharNewCharAck.u64ConnID = DBNewCharReq.u64ConnID;

	if(!m_DBProcManager.CreateNewChar(DBNewCharReq.CharNewCharReq, DBCharNewCharAck.CharNewCharAck))
	{
		DBCharNewCharAck.CharNewCharAck.nRetCode = E_FAILED;
	}
	else
	{
		DBCharNewCharAck.CharNewCharAck.nRetCode = E_SUCCESSED;
	}

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_DB_NEW_CHAR_ACK, 0, 0);
	WriteHelper.Write(DBCharNewCharAck);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetDataLenth() >= (sizeof(DBCharNewCharAck)+22));
	ServiceBase::GetInstancePtr()->SendCmdToConnection(pPacket->m_pConnect->GetConnectionID(), WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CDBCmdHandler::OnCmdDBPickCharReq(NetPacket *pPacket)
{
	StDBCharPickCharReq DBCharPickCharReq;
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(DBCharPickCharReq);

	//需要先取出账号ID, 查询账号ID下是否己有角色登录
	//如果己有角色登录  则将原角色踢掉
	//如查现在登录的角色和原角色相同，

	StDBCharPickCharAck DBCharPickCharAck;
	DBCharPickCharAck.u64ConnID = DBCharPickCharReq.u64ConnID;
	DBCharPickCharAck.CharPickCharAck.u64CharID = DBCharPickCharReq.CharPickCharReq.u64CharID;
	DBCharPickCharAck.CharPickCharAck.nRetCode = E_SUCCESSED;


	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_DB_PICK_CHAR_ACK, 0, 0);
	WriteHelper.Write(DBCharPickCharAck);
	WriteHelper.EndWrite();
	ASSERT(WriteHelper.GetDataBuffer()->GetDataLenth() >= (sizeof(DBCharPickCharAck)+22));
	ServiceBase::GetInstancePtr()->SendCmdToConnection(pPacket->m_pConnect->GetConnectionID(), WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CDBCmdHandler::OnCmdDBLoginReq(NetPacket *pPacket)
{	
	StDBCharLoginReq DBCharLoginReq;
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(DBCharLoginReq);

	StDBCharLoginAck DBCharLoginAck;
	DBCharLoginAck.u64ConnID = DBCharLoginReq.u64ConnID;
	
	UINT32 dwAccountID = m_DBProcManager.VerifyAccount(DBCharLoginReq.CharLoginReq.szAccountName, DBCharLoginReq.CharLoginReq.szPassword);
	if(dwAccountID == 0)
	{
		DBCharLoginAck.CharLoginAck.nRetCode = E_FAILED;
		DBCharLoginAck.CharLoginAck.dwAccountID = 0;
		DBCharLoginAck.CharLoginAck.nCount = 0;
	}
	else
	{
		m_DBProcManager.LoadAccountCharInfo(dwAccountID, DBCharLoginAck.CharLoginAck);
		DBCharLoginAck.CharLoginAck.nRetCode = E_SUCCESSED;
		DBCharLoginAck.CharLoginAck.dwAccountID = dwAccountID;
		//如果这个己有登录角色，则直接把原来的踢掉

		//如果只是登录，还没有先择角色，刚必须要近快完成登录过程，长时间不登录，将被断开的连接

	}

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_DB_LOGIN_ACK, 0, 0);
	WriteHelper.Write(DBCharLoginAck);
	WriteHelper.EndWrite();
	ServiceBase::GetInstancePtr()->SendCmdToConnection(pPacket->m_pConnect->GetConnectionID(), WriteHelper.GetDataBuffer());

	return TRUE;
}

BOOL CDBCmdHandler::OnCmdDBDelCharReq(NetPacket *pPacket)
{
	StDBDelCharReq DBDelCharReq;
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(DBDelCharReq);

	StDBCharDelCharAck DBCharDelCharAck;
	DBCharDelCharAck.u64ConnID = DBDelCharReq.u64ConnID;

	if(!m_DBProcManager.DelChar(DBDelCharReq.CharDelCharReq))
	{
		DBCharDelCharAck.CharDelCharAck.nRetCode = E_FAILED;
	}
	else
	{
		DBCharDelCharAck.CharDelCharAck.nRetCode = E_SUCCESSED;
	}

	CBufferHelper WriteHelper(TRUE, 1024);
	WriteHelper.BeginWrite(CMD_DB_DEL_CHAR_ACK, 0, 0);
	WriteHelper.Write(DBCharDelCharAck);
	WriteHelper.EndWrite();
	ServiceBase::GetInstancePtr()->SendCmdToConnection(pPacket->m_pConnect->GetConnectionID(), WriteHelper.GetDataBuffer());
	return TRUE;
}


BOOL CDBCmdHandler::OnCmdDBLoadCharReq(NetPacket *pPacket)
{
	StDBLoadCharInfoReq DBLoadCharInfoReq;
	CBufferHelper bh(FALSE, pPacket->m_pDataBuffer);
	bh.Read(DBLoadCharInfoReq);

	StDBLoadCharInfoAck DBLoadCharInfoAck;
	DBLoadCharInfoAck.dwProxySvrID = DBLoadCharInfoReq.dwProxySvrID;
	DBLoadCharInfoAck.u64CharID = DBLoadCharInfoReq.u64CharID;

	CBufferHelper WriteHelper(TRUE, 8096);
	WriteHelper.BeginWrite(CMD_DB_LOAD_CHAR_ACK, 0,DBLoadCharInfoReq.u64CharID);

	WriteHelper.Write(DBLoadCharInfoAck);

	CDBPlayerObject *pDBPlayer = m_DBPlayerMgr.GetPlayer(DBLoadCharInfoReq.u64CharID);
	if(pDBPlayer == NULL)
	{
		//读取一条记录，
		//读出成功
		pDBPlayer = m_DBPlayerMgr.CreatePlayerByID(DBLoadCharInfoReq.u64CharID);

		pDBPlayer->Init();

		pDBPlayer->m_u64ObjectID = DBLoadCharInfoReq.u64CharID;

        CHAR szSql[MAX_SQL_LEN];
        sprintf(szSql, "select * from t_charinfo where F_CharID = '%lld'", DBLoadCharInfoReq.u64CharID);

		CppSQLite3Query QueryRes = m_DBProcManager.m_DBConnection.execQuery(szSql);

		if(!QueryRes.eof())
		{
			pDBPlayer->m_dwFeature = QueryRes.getIntField("F_Feature", 0);
			strncpy(pDBPlayer->m_szObjectName, QueryRes.getStringField("F_Name", ""), 32);
			pDBPlayer->m_dwLevel = QueryRes.getIntField("F_Level", 0);
			pDBPlayer->m_dwSceneID = QueryRes.getIntField("F_SceneID", 0);
		}
	}

	pDBPlayer->WriteToDBPacket(&WriteHelper);

	WriteHelper.EndWrite();
	
	ServiceBase::GetInstancePtr()->SendCmdToConnection(pPacket->m_pConnect->GetConnectionID(), WriteHelper.GetDataBuffer());
	
	return TRUE;
}


BOOL CDBCmdHandler::OnCmdDBSaveCharReq(NetPacket *pPacket)
{

	return TRUE;
}

BOOL CDBCmdHandler::LoadPlayerBaseData( UINT64 u64CharID )
{

	return TRUE;
}


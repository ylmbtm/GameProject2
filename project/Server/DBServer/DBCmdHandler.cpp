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
#include "DBStoredProcMgr.h"




CDBCmdHandler::CDBCmdHandler()
{

}

CDBCmdHandler::~CDBCmdHandler()
{

}

BOOL CDBCmdHandler::Init(UINT32 dwReserved)
{
	if(!CCommonCmdHandler::Init(dwReserved))
	{
		return FALSE;
	}

	if(!m_DBProcManager.Init())
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDBCmdHandler::Uninit()
{
	CCommonCmdHandler::Uninit();

	m_DBProcManager.Uninit();

	return TRUE;
}


BOOL CDBCmdHandler::DispatchPacket(NetPacket *pNetPacket)
{
	switch(pNetPacket->m_dwCmdID)
	{
		PROCESS_COMMAND_ITEM(CMD_DB_NEW_CHAR_REQ,		OnCmdDBNewCharReq);
		PROCESS_COMMAND_ITEM(CMD_DB_DEL_CHAR_REQ,		OnCmdDBDelCharReq);
		PROCESS_COMMAND_ITEM(CMD_DB_PICK_CHAR_REQ,		OnCmdDBPickCharReq);
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

BOOL CDBCmdHandler::OnThreadBegin()
{
	m_DBConnection.Init();

	m_DBConnection.Connect("127.0.0.1","root","123456", "db_log", 3306);
	m_DBProceduceMgr.InitStoredProcedures();
	CDBStoredProcedure *pProcedure = NULL;
	
 	pProcedure = m_DBProceduceMgr.GetStoredProcedure(DB_INSERT_PLAYER_INFO);
 	pProcedure->set_int8(0, 10);
	pProcedure->set_string(1, "test", 5);
	pProcedure->set_int32(2, 10);
	pProcedure->set_int32(3, 10);
 	m_DBConnection.Execute(pProcedure);
 
	while(pProcedure->m_DBRecordSet.MoveNext())
	{
		int nValue = pProcedure->m_DBRecordSet.get_int32((size_t)0);
		printf("%d", nValue);
	}

	pProcedure->m_DBRecordSet.ClearRecordSet();

 	//pProcedure->m_DBRecordSet.MoveNext(0);
 


	/*
	pProcedure = m_DBProceduceMgr.GetStoredProcedure(DB_FIND_PLAYER_INFO);
	pProcedure->set_int32(0, 2);
	m_DBConnection.Query(pProcedure);


	int n = pProcedure->m_DBRecordSet.GetRowCount();

	while(pProcedure->m_DBRecordSet.MoveNext())
	{
		int nValue = pProcedure->m_DBRecordSet.get_int32("id");
		nValue = pProcedure->m_DBRecordSet.get_int32("fb");
		nValue = pProcedure->m_DBRecordSet.get_int32("fi");
		char * p = pProcedure->m_DBRecordSet.get_string("fc");

		printf(p);
	}
	*/

	return TRUE;
}

BOOL CDBCmdHandler::OnThreadEnd()
{
	m_DBConnection.Uninit();

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
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBCharNewCharAck)+22));
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
	ASSERT(WriteHelper.GetDataBuffer()->GetTotalLenth() >= (sizeof(DBCharPickCharAck)+22));
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


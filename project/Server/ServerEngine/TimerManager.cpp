#include "stdafx.h"
#include "TimerManager.h"
#include "../../Common/Utility/CommonFunc.h"



TimerManager::TimerManager()
{
	m_pHead = NULL;

	m_pFree = NULL;
}

TimerManager::~TimerManager()
{

}

BOOL TimerManager::AddFixTimer(UINT32 dwTimerID, UINT32 dwData, UINT32 dwHour, UINT32 dwMin, UINT32 dwSec)
{
	TimeEvent *pNewEvent = NULL;
	if(m_pFree == NULL)
	{
		pNewEvent = new TimeEvent;
	}
	else
	{
		pNewEvent = m_pFree;
		m_pFree = m_pFree->m_pNext;
		m_pFree->m_pPrev = NULL;
	}

	pNewEvent->m_pNext = NULL;
	pNewEvent->m_pPrev = NULL;

	pNewEvent->m_dwTimerID = dwTimerID;
	pNewEvent->m_dwData = dwData;
	pNewEvent->m_dwFireTime = CommonFunc::GetDayTime() + dwHour * 3600 + dwMin * 60 + dwSec;

	pNewEvent->m_dwHour = dwHour;
	pNewEvent->m_dwMin = dwMin;
	pNewEvent->m_dwSec = dwSec;
	pNewEvent->m_dwType = 1;

	if(m_pHead == NULL)
	{
		m_pHead = pNewEvent;
	}
	else
	{
		pNewEvent->m_pNext = m_pHead;
		m_pHead->m_pPrev = pNewEvent;
		m_pHead = pNewEvent;
		m_pHead->m_pPrev = NULL;
	}

	//TimeEvent *pInserPos = m_pHead;
	//while(pInserPos != NULL)
	//{
	//	if(pNewEvent->dwFireTime < pInserPos->dwFireTime)
	//	{
	//		
	//
	//		return TRUE;
	//	}
	//}

	return TRUE;
}

BOOL TimerManager::AddDiffTimer(UINT32 dwTimerID, UINT32 dwData, UINT32 dwHour, UINT32 dwMin, UINT32 dwSec)
{
	TimeEvent *pNewEvent = NULL;
	if(m_pFree == NULL)
	{
		pNewEvent = new TimeEvent;
	}
	else
	{
		pNewEvent = m_pFree;
		m_pFree = m_pFree->m_pNext;
		m_pFree->m_pPrev = NULL;
	}

	pNewEvent->m_pNext = NULL;
	pNewEvent->m_pPrev = NULL;

	pNewEvent->m_dwTimerID = dwTimerID;
	pNewEvent->m_dwData = dwData;

	pNewEvent->m_dwFireTime = CommonFunc::GetCurrTime() + dwHour * 3600 + dwMin * 60 + dwSec;

	pNewEvent->m_dwHour = dwHour;
	pNewEvent->m_dwMin = dwMin;
	pNewEvent->m_dwSec = dwSec;
	pNewEvent->m_dwType = 2;
	if(m_pHead == NULL)
	{
		m_pHead = pNewEvent;
	}
	else
	{
		pNewEvent->m_pNext = m_pHead;
		m_pHead->m_pPrev = pNewEvent;
		m_pHead = pNewEvent;
		m_pHead->m_pPrev = NULL;
	}

	return TRUE;
}

BOOL TimerManager::DelTimer( UINT32 dwTimerID )
{
	if(m_pHead == NULL)
	{
		return TRUE;
	}

	if(m_pHead->m_dwTimerID == dwTimerID)
	{
		TimeEvent *pEvent = m_pHead;

		m_pHead = m_pHead->m_pNext;

		pEvent->m_pNext = m_pFree->m_pNext;

		m_pFree = pEvent;

		return TRUE;
	}


	TimeEvent *pEvent = m_pHead;

	while(pEvent->m_pNext != NULL)
	{
		if(pEvent->m_pNext->m_dwTimerID == dwTimerID)
		{
			TimeEvent *pDelEvent = pEvent->m_pNext;

			pEvent->m_pNext = pEvent->m_pNext->m_pNext;

			pDelEvent->m_pNext = m_pFree->m_pNext;

			m_pFree = pDelEvent;

			return TRUE;
		}


		pEvent = pEvent->m_pNext;
	}

	return FALSE;
}

VOID TimerManager::UpdateTimer()
{
	m_dwCurTime = CommonFunc::GetCurrTime();

	TimeEvent *pCurEvent = m_pHead;
	while(pCurEvent != NULL)
	{
		if(m_dwCurTime >= pCurEvent->m_dwFireTime)
		{
			OnTimerEvent(pCurEvent);

			pCurEvent->m_dwRepeateTimes -= 1;
		}

		if(pCurEvent->m_dwRepeateTimes <= 0)
		{
			if(pCurEvent->m_dwType == 1)
			{
				pCurEvent->m_dwFireTime = pCurEvent->m_dwFireTime + 86400;
			}
			else
			{
				pCurEvent->m_dwFireTime = pCurEvent->m_dwFireTime + pCurEvent->m_dwHour*3600+ pCurEvent->m_dwMin*60 + pCurEvent->m_dwSec;
			}
		}
	}
}


VOID TimerManager::OnNewDay( UINT32 dwTimerID, UINT32 dwData)
{

	return ;
}

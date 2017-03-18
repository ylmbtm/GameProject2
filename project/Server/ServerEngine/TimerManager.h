#ifndef _TIMER_MANAGER__
#define _TIMER_MANAGER__

#define ID_NEWDAY  1

struct TimeEvent
{
public:
	TimeEvent()
	{
		m_dwTimerID      = 0;
		m_dwFireTime     = 0;
		m_dwHour		 = 0;
		m_dwMin			 = 0;
		m_dwSec			 = 0;
		m_dwData         = 0;
		m_pNext          = NULL;
		m_pPrev          = NULL;
		m_dwRepeateTimes = 0;
	}

	UINT32 m_dwFireTime;  //触发时间
	UINT32 m_dwHour, m_dwMin, m_dwSec;
	UINT32 m_dwTimerID;
	UINT32 m_dwData;
	TimeEvent *m_pPrev; //前一节点
	TimeEvent *m_pNext; //后一节点

	UINT32  m_dwType;   //事件类型,1 绝对时间定时器,2 相对时间定时器
	INT32   m_dwRepeateTimes;
};

#define BEGIN_TIMER_PROCESS VOID OnTimerEvent( TimeEvent *pEvent ){if(pEvent == NULL){return ;}switch(pEvent->m_dwTimerID){
#define PROCESS_TIMER_ITEM(TIMER_ID, FUNC) case TIMER_ID:{FUNC(pEvent->m_dwTimerID, pEvent->m_dwData);}break;
#define END_TIMER_PROCESS default:{}break;}}

class TimerManager
{
public:
	TimerManager();

	~TimerManager();

public:
	BOOL AddFixTimer(UINT32 dwTimerID, UINT32 dwHour, UINT32 dwMin, UINT32 dwSec, UINT32 dwData);

	BOOL AddDiffTimer(UINT32 dwTimerID, UINT32 dwHour, UINT32 dwMin, UINT32 dwSec, UINT32 dwData);

	BOOL DelTimer(UINT32 dwTimerID);

	VOID UpdateTimer();

	TimeEvent *m_pHead;

	TimeEvent *m_pFree;

	UINT32     m_dwCurTime;
public:
	BEGIN_TIMER_PROCESS
		PROCESS_TIMER_ITEM(ID_NEWDAY, OnNewDay)
	END_TIMER_PROCESS

public:

	VOID   OnNewDay(UINT32 dwTimerID, UINT32 dwData);
};



#endif /* _TIMER_MANAGER__ */

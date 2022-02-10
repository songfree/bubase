// NextTbl.cpp: implementation of the CNextTbl class.
//
//////////////////////////////////////////////////////////////////////

#include "NextTbl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNextTbl::CNextTbl()
{
	m_pRes = NULL;
}

CNextTbl::~CNextTbl()
{
	m_pRes = NULL;
}

bool CNextTbl::Next(S_NEXT_TBL &fn)
{
	bool bRet;
	int id;
	bRet = m_pkey.Next(id);
	if (!bRet)
	{
		return false;
	}
	fn = m_table.m_table[id];
	fn.nNodeId = m_table.m_table[id].nNodeId;
	fn.nNodePrivateId = m_table.m_table[id].nNodePrivateId;
	fn.nDrebSerial = m_table.m_table[id].nDrebSerial;
	fn.nFileSerial = m_table.m_table[id].nDrebSerial;
	fn.nTime = m_table.m_table[id].nTime;
	return true;
}


bool CNextTbl::First(S_NEXT_TBL &fn)
{
	bool bRet;
	int id;
	bRet = m_pkey.First(id);
	if (!bRet)
	{
		return false;
	}
	fn = m_table.m_table[id];
	fn.nNodeId = m_table.m_table[id].nNodeId;
	fn.nNodePrivateId = m_table.m_table[id].nNodePrivateId;
	fn.nDrebSerial = m_table.m_table[id].nDrebSerial;
	fn.nFileSerial = m_table.m_table[id].nDrebSerial;
	fn.nTime = m_table.m_table[id].nTime;
	return bRet;
}


bool CNextTbl::Select(int nodeid,int privateid,int serial,S_NEXT_TBL &fn)
{
	CInt iset;
	if (!m_pkey.Select(iset,nodeid,privateid,serial))
	{
		return false;
	}
	int id;
	iset.First(id);
	fn = m_table.m_table[id];
	fn.nNodeId = m_table.m_table[id].nNodeId;
	fn.nNodePrivateId = m_table.m_table[id].nNodePrivateId;
	fn.nDrebSerial = m_table.m_table[id].nDrebSerial;
	fn.nFileSerial = m_table.m_table[id].nDrebSerial;
	fn.nTime = m_table.m_table[id].nTime;
	return true;
}

bool CNextTbl::Delete(int nodeid,int privateid,int serial)
{
	CInt iset;
	if (!m_pkey.Select(iset,nodeid,privateid,serial))
	{
		return false;
	}
	int id;
	iset.First(id);
	CBF_PMutex pp(&m_mutex);
	m_pkey.Delete(iset,nodeid,privateid,serial);
	m_indextime.Delete(iset,m_table.m_table[id].nTime);
	m_table.Delete(id);
	return true;
}


bool CNextTbl::Update(S_NEXT_TBL fn)
{
	CInt iset;
	if (!m_pkey.Select(iset,fn.nNodeId,fn.nNodePrivateId,fn.nDrebSerial))
	{
		return false;
	}
	int id;
	iset.First(id);
	CBF_PMutex pp(&m_mutex);

	
	m_table.m_table[id].nNodeId = fn.nNodeId;
	m_table.m_table[id].nNodePrivateId = fn.nNodePrivateId ;
	m_table.m_table[id].nDrebSerial =fn.nDrebSerial;
	m_table.m_table[id].nDrebSerial = fn.nFileSerial;
	m_table.m_table[id].nTime = fn.nTime;

	return true;
}


bool CNextTbl::Insert(S_NEXT_TBL fn)
{
	int id;
	//通过主键查找，不存在则增加
	if (!m_pkey.Find(fn.nNodeId,fn.nNodePrivateId,fn.nDrebSerial))
	{
		CBF_PMutex pp(&m_mutex);
		id = m_table.Add(fn);//增加到表
		m_pkey.Add(id,fn.nNodeId,fn.nNodePrivateId,fn.nDrebSerial);//增加主键
		m_indextime.Add(id,fn.nTime);//增加索引
		return true;
	}
	return false;
}


int CNextTbl::DeleteTime(unsigned int stime)
{
	CBF_PMutex pp(&m_mutex);
	bool bRet;
	int id;
	int count=0;
	int curtime = time(NULL);
	bRet = m_indextime.First(id);
	if (!bRet)
	{
		return -1;
	}
	char sNextFile[400];
	CInt iset;
	while (bRet)
	{
		if (m_table.m_table[id].nTime - curtime > stime)
		{
#ifdef _WINDOWS
			sprintf(sNextFile,"%s\\nxt\\%ld.nxt",m_pRes->g_sCurPath,m_table.m_table[id].nFileSerial);
#else
			sprintf(sNextFile,"%s/nxt/%ld.nxt",m_pRes->g_sCurPath,m_table.m_table[id].nFileSerial);
#endif
			UNLINK(sNextFile);//删除文件
			iset.Add(id);
			m_indextime.Delete(iset,m_table.m_table[id].nTime );
			m_pkey.Delete(iset,m_table.m_table[id].nNodeId,m_table.m_table[id].nNodePrivateId,m_table.m_table[id].nDrebSerial);
			m_table.Delete(id);
			char sNextFile[400];

			count++;
		}
		else //时间新，后面的肯定是新的，不能用再去找了
		{
			break;
		}
		bRet = m_indextime.Next(id);
		iset.Clear();
	}
	
	return count;
}

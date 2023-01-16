// GateResource.h: interface for the CGateResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GATERESOURCE_H__B9E4A73C_7ADB_404E_A678_7919376804D6__INCLUDED_)
#define AFX_GATERESOURCE_H__B9E4A73C_7ADB_404E_A678_7919376804D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BF_AIO_Resource.h"
#include "BF_Xml.h"
#include "ICertApiBase.h"
#include "CgateLog.h"
#include "BF_CommDataLog.h"
#include "BF_DrebResource.h"

typedef struct _BC_INFO
{
    int nServiceNo;     //�㲥���ܺ�
    int nLevel;         //���� 1 2 1�ǳɽ��ر��������ر���  2�ǹ㲥�����
}S_BC_INFO;
class CTbl_BcInfo
{
protected:

    CMemTableNew<S_BC_INFO>     m_table;
    CPkeyIntUnordered<1>         m_pkey;//�㲥���ܺ�
    CBF_Mutex                   m_mutex;//������
    CInt m_rid_update;
public:
    int Insert(S_BC_INFO info)
    {
        CBF_PMutex pp(&m_mutex);
        int rid;
        if (!m_pkey.Select(rid, info.nServiceNo))
        {
            rid = m_table.Add(info);
            m_pkey.Add(rid, m_table.m_table[rid].nServiceNo);
            return rid;
        }
        return rid;
    }
    S_BC_INFO* Select(int serviceno)
    {
        CBF_PMutex pp(&m_mutex);
        int rid;
        if (!m_pkey.Select(rid, serviceno))
        {
            return NULL;
        }
        return &(m_table.m_table[rid]);
    }
    
    int  SelectAll(std::vector<S_BC_INFO*>& reslist)
    {
        CBF_PMutex pp(&m_mutex);
        int rid;
        bool bret = m_pkey.First(rid);
        while (bret)
        {
            reslist.push_back(&(m_table.m_table[rid]));
            bret = m_pkey.Next(rid);
        }
        return m_pkey.Size();
    }
    int  Select(std::vector<int>& reslist)
    {
        CBF_PMutex pp(&m_mutex);
        int rid;
        bool bret = m_pkey.First(rid);
        while (bret)
        {
            reslist.push_back(m_table.m_table[rid].nServiceNo);
            bret = m_pkey.Next(rid);
        }
        return m_pkey.Size();
    }
    S_BC_INFO* At(int rowid)
    {
        if (rowid<0 || rowid > m_table.AllSize() - 1)
        {
            return NULL;
        }
        return &(m_table.m_table[rowid]);
    }
};

class CGateResource : public CBF_AIO_Resource 
{
public:
	CGateResource();
	virtual ~CGateResource();

	int   g_nUnCommpress;   //�ǲ��ǶԿͻ��˵����ݽ�ѹ��������
	int   g_nZipFlag;  //�����ͻ��˵������Ƿ�ѹ��
	int   g_nEncryptFlag;  //�����ͻ��˵������Ƿ����
    int   g_nFilterQuo;   //�Ƿ���˹��ڵ�����  ����ʱ������С�Ĳ��ٷ���
	int   g_nSendQueThreadNum;        //���鷢���߳���

	int   g_nQuoSubScribe;//�Ƿ����鶩��   0���ö���  1�趩��

	//std::vector<int>  g_lBCFuncList;//�㲥���� ��Ҫ�����߶��ĵĹ㲥
    CTbl_BcInfo    g_lBCFuncList;    //�㲥���� ��Ҫ�����߶��ĵĹ㲥

	// ������: Init
	// ���  : ������ 2013-6-7 20:25:58
	// ����  : virtual bool 
	// ����  : const char *confile
	// ����  : ��ʼ�����������ļ�
	virtual bool Init(const char *confile, CIErrlog* log = NULL);

	CICertApiBase  *g_pCertClass;          //֤��api������
	int           g_nUseCert;//�Ƿ�ʹ��֤�� 0��ʹ�� 1ʹ��

	CBF_Xml  g_pXml;
	CCgateLog          g_pGateLog;//дcgate��־���߳���

	// ������: GetMsg
	// ���  : ������ 2013-5-29 14:37:52
	// ����  : std::string 
	// ����  : ���س�����Ϣ
	std::string GetMsg();

	char  m_sStartDate[20];

	CBF_DrebResource   g_pDrebRes;//����api��������Ϣ
private:
	// ������: OpenDll
	// ���  : ������ 2013-5-30 15:07:45
	// ����  : bool 
	// ����  : DLLHANDLE *pDll  ��̬���� 
	// ����  : const char *dllname
	// ����  : const char *dllpath
	// ����  : �򿪶�̬��
	bool OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath);

	char          m_sErrMsg[256];

	DLLHANDLE     m_pDll;
};

#endif // !defined(AFX_GATERESOURCE_H__B9E4A73C_7ADB_404E_A678_7919376804D6__INCLUDED_)

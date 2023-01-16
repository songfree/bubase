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
    int nServiceNo;     //广播功能号
    int nLevel;         //级别 1 2 1是成交回报、报单回报等  2是广播行情等
}S_BC_INFO;
class CTbl_BcInfo
{
protected:

    CMemTableNew<S_BC_INFO>     m_table;
    CPkeyIntUnordered<1>         m_pkey;//广播功能号
    CBF_Mutex                   m_mutex;//互斥锁
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

	int   g_nUnCommpress;   //是不是对客户端的数据解压缩至明文
	int   g_nZipFlag;  //发给客户端的数据是否压缩
	int   g_nEncryptFlag;  //发给客户端的数据是否加密
    int   g_nFilterQuo;   //是否过滤过期的行情  行情时间或序号小的不再发送
	int   g_nSendQueThreadNum;        //行情发送线程数

	int   g_nQuoSubScribe;//是否行情订阅   0不用订阅  1需订阅

	//std::vector<int>  g_lBCFuncList;//广播配置 需要向总线订阅的广播
    CTbl_BcInfo    g_lBCFuncList;    //广播配置 需要向总线订阅的广播

	// 函数名: Init
	// 编程  : 王明松 2013-6-7 20:25:58
	// 返回  : virtual bool 
	// 参数  : const char *confile
	// 描述  : 初始化，读配置文件
	virtual bool Init(const char *confile, CIErrlog* log = NULL);

	CICertApiBase  *g_pCertClass;          //证书api操作类
	int           g_nUseCert;//是否使用证书 0不使用 1使用

	CBF_Xml  g_pXml;
	CCgateLog          g_pGateLog;//写cgate日志的线程类

	// 函数名: GetMsg
	// 编程  : 王明松 2013-5-29 14:37:52
	// 返回  : std::string 
	// 描述  : 返回出错信息
	std::string GetMsg();

	char  m_sStartDate[20];

	CBF_DrebResource   g_pDrebRes;//总线api的配置信息
private:
	// 函数名: OpenDll
	// 编程  : 王明松 2013-5-30 15:07:45
	// 返回  : bool 
	// 参数  : DLLHANDLE *pDll  动态库句柄 
	// 参数  : const char *dllname
	// 参数  : const char *dllpath
	// 描述  : 打开动态库
	bool OpenDll(DLLHANDLE *pDll, const char *dllname, const char *dllpath);

	char          m_sErrMsg[256];

	DLLHANDLE     m_pDll;
};

#endif // !defined(AFX_GATERESOURCE_H__B9E4A73C_7ADB_404E_A678_7919376804D6__INCLUDED_)

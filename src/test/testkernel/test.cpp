#include "public.h"
//#include <iostream>
#include "MdbBase.h"
#include "BF_Mutex.h"
#include "DbApi.h"
#include "BF_LogClient.h"
#include "BF_Xml.h"
#include "BF_3Des.h"
#include "BF_Des.h"
#include "BF_BufferPool.h"
#include "BF_HostInfo.h" 
#include "BF_Tools.h"
#include "BF_DesEncrypt.h"

#include "TestTimer.h"
#include "TestDateTime.h"
#include "TestTools.h"
#include "TestSlist.h"
#include "TestThread.h"
#include "TestSocket.h"
#include "TestIOCP.h"
#include "TestQueue.h"
#include "wgetopt.h"


#include "GateHead.h"
//#include <net/if.h>


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_xdpd.lib") 
#pragma comment(lib, "bf_drebapid.lib") 
#pragma comment(lib, "bf_dbapid.lib") 
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma message("Automatically linking with   bf_kerneld.lib bf_xdpd.lib bf_drebapid.lib bf_dbapid.lib bf_dbpubd.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_xdp.lib") 
#pragma comment(lib, "bf_drebapi.lib") 
#pragma comment(lib, "bf_dbapi.lib") 
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with   bf_kernel.lib bf_xdp.lib bf_drebapi.lib bf_dbapi.lib bf_dbpub.lib")
#endif

#endif


#define  DBPWD3DESKEY   "fs27dwms19992017kssh1234" 

typedef struct 
{
	unsigned short serial;
	unsigned short index;
} S_DATA;


class CTest_index_base
{
public:
	int  m_nRowId;//rowid
	virtual bool operator<(const CTest_index_base& dd) const
	{
		return false;
	}
// 	virtual bool operator()(const CTest_index_base f1, const CTest_index_base f2) const 
// 	{
// 		return false;
// 	}
};

class CTest_Index : public CTest_index_base
{
public:
	int aa;
    std::string bb;
	int bbc;

	virtual bool operator<(const CTest_Index& dd) const
	{
		int ret;
		if (aa < dd.aa)
		{
			return true;
		}
		else if (aa == dd.aa)
		{
			ret = bb.compare(dd.bb);
			if (ret <0)
			{
				return true;
			}
			else if (ret>0)
			{
				return false;
			}
			else
			{
				ret = bbc-dd.bbc;
				if (ret < 0)
				{
					return true;
				}
				
				return false;
			}
		}
		else
		{
			return false;
		}
	}
};

typedef struct S_TEST_INDEX_
{
	int rid;
	int aa;
    std::string bb;
	int bbc;
	
	bool operator<(const S_TEST_INDEX_& dd) const
	{
		int ret;
		if (aa < dd.aa)
		{
			return true;
		}
		else if (aa == dd.aa)
		{
			ret = bb.compare(dd.bb);
			if (ret <0)
			{
				return true;
			}
			else if (ret>0)
			{
				return false;
			}
			else
			{
				ret = bbc-dd.bbc;
				if (ret < 0)
				{
					return true;
				}

				return false;
			}
		}
		else
		{
			return false;
		}
	}

}S_TEST_INDEX;

typedef std::set<CTest_Index> SET_;
typedef SET_::iterator LP_SET;





#define  BPUDBDESKEY    "rd402wms"
#define  BPUDB3DESKEY   "rd402wms20092017Ylink123"



typedef struct 
{
	char sOperCode[21];//����Ա����
	char sOperName[41];//����Ա����
	char sOperPhone[21];//����Ա�绰
	char sOrgCode[13];//��������
	char sOperType[2];//����Ա����
//	char sOperData[10000];//����Ա����
}S_MDB_OPER_INFO;

//����Ա
class  CTbl_Oper  
{
protected:
	
	CMemTableNew <S_MDB_OPER_INFO> m_table;
	CPkeyCharV<1>          m_pkey;               //����Ա����
public:
	CTbl_Oper()
	{
		
	}
	virtual ~CTbl_Oper()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_OPER_INFO oper) //ֻ���ڳ�ʼ��ʱʹ�ã����Կ��Բ�����
	{
		if (m_pkey.Find(oper.sOperCode))
		{
			return -1;
		}
		int rid=m_table.Add(oper);
		m_pkey.Add(rid,m_table.m_table[rid].sOperCode);
		return rid;
	}
	
	// ������: *Select
	// ���  : ������ 2017-8-3 10:20:35
	// ����  : S_MDB_OPER_INFO 
	// ����  : const char *opercode
	// ����  : ���ݲ���Ա������Ҳ���Ա��Ϣ
	S_MDB_OPER_INFO *Select(const char *opercode)
	{
		int rid;
		if (!m_pkey.GetRowId(rid,(char *)opercode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}
	S_MDB_OPER_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
	
};

typedef struct 
{
	char sOrgCode[13];//��������
	char sOrgName[41];//��������
	char sOrgType[2];//��������
	char sOrgLevel[2];//��������
	char sUp1Org[13];//��һ������
	char sUpOrg[201];//�ϼ�����  ���ָ�
	char sOrgCity[5];//����
	char sAccNo[24];//�����˺�
}S_MDB_ORG_INFO;
//������Ϣ��  
class CTbl_Org
{
protected:
	
	CMemTableNew <S_MDB_ORG_INFO>      m_table;
// 	CPkeyCharF<13,1>               m_pkey;//��������
// 	CIndexCharF<13,1>            m_index_up1org; //����һ���������е����� ��������һ������
// 	CIndexCharF<201,1>            m_index_uporg;  //���ϼ��������е����� �����������¼�����
	CPkeyCharV<1>               m_pkey;//��������
	CIndexVarChar<1>            m_index_up1org; //����һ���������е����� ��������һ������
	CIndexVarChar<1>            m_index_uporg;  //���ϼ��������е����� �����������¼�����
	CBF_Mutex                   m_mutex;//������
public:
	CTbl_Org()
	{
		
	}
	virtual ~CTbl_Org()
	{
		
	}
	void Clear()
	{
		m_pkey.Clear();
		m_index_uporg.Clear();
		m_index_up1org.Clear();
		m_table.Clear();
	}
	int Insert(S_MDB_ORG_INFO org)
	{
		if (m_pkey.Find(org.sOrgCode))
		{
			return -1;
		}
		int rid = m_table.Add(org);
		m_pkey.Add(rid,m_table.m_table[rid].sOrgCode);
		m_index_uporg.Add(rid,m_table.m_table[rid].sUpOrg);
		m_index_up1org.Add(rid,m_table.m_table[rid].sUp1Org);
		return rid;
	}
	S_MDB_ORG_INFO *Select(char *orgcode)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		if (!m_pkey.GetRowId(rid,orgcode))
		{
			return NULL;
		}
		return &(m_table.m_table[rid]);
	}

	// ������: SelectByParentOrg
	// ���  : ������ 2017-8-4 9:35:07
	// ����  : bool 
	// ����  : char *orgcode
	// ����  : std::vector<S_MDB_ORG_INFO *> &orglist
	// ����  : ���ݻ��������ѯ�˻�����������һ������(���������¼�������һ��)
	bool SelectByParentOrg(char *orgcode,std::vector<S_MDB_ORG_INFO *> &orglist)
	{
		CBF_PMutex pp(&m_mutex);
		std::vector<int> iset;
		if (!m_index_up1org.Select(iset,orgcode))
		{
			return false;
		}
		for (int i=0; i< iset.size(); i++)
		{
			orglist.push_back(&(m_table.m_table[iset[i]]));
		}
		return true;
	}

	// ������: SelectByUpOrg
	// ���  : ������ 2017-8-4 9:36:23
	// ����  : bool 
	// ����  : char *uporg
	// ����  : std::vector<S_ORG_INFO *> &orglist
	// ����  : ���ݻ�����ϣ��������е��¼���������Ϊ�ռ�Ϊ���л���
	bool SelectByUpOrg(char *uporg,std::vector<S_MDB_ORG_INFO *> &orglist)
	{
		CBF_PMutex pp(&m_mutex);
		int rid;
		bool bret;
		if (uporg == NULL || strlen(uporg)<1)
		{
			bret = m_index_uporg.First(rid);
			while (bret)
			{
				orglist.push_back(&(m_table.m_table[rid]));
				bret = m_index_uporg.Next(rid);
			}
			return true;
		}
		int len = strlen(uporg);
		bret = m_index_uporg.FindFirst(rid,uporg);
		if (!bret)
		{
			return false;
		}
		while (bret)
		{
			if (strncmp(uporg,m_table.m_table[rid].sUpOrg,len) == 0)//ǰ����ȣ���ʾ�����ӻ���
			{
				orglist.push_back(&(m_table.m_table[rid]));
			}
			else
			{
				return true;
			}
			bret = m_index_uporg.Next(rid);
		}
		return true;
	}
	

	// ������: GetAllChildOrg
	// ���  : ������ 2018-1-29 14:14:01
	// ����  : int  int <0����,�޴˻���  0�ɹ�  1�ɹ��Ƕ�������
	// ����  : char *org  ��������
	// ����  : std::vector<S_MDB_ORG_INFO *> &orglist
	// ����  : bool isinclude=false  falseֻ����һ��  true�������¼�
	// ����  : ȡ�û������¼�����
	int  GetAllChildOrg(char *org,std::vector<S_MDB_ORG_INFO *> &orglist,bool isinclude=false)
	{
		CBF_PMutex pp(&m_mutex);
		int ret;
		int rid;
		bool bRet;
		char upOrg[201];

		if (!m_pkey.GetRowId(rid,org))
		{
			return -1;
		}
		if (m_table.m_table[rid].sOrgLevel[0] == '0') //��������
		{
			ret = 1;
			if (isinclude) //���������¼��������л���
			{
				bRet = m_pkey.First(rid);
				while (bRet)
				{
					orglist.push_back(&(m_table.m_table[rid]));
					bRet = m_pkey.Next(rid);
				}
			}
			else
			{
				std::vector<int> iset;
				if (!m_index_up1org.Select(iset,m_table.m_table[rid].sOrgCode)) //��һ������
				{
					return -1;
				}
				for (int i=0; i< iset.size(); i++)
				{
					orglist.push_back(&(m_table.m_table[iset[i]]));
				}
			}
			return ret;
		}
		else
		{
			ret = 0;
			if (isinclude)
			{
				strcpy(upOrg,m_table.m_table[rid].sUpOrg);
				strcat(upOrg,"|");
				strcat(upOrg,m_table.m_table[rid].sOrgCode);
				int len = strlen(upOrg);
				bRet = m_index_uporg.FindFirst(rid,upOrg); //�Ƚ��ϼ��������ҵ�ǰ����ȵ�
				if (!bRet)
				{
					return -1;
				}
				while (bRet)
				{
					if (strncmp(upOrg,m_table.m_table[rid].sUpOrg,len) == 0)//ǰ����ȣ���ʾ�����ӻ���
					{
						orglist.push_back(&(m_table.m_table[rid]));
					}
					else
					{
						return -1;
					}
					bRet = m_index_uporg.Next(rid);
				}
			}
			else
			{
				std::vector<int> iset;
				if (!m_index_up1org.Select(iset,m_table.m_table[rid].sOrgCode))
				{
					return -1;
				}
				for (int i=0; i< iset.size(); i++)
				{
					orglist.push_back(&(m_table.m_table[iset[i]]));
				}
			}
			return ret;
		}
	}
	S_MDB_ORG_INFO *At(int rowid)
	{
		if (rowid<0 || rowid > m_table.AllSize()-1)
		{
			return NULL;
		}
		return &(m_table.m_table[rowid]);
	}
};
typedef struct _BC_SERIAL
{
    int node_id;
    int node_pid;
    int serial;
    int timestamp;
}S_BC_SERIAL;

class CTbl_BcSerial
{
protected:

    CMemTableNew <S_BC_SERIAL>  m_table;
    CPkeyIntUnordered<3>                 m_keyId;//����ID
    CBF_Mutex                   m_mutex;//������
public:

    CBF_BufferPool* m_pMemPool;//�ڴ�����;
    CTbl_BcSerial()
    {

    }
    virtual ~CTbl_BcSerial()
    {

    }
    void Clear()
    {
        m_keyId.Clear();
        m_table.Clear();
    }
    int Insert(S_BC_SERIAL info)
    {
        CBF_PMutex pp(&m_mutex);
        if (m_keyId.Find(info.node_id, info.node_pid, info.serial))
        {
            return -1;
        }
        int rid = m_table.Add(info);
        m_keyId.Add(rid, m_table.m_table[rid].node_id, m_table.m_table[rid].node_pid, m_table.m_table[rid].serial);
        return rid;
    }
    S_BC_SERIAL* Select(int nodeid, int nodepid, int serial)
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
        if (!m_keyId.Select(rid, nodeid, nodepid, serial))
        {
            return NULL;
        }
        return &(m_table.m_table[rid]);
    }

    S_BC_SERIAL* At(int rowid)
    {
        if (rowid<0 || rowid > m_table.AllSize() - 1)
        {
            return NULL;
        }
        return &(m_table.m_table[rowid]);
    }

    int Delete()
    {
        int rid;
        CBF_PMutex pp(&m_mutex);
        CInt iset;
        bool bret = m_keyId.First(rid);
        while (bret)
        {
            if (m_table.m_table[rid].timestamp - time(NULL) > 3600) //1Сʱ
            {
                iset.Add(rid);
            }
            bret = m_keyId.Next(rid);
        }
        m_keyId.Delete(iset);
        bret = iset.First(rid);
        while (bret)
        {
            m_table.Delete(rid);
            bret = iset.Next(rid);
        }

        return 0;
    }
    int Size()
    {
        return m_keyId.Size();
    }
};

void GenAcc(const char *oldacc, int syscode, char *txacc)
{
	int len = strlen(oldacc);
	int addlen = 12-len;
	sprintf(txacc,"%d",syscode);
	for (int i=0; i<addlen; i++)
	{
		strcat(txacc,"0");
	}
	strcat(txacc,oldacc);
}
CBF_LogClient pLog;
CBF_Timer pTime;
int OnTimer(unsigned int eventid, void *p)
{
	pLog.LogMp(LOG_WARNNING,__FILE__,__LINE__,"%lld",CBF_Date_Time::GetTickUS());
	printf("OnTimer %d\n",eventid);
	return 0;
}

//T0�ֱֲ�
typedef struct _PortfolioT0holdStruct {
    char trday[8 + 1] = { 0 };
    uint64_t account_id = 0;
    uint64_t portfolio_id = 0;
    char hj_code[32 + 1] = { 0 };
    char cname[32 + 1] = { 0 };
    char preday[8 + 1] = { 0 };
    int32_t pre_total_vol = 0;
    int64_t pre_mtm_cost = 0;
    int32_t total_vol = 0;
    int32_t bonus_onway_vol = 0;
    int32_t normal_vol = 0;
    int32_t total_convsa = 0;
    int32_t valid_convsa = 0;
    int32_t total_marketsa = 0;
    int32_t valid_marketsa = 0;
    int32_t overnight_vol = 0;
    int32_t overnight_pl = 0;
    int64_t mtm_closepl = 0;
    int64_t mtm_posipl = 0;
    int64_t mtm_cost = 0;
    int64_t account_fee = 0;
    int64_t portfolio_fee = 0;
    int64_t price = 0;
    int32_t buy_vol = 0;
    int32_t sell_vol = 0;
    int64_t buy_amt = 0;
    int64_t sell_amt = 0;
    int32_t pending_buy_vol = 0;
    int32_t pending_sell_vol = 0;
    int64_t pending_buy_amt = 0;
    int64_t pending_sell_amt = 0;
    int64_t usedbp = 0;
}S_MDB_PortfolioT0hold;
//�˻�+����+code������
class index_acc_portf_hjcode : public CIndex_Field_Base
{
public:
    UINT64_ account_id = 0;//�ʽ��˺�
    UINT64_ portfolio_id = 0;//����ID
    char    hj_code[33] = { 0 };//����
    virtual bool operator<(const index_acc_portf_hjcode& field) const
    {
        int ret = account_id - field.account_id;
        if (ret < 0)
        {
            return true;
        }
        else if (ret == 0)
        {
            int ret2 = portfolio_id - field.portfolio_id;
            if (ret2 < 0) {
                return true;
            }
            else if (ret2 == 0)
            {
                ret = strcmp(hj_code, field.hj_code);
                if (ret < 0) {
                    return true;
                }
            }
            return false;
        }
        return false;
    }
    //unorderedʹ��
    virtual bool operator == (const index_acc_portf_hjcode& field) const
    {
        if (account_id !=field.account_id)
        {
            return false;
        }
        else if (portfolio_id != field.portfolio_id)
        {
            return false;
        }
        else if (strcmp(hj_code,field.hj_code)!=0)
        {
            return false;
        }
        return true;
    }
};
//����T0�ֱֲ�
class CTbl_PortfolioT0hold {
protected:

    CMemTableNew<S_MDB_PortfolioT0hold> m_table;
    CIndexUint64<1>   m_index_account_id; //�˻�����
    CIndexUint64<2>   m_index_account_portf_id; //�˻�+��������
    CKeyField<index_acc_portf_hjcode>   m_key; //�˻�+����+code ����

    CBF_Mutex m_mutex;//������
    CInt  m_rid_update;//��Ÿ��µ�Rowrid
public:
    CTbl_PortfolioT0hold() {

    }
    virtual ~CTbl_PortfolioT0hold() {

    }
    void Clear() {
        m_index_account_id.Clear();
        m_index_account_portf_id.Clear();
        m_key.Clear();
        m_table.Clear();
    }
    //ɾ���ʽ��˺�
    bool Delete(UINT64_ account_id)
    {
        CBF_PMutex pp(&m_mutex);
        CInt iset;
        bool bret = m_index_account_id.Select(iset, account_id);
        if (!bret)
        {
            return false;
        }
        m_index_account_id.Delete(iset);
        m_index_account_portf_id.Delete(iset);
        m_key.Delete(iset);
        int rid;
        bret = iset.First(rid);
        while (bret)
        {
            m_table.Delete(rid);
            bret = iset.Next(rid);
        }
        return true;
    }
    //д���¼
    int Insert(S_MDB_PortfolioT0hold info) {
        CBF_PMutex pp(&m_mutex);
        index_acc_portf_hjcode bb;
        bb.account_id = info.account_id;
        bb.portfolio_id = info.portfolio_id;
        strncpy(bb.hj_code, info.hj_code, sizeof(bb.hj_code) - 1);
        if (m_key.Find(bb)) {
            return -1;
        }
        int rid = m_table.Add(info);
        bb.m_nRowId = rid;
        m_key.Add(bb);
        m_index_account_id.Add(rid, (UINT64_)m_table.m_table[rid].account_id);
        m_index_account_portf_id.Add(rid, m_table.m_table[rid].account_id, m_table.m_table[rid].portfolio_id);
        return rid;
    }
    int Update(S_MDB_PortfolioT0hold info, bool isupdate = false) {
        CBF_PMutex pp(&m_mutex);
        index_acc_portf_hjcode bb;
        bb.account_id = info.account_id;
        bb.portfolio_id = info.portfolio_id;
        strncpy(bb.hj_code, info.hj_code, sizeof(bb.hj_code) - 1);
        if (m_key.Select(bb)) {
            m_table.m_table[bb.m_nRowId] = info;
            if (isupdate)
            {
                m_rid_update.Add(bb.m_nRowId);
            }
            return bb.m_nRowId;
        }
        int rid = m_table.Add(info);
        bb.m_nRowId = rid;
        m_key.Add(bb);
        m_index_account_id.Add(rid, (UINT64_)m_table.m_table[rid].account_id);
        m_index_account_portf_id.Add(rid, m_table.m_table[rid].account_id, m_table.m_table[rid].portfolio_id);
        m_rid_update.Add(rid);
        return rid;
    }
    void GetUpdateList(std::vector<S_MDB_PortfolioT0hold*>& reslist, bool dellist = true)
    {
        CBF_PMutex pp(&m_mutex);
        int rid;
        bool bret = m_rid_update.First(rid);
        while (bret)
        {
            reslist.push_back(&(m_table.m_table[rid]));
            bret = m_rid_update.Next(rid);
        }
        if (dellist)
        {
            m_rid_update.Clear();
        }
        return;
    }
    //�����˺š����ԡ�����ȡ����¼
    S_MDB_PortfolioT0hold* Select(UINT64_ account_id, UINT64_ portfolio_id, char* hj_code) {
        CBF_PMutex pp(&m_mutex);
        index_acc_portf_hjcode bb;
        bb.account_id = account_id;
        bb.portfolio_id = portfolio_id;
        strncpy(bb.hj_code, hj_code, sizeof(bb.hj_code) - 1);
        if (!m_key.Select(bb)) {
            return NULL;
        }
        return &(m_table.m_table[bb.m_nRowId]);
    }
    //����������ȡrowid
    int GetRowId(UINT64_ account_id, UINT64_ portfolio_id, char* hj_code)
    {
        CBF_PMutex pp(&m_mutex);
        index_acc_portf_hjcode bb;
        bb.account_id = account_id;
        bb.portfolio_id = portfolio_id;
        strncpy(bb.hj_code, hj_code, sizeof(bb.hj_code) - 1);
        if (!m_key.Select(bb)) {
            return -1;
        }
        return bb.m_nRowId;
    }
    //�����ʽ��˺�ȡ������ȯ��
    bool Select(UINT64_ account_id, std::vector<S_MDB_PortfolioT0hold*>& reslist)
    {
        CBF_PMutex pp(&m_mutex);
        int rid;
        bool bret = m_index_account_id.Find(rid, account_id);
        if (!bret)
        {
            return false;
        }
        while (bret)
        {
            reslist.push_back(&(m_table.m_table[rid]));
            bret = m_index_account_id.Next(rid);
        }
        return true;
    }
    //�����ʽ��˺š�����ȡ������ȯ��
    bool Select(UINT64_ account_id, UINT64_ portfolio_id, std::vector<S_MDB_PortfolioT0hold*>& reslist)
    {
        CBF_PMutex pp(&m_mutex);
        int rid;
        bool bret = m_index_account_portf_id.Find(rid, account_id, portfolio_id);
        if (!bret)
        {
            return false;
        }
        while (bret)
        {
            reslist.push_back(&(m_table.m_table[rid]));
            bret = m_index_account_portf_id.Next(rid);
        }
        return true;
    }
    //����rowidȡ����¼
    S_MDB_PortfolioT0hold* At(int rowid) {
        if (rowid < 0 || rowid > m_table.AllSize() - 1) {
            return NULL;
        }
        return &(m_table.m_table[rowid]);
    }
};




//����T0�ֱֲ�
class CTbl_PortfolioT0hold_Unordered {
protected:

    struct _hash
    {
        std::size_t operator() (const index_acc_portf_hjcode& f1) const
        {
            return hash_val(f1.account_id, f1.portfolio_id, (std::string)f1.hj_code);
        }
    };

    CMemTableNew<S_MDB_PortfolioT0hold> m_table;
    CIndexUint64<1>   m_index_account_id; //�˻�����
    CIndexUint64<2>   m_index_account_portf_id; //�˻�+��������
    CKeyFieldUnordered<index_acc_portf_hjcode, _hash>   m_key; //�˻�+����+code ����

    CBF_Mutex m_mutex;//������
    CInt  m_rid_update;//��Ÿ��µ�Rowrid
public:
    CTbl_PortfolioT0hold_Unordered() {

    }
    virtual ~CTbl_PortfolioT0hold_Unordered() {

    }
    void Clear() {
        m_index_account_id.Clear();
        m_index_account_portf_id.Clear();
        m_key.Clear();
        m_table.Clear();
    }
    //ɾ���ʽ��˺�
    bool Delete(UINT64_ account_id)
    {
        CBF_PMutex pp(&m_mutex);
        CInt iset;
        bool bret = m_index_account_id.Select(iset, account_id);
        if (!bret)
        {
            return false;
        }
        m_index_account_id.Delete(iset);
        m_index_account_portf_id.Delete(iset);
        m_key.Delete(iset);
        int rid;
        bret = iset.First(rid);
        while (bret)
        {
            m_table.Delete(rid);
            bret = iset.Next(rid);
        }
        return true;
    }
    //д���¼
    int Insert(S_MDB_PortfolioT0hold info) {
        CBF_PMutex pp(&m_mutex);
        index_acc_portf_hjcode bb;
        bb.account_id = info.account_id;
        bb.portfolio_id = info.portfolio_id;
        strncpy(bb.hj_code, info.hj_code, sizeof(bb.hj_code) - 1);
        if (m_key.Find(bb)) {
            return -1;
        }
        int rid = m_table.Add(info);
        bb.m_nRowId = rid;
        m_key.Add(bb);
        m_index_account_id.Add(rid, (UINT64_)m_table.m_table[rid].account_id);
        m_index_account_portf_id.Add(rid, m_table.m_table[rid].account_id, m_table.m_table[rid].portfolio_id);
        return rid;
    }
    int Update(S_MDB_PortfolioT0hold info, bool isupdate = false) {
        CBF_PMutex pp(&m_mutex);
        index_acc_portf_hjcode bb;
        bb.account_id = info.account_id;
        bb.portfolio_id = info.portfolio_id;
        strncpy(bb.hj_code, info.hj_code, sizeof(bb.hj_code) - 1);
        if (m_key.Select(bb)) {
            m_table.m_table[bb.m_nRowId] = info;
            if (isupdate)
            {
                m_rid_update.Add(bb.m_nRowId);
            }
            return bb.m_nRowId;
        }
        int rid = m_table.Add(info);
        bb.m_nRowId = rid;
        m_key.Add(bb);
        m_index_account_id.Add(rid, (UINT64_)m_table.m_table[rid].account_id);
        m_index_account_portf_id.Add(rid, m_table.m_table[rid].account_id, m_table.m_table[rid].portfolio_id);
        m_rid_update.Add(rid);
        return rid;
    }
    void GetUpdateList(std::vector<S_MDB_PortfolioT0hold*>& reslist, bool dellist = true)
    {
        CBF_PMutex pp(&m_mutex);
        int rid;
        bool bret = m_rid_update.First(rid);
        while (bret)
        {
            reslist.push_back(&(m_table.m_table[rid]));
            bret = m_rid_update.Next(rid);
        }
        if (dellist)
        {
            m_rid_update.Clear();
        }
        return;
    }
    //�����˺š����ԡ�����ȡ����¼
    S_MDB_PortfolioT0hold* Select(UINT64_ account_id, UINT64_ portfolio_id, char* hj_code) {
        CBF_PMutex pp(&m_mutex);
        index_acc_portf_hjcode bb;
        bb.account_id = account_id;
        bb.portfolio_id = portfolio_id;
        strncpy(bb.hj_code, hj_code, sizeof(bb.hj_code) - 1);
        if (!m_key.Select(bb)) {
            return NULL;
        }
        return &(m_table.m_table[bb.m_nRowId]);
    }
    //����������ȡrowid
    int GetRowId(UINT64_ account_id, UINT64_ portfolio_id, char* hj_code)
    {
        CBF_PMutex pp(&m_mutex);
        index_acc_portf_hjcode bb;
        bb.account_id = account_id;
        bb.portfolio_id = portfolio_id;
        strncpy(bb.hj_code, hj_code, sizeof(bb.hj_code) - 1);
        if (!m_key.Select(bb)) {
            return -1;
        }
        return bb.m_nRowId;
    }
    //�����ʽ��˺�ȡ������ȯ��
    bool Select(UINT64_ account_id, std::vector<S_MDB_PortfolioT0hold*>& reslist)
    {
        CBF_PMutex pp(&m_mutex);
        int rid;
        bool bret = m_index_account_id.Find(rid, account_id);
        if (!bret)
        {
            return false;
        }
        while (bret)
        {
            reslist.push_back(&(m_table.m_table[rid]));
            bret = m_index_account_id.Next(rid);
        }
        return true;
    }
    //�����ʽ��˺š�����ȡ������ȯ��
    bool Select(UINT64_ account_id, UINT64_ portfolio_id, std::vector<S_MDB_PortfolioT0hold*>& reslist)
    {
        CBF_PMutex pp(&m_mutex);
        int rid;
        bool bret = m_index_account_portf_id.Find(rid, account_id, portfolio_id);
        if (!bret)
        {
            return false;
        }
        while (bret)
        {
            reslist.push_back(&(m_table.m_table[rid]));
            bret = m_index_account_portf_id.Next(rid);
        }
        return true;
    }
    //����rowidȡ����¼
    S_MDB_PortfolioT0hold* At(int rowid) {
        if (rowid < 0 || rowid > m_table.AllSize() - 1) {
            return NULL;
        }
        return &(m_table.m_table[rowid]);
    }
};

//�ֽڶ���
#ifdef _WINDOWS
#define __PACKED__ 
#pragma pack(push,1)
#else
#ifdef __GNUC__
#define __PACKED__	__attribute__ ((packed))
    //#pragma pack(push,1)
#else
#define __PACKED__
#pragma options align=packed
#endif
#endif


typedef struct f_queryportforder {
    uint64_t account_id = 0; // �ʽ��˻�ID
    uint32_t account_type = 0;// �ʽ��˻�����
    uint64_t portfolio_id = 0; // ���ID
    char broker_account[64] = { 0 };//ȯ���ʽ��˺�
    int32_t broker_id = 0; // ����������
    uint32_t term_id = 0; // �ն�ID
    uint64_t order_time = 0; // ί��ʱ��
    uint32_t command_id = 0; // ָ������
    uint32_t execution = 0; // ִ������
    uint32_t currency_id = 0; //����
    int32_t order_type = 0; // ������Դ
    uint64_t term_order_id = 0; // �ͻ��˶���ID
    uint64_t sys_order_id = 0; // ϵͳ����ID
    char broker_order_id[32] = { 0 }; // ȯ�̵���
    uint64_t p_order_id = 0; // ĸ������
    uint32_t market_id = 0; //
    char market_code[32] = { 0 }; // ֤ȯ����
    char market_name[128] = { 0 }; // ֤ȯ����
    int64_t order_price = 0; // ί�м۸�
    uint32_t order_vol = 0; // ί������
    uint32_t cancel_vol = 0; // ��������
    uint32_t match_vol = 0; // �ѳɽ�����
    int64_t match_amt = 0; // �ѳɽ����
    uint64_t match_price = 0; // �ɽ��۸�
    uint64_t trade_time = 0; // ���ɽ�ʱ��
    uint32_t order_stat = 0; // ����״̬
    int32_t ret_code = 0; // ��Ӧ��
    char ret_msg[512] = { 0 }; // ��Ӧ��Ϣ
}s_f_queryportforder;
typedef struct f_sendorder_ans {
    int32_t ret_code = 0; // ������
    char ret_msg[512] = { 0 }; // ������Ϣ
    uint64_t term_order_id = 0; // ������ˮ��
    uint32_t term_id = 0; // ԭί���ն�ID
    uint64_t sys_order_id = 0; // ϵͳ��ˮ��
    char broker_order_id[32] = { 0 }; // ��̨��ˮ��
    char broker_account[64] = { 0 }; // �ʽ��˻�����
    int32_t broker_id = 0; // ȯ��ID
    int32_t account_type = 0; // �˻�����
    uint64_t account_id = 0; // �ʲ��˺�ID
    uint64_t portfolio_id = 0; // ����ID
    uint32_t market_id = 0; // �г�ID
    char market_code[32] = { 0 }; // ֤ȯ����
    char market_name[128] = { 0 }; // ֤ȯ����
    uint32_t command_id = 0; // ָ������
    uint32_t execution = 0; // �۸�����
    uint32_t hedge_flag = 0; // ���ұ�־
    uint64_t order_price = 0; // ί�м۸�
    uint32_t order_vol = 0; // ί������
    uint32_t cancel_vol = 0; // ��������
    uint32_t match_vol = 0; // �ɽ�����
    uint64_t match_amt = 0; // �ɽ����
    uint64_t match_price = 0; // �ɽ��۸�
    uint64_t trade_time = 0; // ����ʱ��
    uint32_t order_stat = 0; // ����״̬
    uint64_t order_time = 0; // ί��ʱ��
    uint64_t update_time = 0; // ����ʱ��
    char tract_code[32] = { 0 };  // �ɶ��˻�
    int32_t order_type = 0;     //������Դ
}s_f_sendorder_ans;
typedef struct f_cancelorder_ans {
    int32_t ret_code = 0;                   // ��Ӧ��
    char ret_msg[512] = { 0 };                // ��Ӧ��Ϣ
    uint64_t term_order_id = 0;             // ԭί������������
    uint64_t sys_order_id = 0;              // ԭϵͳ������
    uint32_t term_id = 0;                   // ԭί���ն�ID
    char cancel_broker_order_id[32] = { 0 };  // ��������ȯ��
    uint64_t order_time = 0;                // ԭί��ʱ��
}f_cancelorder_ans_;
//ȡ���ֽڶ���
#ifdef _WINDOWS
#pragma pack(pop)
#else
#ifdef __GNUC__
    //#pragma pack(pop)
#else
#pragma options align=reset
#endif
#endif
#undef __PACKED__

#ifdef _WINDOWS
#define struct_stat struct _stat
#define FSTAT    _stat
#endif
int WriteBinFile(const char *filename,const char *data,int datalen)
{
    struct_stat fs;
    FSTAT(filename, &fs);
    int fsize = fs.st_size;
	int recnum=0;
	int nret;
	if (fsize > 0)	//�ļ������ݣ��ж������Ƿ����
	{
		if (fsize < 4)
		{
			return -100;
		}
		int dlen = fsize - 4;
		if (dlen % datalen != 0)	  //�ļ����ݴ�С����
		{
			return -101;
		}
        FILE* fp = fopen(filename, "rb+");
        if (fp == NULL)
        {
            return -1;
        }
		nret = fseek(fp, 0, SEEK_END);
        //д����
        nret = fwrite(data, 1, datalen, fp);
        if (nret != datalen)
        {
            fclose(fp);
            return -1;
        }
        //�ص�ͷλ��
		nret =fseek(fp, 0, SEEK_SET);

		//��4�ֽڼ�¼��
		nret = fread(&recnum, 1, sizeof(int), fp);
        if (nret != sizeof(int))
        {
            fclose(fp);
            return -1;
        }
		recnum++;	 //��¼��+1
        //�ص�ͷλ��
		nret = fseek(fp, 0, SEEK_SET);
		//д��¼��
        nret = fwrite(&recnum, 1, sizeof(int), fp);
        if (nret != sizeof(int))
        {
            fclose(fp);
            return -1;
        }
		fclose(fp);
		return 	recnum;
	}
	else
	{
		FILE* fp = fopen(filename, "wb+");
        if (fp == NULL)
        {
            return -1;
        }
		recnum++;
		nret = fwrite(&recnum,1, sizeof(int), fp);
		if (nret != sizeof(int))
		{
			fclose(fp);
			return -1;
		}
		nret = fwrite(data,1, datalen, fp);
        if (nret != datalen)
        {
            fclose(fp);
            return -1;
        }
		fclose(fp);
		return 1;
	}
}
void Reverse(unsigned char* buf, int len)
{
    int i;
    unsigned char buf1[10];
    for (i = 0; i < len; i++)
    {
        buf1[i] = buf[len - i - 1];
    }
    memcpy(buf, buf1, len);
}
int testbinfile()
{
    printf("sizof=%d  %ld\n", sizeof(f_cancelorder_ans_), time(NULL));
    struct_stat fs;
    FSTAT("10000012_ORDER_STATUS.20211210.bin", &fs);
    int fsize = fs.st_size;

    FILE* fp = fopen("10000012_ORDER_STATUS.20211210.bin", "rb+");
    if (fp == NULL)
    {
        return -1;
    }
    //s_f_queryportforder aaa;
    //fseek(fp,0,1);
    //int recnum=1;
    //fwrite(&recnum,4,1,fp);
    //fseek(fp, fsize, 1);
    //fwrite(&aaa, sizeof(s_f_queryportforder), 1, fp);
    //fclose(fp);

    //fp = fopen("10000012_ORDER_STATUS.20211203.bin", "rb+");
    //if (fp == NULL)
    //{
    //    return -1;
    //}

    printf("s_f_queryportforder size=%d", sizeof(s_f_queryportforder));
    char databuffer[64 * 1024];
    int  databuffersize = sizeof(databuffer);
    int ret = fread(databuffer, 1, databuffersize, fp);
    s_f_queryportforder* pp = NULL;
    if (ret > 0)
    {
        char* p = databuffer;
        int a = fsize / sizeof(s_f_queryportforder);
        for (int i = 0; i < a; i++)
        {
            //memcpy(&a, p, sizeof(int));
            //Reverse((unsigned char*)&a, sizeof(int));
            //printf("     size=%d \n", a);
            //p=p+ sizeof(int);
            pp = (s_f_queryportforder*)(p);
            printf("account_id=%llu  sys_order_id=%llu  order_stat=%d \n", pp->account_id, pp->sys_order_id, pp->order_stat);
            WriteBinFile("test.bin", (const char*)pp, sizeof(s_f_queryportforder));
            p = p + sizeof(s_f_queryportforder);

        }
    }
    fp = fopen("test.bin", "rb + ");
    if (fp == NULL)
    {
        return -1;
    }
    int recnum = 0;
    ret = fread(&recnum, 1, 4, fp);
    return 0;
}
int testdb(int argc, char* argv[])
{
    printf("intsize=%d\n",sizeof(int));
    printf("shortsize=%d\n",sizeof(short));
    printf("longsize=%d\n",sizeof(long));
    printf("doublesize=%d\n",sizeof(double));
    CDbApi dbapi;
    if (argc <4)
    {
    	    if (!dbapi.Connect("127.0.0.1:3306@risk","risk","risk",6))
    	    {
    		    printf("�������ݿ���� %d%s \n",dbapi.m_dbconn.m_nErrCode,dbapi.m_dbconn.m_sErrInfo.c_str());
    		    return -1;
    	    }
    }
    else
    {
        printf("cmd dbname dbuser db pwd\n");
        if (!dbapi.Connect(argv[1], argv[2], argv[3], 6))
        {
            printf("�������ݿ���� %d%s \n", dbapi.m_dbconn.m_nErrCode, dbapi.m_dbconn.m_sErrInfo.c_str());
            return -1;
        }
    }
    //DELIMITER $$
    //	DROP PROCEDURE IF EXISTS `sp_test_wms` $$
    //	CREATE PROCEDURE `sp_test_wms` (
    //	in i_operid char(20),
    //	out o_errcode int )

    //	BEGIN
    //	SET o_errcode = 1999;
    //insert into contract values('a2001','a','20200115');

    //END$$
    //	DELIMITER ;

    char sMsg[255];
    int ret;
    memset(sMsg, 0, sizeof(sMsg));
    dbapi.setCommandText("sp_test_wms",1);
    dbapi.Param("i_operid").setAsString() = "admin1";		//������Աid
    if (!dbapi.Execute())
    {
    	printf("ִ��sp_test_wms���� %s\n",dbapi.GetCmdErrInfo());
    }
    else
    {
    	ret = dbapi.Param("o_errcode").asLong();
    	printf("ִ��sp_test_wms���� %d\n",ret);
    }
    dbapi.setCommandText("sp_test_wms2",1);
    dbapi.Param("i_operid").setAsString() = "admin1";		//������Աid
    if (!dbapi.Execute())
    {
    	printf("ִ��sp_test_wms2���� %s\n",dbapi.GetCmdErrInfo());
    }
    else
    {
    	ret = dbapi.Param("o_errcode").asLong();
    	printf("ִ��sp_test_wms2���� %d\n",ret);
    }


    dbapi.setCommandText("sp_query_oper",1);
    dbapi.Param("i_operid").setAsString() = "admin1";
    
    if (!dbapi.Execute())
    {
    	printf("ִ��sp_query_oper���� %s\n",dbapi.GetCmdErrInfo());
    	return -1;
    }
    ret = dbapi.Param("o_errcode").asLong();
    if (ret != 0)
    {
    	printf("ִ��sp_query_oper���� %d\n",ret);
    	return ret;
    }
    printf("ִ��sp_query_oper���� %d\n",ret);
    while (dbapi.FetchNext())
    {
    	std::string ss = dbapi.Field("status").asString().GetMultiByteChars();
    	//������״̬�Ĳ���Ա�����ؽ���
    	if(ss.compare("0") != 0)
    	{
    		printf("����Ա״̬����\n");
    		continue;
    	}
    	std::string userid = dbapi.Field("operid").asString().GetMultiByteChars();
    	std::string utype = dbapi.Field("type").asString().GetMultiByteChars();
    	std::string upwd = dbapi.Field("passwd").asString().GetMultiByteChars();
    	std::string uname = dbapi.Field("name").asString().GetMultiByteChars();
    	printf("����Ա��Ϣ %s %s %s %s \n",userid.c_str(),utype.c_str(),upwd.c_str(),uname.c_str());
    }

    std::string errcode=dbapi.Param("o_errmsg").setAsString().GetMultiByteChars();
    printf("ִ��sp_query_oper������Ϣ %s\n",errcode.c_str());


    return 0;
    
}
int testsettle(int argc, char* argv[])
{
    printf("intsize=%d\n", sizeof(int));
    printf("shortsize=%d\n", sizeof(short));
    printf("longsize=%d\n", sizeof(long));
    printf("doublesize=%d\n", sizeof(double));
    CDbApi dbapi;
    if (argc < 4)
    {
        if (!dbapi.Connect("127.0.0.1:3306@risk", "risk", "risk", 6))
        {
            printf("�������ݿ���� %d%s \n", dbapi.m_dbconn.m_nErrCode, dbapi.m_dbconn.m_sErrInfo.c_str());
            return -1;
        }
    }
    else
    {
        printf("cmd dbname dbuser db pwd\n");
        if (!dbapi.Connect(argv[1], argv[2], argv[3], 6))
        {
            printf("�������ݿ���� %d%s \n", dbapi.m_dbconn.m_nErrCode, dbapi.m_dbconn.m_sErrInfo.c_str());
            return -1;
        }
    }
    ///ͨ����ȡ��־��ý����Ƿ�ɹ�
    int i;
    int nfieldcount=0;
    std::string sname;
    std::string svalue;
    int rec=0;

    dbapi.setCommandText("sp_settle_step1",1);
    dbapi.Param("i_operid").setAsString() = "admin2";
    dbapi.Param("i_pid").setAsLong() = 16826;
    dbapi.Param("i_settle_date").setAsString() = "20200821";
    dbapi.Param("i_brokerid").setAsString() = "000003";


    if (!dbapi.Execute())
    {
    	printf("Execute sp_settle_step1 error\n");
    	return -1;
    }
    nfieldcount = dbapi.FieldCount();//ȡ���ֶ���


    while(dbapi.FetchNext())
    {
        for (int i = 0; i < nfieldcount; i++)
        {
            sname = dbapi.Field(i).Name();
            svalue = dbapi.Field(i).asString();
            printf("sp_settle_step1  ���ؼ�¼��[%d]         field [%s:%s] \n", rec, sname.c_str(), svalue.c_str());
        }
        rec++;

    }
    i = dbapi.Param("o_errcode").asLong();
    if (i != 0)
    {
        	printf("sp_settle_step1 ���ز���  o_errcode:o_errmsg = %d:%s \n",i, dbapi.Param("o_errmsg").asString().GetMultiByteChars());
        	return -1;
    }
    printf("sp_settle_step1 ���ز���  o_errcode:o_errmsg = %d:%s \n",i, dbapi.Param("o_errmsg").asString().GetMultiByteChars());

    try
    {
    	    dbapi.setCommandText("sp_query_t_settle_result",1);
        	dbapi.Param("i_operid").setAsString() = "admin2";
        	dbapi.Param("i_pid").setAsLong() = 16826;
    }
    catch (SAException &err)
    {
    	    printf("sqlapi err %d:%s\n",err.ErrNativeCode(),err.ErrText().GetMultiByteChars());
        	return -1;
    }

    if (!dbapi.Execute())
    {
        	printf("Execute sp_query_t_settle_result error %s \n",dbapi.GetCmdErrInfo());
        	return -1;
    }
    rec=0;
    nfieldcount = dbapi.FieldCount();//ȡ���ֶ���
    while(dbapi.FetchNext())
    {
        printf("sp_query_t_settle_result FetchNext rec=%d nfieldcount=%d \n", rec, nfieldcount);
        for (int i = 0; i < nfieldcount; i++)
        {
            sname = dbapi.Field(i).Name();
            svalue = dbapi.Field(i).asString();
            printf("sp_query_t_settle_result ���ؼ�¼��[%d]          field [%s:%s] \n", rec, sname.c_str(), svalue.c_str());
        }
        rec++;

    }
    i = dbapi.Param("o_errcode").asLong();
    if (i != 0)
    {
        	printf("sp_query_t_settle_result ���� %d:%s \n",i, dbapi.Param("o_errmsg").asString().GetMultiByteChars());
        	return -1;
    }
    printf("sp_query_t_settle_result ���� %d:%s \n",i, dbapi.Param("o_errmsg").asString().GetMultiByteChars());


    
    dbapi.setCommandText("sp_settle_liq",1);
    try
    {
        	dbapi.Param("i_operid").setAsString() = "admin2";
    }
    catch (SAException &err)
    {
        	printf("sqlapi err %d:%s\n",err.ErrNativeCode(),err.ErrText().GetMultiByteChars());
        	return -1;
    }
    dbapi.Param("i_pid").setAsLong() = 16826;
    
    dbapi.Param("i_settle_date").setAsString() = "20200821";
    dbapi.Param("i_brokerid").setAsString() = "000003";

    if (dbapi.Execute())
    {
        	nfieldcount = dbapi.FieldCount();//ȡ���ֶ���


    	    while(dbapi.FetchNext())
    	    {
    		    for (int i=0;i<nfieldcount;i++)
    		    {
    			    sname = dbapi.Field(i).Name();
    			    svalue = dbapi.Field(i).asString();
    			    printf("sp_settle_liq  ���ؼ�¼��[%d]         field [%s:%s] \n",rec,sname.c_str(),svalue.c_str());
    		    }
    		    rec++;

    	    }
    	    i = dbapi.Param("o_errcode").asLong();
    	    if (i != 0)
    	    {
    		    printf("sp_settle_liq ���� %d:%s \n",i, dbapi.Param("o_errmsg").asString().GetMultiByteChars());
    		    return -1;
    	    }
    	    printf("sp_settle_liq ���ز���  o_errcode:o_errmsg = %d:%s \n",i, dbapi.Param("o_errmsg").asString().GetMultiByteChars());
    }
    else
    {
        	printf("Execute sp_settle_liq error\n");
    }

    
    printf("��ʼsp_query_t_settle_result\n");
    try
    {
    	    dbapi.setCommandText("sp_query_t_settle_result",1);
    	    dbapi.Param("i_operid").setAsString() = "admin2";
        	dbapi.Param("i_pid").setAsLong() = 16826;
    }
    catch (SAException &err)
    {
        	printf("sqlapi err %d:%s\n",err.ErrNativeCode(),err.ErrText().GetMultiByteChars());
        	return -1;
    }
    
    if (!dbapi.Execute())
    {
        	printf("Execute sp_query_t_settle_result error %s \n",dbapi.GetCmdErrInfo());
        	return -1;
    }
    rec=0;
    nfieldcount = dbapi.FieldCount();//ȡ���ֶ���
    while(dbapi.FetchNext())
    {
        for (int i = 0; i < nfieldcount; i++)
        {
            sname = dbapi.Field(i).Name();
            svalue = dbapi.Field(i).asString();
            printf("sp_query_t_settle_result ���ؼ�¼��[%d]          field [%s:%s] \n", rec, sname.c_str(), svalue.c_str());
        }
        rec++;

    }
    i = dbapi.Param("o_errcode").asLong();
    if (i != 0)
    {
        	printf("sp_query_t_settle_result ���� %d:%s \n",i, dbapi.Param("o_errmsg").asString().GetMultiByteChars());
        	return -1;
    }
    printf("sp_query_t_settle_result ���� %d:%s \n",i, dbapi.Param("o_errmsg").asString().GetMultiByteChars());
    return 0;
}
int testmdb()
{
    CTbl_PortfolioT0hold   tbl;
    S_MDB_PortfolioT0hold hold;
    strcpy(hold.hj_code, "000001.sz");
    hold.account_id = 10000;
    hold.portfolio_id = 0;
    tbl.Insert(hold);
    hold.portfolio_id = 1;
    tbl.Insert(hold);

    strcpy(hold.hj_code, "000002.sz");
    hold.account_id = 10001;
    hold.portfolio_id = 0;
    tbl.Insert(hold);
    hold.portfolio_id = 1;
    tbl.Insert(hold);


    tbl.Delete(10000);

    return 0;
}
int testdes()
{
    typedef struct aa_
    {
        char bb[100] = {};
        int aa = 0;
    };
    char buffer[300];
    strcpy(buffer, "b414895697b45a26");
    char errmsg[200];
    int len = strlen(buffer);
    CBF_3Des des;
    des.UnDes3Buffer(buffer, len, DBPWD3DESKEY, errmsg);
    aa_ a;
    pLog.LogMp(LOG_WARNNING, __FILE__, __LINE__, "��ʼֵ  %s %d", a.bb, a.aa);

    char tmpchar1[20] = { '0' };
    char tmpchar2[22];

    memcpy(tmpchar1, "222", 3);
    memcpy(tmpchar1 + 10, "1234567890", 10);

    strcpy(tmpchar2, "1234567890123456789");
    strncpy(tmpchar1, tmpchar2, 10);
    pLog.LogMp(LOG_WARNNING, __FILE__, __LINE__, "��ʼֵ  %s", tmpchar1);

    	CBF_3Des      m_p3Des;
	char sDbPwd[65];
	char m_sErrMsg[256];
 	strcpy(sDbPwd,"3456789012");
 	GenAcc(sDbPwd,5,m_sErrMsg);

    strcpy(sDbPwd,"fs");
    len = sizeof(sDbPwd);
    m_p3Des.Des3Buffer(sDbPwd,len,BPUDB3DESKEY,m_sErrMsg);

    strcpy(sDbPwd,"5c19659d70aa1bcc");
    len = strlen(sDbPwd);
    m_p3Des.UnDes3Buffer(sDbPwd,len,BPUDB3DESKEY,m_sErrMsg);

    printf("sDbPwd=[%s]\n",sDbPwd);
    return 0;
}
int testdes2()
{
	char deskey[25];
	sprintf(deskey,"ylink012345 789,wms12345");
	CBF_3Des pdes;

	char buffer[8192];
	sprintf(buffer,"ylink123 ;.yww");
	int bufsize=8192;
	char errmsg[256];
	printf("����[%s]\n",buffer);
	if (!pdes.Des3Buffer(buffer,bufsize,deskey,errmsg))
	{
		printf("���ܳ��� %s\n",errmsg);
		return -1;
	}
	printf("����[%s]\n",buffer);
	bufsize = strlen(buffer);
	if (!pdes.UnDes3Buffer(buffer,bufsize,deskey,errmsg))
	{
		printf("���ܳ��� %s\n",errmsg);
		return -1;
	}
	printf("����[%s]\n",buffer);
    return 0;

}
int testtimer()
{
    pTime.Init(100);
    pLog.LogMp(LOG_WARNNING, __FILE__, __LINE__, "%lld", CBF_Date_Time::GetTickUS());
    //pTime.SetTimer(1,1800000000,OnTimer,NULL,2);
    pTime.Init(1000, false);
    pTime.SetTimer(1, 3000, OnTimer, NULL, 0);
    //pTime.SetTimer(2,2000000,OnTimer,NULL,2);
    pTime.SetTimer(2, 1000, OnTimer, NULL);
    //pTime.KillTimer(2);
    pTime.Start();
    SLEEP_SECONDS(20);
    pTime.SetTimer(4, 0, OnTimer, NULL, 0);
    SLEEP_SECONDS(1);
    pTime.SetTimer(4, 0, OnTimer, NULL, 0);
    SLEEP_SECONDS(1);
    pTime.SetTimer(4, 0, OnTimer, NULL, 0);
    SLEEP_SECONDS(1);
    pTime.SetTimer(4, 0, OnTimer, NULL, 0);
    pTime.Stop();
    pTime.Join();
    SLEEP_SECONDS(5);
    return 0;
}
int testmdb2()
{
    CTbl_Org org;
    S_MDB_ORG_INFO info;
    bzero(&info,sizeof(S_MDB_ORG_INFO));
    int i;
    int rid;
    for (i=0;i<1000;i++)
    {
        sprintf(info.sOrgCode, "org%d", i);
        sprintf(info.sOrgName, "data%d", i);
        rid = org.Insert(info);
        if (rid < 0)
        {
            printf("Insert error\n");
        }
    }
    S_MDB_ORG_INFO *po = NULL;
    char tmpchar[100];
    sprintf(tmpchar,"org%d",110);
    po = org.Select(tmpchar);
    printf("org[%s] sOrgName[%s]\n",po->sOrgCode,po->sOrgName);
    sprintf(po->sOrgName,"111111111111111111111111");

    for (i= 100; i<120; i++)
    {
        sprintf(tmpchar, "org%d", i);
        po = org.Select(tmpchar);
        if (po != NULL)
        {
            printf("org[%s] sOrgName[%s]\n", po->sOrgCode, po->sOrgName);
        }
        else
        {
            printf("%s not found\n", tmpchar);
        }
    }


    return 0;
}
int testgate()
{
    char uporg[200];
 	char uporg2[200];
 	strcpy(uporg,"1|2|3|4|1");
 	strcpy(uporg2,"1|2|3|4");
 	if (strncmp(uporg,uporg2,strlen(uporg)) ==0)
 	{
 		printf("true \n");
 		return 0;
 	}
 	CBF_Xml xml;
 	if (!xml.FromFile("cgateapi.xml"))
 	{
 		return -1;
 	}
 	CXmlNode *gatenode = xml.GetNodeByPath("/����API/����");
 	if (gatenode == NULL)
 	{
 		return -1;
 	}
 	std::string ip;
 	CXmlNode *node = (CXmlNode *)gatenode->GetFirstChild();
 	while (node != NULL)
 	{
 		if (node->GetAttribute("ip",false,ip)==NULL)
 		{
 			return -1;
 		}
 		printf("IP=%s",ip.c_str());
 		node = (CXmlNode *)node->getNextSibling();
 	}
 	return 0;
}
int testmdb3()
{
    class index_1 : public CIndex_Field_Base
    {
    public:
        int aa;
        std::string bb;
        int bbc;


        virtual bool operator<(const index_1& field) const
        {
            int ret;
            if (aa < field.aa)
            {
                return true;
            }
            else if (aa == field.aa)
            {
                ret = bb.compare(field.bb);
                if (ret < 0)
                {
                    return true;
                }
                else if (ret > 0)
                {
                    return false;
                }
                else
                {
                    ret = bbc - field.bbc;
                    if (ret < 0)
                    {
                        return true;
                    }

                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    };

    CKeyField<index_1> key_3;
    CIndexField<index_1> index_3;


    SET_ m_index;

    // 	pair<LP_SET,bool> r = m_index.insert(10);
    // 	r = m_index.insert(3);
    // 	r = m_index.insert(21);
    index_1 tit;
    tit.m_nRowId = 0;
    tit.aa = 1;
    tit.bb = "aa";
    tit.bbc = 99;

    key_3.Add(tit);
    index_3.Add(tit);

    tit.m_nRowId = 1;
    tit.aa = 1;
    tit.bb = "bb";
    tit.bbc = 88;

    key_3.Add(tit);
    index_3.Add(tit);

    tit.m_nRowId = 2;
    tit.aa = 0;
    tit.bb = "bb";
    tit.bbc = 88;

    key_3.Add(tit);
    index_3.Add(tit);

    tit.m_nRowId = 3;
    tit.aa = 1;
    tit.bb = "aa";
    tit.bbc = 88;

    key_3.Add(tit);
    index_3.Add(tit);
    tit.m_nRowId = 4;
    index_3.Add(tit);


    if (key_3.Find(tit))
    {
        printf("����[%d %s %d]�ظ�\n", tit.aa, tit.bb.c_str(), tit.bbc);
    }

    bool bret = key_3.m_key.First(tit);
    while (bret)
    {
        printf("rid=%d %d %s %d \n", tit.m_nRowId, tit.aa, tit.bb.c_str(), tit.bbc);
        bret = key_3.m_key.Next(tit);

    }
    printf("\n");
    bret = index_3.m_index.First(tit);
    while (bret)
    {
        printf("rid=%d %d %s %d \n", tit.m_nRowId, tit.aa, tit.bb.c_str(), tit.bbc);
        bret = index_3.m_index.Next(tit);

    }
    //	int getnum=100000000;
    //	int nbegin = time(NULL);
    //	for (int k=0; k<getnum;k++)
    //	{
    //		CBF_Date_Time::GetTickUS();
    //	}
    //	nbegin = time(NULL)-nbegin;
    //	printf("GetTickUS %d�κ�ʱ[%d]�� ƽ��[%lf]΢��\n",getnum,nbegin,1.00*nbegin*1000000/getnum);
    //
    return 0;
}
int testmdb4()
{
    
	CIndexInt<1> iindex;
	iindex.Add(1,1);
	iindex.Add(2,1);
	iindex.Add(3,1);
	iindex.Add(4,2);
	iindex.Add(5,2);
	iindex.Add(6,2);
	iindex.Add(7,2);

	CInt iset;
	iindex.Select(iset,2);
	int id;
	bool bret = iset.First(id);
	while (bret)
	{
		printf("rowid:%d \n",id);
		bret = iset.Next(id);
	}
    return 0;

}
int testhost()
{
    CBF_HostInfo hostinfo;
    S_MemoryInf meminfo;
    bzero(&meminfo, sizeof(S_MemoryInf));
    int nRet = hostinfo.GetEms(meminfo);
    printf("���ڴ�[%d]MB ʹ��[%d]MB ʹ����[%d]\n", meminfo.EmsTotal, meminfo.EmsUse, meminfo.EmsUseRadio);

    nRet = hostinfo.GetCpu();
    printf("CPUʹ����[%d]\n", nRet);
    return 0;
}
int testsocket()
{
    struct sockaddr_in broadcast_addr;//�㲥��ַ
 	//��ʹ�õ�����ӿ����ָ��Ƶ�ifr.ifr_name�У����ڲ�ͬ�������ӿڵĹ㲥��ַ�ǲ�һ���ģ����ָ�������ӿ�
 //	struct ifreq ifr;
 //	strcpy(ifr.ifr_name,IFNAME);
 //	//��������������ӿڵĹ㲥��ַ
 //	SOCKET_HANDLE sock;
 //	CBF_SocketUdp udp;
 //	udp.Create();
 //	sock = udp.GetSocket();
 //	if(ioctl(sock,SIOCGIFBRDADDR,&ifr) == -1)
 //	{
 //		printf("ioctl error\n");
 //		return -1;
 //	}
 //	//����õĹ㲥��ַ���Ƶ�broadcast_addr
 //	memcpy(&broadcast_addr,&ifr.ifr_broadaddr,sizeof(struct sockaddr_in));
 //	//���ù㲥�˿ں�
 //	printf("broadcast IP is:%s\n",inet_ntoa(broadcast_addr.sin_addr));
 //	broadcast_addr.sin_family=AF_INET;
	//broadcast_addr.sin_port=htons(MCAST_PORT);
 //   //Ĭ�ϵ��׽���������sock�ǲ�֧�ֹ㲥�����������׽�����������֧�ֹ㲥
	//ret=setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&so_broadcast,sizeof(so_broadcast));
    return 0;
}
int testbuffer()
{
    	CBF_BufferPool pool;

	pool.Init(100,8192,2000);
	void *aa = NULL;
	std::stack<void *>savepoint;
	int j;
	for (j=0; j<10000; j++)
	{
		aa = pool.PoolMalloc();
		savepoint.push(aa);

	}
	
	for (j=0; j<7000; j++)
	{
		void *aa = savepoint.top();
		savepoint.pop();
		pool.PoolFree(aa);
	}

	aa = pool.PoolMalloc();

	int total;
	int unused;
	pool.PoolInfo(total,unused);
	printf("poolbuffer total[%d] unused[%d] \n",total,unused);

	CBF_Date_Time pp1;
	CBF_Date_Time pp2;
	printf("��ǰ����[%s]\n",pp1.ToStringD8().c_str());
	pp2.Set("20280606");
	int tnums = pp2-pp1;
	printf("��20280606����[%d]��\n",tnums);


	return 0;
}
int testmdb5()
{
    
	CIndexInt<2>              m_pkey;         //id+˽��id������Ψһ����
	CPkeyUInt<2>              key;
	key.Add(1,100,1);
	key.Add(2,100,2);

	CInt iset;
	if (!key.Select(iset,100,3))
	{
		printf("not fund");
	}
	else
	{
		printf("fund");
	}
    return 0;
}
int teststring()
{
    

	std::string str = " 123456789 ";
	int pos;
	int offset=str.length()-1;
	pos=str.rfind(' ', offset);
    std::vector<int>deleteList;
	while (pos != -1 ) 	
	{
		if (pos == offset)
		{
			deleteList.push_back(pos);//��¼Ҫɾ���Ŀո�λ��
			offset--;
		}
		else
		{
			break;
		}
		pos=str.rfind(' ', offset);
	}
	//ɾ���ַ�
	if (deleteList.size()>0)
	{
		str = str.substr(0,deleteList[deleteList.size()-1]);
	}
    return 0;

}
int testxmlandsep()
{
    	CBF_Xml xml;
	xml.FromFile("arbmain.xml");

	xml.ToFile("aa.xml");


 	int ret,len;
 	char databuf[65535];
 	bzero(databuf,sizeof(databuf));
 	FILE *fp = fopen("c:/log/sep3.txt","r");
 	if (fp == NULL)
 	{
 		return -1;
 	}
 	ret = fread(databuf,1,65534,fp);
 	databuf[ret]=0;
 	CBF_Slist ss;
 	ss.Clear();
 	ss.SetSeparateString("��");
 	ret = ss.FillChnSepString(databuf);
 	for (int i=0;i<ret;i++)
 	{
 		printf("%d   %s \n",i,ss.GetAt(i).c_str());
 	}
 	ss.Clear();
 	ret = ss.FillSepString(databuf);
 	for (int i=0;i<ret;i++)
 	{
 		printf("%d   %s \n",i,ss.GetAt(i).c_str());
 	}
 	return 0;
}
int testxml(int argc, char* argv[])
{
    char curpath[200];
 	char prgname[200];
 	char prgpath[200];
 	bzero(curpath,sizeof(curpath));
 	bzero(prgname,sizeof(prgname));
 	bzero(prgpath,sizeof(prgpath));
 	
 	CBF_Tools::GetModuleNamePath(argv,curpath,prgpath,prgname);
 	printf("����GetModuleNamePath \n     curpath=[%s] \n     prgpath=[%s] \n     prgname=[%s] \n",curpath,prgpath,prgname);
 
 	
    
 	int opt = CBF_Tools::Getopt(argc,argv,(char *)"b?:i:h?");
    while( opt != -1 ) 
 	{
 		printf("opt=%c optarg=%s optind=%d\n",(char)opt,optarg,optind);
        opt = CBF_Tools::Getopt(argc,argv,(char*)"");
    }
    
 	CBF_Xml xml;
 	xml.AddNodeByPath("/",false,"package",false,"",false);
 	xml.AddNodeByPath("/package",false,"head",false,"",false);
 	xml.AddNodeByPath("/package",false,"pack",false,"",false);
 	xml.AddNodeByPath("/package/head",false,"������",false,"9906",false);
 	xml.AddNodeByPath("/package/pack",false,"��ˮ��",false,"2012042500000001",false);
 	xml.AddNodeByPath("/package",false,"head2",false,"",false);
 	xml.AddNodeByPath("/package/head",false,"������2",false,"",false);
 	xml.AddNodeByPath("/package/head",false,"������",false,"",false);
 
 	xml.ToFile("test.xml");
    return 0;
}
int testcompress()
{
    int ret;
    int len;
    
    unsigned int dlen;
    char buffer[65534];
    int   tdata;

 	char outbuffer[65534];
    char data[65534];
    char key[49];
 	bzero(outbuffer,sizeof(outbuffer));
 
 	
 	bzero(data,sizeof(data));
 	
 	strcpy(key,"wms13711songfree");
 //	strcpy(key,"12594490316530061259449031653006");
 	
    bzero(buffer, sizeof(buffer));
    CBF_Des  pDes;
    CBF_DesEnctypt  encrypt;
 	
 	S_DATA *dtp = (S_DATA *)&tdata;
 	dtp->index = 65534;
 	dtp->serial = 65534;
 
 	printf("int value:%d \n",tdata);
 
 	dtp = (S_DATA *)&tdata;
 	printf("index:%d serial:%d\n",dtp->index,dtp->serial);
 
 
 	strcpy(data,"11111111111111111111111111111111111111");
 	dlen = sizeof(outbuffer);
 	CBF_Tools::Compress((unsigned char *)outbuffer,dlen,(unsigned char *)data,strlen(data));
 	int buflen = dlen;
 	pDes.Encryptchar(outbuffer,buflen,key);
 	
 	pDes.Uncryptchar(outbuffer,buflen,key);
 	dlen = sizeof(data);
 	CBF_Tools::Uncompress((unsigned char *)data,dlen,(unsigned char *)outbuffer,buflen);
 
 
 	dlen = sizeof(buffer);
 
 	if (!CBF_Tools::Compress((unsigned char *)buffer,dlen,(unsigned char *)data,strlen(data)))
 	{
 		printf("ѹ��ʧ��\n");
 		return -1;
 	}
 
 	printf("ѹ����%s��%d\n",buffer,dlen);
 	len = dlen;
 
 	bzero(outbuffer,sizeof(outbuffer));
 	memcpy(outbuffer,buffer,dlen);
 	
 	ret = encrypt.encrypt64(buffer,len,key);
 
 	printf("���ܺ�%s��%d\n",buffer,len);
 
 	ret = encrypt.unencrypt64(buffer,len,key);
 
 	printf("���ܺ�%s��%d\n",buffer,len);
 
 	if (memcmp(buffer,outbuffer,len)!= 0)
 	{
 		printf("�ӽ��ܽ����һ��\n");
 	}
 	
 	bzero(outbuffer,sizeof(outbuffer));
 	if (!CBF_Tools::Uncompress((unsigned char *)outbuffer,dlen,(unsigned char *)buffer,len))
 	{
 		printf("��ѹ��ʧ��\n");
 		return -1;
 	}
    
 	return 0;
}
int testthread()
{
    int i;
    int maxthread=1;
    int threadnum[100];
    int pthread[5];
    pthread[0]=1;
    pthread[1]=2;
    pthread[2]=3;
    pthread[3]=4;
    pthread[4]=5;
    printf("thread%d \n",pthread[0]);
    for (i=1;i<100;i++)
    {
    	if ( i== pthread[0] || (i>maxthread && (i%maxthread==pthread[0] || (i%maxthread==0 && maxthread ==  pthread[0]) )))
    	{
    		printf(" %3d ",i);
    	}
    }
    printf("\nthread%d \n",pthread[1]);
    for (i=1;i<100;i++)
    {
    	if ( i== pthread[1] || (i>maxthread && (i%maxthread==pthread[1] || (i%maxthread==0 && maxthread ==  pthread[1]) )))
    	{
    		printf(" %3d ",i);
    	}
    }
    printf("\nthread%d \n",pthread[2]);
    for (i=1;i<100;i++)
    {
    	if ( i== pthread[2] || (i>maxthread && (i%maxthread==pthread[2] || (i%maxthread==0 && maxthread ==  pthread[2]) )))
    	{
    		printf(" %3d ",i);
    	}
    }
    printf("\nthread%d \n",pthread[3]);
    for (i=1;i<100;i++)
    {
    	if ( i== pthread[3] || (i>maxthread && (i%maxthread==pthread[3] || (i%maxthread==0 && maxthread ==  pthread[3]) )))
    	{
    		printf(" %3d ",i);
    	}
    }
    printf("\n");
    return 0;
}
int testmenu()
{
#ifndef _WINDOWS
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
#endif




    	CTestDateTime dt;
    	CTestTimer    ttimer;
    	CTestSlist tslist;
    	CTestTools  ttools;
    	CTestThread tt;
    	CTestSocket tsocket;
    	CTestQueue tqueue;
    //	tqueue.Test();
    
    	
    
    	while (1)
    	{
    		char tmpchar[20];
    		bzero(tmpchar,sizeof(tmpchar));
    		printf("\n");
    		printf("1   --   ��������ʱ��\n");
    		printf("2   --   ���Զ�ʱ��\n");
    		printf("3   --   ���Էָ���\n");
    		printf("4   --   ���Թ���С����\n");
    		printf("5   --   �����̼߳�����\n");
    		printf("6   --   ����SOCKETͨѶ\n");
    		CBF_Tools::GetConsoleEnter("��ѡ�������Ŀ",2,tmpchar);
    		printf("\n");
    		switch (atoi(tmpchar))
    		{
    			case 1:
    				//��������ʱ��
    				dt.Test();
    				break;
    			case 2:
    				//���Զ�ʱ��
    				ttimer.Test();
    				break;
    			case 3:
    				//���Էָ���
    				tslist.Test();
    				break;
    			case 4:
    				//���Թ���С����
    				ttools.Test();
    				break;
    			case 5:
    				//�����߳�
    				tt.Test();
    				break;
    			case 6:
    				//����SOCKETͨѶ
    				tsocket.Test();
    				break;
    			default:
    				break;
    		}
    		CBF_Tools::GetConsoleEnter("�Ƿ��˳���������(y/n)",1,tmpchar);
    		
    		if (tmpchar[0] == 'y' || tmpchar[0] == 'Y')
    		{
    			break;
    		} 
    	}
    return 0;
}

int testmdb6()
{

    class index_unordered : public CIndex_Field_Base
    {
    public:
        int aa;
        std::string bb;
        int bbc;


        virtual bool operator == (const index_unordered& field) const
        {
            int ret;
            if (aa != field.aa)
            {
                return false;
            }
            else if (bb.compare(field.bb) != 0)
            {
                return false;
            }
            else if (bbc != field.bbc)
            {
                return false;
            }
            return true;
        }
    };
    struct _hash
    {
        std::size_t operator() (const index_unordered& f1) const
        {
            return hash_val(f1.aa,f1.bb,f1.bbc);
        }
    };

    

    CKeyFieldUnordered<index_unordered, _hash> key_3;
    

    // 	pair<LP_SET,bool> r = m_index.insert(10);
    // 	r = m_index.insert(3);
    // 	r = m_index.insert(21);
    index_unordered tit;
    tit.m_nRowId = 0;
    tit.aa = 1;
    tit.bb = "aa";
    tit.bbc = 99;

    key_3.Add(tit);
    

    tit.m_nRowId = 1;
    tit.aa = 1;
    tit.bb = "bb";
    tit.bbc = 88;

    key_3.Add(tit);


    tit.m_nRowId = 2;
    tit.aa = 0;
    tit.bb = "bb";
    tit.bbc = 88;

    key_3.Add(tit);


    tit.m_nRowId = 3;
    tit.aa = 1;
    tit.bb = "aa";
    tit.bbc = 88;

    key_3.Add(tit);
    tit.m_nRowId = 4;

    if (key_3.Find(tit))
    {
        printf("����[%d %s %d]�ظ�\n", tit.aa, tit.bb.c_str(), tit.bbc);
    }

    bool bret = key_3.First(tit);
    while (bret)
    {
        printf("rid=%d %d %s %d \n", tit.m_nRowId, tit.aa, tit.bb.c_str(), tit.bbc);
        bret = key_3.Next(tit);

    }
    printf("\n");

    return 0;
}
int testmdbUnordered()
{
    CPkeyIntUnordered<1> key;
    key.Add(1,3);
    key.Add(2,40000000);
    bool bret = key.Find(3);
    
    // 100��β���
    int testNum = 1000000;
    std::vector<S_BC_SERIAL> vecPlayer;
    S_BC_SERIAL info;
    info.node_id = 100;
    info.node_pid = 1;
    info.timestamp = time(NULL);
    // ��ʼ��
    for (int i = 0; i < testNum; i++)
    {
        info.serial = i;
        vecPlayer.push_back(info);
    }
    // �������
    std::random_shuffle(vecPlayer.begin(), vecPlayer.end());
   
    CTbl_BcSerial tbl_serial;
    UINT64_ tms= CBF_Date_Time::GetTickCount(); 
    for (std::vector<S_BC_SERIAL>::iterator it = vecPlayer.begin(); it != vecPlayer.end(); it++)
    {
        tbl_serial.Insert(*it);
    }
    UINT64_ tms_end = CBF_Date_Time::GetTickCount();
    
    printf("bubase mdbbase insert %d �κ�ʱ %d ƽ�� %f \n", testNum, tms_end - tms, 1.00*(tms_end - tms) / testNum);
    std::vector<const S_BC_SERIAL*>reslist;
    tms = CBF_Date_Time::GetTickCount();
    for (std::vector<S_BC_SERIAL>::iterator it = vecPlayer.begin(); it != vecPlayer.end(); it++)
    {
        S_BC_SERIAL* pbc =tbl_serial.Select(it->node_id,it->node_pid,it->serial);
        if (pbc == NULL)
        {
            printf("select is NULL\n");
        }
    }
    tms_end = CBF_Date_Time::GetTickCount();
    
    printf("bubase mdbbase select %d �κ�ʱ %d ƽ�� %f \n", testNum, tms_end - tms, 1.00 * (tms_end - tms) / testNum);
    return 0;
}

int main(int argc,char *argv[])
{
	pLog.SetLogPara(LOG_DEBUG,"","test.log");
	pLog.StartLog();
    //testmdbUnordered();
    testmdb6();
	return 0;
}

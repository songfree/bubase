// FuncTbl.h: interface for the CFuncTbl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUNCTBL_H__17F68A7D_AC29_4ACD_9FD9_31FC164D69A9__INCLUDED_)
#define AFX_FUNCTBL_H__17F68A7D_AC29_4ACD_9FD9_31FC164D69A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MdbBase.h"
#include "public.h"
#include "IBuModule.h"


#ifdef BUMODULE_EXPORTS
#define BUMODULE_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define BUMODULE_EXPORT __declspec(dllimport)
#else
#define BUMODULE_EXPORT 
#endif
#endif

class BUMODULE_EXPORT CFuncTbl  
{
protected:
	
	CMemTableNew <S_TBL_FUNC> m_table;
	CPkeyIntUnordered<1>                 m_pkey;               //���ܺŵ�key
public:

	// ������: Clear
	// ���  : ������ 2010-7-1 14:54:35
	// ����  : void 
	// ����  : ��ձ�����
	void Clear();

	// ������: Next
	// ���  : ������ 2010-6-29 10:53:55
	// ����  : bool ���򷵻�false
	// ����  : S_TBL_FUNC &funcinfo
	// ����  : ������һ��������Ϣ�����򷵻�false
	bool Next(S_TBL_FUNC &funcinfo);

	// ������: First
	// ���  : ������ 2010-6-29 10:53:20
	// ����  : bool ���򷵻�false
	// ����  : S_TBL_FUNC &funcinfo
	// ����  : ���ص�һ��������Ϣ
	bool First(S_TBL_FUNC &funcinfo);

	// ������: Select
	// ���  : ������ 2010-6-29 10:51:02
	// ����  : bool �����ڷ���false
	// ����  : int nFuncNo  Ҫ���ҵĹ��ܺ�
	// ����  : S_TBL_FUNC &funcinfo
	// ����  : ���ҹ�����Ϣ
	bool Select(int nFuncNo,S_TBL_FUNC &funcinfo);


	// ������: Find
	// ���  : ������ 2013-4-25 11:45:13
	// ����  : S_TBL_FUNC &  
	// ����  : int nFuncNo Ҫ���ҵĹ��ܺ�
	// ����  : ���ҹ�����Ϣ
	S_TBL_FUNC & Find(int nFuncNo);

	// ������: Update
	// ���  : ������ 2010-6-29 10:50:32
	// ����  : bool �����ܲ������򷵻�false
	// ����  : S_TBL_FUNC funcinfo
	// ����  : ����һ��������Ϣ
	bool Update(S_TBL_FUNC funcinfo);

	// ������: Insert
	// ���  : ������ 2010-6-29 10:49:58
	// ����  : bool �����ܺŴ����򷵻�false
	// ����  : S_TBL_FUNC funcinfo
	// ����  : insertһ��������Ϣ
	bool Insert(S_TBL_FUNC funcinfo);

	CFuncTbl();
	virtual ~CFuncTbl();
protected:

	S_TBL_FUNC  m_pNull;


};


class BUMODULE_EXPORT CFuncTblOld 
{
protected:
	
	CMemTable <S_TBL_FUNC_OLD> m_table;
	CIndexInt<1>                 m_pkey;               //���ܺŵ�key
public:
	
	// ������: Clear
	// ���  : ������ 2010-7-1 14:54:35
	// ����  : void 
	// ����  : ��ձ�����
	void Clear();
	
	// ������: Next
	// ���  : ������ 2010-6-29 10:53:55
	// ����  : bool ���򷵻�false
	// ����  : S_TBL_FUNC_OLD &funcinfo
	// ����  : ������һ��������Ϣ�����򷵻�false
	bool Next(S_TBL_FUNC_OLD &funcinfo);
	
	// ������: First
	// ���  : ������ 2010-6-29 10:53:20
	// ����  : bool ���򷵻�false
	// ����  : S_TBL_FUNC_OLD &funcinfo
	// ����  : ���ص�һ��������Ϣ
	bool First(S_TBL_FUNC_OLD &funcinfo);
	
	// ������: Select
	// ���  : ������ 2010-6-29 10:51:02
	// ����  : bool �����ڷ���false
	// ����  : int nFuncNo  Ҫ���ҵĹ��ܺ�
	// ����  : S_TBL_FUNC_OLD &funcinfo
	// ����  : ���ҹ�����Ϣ
	bool Select(int nFuncNo,S_TBL_FUNC_OLD &funcinfo);
	
	
	// ������: Find
	// ���  : ������ 2013-4-25 11:45:13
	// ����  : S_TBL_FUNC_OLD &  
	// ����  : int nFuncNo Ҫ���ҵĹ��ܺ�
	// ����  : ���ҹ�����Ϣ
	S_TBL_FUNC_OLD & Find(int nFuncNo);
	
	// ������: Update
	// ���  : ������ 2010-6-29 10:50:32
	// ����  : bool �����ܲ������򷵻�false
	// ����  : S_TBL_FUNC_OLD funcinfo
	// ����  : ����һ��������Ϣ
	bool Update(S_TBL_FUNC_OLD funcinfo);
	
	// ������: Insert
	// ���  : ������ 2010-6-29 10:49:58
	// ����  : bool �����ܺŴ����򷵻�false
	// ����  : S_TBL_FUNC_OLD funcinfo
	// ����  : insertһ��������Ϣ
	bool Insert(S_TBL_FUNC_OLD funcinfo);
	
	CFuncTblOld();
	virtual ~CFuncTblOld();
protected:
	
	S_TBL_FUNC_OLD  m_pNull;
	
	
};

#undef  BUMODULE_EXPORT
#endif // !defined(AFX_FUNCTBL_H__17F68A7D_AC29_4ACD_9FD9_31FC164D69A9__INCLUDED_)

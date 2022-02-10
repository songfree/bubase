// BuModule.h: interface for the CBuModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUMODULE_H__0D5DA66A_18DD_4BA9_B754_79E1C9529474__INCLUDED_)
#define AFX_BUMODULE_H__0D5DA66A_18DD_4BA9_B754_79E1C9529474__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "IBuModule.h"
#include "BF_SimpleLog.h"
#include "FuncTbl.h"
#include "DbApi.h"
#include "BF_Xml.h"
#include "IPcLink.h"
#include "BpcHead.h"
#include "BF_Tools.h"
#include "XmlPackArray.h"
#include "dreberrcode.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "bf_oldxmld.lib") 
#pragma comment(lib, "bf_dbapid.lib") 
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib bf_dbapid.lib bf_oldxmld.lib ")
#else
#pragma comment(lib, "bf_oldxml.lib") 
#pragma comment(lib, "bf_dbapi.lib") 
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with  bf_kernel.lib  bf_dbapi.lib bf_oldxml.lib")
#endif
#endif

#ifdef BUMODULE_EXPORTS
#define BUMODULE_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define BUMODULE_EXPORT __declspec(dllimport)
#else
#define BUMODULE_EXPORT 
#endif
#endif

class BUMODULE_EXPORT CBuModule   : public CIBuModule
{
public:
	void BindOldFunc();

	// 函数名: BpuCall
	// 编程  : 王明松 2013-7-17 15:14:22
	// 返回  : int  -1失败 0成功
	// 参数  : PBPCCOMMSTRU data
	// 参数  : LIST_BPC_RSMSG &ansdata
	// 描述  : 调用本BPU内的交易
	int BpuCall(PBPCCOMMSTRU data,LIST_BPC_RSMSG &ansdata);

	// 函数名: BpcCall
	// 编程  : 王明松 2013-7-17 15:13:34
	// 返回  : int -1失败 0成功 
	// 参数  : PBPCCOMMSTRU data
	// 参数  : LIST_BPC_RSMSG &ansdata
	// 参数  : unsigned int ts
	// 描述  : 调用BPC里的交易，不去总线。要填写data里的sDbHead里的d_info里的值
	int BpcCall(PBPCCOMMSTRU data,LIST_BPC_RSMSG &ansdata,unsigned int ts);

	// 函数名: TransData
	// 编程  : 王明松 2013-7-17 15:12:20
	// 返回  : int -1失败 0成功 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 消息转移，将请求的sDbHead全复制过来，要转移的目标填写在d_info里
	int TransData(PBPCCOMMSTRU data);

	// 函数名: ExtCall
	// 编程  : 王明松 2013-7-17 15:11:20
	// 返回  : int -1失败 0成功 
	// 参数  : PBPCCOMMSTRU data
	// 参数  : LIST_BPC_RSMSG &ansdata
	// 参数  : unsigned int ts
	// 描述  : 外调总线里的交易  要填写data里的sDbHead里的d_info里的值
	int ExtCall(PBPCCOMMSTRU data,LIST_BPC_RSMSG &ansdata,unsigned int ts);

	// 函数名: AnsData
	// 编程  : 王明松 2013-7-17 14:37:46
	// 返回  : int -1失败 0成功 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 应答数据，只设置应答标志为1
	int AnsData(PBPCCOMMSTRU data);

	
	// 函数名: NextFuncInfo
	// 编程  : 王明松 2010-7-1 9:20:42
	// 返回  : virtual bool 
	// 参数  : S_TBL_FUNC &funcinfo
	// 描述  : 得到下一个功能信息，前提是要调用FirstFuncInfo
	virtual bool NextFuncInfo(S_TBL_FUNC &funcinfo);
	
	// 函数名: FirstFuncInfo
	// 编程  : 王明松 2010-7-1 9:20:30
	// 返回  : virtual bool 
	// 参数  : S_TBL_FUNC &funcinfo
	// 描述  : 得到第一个功能信息
	virtual bool FirstFuncInfo(S_TBL_FUNC &funcinfo);

	// 函数名: BuInit
	// 编程  : 王明松 2010-6-29 16:17:11
	// 返回  : virtual bool 
	// 描述  : 初始化，可在此方法里做一些特殊处理，但要置m_bIsInit为true
	virtual bool BuInit();
	
	// 函数名: Run
	// 编程  : 王明松 2010-6-29 16:17:00
	// 返回  : virtual int 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 调用原子功能
	virtual int Run(PBPCCOMMSTRU data);

	// 函数名: RunXml
	// 编程  : 王明松 2010-6-29 16:17:00
	// 返回  : virtual int 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 统一调用兼容老框架里的原子方子，注册时所有的老框架的交易都绑定到此方法
	virtual int RunXml(PBPCCOMMSTRU data);

	// 函数名: SetPuPara
	// 编程  : 王明松 2010-6-30 11:00:30
	// 返回  : void 
	// 参数  : CBuXml *conxml
	// 参数  : CICallBu *callbu
	// 参数  : CDbApi *sqldb
	// 参数  : InitFuncInfo *bulog
	// 描述  : 设置参数
	virtual void SetPuPara(CBF_Xml *conxml,CIPcLink *pclink,CDbApi *sqldb,CBF_SimpleLog *bulog);

	// 函数名: InitFuncInfo
	// 编程  : 王明松 2010-6-29 16:16:28
	// 返回  : void 
	// 参数  : int funcNo
	// 参数  : FuncPointer pfunc
	// 参数  : const char *funcName
	// 参数  : double version
	// 参数  : const char *author
	// 参数  : int level
	// 参数  : bool repealFlag
	// 参数  : bool isTxUnit
	// 参数  : bool isStop
	// 描述  : 设置功能信息，在构建方法里调用
	void InitFuncInfo(int funcNo,FuncPointer pfunc,const char *funcName,double version,const char *author,int level,bool repealFlag,bool isTxUnit,bool isStop);

	/// 函数名: setFuncInfo
	/// 编程  : 王明松 2007-4-28 15:08:26
	/// 返回  : void 
	/// 参数  : int funcNo  功能号
	/// 参数  : FuncPointer pfunc 函数指针
	/// 参数  : char *funcName  功能名称
	/// 参数  : double version  版本，功能号相同的话只使用最新的版本
	/// 参数  : char *author  作者
	/// 参数  : int level     级别
	/// 参数  : bool repealFlag   冲正标志
	/// 参数  : bool isTxUnit     是否交易单位
	/// 参数  : bool isStop       功能是否停止
	/// 参数  : int appType       应用类型  可根据此类型来连接不同的数据库
	/// 描述  : 设置功能的参数，兼容老框架的xmlpack的方法
	void setFuncInfo(int funcNo,FuncPointerOld pfunc,char *funcName,double version,char *author,int level,bool repealFlag,bool isTxUnit,bool isStop,int appType);
	
	
	CBuModule();
	virtual ~CBuModule();

	

protected:
	CBF_Xml       *m_pConfigXml;//配置文件
	CIPcLink      *m_pPcLink;//外部调用
	CBF_SimpleLog *m_pLog;  //日志
	CDbApi        *m_pSqlCmd; //数据库连接
	bool           m_bIsInit;  //Init方法调用成功后置为true
	bool           m_bIsSetPara;//是否已设置参数
	CFuncTbl       m_pFuncTbl; //功能列表
	CFuncTblOld    m_pFuncTblOld;//存放兼容老框架的交易信息

	BPCCOMMSTRU    m_pAnsData; //仅用于应答，其他地方不要用，当调用AnsData后数据因字节序处理会发生变化，所以每次要重新置值

	CXmlPack       m_xmlpack;
	CXmlPackArray  m_muxmlpack;
};
#undef  BUMODULE_EXPORT
#endif // !defined(AFX_BUMODULE_H__0D5DA66A_18DD_4BA9_B754_79E1C9529474__INCLUDED_)

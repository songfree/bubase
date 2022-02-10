/**
* Copyright (c) 2008 songfree
* All rights reserved.
*
* 文件名称:MoniSvr.cpp
* 摘要:
*       
*        
*
* 当前版本号:1.0.0
*
* 版本历史信息:
* 版本号            作者/修改者       完成日期                 原因
* 1.0.0                AUTO           20131127                  创建
**/
// MoniSvr.h: interface for the CMoniSvr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(BUBASE_AUTO_APPWIZARD_20131127_4261728_INCLUDED_)
#define BUBASE_AUTO_APPWIZARD_20131127_4261728_INCLUDED_

#include "BuModule.h"



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class DLL_EXPORT CMoniSvr : public CBuModule  
{
public:
	
public:
	CMoniSvr();
	virtual ~CMoniSvr();

public:
	// 函数名: BuInit
	// 编程  : 王明松 2010-6-29 16:17:11
	// 返回  : virtual bool 
	// 描述  : 初始化，可在此方法里做一些特殊处理，但要置m_bIsInit为true
	virtual bool BuInit();
	

	// 函数名: SetBuPara
	// 编程  : 王明松 2010-6-30 11:00:30
	// 返回  : void 
	// 参数  : CBuXml *conxml
	// 参数  : CICallBu *callbu
	// 参数  : CDbApi *sqldb
	// 参数  : CIBuLog *bulog
	// 描述  : 设置参数
	virtual void SetBuPara(CBF_Xml *conxml,CIPcLink *pclink,CDbApi *sqldb,CBF_SimpleLog *bulog);
//      在此处增加新功能
public:
	bool GetSerialDb2(int &no);

	// 函数名: AddPcNode
	// 编程  : 王明松 2013-11-29 16:11:17
	// 返回  : int 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 增加物理节点
	int AddPcNode(PBPCCOMMSTRU data);

	// 函数名: NodeReport
	// 编程  : 王明松 2013-11-27 10:16:27
	// 返回  : int 
	// 参数  : PBPCCOMMSTRU data
	// 描述  : 节点报告处理
	int NodeReport(PBPCCOMMSTRU data);
	

	
protected:

	// 函数名: UpdateByLastTime
	// 编程  : 王明松 2013-11-29 16:09:47
	// 返回  : int 
	// 描述  : 更新状态，根据最后更新时间
	int UpdateByLastTime();

	// 函数名: Bpc2Report
	// 编程  : 王明松 2013-11-29 16:09:33
	// 返回  : int 
	// 参数  : CBF_Xml *xml
	// 描述  : 类BPC报告处理
	int Bpc2Report(CBF_Xml *xml);

	// 函数名: BpcReport
	// 编程  : 王明松 2013-11-29 16:09:21
	// 返回  : int 
	// 参数  : CBF_Xml *xml
	// 描述  : BPC报告处理
	int BpcReport(CBF_Xml *xml);

	// 函数名: DrebReport
	// 编程  : 王明松 2013-11-29 16:09:09
	// 返回  : int 
	// 参数  : CBF_Xml *xml
	// 描述  : DREB节点报告处理
	int DrebReport(CBF_Xml *xml);

	// 函数名: GetSerial
	// 编程  : 王明松 2013-11-29 16:08:51
	// 返回  : bool 
	// 参数  : char *logserial
	// 描述  : 取流水，删除日志里多的记录
	bool GetSerial(char *logserial);

	int   m_nSaveLogNum;//保存的日志数
	int   m_nWriteBFMLog;
	int   m_nStatusTimeOut;
	int   m_nNodeType;//节点类型
	int   m_nNodeId;//公共节点号
	int   m_nNodePrivateId;//私有节点号
	int   m_nReportType;//报告类型
	int   m_nPcNodeId;  //机器id
	std::string  m_sStartTime;//启动时间

	unsigned int m_nCurTime;

	char m_sSql[8*1024];

	CBF_Date_Time m_pDate;

};
//注意：下面的定义在每个类里面都要有，主程序根据此方法获取本类的实例
extern "C" 
{ 
    DLL_EXPORT CMoniSvr * getInstance() ; 
}

#endif // !BUBASE_AUTO_APPWIZARD_20131127_4261728_INCLUDED_)

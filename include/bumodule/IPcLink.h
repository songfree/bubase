// IPcLink.h: interface for the CIPcLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPCLINK_H__189DF427_9657_4008_9915_8DA339C13457__INCLUDED_)
#define AFX_IPCLINK_H__189DF427_9657_4008_9915_8DA339C13457__INCLUDED_

//BPC或FPC连接的基类
#include "public.h"
#include "BpcEndian.h"
#include "BpcHead.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CIPcLink  
{
public:
	/// 函数名: SetGPara1
	/// 编程  : 王明松 2008-12-30 9:42:48
	/// 返回  : virtual bool true成功false失败
	/// 参数  : bool setflag=true
	/// 描述  : 接口虚函数，这里要实现.设置全局变量1,若已设置则返回false
	virtual bool SetGPara1(bool setflag=true)=0;
	
	
	/// 函数名: isSetGPara2
	/// 编程  : 王明松 2008-12-30 9:36:10
	/// 返回  : virtual bool true成功false失败
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量2,若已设置则返回false
	virtual bool SetGPara2(bool setflag=true)=0;
	
	
	/// 函数名: isSetGPara3
	/// 编程  : 王明松 2008-12-30 9:36:13
	/// 返回  : virtual bool true成功false失败
	/// 参数  : bool setflag=true
	/// 描述  : 设置全局变量3,若已设置则返回false
	virtual bool SetGPara3(bool setflag=true)=0;
	
	/// 函数名: GetThreadShareSerial
	/// 编程  : 王明松 2008-12-30 9:36:13
	/// 返回  : virtual long 流水号
	/// 描述  : 取得唯一的流水号
	virtual long GetThreadShareSerial()=0;


	// 函数名: AnsData
	// 编程  : 王明松 2013-4-23 15:16:43
	// 返回  : virtual int -1失败 0成功
	// 参数  : S_BPC_RSMSG *data
	// 描述  : 应答数据,由BPU业务里面的原子调用
	virtual int AnsData(PBPCCOMMSTRU data) =0;

	// 函数名: DPTrans
	// 编程  : 王明松 2013-4-23 15:11:48
	// 返回  : virtual int -1失败 0成功
	// 参数  : PBPCCOMMSTRU calldata
	// 描述  : 转移至其它服务处理
	virtual int DPTrans(PBPCCOMMSTRU calldata) =0;

	// 函数名: BpuCall
	// 编程  : 王明松 2013-4-23 15:09:13
	// 返回  : virtual int -1失败 0成功
	// 参数  : PBPCCOMMSTRU calldata
	// 参数  : LIST_BPC_RSMSG &ansdata
	// 描述  : 调用BPU内的功能，给流程控制使用
	virtual int BpuCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata) =0;

	// 函数名: BpcCall
	// 编程  : 王明松 2013-4-23 15:08:18
	// 返回  : virtual int -1失败 0成功
	// 参数  : PBPCCOMMSTRU calldata
	// 参数  : LIST_BPC_RSMSG &ansdata
	// 描述  : 调用bpc功能
	virtual int BpcCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout) =0;

	// 函数名: ExtCall
	// 编程  : 王明松 2013-4-23 15:01:25
	// 返回  : virtual int -1失败 0成功
	// 参数  : PBPCCOMMSTRU calldata
	// 参数  : vector<S_BPC_RSMSG> &ansdata
	// 描述  : 外调BPC
	virtual int ExtCall(PBPCCOMMSTRU calldata,LIST_BPC_RSMSG &ansdata,unsigned int stimeout) =0;
	

	// 函数名:  InitLink
	// 编程  : 王明松 2013-4-23 14:44:14
	// 返回  : virtual bool true成功 false失败
	// 参数  : const char *confile
	// 描述  : 初始化连接配置 
	virtual bool  InitLink(const char *confile) =0;
	CIPcLink()
	{

	}
	virtual ~CIPcLink()
	{

	}



};

#endif // !defined(AFX_IPCLINK_H__189DF427_9657_4008_9915_8DA339C13457__INCLUDED_)

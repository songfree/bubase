/// TransPackNew.h: interface for the CTransPack class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSPACKNEW_H__C279BF91_1D35_4A2B_B76E_F9BF45B56C62__INCLUDED_)
#define AFX_TRANSPACKNEW_H__C279BF91_1D35_4A2B_B76E_F9BF45B56C62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000
#include "public.h"
#include "PackConvert.h"

#ifdef DPTOOL_EXPORTS
#define DPTOOL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DPTOOL_EXPORT __declspec(dllimport)
#else
#define DPTOOL_EXPORT 
#endif
#endif


typedef struct _S_SERVER_INFO_
{
	std::string sip;///服务IP
	int    nport;///服务端口
	std::string sid;///服务ID
	std::string sname;///服务名
	std::string stransname;///转换配置名称
	bool    bisTrans;///是否包转换
};
typedef struct _S_TRANS_INFO_
{
	std::string stransname;///转换配置名称
	std::string sfilename; ///转换配置文件
	CPackConvert *packTrans;///包转换类
};
class DPTOOL_EXPORT CTransPackNew  
{
public:
	CTransPackNew();
	virtual ~CTransPackNew();
	bool m_bisUse;
	char m_errMsg[256];
public:

	/// 函数名: IsPut
	/// 编程  : 王明松 2008-6-27 15:13:07
	/// 返回  : bool 
	/// 参数  : _S_SERVER_INFO sinfo
	/// 描述  : 配置是否可放入
	bool IsPut(_S_SERVER_INFO_ sinfo);

	/// 函数名: GetServerId
	/// 编程  : 王明松 2008-4-27 15:12:32
	/// 返回  : int <0失败 =0成功
	/// 参数  : string ip
	/// 参数  : string &id
	/// 描述  : 根据IP地址获取服务ID
	int GetServerId(std::string ip, std::string &id);

	/// 函数名: GetServerPara
	/// 编程  : 王明松 2008-4-27 15:11:46
	/// 返回  : int <0失败 =0成功
	/// 参数  : string sid
	/// 参数  : string &ip
	/// 参数  : int &port
	/// 描述  : 根据服务ID获取IP及端口
	int GetServerPara(std::string sid, std::string &ip,int &port);

	/// 函数名: GetTransByServerId
	/// 编程  : 王明松 2008-4-27 15:10:35
	/// 返回  : CPackConvert * 
	/// 参数  : string sid
	/// 参数  : int &istrans 是否要转换，1要0否-1未找到配置
	/// 描述  : 通过服务ID来获取转换类实例 
	CPackConvert * GetTransByServerId(std::string sid,int &istrans);

	/// 函数名: GetTransByIp
	/// 编程  : 王明松 2008-4-27 15:10:31
	/// 返回  : CPackConvert * 
	/// 参数  : string sip
	/// 参数  : int &istrans 是否要转换，1要0否-1未找到配置
	/// 描述  : 通过连接的IP获取转换类实例
	CPackConvert * GetTransByIp(std::string sip,int &istrans);

	/// 函数名: Init
	/// 编程  : 王明松 2008-4-27 15:10:12
	/// 返回  : int <0失败 =0成功
	/// 参数  : CXmlPack *xmlpack
	/// 描述  : 初始化转换
	int Init(CXmlPack *xmlpack);

protected:

	std::vector <_S_SERVER_INFO_> m_serverList;//接入配置
	std::vector <_S_TRANS_INFO_> m_transList;//转换配置
};
#undef DPTOOL_EXPORT
#endif /// !defined(AFX_TRANSPACKNEW_H__C279BF91_1D35_4A2B_B76E_F9BF45B56C62__INCLUDED_)

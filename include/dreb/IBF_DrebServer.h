// BF_DrebServer.h: interface for the CBF_DrebServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IBF_DREBSERVER_H__8810FE91_B2B1_4312_9C27_4C97F9219A30__INCLUDED_)
#define AFX_IBF_DREBSERVER_H__8810FE91_B2B1_4312_9C27_4C97F9219A30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "public.h"
#include "DrebPubDefine.h"
#include "BpcHead.h"
#include "BF_BufferPool.h"
#include "BF_LogClient.h"
#include "BF_HostInfo.h"
#include "dreberrcode.h"

class  CIBF_DrebServer 
{
public:
    virtual int GetDataLogLevel()=0;

    // 函数名: GetErrMsg
    // 编程  : 王明松 2015-7-21 10:28:00
    // 返回  : std::string 
    // 描述  : 取得出错信息
    virtual std::string GetErrMsg()=0;

    // 函数名: SetDataLogLevel
    // 编程  : 王明松 2015-7-15 15:01:38
    // 返回  : void 
    // 参数  : int loglevel
    // 描述  : 设置报文日志级别，即时生效
    virtual void SetDataLogLevel(int loglevel)=0;


    // 函数名: UnzipBuf
    // 编程  : 王明松 2014-12-15 8:47:49
    // 返回  : bool 
    // 参数  : BPCCOMMSTRU &data
    // 描述  : 解开报文数据，针对监控指令
    virtual bool UnzipBuf(BPCCOMMSTRU& data)=0;

    // 函数名: GetLogPoint
    // 编程  : 王明松 2014-12-12 10:31:28
    // 返回  : CIErrlog * 
    // 描述  : 返回日志类指针，可通过此指针写入日志信息
    virtual CIErrlog* GetLogPoint()=0;

    // 函数名: GetDrebInfo
    // 编程  : 王明松 2014-12-10 16:35:34
    // 返回  : bool 
    // 参数  : int index  连接索引
    // 参数  : char *sip  ip地址
    // 参数  : int &port  端口
    // 参数  : int &drebid   公共节点号
    // 参数  : int &privateid  私有节点号
    // 参数  : char *status  状态   "未连接"  "正常"   "正在连接"
    // 描述  : 获取DREB总线连接信息
    virtual bool GetDrebInfo(int index, char* sip, int& port, int& drebid, int& privateid, char* status)=0;

    // 函数名: GetBufferPoolInfo
    // 编程  : 王明松 2014-12-10 16:30:51
    // 返回  : void 
    // 参数  : int &total    总共分配了多少块
    // 参数  : int &unused   还有多少块未使用
    // 参数  : int &bufsize  每块大小
    // 描述  : 获取内存缓冲池信息
    virtual void GetBufferPoolInfo(int& total, int& unused, int& bufsize)=0;

    // 函数名: RegisterDreb
    // 编程  : 王明松 2013-4-8 15:10:22
    // 返回  : void 
    // 参数  : int index        dreb连接索引
    // 参数  : CInt *funclist   交易列表
    // 描述  : 在连接的DREB上发注册消息,将交易码注册到总线上,这样可以按交易码路由，但广播不受此限制，可调用多次来进行注册
    virtual void RegisterDreb(int index, vector<int>* funclist)=0;


    // 函数名: UnRegisterDreb
    // 编程  : 王明松 2014-12-12 10:07:43
    // 返回  : void 
    // 描述  : 取消在dreb上注册的交易
    virtual void UnRegisterDreb()=0;

    // 函数名: GetMsgData
    // 编程  : 王明松 2014-12-10 15:06:39
    // 返回  : bool true成功 
    // 参数  : S_BPC_RSMSG &rdata  消息
    // 参数  : unsigned int waitetms=1000  没有消息的等待时间
    // 参数  : bool get_front=true  true从头取,false从尾取 
    // 描述  : 从队列里取消息进行处理，注意要调用poolfree释放rdata里的空间
    virtual bool GetMsgData(S_BPC_RSMSG& rdata, unsigned int waitetms = 1000, bool get_front = true)=0;

    // 函数名: SendMsg
    // 编程  : 王明松 2014-12-10 15:00:22
    // 返回  : int  -1发送失败 0发送成功  >0放入队列
    // 参数  : S_BPC_RSMSG &sdata
    // 描述  : 向DREB发送数据 注意bpchead里的index填写，若是应答原样即可，新请求可填一个比较大的值，这样让本api选择一个连接发送
    //          注意在本方法里会主动释放sdata空间
    //          a_cNodePrivateId来标识是否转移交易，当总线收到判断此标识为100时，不对s_info进行重置
    virtual int SendMsg(S_BPC_RSMSG& sdata)=0;

    // 函数名: Stop
    // 编程  : 王明松 2014-12-10 14:15:10
    // 返回  : void 
    // 描述  : 停止api线程
    virtual void Stop()=0;

    // 函数名: Start
    // 编程  : 王明松 2014-12-10 14:14:58
    // 返回  : bool 
    // 描述  : 启动api线程
    virtual bool Start()=0;


    // 函数名: MonitorThread
    // 编程  : 王明松 2014-12-15 8:48:34
    // 返回  : void 
    // 描述  : 监控线程是否停止，若停了则重启
    virtual void MonitorThread()=0;



    // 函数名: PoolFree
    // 编程  : 王明松 2014-12-5 11:32:37
    // 返回  : void 
    // 参数  : void *ppoint
    // 描述  : 释放空间
    virtual void PoolFree(void* ppoint)=0;

    // 函数名: PoolMalloc
    // 编程  : 王明松 2014-12-5 11:32:41
    // 返回  : void * 
    // 描述  : 分配一个空间
    virtual void* PoolMalloc()=0;

    virtual CBF_BufferPool* GetBufferPool()=0;

    // 函数名: GetHostInfo
    // 编程  : 王明松 2014-12-10 14:27:21
    // 返回  : void 
    // 参数  : S_MONITOR_HOST &host
    // 参数  : vector<S_MONITOR_DISK>&disk
    // 描述  : 获取主机cpu、内存、磁盘信息，前提是资源的标志g_nMonitorHost为1
    virtual void GetHostInfo(S_MONITOR_HOST& host, vector<S_MONITOR_DISK>& disk)=0;

};

#endif // !defined(AFX_IBF_DREBSERVER_H__8810FE91_B2B1_4312_9C27_4C97F9219A30__INCLUDED_)

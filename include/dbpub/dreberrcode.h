// dreberrcode.h: interface for the dreberrcode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBERRCODE_H__FD1208CA_2314_4920_98EA_D003257FD554__INCLUDED_)
#define AFX_DREBERRCODE_H__FD1208CA_2314_4920_98EA_D003257FD554__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define  SUCCESS              0   //成功
#define  ERR_SVRID            1   //服务号不符
#define  ERR_PRIVATEID        2   //私有服务号不符
#define  ERR_REGISTER         3   //服务已注册
#define  ERR_SVRNOTREGISTER   4   //服务未注册
#define  ERR_DREBID           5   //通讯节点ID不符
#define  ERR_DREBPRIVATEID    6   //通讯私有节点ID不符
#define  ERR_DREBREGISTER     7   //通讯节点已注册
#define  ERR_DREBNODE         8   //通讯节点重复
#define  ERR_QUEUE            9    //入队列失败
#define  ERR_DREBROUTE        10   //通讯节点路由不存在
#define  ERR_VERSION          11   //连接版本不符
#define  ERR_SERVICENO        12   //注册功能号不符
#define  ERR_FUNCNOTFUND      13   //功能号不存在
#define  ERR_NEXTFILENOT      14   //后续包文件不存在
#define  ERR_NOBPUGROUP       15   //无此BPU组
#define  ERR_NOBPUFREE        16   //无BPU空闲来处理
#define  ERR_TIMEOUT          17   //请求在分配队列已超时
#define  ERR_DATA_ENCRYPTZIP  18   //数据处理错，解密解压缩错
#define  ERR_SYSTEM           19   //系统错

#define  ERR_BPC_REGTX          21   //注册交易错误
#define  ERR_BPC_NOGROUP        22   //注册交易错误,无BPU组
#define  ERR_BPC_NOREGISTER     23   //注册交易错误,无需注册
#define  ERR_BPC_BPULIMIT       24   //注册交易错误,bpu连接限制,已满
#define  ERR_BPC_TXFIND         25   //注册交易错误,交易已注册
#define  ERR_START_DREBTIMER     26   //启动dreb监控定时器失败
#define  ERR_STOP_DREBTIMER      27   //停止dreb监控定时器失败

#define  ERR_BPU_INIT             40   //BPU初始化出错
#define  ERR_BPU_CATCH            41   //BPU运行异常出错
#define  ERR_BPU_NOTFUNC          42   //BPU无此交易
#define  ERR_BPU_DATA             43   //BPU交易数据不符
#define  ERR_BPU_MONITORDATA      44   //BPU监控数据不符
#define  ERR_MONITOR_DOWNLOAD     45   //下载文件出错
#define  ERR_MONITOR_UPLOAD       46   //上传文件出错
#endif // !defined(AFX_DREBERRCODE_H__FD1208CA_2314_4920_98EA_D003257FD554__INCLUDED_)

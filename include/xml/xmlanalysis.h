#ifndef CXMLANALYSIS_H_HEADER_INCLUDED_C1239117
#define CXMLANALYSIS_H_HEADER_INCLUDED_C1239117

#ifdef XML_EXPORTS
#define XML_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define XML_EXPORT __declspec(dllimport)
#else
#define XML_EXPORT 
#endif
#endif



/* *** Updated by songfree at 2007-4-30 17:06:36 ****** 
下面是五个在XML文档中预定义好的实体：
&lt; <  小于号
&gt; >  大于号
&amp; & 和
&apos; ' 单引号
&quot; " 双引号
*******************************************************/

 




class CNode;
class CAttribute;

//##ModelId=3EDC620B0251
//##Documentation
//## //分析XML数据报，重组XML数据报
class XML_EXPORT CXMLAnalysis
{
public:
	bool GetNodeName(const char *buffer,int buflen, std::string &nodename);
	bool FindEndNode(int bOffset,int eOffset,std::string nodename,int &vend,int &nextnodebegin);
	void setItem(CNode *pNode, char *value);
	/// 函数名: *analyseXML
	/// 编程  : 王明松 2006-1-23 14:21:23
	/// 返回  : CNode 
	/// 参数  : const char * buffer
	/// 参数  : int bufferlen
	/// 参数  : CNode * parentNode
	/// 描述  : 分析xml
	CNode *analyseXML(const char * buffer,int bufferlen,CNode * parentNode);



	/// 函数名: *analyseXML2
	/// 编程  : 王明松 2009-1-23 14:34:16
	/// 返回  : CNode 
	/// 参数  : int beginoffset
	/// 参数  : int endoffset
	/// 参数  : CNode * parentNode
	/// 描述  : 分析数据beginoffset和endoffset之间的xml数据至parentNode节点
	CNode *analyseXML2(int beginoffset,int endoffset,CNode * parentNode);

	CNode *analyseXML2Old(const char * buffer,int bufferlen, CNode * parentNode);
private:
	char *m_xmldatabuf;
public:

	/// 函数名: AddSlash
	/// 编程  : 王明松 2009-1-24 8:36:34
	/// 返回  : void 
	/// 参数  : char *path
	/// 描述  : 路径处理
	void AddSlash(char *path);

	/// 函数名: makedir
	/// 编程  : 王明松 2009-1-24 8:33:34
	/// 返回  : void 
	/// 参数  : char *path
	/// 描述  : 建立路径中没有的目录
	void makedir(char *path);

	/// 函数名: *Today
	/// 编程  : 王明松 2009-1-24 8:33:53
	/// 返回  : char 
	/// 描述  : 取当前日期YYYYMMDD
	char *Today();

	/// 函数名: getName
	/// 编程  : 王明松 2009-1-24 8:34:20
	/// 返回  : int 
	/// 参数  : char *filename
	/// 描述  : 去掉文件名前的路径
	int getName(char *filename);

	/// 函数名: *Get_Date
	/// 编程  : 王明松 2009-1-24 8:34:35
	/// 返回  : char 
	/// 描述  : 取日期时间yyyymmdd hh:mm:ss
	char *Get_Date();

	/// 函数名: LogMp
	/// 编程  : 王明松 2009-1-24 8:20:56
	/// 返回  : int 
	/// 参数  : int level
	/// 参数  : const char *filename
	/// 参数  : int line
	/// 参数  : const char *fmt
	/// 参数  : ...
	/// 描述  : 写日志
	int LogMp(int level, const char *filename, int line,const char *data,int datalen,const char *fmt,...);

	/// 函数名: AnalyseAttr
	/// 编程  : 王明松 2009-1-24 8:16:55
	/// 返回  : bool 
	/// 参数  : char *buffer
	/// 参数  : int buflen
	/// 参数  : CNode *curnode
	/// 描述  : 分析属性，将属性加入节点
	bool AnalyseAttr(const char *buffer,int buflen,CNode *curnode);

	/// 函数名: addNode
	/// 编程  : 王明松 2009-1-23 15:49:04
	/// 返回  : bool 
	/// 输入输出参数  : CNode *desnode
	/// 输入参数  : CNode *newnode
	/// 输入参数  : bool first 是否加到头  true加到头false加到尾
	/// 描述  : 将新节点加入到目标节点的子节点中
	bool addNode(CNode *desnode,CNode *newnode,bool first=false);

	/// 函数名: AnalyseNodeAttr
	/// 编程  : 王明松 2009-1-23 15:49:08
	/// 返回  : bool 
	/// 输入参数  : char *buffer 名称及属性数据缓冲
	/// 输入参数  : int buflen 名称及属性数据缓冲长度
	/// 输出参数  : CNode *curnode
	/// 描述  : 分析节点名称及属性数据缓冲
	bool AnalyseNodeAttr(const char *buffer,int buflen,CNode *curnode);

	/// 函数名: deleteRemark
	/// 编程  : 王明松 2009-1-23 14:21:09
	/// 返回  : void 
	/// 输入输出参数  : char *buffer
	/// 描述  : 去掉注释
	void deleteRemark(char *buffer);
	CXMLAnalysis();
	virtual ~CXMLAnalysis();
	char m_errMsg[256];

	char m_logFileName[200];///日志文件名
	char m_logFilePath[200];///日志文件名
	int  m_logLevel;///日志级别，大于此级别的日志将不写入日志文件0严重错误1重要错误2一般错误3警告4提示警告5调试信息
};
#undef XML_EXPORT


#endif /* CXMLANALYSIS_H_HEADER_INCLUDED_C1239117 */

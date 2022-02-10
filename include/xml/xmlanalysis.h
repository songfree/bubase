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
�����������XML�ĵ���Ԥ����õ�ʵ�壺
&lt; <  С�ں�
&gt; >  ���ں�
&amp; & ��
&apos; ' ������
&quot; " ˫����
*******************************************************/

 




class CNode;
class CAttribute;

//##ModelId=3EDC620B0251
//##Documentation
//## //����XML���ݱ�������XML���ݱ�
class XML_EXPORT CXMLAnalysis
{
public:
	bool GetNodeName(const char *buffer,int buflen, std::string &nodename);
	bool FindEndNode(int bOffset,int eOffset,std::string nodename,int &vend,int &nextnodebegin);
	void setItem(CNode *pNode, char *value);
	/// ������: *analyseXML
	/// ���  : ������ 2006-1-23 14:21:23
	/// ����  : CNode 
	/// ����  : const char * buffer
	/// ����  : int bufferlen
	/// ����  : CNode * parentNode
	/// ����  : ����xml
	CNode *analyseXML(const char * buffer,int bufferlen,CNode * parentNode);



	/// ������: *analyseXML2
	/// ���  : ������ 2009-1-23 14:34:16
	/// ����  : CNode 
	/// ����  : int beginoffset
	/// ����  : int endoffset
	/// ����  : CNode * parentNode
	/// ����  : ��������beginoffset��endoffset֮���xml������parentNode�ڵ�
	CNode *analyseXML2(int beginoffset,int endoffset,CNode * parentNode);

	CNode *analyseXML2Old(const char * buffer,int bufferlen, CNode * parentNode);
private:
	char *m_xmldatabuf;
public:

	/// ������: AddSlash
	/// ���  : ������ 2009-1-24 8:36:34
	/// ����  : void 
	/// ����  : char *path
	/// ����  : ·������
	void AddSlash(char *path);

	/// ������: makedir
	/// ���  : ������ 2009-1-24 8:33:34
	/// ����  : void 
	/// ����  : char *path
	/// ����  : ����·����û�е�Ŀ¼
	void makedir(char *path);

	/// ������: *Today
	/// ���  : ������ 2009-1-24 8:33:53
	/// ����  : char 
	/// ����  : ȡ��ǰ����YYYYMMDD
	char *Today();

	/// ������: getName
	/// ���  : ������ 2009-1-24 8:34:20
	/// ����  : int 
	/// ����  : char *filename
	/// ����  : ȥ���ļ���ǰ��·��
	int getName(char *filename);

	/// ������: *Get_Date
	/// ���  : ������ 2009-1-24 8:34:35
	/// ����  : char 
	/// ����  : ȡ����ʱ��yyyymmdd hh:mm:ss
	char *Get_Date();

	/// ������: LogMp
	/// ���  : ������ 2009-1-24 8:20:56
	/// ����  : int 
	/// ����  : int level
	/// ����  : const char *filename
	/// ����  : int line
	/// ����  : const char *fmt
	/// ����  : ...
	/// ����  : д��־
	int LogMp(int level, const char *filename, int line,const char *data,int datalen,const char *fmt,...);

	/// ������: AnalyseAttr
	/// ���  : ������ 2009-1-24 8:16:55
	/// ����  : bool 
	/// ����  : char *buffer
	/// ����  : int buflen
	/// ����  : CNode *curnode
	/// ����  : �������ԣ������Լ���ڵ�
	bool AnalyseAttr(const char *buffer,int buflen,CNode *curnode);

	/// ������: addNode
	/// ���  : ������ 2009-1-23 15:49:04
	/// ����  : bool 
	/// �����������  : CNode *desnode
	/// �������  : CNode *newnode
	/// �������  : bool first �Ƿ�ӵ�ͷ  true�ӵ�ͷfalse�ӵ�β
	/// ����  : ���½ڵ���뵽Ŀ��ڵ���ӽڵ���
	bool addNode(CNode *desnode,CNode *newnode,bool first=false);

	/// ������: AnalyseNodeAttr
	/// ���  : ������ 2009-1-23 15:49:08
	/// ����  : bool 
	/// �������  : char *buffer ���Ƽ��������ݻ���
	/// �������  : int buflen ���Ƽ��������ݻ��峤��
	/// �������  : CNode *curnode
	/// ����  : �����ڵ����Ƽ��������ݻ���
	bool AnalyseNodeAttr(const char *buffer,int buflen,CNode *curnode);

	/// ������: deleteRemark
	/// ���  : ������ 2009-1-23 14:21:09
	/// ����  : void 
	/// �����������  : char *buffer
	/// ����  : ȥ��ע��
	void deleteRemark(char *buffer);
	CXMLAnalysis();
	virtual ~CXMLAnalysis();
	char m_errMsg[256];

	char m_logFileName[200];///��־�ļ���
	char m_logFilePath[200];///��־�ļ���
	int  m_logLevel;///��־���𣬴��ڴ˼������־����д����־�ļ�0���ش���1��Ҫ����2һ�����3����4��ʾ����5������Ϣ
};
#undef XML_EXPORT


#endif /* CXMLANALYSIS_H_HEADER_INCLUDED_C1239117 */

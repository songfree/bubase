/// XmlPack.h: interface for the CXmlPack class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLPACK_H__C509FC3C_B8B1_4911_A3F6_96C274BC9748__INCLUDED_)
#define AFX_XMLPACK_H__C509FC3C_B8B1_4911_A3F6_96C274BC9748__INCLUDED_

#include "public.h"
#include "attribute.h"
#include "node.h"
#include "xmlanalysis.h"
#include "xml.h"


/*xml����ʽ
<package>
	<head>
		<��������>302003|</��������>
        <Ӧ������>2|</Ӧ������>
		<DRTPID> 100  </DRTPID>
		<DRTPPORT> 9000  </DRTPPORT>
		<DRTPIP> 172.28.10.90  </DRTPIP>
		<SOURCENO> 0  </SOURCENO>
		<BCCFUNC> 10000  </BCCFUNC>
		<ҵ����ˮ��> 0  </ҵ����ˮ��>
		<ҵ����> 0  </ҵ����>
		<�ܼ�¼��> 1  </�ܼ�¼��>
		<������¼��> 1  </������¼��>
		<������> 0 </������>
		<NEXTFLAG> 0  </NEXTFLAG>
		<FIRSTFLAG> 0  </FIRSTFLAG>
		<BINFLAG>0</BINFLAG> ///���������ݱ�־
		<BINLENGTH>0</BINLENGTH> ///���������ݳ���
	</head>
	<pack>
		<�����ʺ�> 9558801001  </�����ʺ�>
		<�ͻ���> 9000000123  </�ͻ���>
		<��������> 1  </��������>
		<����> 10  </����>
		<�۸�> 140  </�۸�>
		<��Լ����> 99.99��  </��Լ����>
		<��Լ����> Au99.99  </��Լ����>
	</pack>
</package>
*/
#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#ifdef XML_EXPORTS
#define XML_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define XML_EXPORT __declspec(dllimport)
#else
#define XML_EXPORT 
#endif
#endif
#define MAXBUF 8192


class CXmlPackArray;
class XML_EXPORT CXmlPack  
{
public:   ///����bizinfo�ķ���

	/// ������: GetPackMuValueCount
	/// ���  : ������ 2008-7-3 14:33:53
	/// ����  : int <0��ʾû�д˽ڵ� =0��ʾû�����ڵ�
	/// ����  : std::string name
	/// ����  : ��ȡָ��pack�ڵ�Ľڵ���Ϊname�������ڵ���
	int GetPackMuValueCount(std::string name);

	/// ������: GetPackMuValue
	/// ���  : ������ 2008-7-3 14:24:35
	/// ����  : int <0 ʧ�� =0�ɹ�
	/// ����  : std::string name pack����ֶ�����
	/// ����  : int recNo        ��¼�� ��0��ʼ
	/// ����  : std::string &value   ֵ
	/// ����  : ��pack����ֶ�ȡ����������ڵ�ֵ��recNo��ʶΪ�ڼ�����¼
	int GetPackMuValue(std::string name,int recNo,std::string &value);


	/// ������: SetPackMuValue
	/// ���  : ������ 2008-7-3 14:52:50
	/// ����  : CNode * NULLʧ��
	/// �������  : std::string name
	/// �������  : std::string value
	/// �������  : bool istrans=true   ��Ϊfalse������ת���ת�����������ת���ת��
	/// ����  : ���ö��¼�ڵ�  pack->name->record���
	CNode *SetPackMuValue(std::string name,std::string value,bool istrans=true);


public: ///xmlpack����

	/// ������: getXmlDataLen
	/// ���  : ������ 2008-7-3 14:21:26
	/// ����  : int ���ݳ���
	/// ����  : ��ȡxml�����ݳ���
	int getXmlDataLen();

	/// ������: tobuffer
	/// ���  : ������ 2008-6-19 19:00:34
	/// ����  : bool 
	/// ����  : char *buffer
	/// ����  : int &bufsize ���ݻ��峤�ȣ������Ȳ�������false,�ɹ�Ϊ���ݳ���
	/// ����  : xml����������ݵ����ݻ���
	bool tobuffer(char *buffer,int &bufsize);

	/// ������: XmlCopy
	/// ���  : ������ 2008-5-23 14:45:01
	/// ����  : bool 
	/// ����  : CXmlPack *xmlpack
	/// ����  : xmlpack���и��ƣ���Դxmlpack��ȫһ��,xmlpack����
	bool XmlCopy(CXmlPack *xmlpack);


	/// ������: XmlPointCopy
	/// ���  : ������ 2009-1-20 9:34:08
	/// ����  : bool 
	/// ����  : CXmlPack *xmlpack
	/// ����  : ��Ŀ��xmlpack��ָ�븴�Ƶ���xml�У�xmlpack��Ϊ��
	bool XmlPointCopy(CXmlPack *xmlpack);


	/// ������: readXmlPack
	/// ���  : ������ 2006-7-3 14:53:49
	/// ����  : bool 
	/// ����  : char *xmlBuf
	/// ����  : ��xmlBuf����xmlpack�����ݰ�
	bool readXmlPack(char *xmlBuf);



	/// ������: getNextFlag
	/// ���  : ������ 2007-4-30 10:22:08
	/// ����  : bool 
	/// ����  : ��ȡ��������־
	bool getNextFlag();

	/// ������: setNextFlag
	/// ���  : ������ 2007-4-30 10:21:56
	/// ����  : bool 
	/// ����  : bool isnext=false
	/// ����  : �����Ƿ��к�����
	bool setNextFlag(bool isnext=false);

	/// ������: getFirstFlag
	/// ���  : ������ 2007-4-30 10:21:27
	/// ����  : bool 
	/// ����  : ���õ�һ���������־
	bool getFirstFlag();

	/// ������: setFirstFlag
	/// ���  : ������ 2007-4-30 10:21:01
	/// ����  : bool 
	/// ����  : bool isfirst=true
	/// ����  : �����Ƿ��һ�������
	bool setFirstFlag(bool isfirst=true);

	/// ������: getTotalRec
	/// ���  : ������ 2007-4-30 10:20:50
	/// ����  : bool 
	/// ����  : int &rec
	/// ����  : ��ȡ�ܹ���¼��
	bool getTotalRec(int &rec);

	/// ������: setTotalRec
	/// ���  : ������ 2007-4-30 10:20:34
	/// ����  : bool 
	/// ����  : int rec
	/// ����  : �����ܹ���¼��
	bool setTotalRec(int rec);

	/// ������: getRecCount
	/// ���  : ������ 2007-4-30 10:20:19
	/// ����  : bool 
	/// ����  : int &rec
	/// ����  : ��ȡ������¼��
	bool getRecCount(int &rec);

	/// ������: setRecCount
	/// ���  : ������ 2007-4-30 10:19:54
	/// ����  : bool 
	/// ����  : int rec
	/// ����  : ���ñ�����¼��
	bool setRecCount(int rec);

	/// ������: getRetMsg
	/// ���  : ������ 2007-4-30 10:19:44
	/// ����  : bool 
	/// ����  : char *msg
	/// ����  : ��ȡ������Ϣ
	bool getRetMsg(char *msg);

	/// ������: getRetCode
	/// ���  : ������ 2007-4-30 10:19:34
	/// ����  : bool 
	/// ����  : int &retCode
	/// ����  : ��ȡ������
	bool getRetCode(int &retCode);

	/// ������: getRetCode
	/// ���  : ������ 2007-4-30 10:19:34
	/// ����  : bool 
	/// ����  : char *retCode
	/// ����  : ��ȡ������
	bool getRetCode(char *retCode);

	/// ������: setReturn
	/// ���  : ������ 2007-4-30 9:57:57
	/// ����  : bool 
	/// ����  : int retCode
	/// ����  : const char *msg
	/// ����  : ���÷����뷵����Ϣ
	bool setReturn(int retCode,const char *msg);

	/// ������: setReturn
	/// ���  : ������ 2007-4-30 9:57:57
	/// ����  : bool 
	/// ����  : const char * retCode
	/// ����  : const char *msg
	/// ����  : ���÷����뷵����Ϣ
	bool setReturn(const char *retCode,const char *msg);

	/// ������: setRetMsg
	/// ���  : ������ 2007-4-30 9:57:45
	/// ����  : bool 
	/// ����  : const char *msg
	/// ����  : ���÷�����Ϣ
	bool setRetMsg(const char *msg);

	/// ������: setRetCode
	/// ���  : ������ 2007-4-30 9:57:34
	/// ����  : bool 
	/// ����  : int retCode
	/// ����  : ���÷�����
	bool setRetCode(int retCode);
	
	/// ������: setRetCode
	/// ���  : ������ 2007-4-30 9:57:34
	/// ����  : bool 
	/// ����  : const char * retCode
	/// ����  : ���÷�����
	bool setRetCode(const char *retCode);

	/// ������: getBinData
	/// ���  : ������ 2006-6-9 12:38:40
	/// ����  : bool 
	/// ����  : char *data
	/// ����  : int &datalen
	/// ����  : ȡ����������
	bool getBinData(char *data,int &datalen);

	/// ������: setBinData
	/// ���  : ������ 2006-6-9 12:25:08
	/// ����  : bool 
	/// ����  : char *data
	/// ����  : int datalen
	/// ����  : ���ö���������
	bool setBinData(char *data,int datalen);

	/// ������: setBin
	/// ���  : ������ 2006-7-3 15:00:48
	/// ����  : bool 
	/// ����  : bool binflag
	/// ����  : �����Ƿ�����Ƹ�ʽ true�Ƕ�����
	bool setBin(bool binflag);

	/// ������: isBin
	/// ���  : ������ 2006-7-3 15:00:35
	/// ����  : bool 
	/// ����  : �Ƿ��Ƕ����Ƹ�ʽ
	bool isBin();
	/// ������: modiPack
	/// ���  : ������ 2006-6-8 17:24:09
	/// ����  : bool 
	/// ����  : CXmlPack *xmlpack  ����
	/// ����  : bool isUpdate  �Ƿ�updateԭpack��ͬ�ֶ�
	/// ����  : �ò���xmlpack���pack�ֶθ��±��� isUpdate�Ƿ�updateԭpack��ͬ�ֶ�
	bool modiPack(CXmlPack *xmlpack,bool isUpdate);

	/// ������: xmlCopyNv
	/// ���  : ������ 2006-6-6 13:42:47
	/// ����  : bool 
	/// ����  : CXmlPack *xmlpack ����
	/// ����  : copy����xmlpack���head��pack�ڵ��name��value������
	bool xmlCopyNV(CXmlPack *xmlpack);
	

	/// ������: deletePack
	/// ���  : ������ 2006-7-3 15:01:49
	/// ����  : int 
	/// ����  : ɾ��pack�ڵ�����ӽڵ㣬�����pack�������
	int deletePack();

	/// ������: gotoPack
	/// ���  : ������ 2006-7-3 15:01:27
	/// ����  : int 
	/// ����  : ��ǰ�ڵ㵽pack�ڵ�
	int gotoPack();

	/// ������: gotoHead
	/// ���  : ������ 2006-7-3 15:01:12
	/// ����  : int 
	/// ����  : ��ǰ�ڵ㵽head�ڵ�
	int gotoHead();
	
	/// ������: newPackage
	/// ���  : ������ 2006-7-3 15:02:25
	/// ����  : virtual int 
	/// ����  : ����һ��ȫ�µ�xml��
	virtual int newPackage();


	/// ������: trim
	/// ���  : ������ 2006-7-3 15:02:41
	/// ����  : int 
	/// ����  : std::string &s
	/// ����  : ȥ���ո�
	int trim(std::string &s);

	/// ������: *rtrim
	/// ���  : ������ 2008-7-3 15:02:54
	/// ����  : char *
	/// ����  : char *str
	/// ����  : ȥ���ҿո�
	char *rtrim(char *str);

	/// ������: *ltrim
	/// ���  : ������ 2008-7-3 15:03:07
	/// ����  : char 
	/// ����  : char *str
	/// ����  : ȥ����ո�
	char *ltrim(char *str);

	/// ������: trim
	/// ���  : ������ 2008-7-3 15:03:17
	/// ����  : int 
	/// ����  : char *s
	/// ����  : ȥ�����пո�
	int trim(char *s);

	/// ������: getPackValueChar
	/// ���  : ������ 2006-7-3 15:03:34
	/// ����  : int <0�޴˽ڵ� =0�ɹ�
	/// ����  : const char *name
	/// ����  : char *value
	/// ����  : ȡpack���name�ڵ��ֵ,ȥ���ո�,��ǰ�ڵ����˽ڵ�
	int getPackValueChar(const char *name, char *value);
	///ȡheadֵ,ȥ���ո�

	/// ������: getHeadValueChar
	/// ���  : ������ 2006-7-3 15:04:06
	/// ����  : int <0�޴˽ڵ� =0�ɹ�
	/// ����  : const char *name
	/// ����  : char *value
	/// ����  : ȡheadֵ,ȥ���ո�,��ǰ�ڵ����˽ڵ�
	int getHeadValueChar(const char *name, char *value);

	/// ������: getPackValue
	/// ���  : ������ 2006-7-3 15:04:58
	/// ����  : int <0�޴˽ڵ� =0�ɹ�
	/// ����  : const std::string name
	/// ����  : std::string &value
	/// ����  : bool istrim=true
	/// ����  : ȡpack���name�ڵ��ֵ,ȥ���ո�,��ǰ�ڵ����˽ڵ�
	int getPackValue(const std::string name,std::string &value,bool istrim=true);
	int getPackValue(const std::string name,double &value);
	int getPackValue(const std::string name,int &value);
	int getPackValueI(const std::string name);
	double getPackValueD(const std::string name);

	
	std::string getPackValueS(const std::string name,bool istrim=false);


	/// ������: getHeadValue
	/// ���  : ������ 2006-7-3 15:05:47
	/// ����  : int 
	/// ����  : const std::string name
	/// ����  : double &value
	/// ����  : ȡhead���name�ڵ��ֵ,ȥ���ո�,��ǰ�ڵ����˽ڵ�
	int getHeadValue(const std::string name,double &value);
	int getHeadValue(const std::string name,int &value);
	int getHeadValue(const std::string name,std::string &value,bool istrim=true);
	int getHeadValueI(const std::string name);
	double getHeadValueD(const std::string name);
	
	std::string getHeadValueS(const std::string name,bool istrim=false);

	/// ������: getPackValue
	/// ���  : ������ 2006-6-27 9:37:12
	/// ����  : char * 
	/// ����  : const string name
	/// ����  : ��ȡchar *ֵ��ע�ⲻ����sprintf()��ʹ�ö���˷����� ��ǰ�ڵ����˽ڵ�
	char * getPackValue(const std::string name);


	/// ������: *getHeadValue
	/// ���  : ������ 2006-7-3 15:06:35
	/// ����  : char 
	/// ����  : const std::string name
	/// ����  :  ��ȡchar *ֵ��ע�ⲻ����sprintf()��ʹ�ö���˷����� ��ǰ�ڵ����˽ڵ�
	char *getHeadValue(const std::string name);




public: ///���pack1 pack2�ķ���

	/// ������: newPackFromMuPack
	/// ���  : ������ 2007-4-30 11:27:02
	/// ����  : bool 
	/// ����  : CXmlPackArray muxmlpack
	/// ����  : �Ӷ���������ɶ��¼
	bool newPackFromMuPack(CXmlPackArray muxmlpack);
	

	/// ������: newPackFromMuPackPoint
	/// ���  : ������ 2009-1-20 16:09:29
	/// ����  : bool 
	/// ����  : CXmlPackArray muxmlpack
	/// ����  : �Ӷ���������ɶ��¼,��muxmlpack���ָ�븴�Ƶ�������muxmlpackԭָ���ÿ�
	bool newPackFromMuPackPoint(CXmlPackArray muxmlpack);
	/// ������: addMuPack
	/// ���  : ������ 2007-4-30 11:23:21
	/// ����  : bool 
	/// ����  : CXmlPackArray muxmlpack
	/// ����  : ���Ӷ������ļ�¼������  pack pack1 pack2��
	bool addMuPack(CXmlPackArray muxmlpack);


	/// ������: addMuPackPoint
	/// ���  : ������ 2009-1-20 16:09:58
	/// ����  : bool 
	/// ����  : CXmlPackArray muxmlpack
	/// ����  : ���Ӷ������ļ�¼������(ָ�븴�Ʒ�ʽmuxmlpackԭָ���ÿ�) pack pack1 pack2��
	bool addMuPackPoint(CXmlPackArray muxmlpack);
	
	/// ������: getMuPack
	/// ���  : ������ 2007-4-30 11:19:21
	/// ����  : bool 
	/// ����  : CXmlPackArray *muxmlpack ���
	/// ����  : ȡ���еļ�¼��muxmlapck���� pack pack1 pack2��
	bool getMuPack(CXmlPackArray *muxmlpack);


	/// ������: getMuPackPoint
	/// ���  : ������ 2009-1-20 16:03:32
	/// ����  : bool 
	/// ����  : CXmlPackArray *muxmlpack
	/// ����  : �������Ķ�packȡ��muxmlpack�У�ָ��ķ�ʽ��ԭָ�������ɾ��
	bool getMuPackPoint(CXmlPackArray *muxmlpack);
	/// ������: getPack
	/// ���  : ������ 2007-4-30 10:32:10
	/// ����  : bool 
	/// ����  : int recNo  ��¼id  ��1��ʼ
	/// ����  : CXmlPack *xmlpack  ���
	/// ����  : �ӱ�����ȡ�����¼��xmlpack�У�xmlpack����newһ��������Դ��ȡhead������
	bool getPack(int recNo,CXmlPack *xmlpack);


	/// ������: getPackPoint
	/// ���  : ������ 2009-1-20 15:56:23
	/// ����  : bool 
	/// ����  : int recNo
	/// ����  : CXmlPack *xmlpack
	/// ����  : �ӱ�����ȡ�����¼��xmlpack��(ָ�뷽ʽ��������¼ɾ��)��xmlpackҪ����xmlpack�ĸ�ʽ
	bool getPackPoint(int recNo,CXmlPack *xmlpack);
	
	/// ������: addPack
	/// ���  : ������ 2007-4-30 10:29:35
	/// ����  : int  0�ɹ�
	/// ����  : CXmlPack xmlpack
	/// ����  : ��xmlpack��pack������Ϊ���¼���ӵ������� pack%d
	int addPack(CXmlPack *xmlpack);


	/// ������: addPackPoint
	/// ���  : ������ 2009-1-20 10:19:54
	/// ����  : int 0�ɹ�
	/// ����  : CXmlPack *xmlpack
	/// ����  : ��xmlpack��packָ����Ϊ���¼���ӵ������� pack%d��ԭxmlpack��pack��������
	int addPackPoint(CXmlPack *xmlpack);

	/// ������: addPack
	/// ���  : ������ 2007-4-30 10:24:58
	/// ����  : int  ���ر�����¼��
	/// ����  : string &packname ���صļ�¼�ڵ���
	/// ����  : ����һ���յ�pack��¼,pack%d������¼��
	int addPack(std::string &packname);

	///���ö��¼�������� 

	/// ������: setPackNameValueI
	/// ���  : ������ 2009-2-4 12:36:21
	/// ����  : int 
	/// �������  : int packno
	/// �������  : std::string name
	/// �������  : int value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// ����  : ���ö�pack���ֶ�����
	int setPackNameValueI(int packno, std::string name, int value,bool isupdate=true);

	/// ������: setPackNameValueI
	/// ���  : ������ 2009-2-4 12:36:25
	/// ����  : int 
	/// �������  : int packno
	/// �������  : std::string name
	/// �������  : long value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// ����  : ���ö�pack���ֶ�����
	int setPackNameValueI(int packno, std::string name, long value,bool isupdate=true);

	/// ������: setPackNameValueI
	/// ���  : ������ 2009-2-4 12:36:28
	/// ����  : int 
	/// �������  : int packno
	/// �������  : std::string name
	/// �������  : double value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// ����  : ���ö�pack���ֶ�����
	int setPackNameValueI(int packno, std::string name, double value,bool isupdate=true);
	

	/// ������: setPackNameValueI
	/// ���  : ������ 2009-2-4 12:36:33
	/// ����  : int 
	/// �������  : int packno
	/// �������  : std::string name
	/// �������  : std::string value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// �������  : bool istrans=true   ��Ϊfalse������ת���ת�����������ת���ת��
	/// ����  : ���ö�pack���ֶ�����
	int setPackNameValueI(int packno, std::string name, std::string value,bool isupdate=true,bool istrans=true);
	/// ������: setPackNameValue
	/// ���  : ������ 2006-7-3 15:07:46
	/// ����  : int 
	/// �������  : const std::string item
	/// �������  : std::string name
	/// �������  : std::string value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// �������  : bool istrans=true   ��Ϊfalse������ת���ת�����������ת���ת��
	/// ����  : ���ö�pack���ֶ�����
	int setPackNameValue(const std::string item, std::string name, std::string value,bool isupdate=true,bool istrans=true);

public: ///����xmlpack����

	/// ������: SetPackNode
	/// ���  : ������ 2009-1-24 10:57:31
	/// ����  : bool 
	/// ����  : CNode *pnode  ������m_packnodeҲΪ��
	/// ����  : ��pnode��Ϊpack�ڵ��pack���ٽ��ڵ㣬�ڵ�����Ϊpnode.m_name ����ڵ���Ϊpack��m_packnodeΪ�˽ڵ�
	bool SetPackNode(CNode *pnode);

	/// ������: XmlCopyPack
	/// ���  : ������ 2009-1-16 10:11:56
	/// ����  : bool 
	/// ����  : CXmlPack *xmlpack
	/// ����  : ����xmlpack��pack�ڵ㵽������pack�ڵ�,��pack�ڵ�ɾ�����ٸ���
	bool XmlCopyPack(CXmlPack *xmlpack);

	/// ������: XmlCopyHead
	/// ���  : ������ 2009-1-16 10:11:52
	/// ����  : bool 
	/// ����  : CXmlPack *xmlpack
	/// ����  : ����xmlpack��head�ڵ㵽������head�ڵ�,��head�ڵ�ɾ�����ٸ���
	bool XmlCopyHead(CXmlPack *xmlpack);

	
	/// ������: setPackNameValue
	/// ���  : ������ 2008-7-3 15:08:55
	/// ����  : int =0�ɹ�
	/// �������  : std::string name
	/// �������  : int value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// ����  : ���ð������� ��ǰ�ڵ����˽ڵ�
	int setPackNameValue(std::string name, int value,bool isupdate=true);

	/// ������: setPackNameValue
	/// ���  : ������ 2008-7-3 15:08:55
	/// ����  : int =0�ɹ�
	/// �������  : std::string name
	/// �������  : long value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// ����  : ���ð������� ��ǰ�ڵ����˽ڵ�
	int setPackNameValue(std::string name, long value,bool isupdate=true);

	/// ������: setPackNameValue
	/// ���  : ������ 2008-7-3 15:08:55
	/// ����  : int =0�ɹ�
	/// �������  : std::string name
	/// �������  : double value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// ����  : ���ð������� ��ǰ�ڵ����˽ڵ�
	int setPackNameValue(std::string name, double value,bool isupdate=true);
	int setPackNameValue(std::string name, double value,int dec,bool isupdate=true);

	/// ������: setPackNameValue
	/// ���  : ������ 2008-7-3 15:08:55
	/// ����  : int =0�ɹ�
	/// �������  : std::string name
	/// �������  : string value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// �������  : bool istrans=true   ��Ϊfalse������ת���ת�����������ת���ת��
	/// ����  : ���ð������� ��ǰ�ڵ����˽ڵ�
	int setPackNameValue(std::string name ,std::string value,bool isupdate=true,bool istrans=true);


	/// ������: setHeadNameValue
	/// ���  : ������ 2006-7-3 15:09:09
	/// ����  : int =0�ɹ�
	/// �������  : std::string name
	/// �������  : long value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// ����  : ���ð�ͷ���� ��ǰ�ڵ����˽ڵ�
	int setHeadNameValue(std::string name, long value,bool isupdate=true);

	/// ������: setHeadNameValue
	/// ���  : ������ 2006-7-3 15:09:09
	/// ����  : int =0�ɹ�
	/// �������  : std::string name
	/// �������  : double value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// ����  : ���ð�ͷ���� ��ǰ�ڵ����˽ڵ�
	int setHeadNameValue(std::string name, double value,bool isupdate=true);
	int setHeadNameValue(std::string name, double value,int dec,bool isupdate=true);
	/// ������: setHeadNameValue
	/// ���  : ������ 2006-7-3 15:09:09
	/// ����  : int =0�ɹ�
	/// �������  : std::string name
	/// �������  : int value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// ����  : ���ð�ͷ���� ��ǰ�ڵ����˽ڵ�
	int setHeadNameValue(std::string name, int value,bool isupdate=true);

	/// ������: setHeadNameValue
	/// ���  : ������ 2006-7-3 15:09:09
	/// ����  : int =0�ɹ�
	/// �������  : std::string name
	/// �������  : string value
	/// �������  : bool isupdate=true  ��Ϊfalseֱ�����ӽڵ㣬������в��Ҹ��»�����
	/// �������  : bool istrans=true   ��Ϊfalse������ת���ת�����������ת���ת��
	/// ����  : ���ð�ͷ���� ��ǰ�ڵ����˽ڵ�
	int setHeadNameValue(std::string name,std::string value,bool isupdate=true,bool istrans=true);


	/// ������: readPack
	/// ���  : ������ 2006-7-3 15:09:27
	/// ����  : bool 
	/// ����  : ��xml�����Ƿ���϶����xml����ʽ
	bool readPack();

	/// ������: toXmlBuf
	/// ���  : ������ 2008-6-19 19:02:57
	/// ����  : bool 
	/// ����  : char *xmlBuf
	/// ����  : ��xml��������buffer,�������ȼ�飬Ҫ������Ļ��幻��,����ʹ��toString
	bool toXmlBuf(char *xmlBuf);


	/// ������: toBuf
	/// ���  : ������ 2008-6-19 19:02:16
	/// ����  : bool 
	/// ����  : char *Buf
	/// ����  : int &buflen
	/// ����  : ��xml���������������buffer,�������ȼ�飬Ҫ������Ļ��幻��,����ʹ��tobuffer
	bool toBuf(char *Buf,int &buflen);

	///

	/// ������: toString
	/// ���  : ������ 2008-7-3 15:09:50
	/// ����  : bool 
	/// ����  : std::string &buffer
	/// ����  : ��xml��ʽ����string����buffer
	bool toString(std::string &buffer);


	/// ������: fromBuf
	/// ���  : ������ 2008-7-3 15:10:08
	/// ����  : bool 
	/// ����  : char *xmlBuf
	/// ����  : ��buffer����xml����
	bool fromBuf(char *xmlBuf);

	/// ������: toFile
	/// ���  : ������ 2008-7-3 15:10:17
	/// ����  : int 
	/// ����  : char *filename
	/// ����  : ��xml����д���ļ�
	int toFile(char *filename);

	/// ������: fromFile
	/// ���  : ������ 2008-7-3 15:10:26
	/// ����  : int 
	/// ����  : char *filename
	/// ����  : ���ļ�����xml����
	int fromFile(char *filename);

	/// ������: *getMsg
	/// ���  : ������ 2008-7-3 15:10:35
	/// ����  : char 
	/// ����  : ��ȡ������Ϣ
	char *getMsg();



	/// ������: toBufOld
	/// ���  : ������ 2008-11-25 16:21:02
	/// ����  : bool 
	/// ����  : char *Buf
	/// ����  : int &buflen
	/// ����  : ��</>����ʽ��xml
	bool toBufOld(char *Buf,int &buflen);

	CXmlPack();
	///��buffer����xml��
	CXmlPack(char *xmlBuf);
	virtual ~CXmlPack();
	CXML m_xml;        /*xml����������*/
	CNode *m_rootnode;/*xml���ڵ�*/
	char m_errMsg[1025];
	
	
protected:
	CNode *m_headnode;/*xml��ͷ�ڵ�*/
	CNode *m_packnode;/*xml����ڵ�*/
	int m_rowbuflen;///��Ŷ��������ݵĻ��峤��
	char *m_rowbuf; ///��Ŷ��������ݵĻ���

	CNode *m_pRecNode;//������¼���ڵ�
};
#undef XML_EXPORT
#endif /// !defined(AFX_XMLPACK_H__C509FC3C_B8B1_4911_A3F6_96C274BC9748__INCLUDED_)

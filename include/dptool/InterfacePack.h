/// InterfacePack.h: interface for the CInterfacePack class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERFACEPACK_H__ACF796B0_9EC2_47FD_B214_7438ADC630FA__INCLUDED_)
#define AFX_INTERFACEPACK_H__ACF796B0_9EC2_47FD_B214_7438ADC630FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#include "public.h"
#include "attribute.h"
#include "node.h"
#include "xmlanalysis.h"
#include "xml.h"
//#include "pubstrtime.h"
#include "BF_Tools.h"
#include "BF_SimpleLog.h"
#include "BF_Slist.h"



#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_oldxmld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib bf_oldxmld.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_oldxml.lib") 
#pragma message("Automatically linking with  bf_kernel.lib bf_oldxml.lib")
#endif
#endif

/*�ӿڰ���ʽ
<?xml version="1.0" encoding="GB2312"?>
<!-- edited with XML Spy v4.2 U (http:///www.xmlspy.com) by lock (AIsys) -->
<interface>
	<dict>
		<������֤��־>0��1���鲻����1��</������֤��־>
		<֤������>0:���֤1:����2:����֤3:ʿ��֤4:����֤6:���ڲ�7:����8:��9:����֤</֤������>
		<�ɹ���־>0�ɹ�1ʧ��9δ֪</�ɹ���־>
		<����>000�����001�۱�002��Ԫ003��Ԫ004ŷԪ</����>
		<�˻�����>0:��˽�˺�1:��˽��2:�Թ��˺�3:�Թ���</�˻�����>
	</dict>
	<conv>
		<separator>�ָ���</separator>
		<xpack>xpack��</xpack>
		<xml>xml��</xml>
		<standard>
			<no1 name="������" type="char" length="16" offset="100" left="yes" trans="1" var="~1" update="yes"/>
			<no2 name="������" type="integer" length="10" offset="10" left="no" trans="0" var="@11" update="no"/>
			<no3 name="������" type="double" length="2" left="no"/>
		</standard>
	</conv>
	<type>
		<char/>
		<integer/>
		<double/>
	</type>
	<trans>
	    <��������>0</��������>
		<Ҫ������>1</Ҫ������>
	</trans>
	<var>
	    <����ĳ���>~</����ĳ���>
		<����ı����򷽷�>@</����ı����򷽷�>
	</var>
	<update>
	    <��������xml����>yes</��������xml����>
	    <����������xml����>no</����������xml����>
	</update>
	<������ conv="standard" separator="|" offset="20">10</������>
	<���뷽��>
	    <transBUYFLAG conv="0">
	        <b>0</b>
	        <s>1</s>
	        <default>0</default>
	    </transBUYFLAG>
	    <transBUYFLAGNAME conv="0">
	        <b>����</b>
	        <s>����</s>
	        <default>����</default>
	    </transBUYFLAGNAME>
	    <transEOFLAG conv="0">
	        <0>0</0>
	        <1>0</1>
	        <default>0</default>
	    </transEOFLAG>
	    <transEOFLAGNAME conv="0">
	        <0>����</0>
	        <1>ƽ��</1>
	        <default>��</default>
	    </transEOFLAGNAME >
		<transDONEQTY conv="1">
			<v1>lvol0</v1>
			<v2>-</v2>
			<v3>lvol2</v3>
		</transDONEQTY>
	<���뷽��/>
	<������>5</������>
	<��ͷ>
		<��ͷ0 name="���������÷���">
		    <���ı�ʶ type="char" offset="0" length="5" left="no" trans="0" var="" trim="1"></���ı�ʶ>
		    <������ type="char"   offset="5" length="6" left="yes" trans="1" var="@transtxcode"></������>
		    <���� type="char" offset="11" length="8" left="no" trans="0" var=""></����>
		    <ʱ�� type="char" offset="19" length="8" left="no" trans="0" var=""></ʱ��>
		    <���е��� type="char" offset="27" length="8" left="no" trans="0" var=""></���е���>
		    <�������� type="char" offset="35" length="8" left="no" trans="0" var=""></��������>
		    <���й�Ա type="char" offset="43" length="8" left="no" trans="0" var=""></���й�Ա>
		    <������� type="char" offset="51" length="8" left="no" trans="0" var=""></�������>
		    <�ն˺� type="char" offset="59" length="6" left="no" trans="0" var=""></�ն˺�>
		    <������Դ type="char" offset="65" length="8" left="no" trans="0" var=""></������Դ>
		    <�ն����� type="char" offset="73" length="8" left="no" trans="0" var=""></�ն�����>
		    <������־ type="char" offset="81" length="8" left="no" trans="0" var=""></������־>
		    <ԭ����ʱ�� type="char" offset="89" length="8" left="no" trans="0" var=""></ԭ����ʱ��>
		</��ͷ0>
		    
		<��ͷ1 name="�ָ������÷���">
			<���ı�ʶ type="char" offset="0" length="5" left="no" trans="0" var="" ></���ı�ʶ>
			<������ type="char"   offset="1" length="6" left="yes" trans="1" var="@transtxcode"></������>
			<���� type="char" offset="2" length="8" left="no" trans="0" var=""></����>
			<ʱ�� type="char" offset="3" length="8" left="no" trans="0" var=""></ʱ��>
			<���е��� type="char" offset="4" length="8" left="no" trans="0" var=""></���е���>
			<�������� type="char" offset="5" length="8" left="no" trans="0" var=""></��������>
			<���й�Ա type="char" offset="6" length="8" left="no" trans="0" var=""></���й�Ա>
			<������� type="char" offset="7" length="8" left="no" trans="0" var=""></�������>
			<�ն˺� type="char" offset="8" length="8" left="no" trans="0" var=""></�ն˺�>
			<������Դ type="char" offset="9" length="8" left="no" trans="0" var=""></������Դ>
			<�ն����� type="char" offset="10" length="8" left="no" trans="0" var=""></�ն�����>
			<������־ type="char" offset="11" length="8" left="no" trans="0" var=""></������־>
			<ԭ����ʱ�� type="char" offset="12" length="8" left="no" trans="0" var=""></ԭ����ʱ��>
		</��ͷ1>
	</��ͷ>
	<�������󷵻�>
		<������Ϣ type="char" trans="0">vsmess</������Ϣ>
	<�������󷵻�/>
	<func_list>
		<f8612 name="�ʽ𶳽�ⶳ" conv="separator" separator="|" author="songfree" oldfunc="">
			<���� ret="1">
			    <include>��ͷ1</include>
				<�����˺� type="char" offset="13" length="23" left="yes" trans="0" var="" trim="1"/>
				<�������־ type="char" offset="14" length="1" left="yes" trans="0" var=""/>
				<ת�˽�� type="double" offset="15" length="16" dec="2" left="no" trans="0" var=""/>
			</����>
			<������0 code="0" conv="separator" separator="|" file="1">
			    <include>��ͷ1</include>
			</������0>
		</f8612>
		<f8613 name="��֤�����" conv="standard"  author="songfree" oldfunc="">
			<���� ret="1">
				<include>��ͷ0</include>
				<�����˺� type="char" offset="97" length="23" left="yes" trans="0" var="" trim="1"/>
				<�������־ type="char" offset="120" length="1" left="yes" trans="0" var=""/>
				<ת�˽�� type="double" offset="121" length="16" dec="2" left="no" trans="0" var=""/>
			</����>
			<������0 code="0" conv="separator" separator="|" file="1">
				<include>��ͷ1</include>
			</������0>
		</f8613>
		<f690613 name="���ն���-����" conv="xpack" author="songfree" oldfunc="330312">
			<���� ret="1">
				<���� type="char" trans="0" >sdate0</����>
				<Ӧ�ô��� type="char" trans="0" >vsmess</Ӧ�ô���>
				<����Ա type="char" trans="0">sorder0</����Ա>
			</����>
			<������0 code="0" conv="xpack" >
				<������ˮ�� type="char" trans="0">sserial0</������ˮ��>
				<�����˺� type="char" trans="0">sbank_acc</�����˺�>
				<Ӧ�ÿͻ��� type="char" trans="0">sholder_ac_no2</Ӧ�ÿͻ���>
				<Ӧ�ô��� type="char" trans="0">vsmess</Ӧ�ô���>
				<���� type="char" trans="0">sdate0</����>
				<ʱ�� type="char" trans="0">stime0</ʱ��>
				<ת�˽�� type="double" trans="0">damt0</ת�˽��>
				<���� type="char" trans="0">sbranch_code2</����>
				<�����״̬ type="char" trans="0">sstatus0</�����״̬>
				<���ع�̨��־ type="char" trans="0">smain_flag</���ع�̨��־>
				<�ʽ�������� type="char" trans="0">smain_flag2</�ʽ��������>
				<Ӧ����ˮ�� type="char" trans="0">sserial4</Ӧ����ˮ��>
				<���� type="char" trans="0">sstatus1</����>
				<������Ϣ type="char" trans="0">vsmess</������Ϣ>
			</������0>
		</f690613>
	</func_list>
</interface>
*/

typedef struct _func_attr
{
	int  funcNo;///���ܺ�
	int  conv_type;///���� 0 xml 1xpack 2 �ָ��� 3����
	char separator[3];///�ָ���
}S_FUNC_ATTR;///����ת������

typedef struct _funcfieldinfo
{
	std::string name;///����
	int fieldtype;///���� 0-char 1-integer 2-double
	int fieldlength;///����
	int fieldoffset;///���ֶ�����bufλ��(����Ϊλ�ơ��ָ�Ϊ���)
	bool isTrans;///�Ƿ���
	bool isVar;///�Ƿ����
	bool isTrim;///�Ƿ�ȥ���ո�
	std::string transValue;///�����ֶζ���
	bool isupdate;///�Ƿ����xml�����ֶ�
	int left;///���Ҷ��� 0-��� 1�Ҷ�
	int  conv_type;///���� 0 xml 1xpack 2 �ָ��� 3����
	int  dec;///С��λ��
	char separator[3];///�ָ���
	std::string value;///ֵ
	std::string include;///�����İ�ͷ
}S_FUNC_FIELD_INFO; ///�ֶ�����

typedef struct 
{
	int retNum;///���ذ���Ŀ
	std::vector<S_FUNC_FIELD_INFO> funcReq;///������ֶ��б�
}S_FUNC_REQ_FIELD_LIST; ///���������

typedef struct 
{
	int retCode;///������
	int  conv_type;///���ذ����� 0 xml 1xpack 2 �ָ��� 3����
	char separator[3];///���ذ��ָ���
	int  fileFlag;///���ذ��Ƿ�д���ļ�
	char include[100];///Ӧ���Ƿ񵥶����ɰ�ͷ��������
	std::vector<S_FUNC_FIELD_INFO> funcRes;///���ذ��ֶ��б�
}S_FUNC_RES_FIELD_LIST; ///Ӧ�������

typedef struct 
{
	S_FUNC_ATTR funcAttr;
	S_FUNC_REQ_FIELD_LIST funcReq;///���������
	std::vector<S_FUNC_RES_FIELD_LIST> funcRes;///���ض�����
}S_FUNC_CONF_INFO;  ///���ܽ���������Ϣ

typedef struct 
{
	std::string headName;///��ͷ����
	int    headLength;///��ͷ����
	std::vector<S_FUNC_FIELD_INFO> headinfo;///��ͷ��Ϣ
}S_PACKHEAD_INFO; ///include��ͷ��Ϣ

typedef struct 
{
	std::string transName;///��������
	std::string conv;///�����־0���ַ���1����
	std::vector<S_FUNC_FIELD_INFO> transinfo;///������Ϣ
}S_TRANS_INFO; ///������ṹ��Ϣ


#ifdef DPTOOL_EXPORTS
#define DPTOOL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DPTOOL_EXPORT __declspec(dllimport)
#else
#define DPTOOL_EXPORT 
#endif
#endif
class DPTOOL_EXPORT CInterfacePack  
{
public:
	CInterfacePack();
	virtual ~CInterfacePack();
	std::vector <S_TRANS_INFO> m_trans_info;///���еķ�����Ϣ
	std::vector <S_PACKHEAD_INFO> m_include_head;///���е�includeͷ
	std::vector <S_FUNC_CONF_INFO> m_func_conf_info;///���й���������Ϣ
	S_FUNC_FIELD_INFO m_txcode_info;///ȡ������ڵ���Ϣ
	
	CXML m_xml;        /*xml����������*/
	char m_errMsg[1025];
	CBF_SimpleLog m_publog;
protected:
	/// ������: readPack
	/// ���  : ������ 2006-6-8 13:00:12
	/// ����  : bool 
	/// ����  : ��xml�ж�ȡ�ӿ�������Ϣ
	bool readPack();
	/// ������: getField
	/// ���  : ������ 2006-6-8 12:55:56
	/// ����  : int 
	/// ����  : CNode *fieldnode �ڵ�
	/// ����  : string &name     �ڵ�����
	/// ����  : int &type        �ڵ�����
	/// ����  : int &length      �ڵ㳤��
	/// ����  : int &left        �����־
	/// ����  : string &value    �ڵ�ֵ
	/// ����  : ȡһ�����ýڵ��ֵ������
	bool getField(CNode *fieldnode,S_FUNC_FIELD_INFO *fieldpack);

	/// ������: readHead
	/// ���  : ������ 2006-6-8 12:56:59
	/// ����  : bool 
	/// ����  : CNode *m_readnode
	/// ����  : ��ȡ��ͷ�ڵ�������m_include_head
	bool readHead(CNode *readnode);

	/// ������: readTrans
	/// ���  : ������ 2006-6-12 17:02:27
	/// ����  : bool 
	/// ����  : CNode *transnode
	/// ����  : ��ȡ�������Ϣ
	bool readTrans(CNode *transnode);

	/// ������: readFunc
	/// ���  : ������ 2006-6-8 12:57:33
	/// ����  : int 
	/// ����  : CNode *funclistnode
	/// ����  : ��ȡ����������Ϣ��m_func_conf_info
	bool readFunc(CNode *funclistnode);

	/// ������: *rtrim
	/// ���  : ������ 2006-6-8 13:01:32
	/// ����  : char 
	/// ����  : char *str
	/// ����  : ȥ���ҿո�
	char *rtrim(char *str);

	/// ������: *ltrim
	/// ���  : ������ 2006-6-8 13:01:35
	/// ����  : char 
	/// ����  : char *str
	/// ����  : ȥ����ո�
	char *ltrim(char *str);

	/// ������: trim
	/// ���  : ������ 2006-6-8 13:01:39
	/// ����  : int 
	/// ����  : char *s
	/// ����  : ȥ���ո�
	int trim(char *s);

	/// ������: trim
	/// ���  : ������ 2006-6-8 13:01:43
	/// ����  : int 
	/// ����  : string &s
	/// ����  : ȥ���ո�
	int trim(std::string &s);
	
protected:
	CNode *m_rootnode;/*xml���ڵ�*/
	CNode *m_headnode;/*��ͷ�ڵ�*/
	CNode *m_funclistnode;/*�����б�ڵ�*/
	CNode *m_transnode;///����ڵ�
	CNode *m_txcodenode;///ȡ������Ľڵ�
public:
	bool resetField(S_FUNC_FIELD_INFO *field);

	/// ������: getTxCode
	/// ���  : ������ 2006-8-2 17:16:40
	/// ����  : int 
	/// ����  : char *buf
	/// ����  : int buflen
	/// ����  : int &txcode
	/// ����  : ����������Ϣ�����ݻ�����ȡ��������
	int getTxCode(char *buf,int buflen,int &txcode);


	/// ������: getTransConv
	/// ���  : ������ 2006-6-12 17:33:54
	/// ����  : string 
	/// ����  : string transname
	/// ����  : ��ȡ�������Ƶ�����   0ת�� 1�Ӽ��˳�����
	std::string getTransConv(std::string transname);

	/// ������: getTransString
	/// ���  : ������ 2006-6-12 17:27:06
	/// ����  : string  ������ֵ
	/// ����  : string transname ��������
	/// ����  : string sourceValue Ҫ������ֶ�ֵ
	/// ����  : ��ȡ������ֵ
	std::string getTransString(std::string transname,std::string sourceValue);

	/// ������: *getHeadInfo
	/// ���  : ������ 2006-8-2 17:13:12
	/// ����  : S_PACKHEAD_INFO  
	/// ����  : string headname
	/// ����  : ���ݰ�ͷ����ȡ��ͷ��Ϣ
	S_PACKHEAD_INFO  *getHeadInfo(std::string headname);

	/// ������: *getFuncInfo
	/// ���  : ������ 2006-8-2 17:13:37
	/// ����  : S_FUNC_CONF_INFO 
	/// ����  : int txcode
	/// ����  : ���ݽ�����ȡ����������Ϣ
	S_FUNC_CONF_INFO *getFuncInfo(int txcode);
	/// ������: fromFile
	/// ���  : ������ 2006-6-8 12:58:32
	/// ����  : int 
	/// ����  : char *filename
	/// ����  : �ӽӿ������ļ���ȡ�ӿ�������Ϣ
	int fromFile(char *filename);
	///��ȡ������Ϣ
	char *getMsg();

	/// ������: fromBuf
	/// ���  : ������ 2006-6-8 12:59:50
	/// ����  : bool 
	/// ����  : char *xmlBuf
	/// ����  : �����ݻ����ȡ�ӿ�������Ϣ
	bool fromBuf(char *xmlBuf);
};

#endif /// !defined(AFX_INTERFACEPACK_H__ACF796B0_9EC2_47FD_B214_7438ADC630FA__INCLUDED_)

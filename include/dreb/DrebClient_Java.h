// DrebClient_Java.h: interface for the CDrebClient_Java class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBCLIENT_JAVA_H__3A4F4813_2022_4BF3_AB23_0CB8ED0AE339__INCLUDED_)
#define AFX_DREBCLIENT_JAVA_H__3A4F4813_2022_4BF3_AB23_0CB8ED0AE339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DrebPubDefine.h"
#include "DrebEndian.h"
#include "MdbBase.h"
#include "DrebClient.h"

#ifdef DREBAPI_JAVA_EXPORTS
#define DREBAPI_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define DREBAPI_EXPORT __declspec(dllimport)
#else
#define DREBAPI_EXPORT 
#endif
#endif

#define    MAXINSTANCE_NUM    120  //���200��ʵ��
#define    INSTANCENUM_ERR    121  //��������
#define    INSTANCENEW_ERR    122  //NEW�����
#define    INSTANCEINIT_ERR   123  //��init����

//java������������򣬴����ֽ����飬JAVA�����ÿ����ֽ������⣬�ɱ�������ж��Ƿ���Ҫת��

class DREBAPI_EXPORT CDrebClient_Java
{
public:

	// ������: GetErrMsg
	// ���  : ������ 2016-9-6 16:18:50
	// ����  : char  0ʧ�� 1�ɹ� 
	// ����  : char index ʵ����Ӧ������
	// ����  : char *errmsg ������Ϣ
	// ����  : ȡ�ó�����Ϣ
	char GetErrMsg(char index,char *errmsg);

	// ������: SetPara
	// ���  : ������ 2016-9-6 10:20:41
	// ����  : void 
	// ����  : char index ʵ����Ӧ������
	// ����  : char *ip   ip��ַ
	// ����  : char *port      4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
	// ����  : char *timeout   4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
	// ����  : ����ͨѶ����
	void SetPara(char index,char *ip,char *port ,char *timeoutms);

	// ������: SetCallerEndian
	// ���  : ������ 2016-9-6 9:49:37
	// ����  : void 
	// ����  : char bigendian  1������:JAVA C/C++(hp_ux aix)   2������ C/C++(windows/linux)
	// ����  : ���õ������ֽ���   Ĭ��Ϊ1������
	void SetCallerEndian(char bigendian);

	// ������: RecvData
	// ���  : ������ 2016-9-6 9:12:55
	// ����  : char 0��ָ��ʱ����δ�յ�����  100�ɹ� 102����ʧ�� 101δ���� 103���ճ��ȳ��� 1���ӶϿ� 2ͨѶƽ̨Ӧ�����ݲ��� 3CRC����
	// ����  : char index ʵ����Ӧ������
	// ����  : char *drebheaddata   ��Ӧ���ߵ�COMMSTRU�ṹ
	// ����  : char *timeoutms  ���ճ�ʱ ����
	// ����  : ����Ӧ��
	char RecvData(char index,char *drebheaddata,char *timeoutms);

	// ������: SendData
	// ���  : ������ 2016-9-6 9:11:29
	// ����  : char 0ʧ��  1���ͳ��� 2����̫��  100���ͳɹ�
	// ����  : char index ʵ����Ӧ������
	// ����  : char *drebheaddata  ��Ӧ���ߵ�COMMSTRU�ṹ  
	// ����  : ��������ע����ͬ���������ӣ����ͳɹ���ȷ��Ҫ��Ӧ���Ҫ���ϵ���RecvData
	char SendData(char index,char *drebheaddata);

	// ������: GetDrebHead
	// ���  : ������ 2016-9-5 16:23:25
	// ����  : char  0ʧ��  1�ɹ�
	// ����  : char index   ʵ����Ӧ������
	// ����  : char *head   DREB_HEAD�Ľṹ���ݣ���תΪ������ ע��head�ռ�Ҫ���ڵ���DREBHEADLEN
	// ����  : ȡ�����������ߵı���ͷ��Ϣ
	char GetDrebHead(char index,char *head);

	// ������: IsConnect
	// ���  : ������ 2016-9-5 16:10:46
	// ����  : char 0û���ϣ�1�ɹ�����
	// ����  : char index ʵ����Ӧ������
	// ����  : �Ƿ�����������
	char IsConnect(char index);

	// ������: Close
	// ���  : ������ 2016-9-5 15:44:09
	// ����  : void 
	// ����  : char index ʵ����Ӧ������
	// ����  : �ر���������
	void Close(char index);

	// ������: Connect
	// ���  : ������ 2016-9-5 15:42:57
	// ����  : char 0ʧ�� 1�ɹ�
	// ����  : char index ʵ����Ӧ������
	// ����  : ��������
	char Connect(char index);

	// ������: FreeInstance
	// ���  : ������ 2016-9-5 15:19:56
	// ����  : void 
	// ����  : char index  ʵ����Ӧ������
	// ����  : �ͷ�һ���ͻ���ʵ��
	void FreeInstance(char index);

	// ������: NewInstance
	// ���  : ������ 2016-9-5 14:56:53
	// ����  : char  ���ص�Ϊʵ����Ӧ��������ÿ�ε���Ҫ���������  ����120Ϊʧ�ܣ���ͬʱֻ����120��ʵ����
	// ����  : char *ip   ip��ַ
	// ����  : char *port      4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
	// ����  : char *timeout   4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
	// ����  : ����һ���µĿͻ���ʵ��  
	char NewInstance(char *ip,char *port,char *timeout);

	

	CDrebClient_Java();
	virtual ~CDrebClient_Java();

	

protected:
// 	typedef struct 
// 	{
// 		CDrebClient *client;
// 		int         id;
// 	}S_DREB_CLIENT;
	CDrebEndian  m_pEndian;//�ֽ���ת��
	CInstanceList<CDrebClient> m_pInstanceList;  //����java����new���ɵ�ʵ����delete��java������������

};

//����������������ṩ��java���õ�c����
extern "C"
{	
	// ������: GetErrMsg
	// ���  : ������ 2016-9-6 16:18:50
	// ����  : char  0ʧ�� 1�ɹ� 
	// ����  : char index ʵ����Ӧ������
	// ����  : char *errmsg ������Ϣ
	// ����  : ȡ�ó�����Ϣ
	DREBAPI_EXPORT char BFDREB_CLIENT_GetErrMsg(char index,char *errmsg);

	// ������: SetCallerEndian
	// ���  : ������ 2016-9-6 9:49:37
	// ����  : void 
	// ����  : char bigendian  1������:JAVA C/C++(hp_ux aix)   2������ C/C++(windows/linux)
	// ����  : ���õ������ֽ���   Ĭ��Ϊ1������
	DREBAPI_EXPORT	void BFDREB_CLIENT_SetCallerEndian(char bigendian);
	
	// ������: RecvData
	// ���  : ������ 2016-9-6 9:12:55
	// ����  : char 0��ָ��ʱ����δ�յ�����  100�ɹ� 102����ʧ�� 101δ���� 103���ճ��ȳ��� 1���ӶϿ� 2ͨѶƽ̨Ӧ�����ݲ��� 3CRC����
	// ����  : char index ʵ����Ӧ������
	// ����  : char *drebheaddata   ��Ӧ���ߵ�COMMSTRU�ṹ
	// ����  : char *timeoutms  ���ճ�ʱ ����
	// ����  : ����Ӧ��
	DREBAPI_EXPORT char BFDREB_CLIENT_RecvData(char index,char *drebheaddata,char *timeoutms);
	
	// ������: SendData
	// ���  : ������ 2016-9-6 9:11:29
	// ����  : char 0ʧ��  1���ͳ��� 2����̫��  100���ͳɹ�
	// ����  : char index ʵ����Ӧ������
	// ����  : char *drebheaddata  ��Ӧ���ߵ�COMMSTRU�ṹ  
	// ����  : ��������ע����ͬ���������ӣ����ͳɹ���ȷ��Ҫ��Ӧ���Ҫ���ϵ���RecvData
	DREBAPI_EXPORT char BFDREB_CLIENT_SendData(char index,char *drebheaddata);
	
	// ������: GetDrebHead
	// ���  : ������ 2016-9-5 16:23:25
	// ����  : char  0ʧ��  1�ɹ�
	// ����  : char index   ʵ����Ӧ������
	// ����  : char *head   DREB_HEAD�Ľṹ���ݣ���תΪ������ ע��head�ռ�Ҫ���ڵ���DREBHEADLEN
	// ����  : ȡ�����������ߵı���ͷ��Ϣ
	DREBAPI_EXPORT char BFDREB_CLIENT_GetDrebHead(char index,char *head);
	
	// ������: IsConnect
	// ���  : ������ 2016-9-5 16:10:46
	// ����  : char 0û���ϣ�1�ɹ�����
	// ����  : char index ʵ����Ӧ������
	// ����  : �Ƿ�����������
	DREBAPI_EXPORT char BFDREB_CLIENT_IsConnect(char index);
	
	// ������: Close
	// ���  : ������ 2016-9-5 15:44:09
	// ����  : void 
	// ����  : char index ʵ����Ӧ������
	// ����  : �ر���������
	DREBAPI_EXPORT void BFDREB_CLIENT_Close(char index);
	
	// ������: Connect
	// ���  : ������ 2016-9-5 15:42:57
	// ����  : char 0ʧ�� 1�ɹ�
	// ����  : char index ʵ����Ӧ������
	// ����  : ��������
	DREBAPI_EXPORT char BFDREB_CLIENT_Connect(char index);
	
	// ������: FreeInstance
	// ���  : ������ 2016-9-5 15:19:56
	// ����  : void 
	// ����  : char index  ʵ����Ӧ������
	// ����  : �ͷ�һ���ͻ���ʵ��
	DREBAPI_EXPORT void BFDREB_CLIENT_FreeInstance(char index);
	
	// ������: NewInstance
	// ���  : ������ 2016-9-5 14:56:53
	// ����  : char  ���ص�Ϊʵ����Ӧ��������ÿ�ε���Ҫ���������  ����120Ϊʧ�ܣ���ͬʱֻ����120��ʵ����
	// ����  : char *ip   ip��ַ
	// ����  : char *port      4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
	// ����  : char *timeout   4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
	// ����  : ����һ���µĿͻ���ʵ��  
	DREBAPI_EXPORT char BFDREB_CLIENT_NewInstance(char *ip,char *port,char *timeout);

	// ������: SetPara
	// ���  : ������ 2016-9-6 10:20:41
	// ����  : void 
	// ����  : char index ʵ����Ӧ������
	// ����  : char *ip   ip��ַ
	// ����  : char *port      4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
	// ����  : char *timeout   4�ֽڵ�����   Ϊ�˴����� javaͳһʹ��bytes�ֽ����鴫������ֵ
	// ����  : ����ͨѶ����
	DREBAPI_EXPORT void BFDREB_CLIENT_SetPara(char index,char *ip,char *port ,char *timeoutms);

}



#endif // !defined(AFX_DREBCLIENT_JAVA_H__3A4F4813_2022_4BF3_AB23_0CB8ED0AE339__INCLUDED_)

/********************************************************************
	created:	2005/07/08
	created:	8:7:2005   19:44
	purpose:	 ����ϣ���ṩһ�ֿ����ڶ�����£�ʹ�ù����ڴ��ķ���
            Ϊ�˴����������̶�ʹ�õĹ����ڴ�飬����ͳһʹ��һ��key_t
            ��һ����0xXXXXXXXX��ʾ�Ĺؼ��֣�Ϊ�˼�ʹ�÷���������Ͳ���
            ftok��������ֵ�ˡ�

            CShareMemory�����ڴ���Դʹ�÷�����
            1. ������Ϊ���ṩ��һ����ͬһ�����ϵĲ�ͬ����ͨ�������ڴ���Դ
            ���ٹ���һЩ���ݣ�
            2. ����ʹ�ø�������ʹ����ͬ��KEY���һ����ͬ�Ĺ����ڴ���Դ��
            3. ��ȡ��������Open(...)���õ��������̷��ʹ����ڴ�ӳ���ڴ��ַshmptr��
            4. �ڵõ�ӳ���ڴ��ַshmptr�󣬶�д���ݿ���ֱ��ͨ�������ж�д��
            5. Ϊ�˱�����ͬһʱ��ͬʱ�����������ݣ��ڱ�Ҫ��ʱ�򣬿��ԶԹ����ڴ���Դ
            ���м�����
            6. �ڼ����������ʱ������ֱ�����ڴ�ӳ���ַ������Ҫͨ����������ʵ��
            ͨ��Lock()������Unlock()������
            7. ����Ҫ�������ڴ���Դ��ϵͳ��ɾ���������Ҫ�ñ���ʵ������Remove(...)��
            ɾ����ӦKEY�Ĺ����ڴ���Դ�������ʵ���Ѿ��ɹ��򿪲���û�б��رգ���ô��
            ɾ����ʵ���Ĺ����ڴ���Դ�����ʱ��ͺ�semkey��shmkey�޹ء�
*********************************************************************/


/// ShareMemory.h: interface for the CShareMemory class.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHAREMEMORY_H__385E9967_54D4_4A8A_AD31_0445E3EF56CC__INCLUDED_)
#define AFX_SHAREMEMORY_H__385E9967_54D4_4A8A_AD31_0445E3EF56CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /// _MSC_VER > 1000

#include "public.h"
#ifdef WIN32
#include <windows.h>
typedef int key_t;
typedef HANDLE SHMID;
#define INVALID_ID NULL
#else
#include <sys/shm.h>
typedef int SHMID;
#define INVALID_ID -1
#endif

#ifdef KERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif

class KERNEL_EXPORT CShareMemory  
{
public:
	int m_fileno;        /// ���ڼ������ļ�, Ҳ���ǻ����ļ�
	SHMID m_shmid;       /// �����ڴ�ID
	int m_size;          /// �������ڴ泤��
	void *m_address;     /// �����ڴ�ռ��ڱ������еĵ�ַ
	char m_szmsg[256];   /// �ڴ����ʱ�����ڴ�Ŵ�����Ϣ

public:
	/// ������: Unlock
    /// ���  : wms 2005-7-9 0:12:45
    /// ����  : �������Դ�Ǳ����̼����ģ�����н���
    /// ����  : bool: �����Ƿ�ɹ�������false������GetMessage()�õ�������Ϣ 
	bool Unlock(int offset=0, int len = -1);
	
	/// ������: Lock
    /// ���  : wms 2005-7-9 0:11:47
    /// ����  : �Ա���Դ���м���������ģʽ��
    /// ����  : bool: �Ƿ�����ɹ�������false������GetMessage()�õ�������Ϣ
	bool Lock(int offset=0,int len=-1);


	/// ������: Remove
	/// ���  : ������ 2007-4-25 11:07:24
	/// ����  : static bool 
	/// ����  : const char *name ָ���ù����ڴ������
	/// ����  : ɾ��ָ���Ĺ����ڴ���Դ
	static bool Remove(const char *name);

	/// ������: Open
	/// ���  : ������ 2007-4-25 11:08:51
	/// ����  : void * NULL����ʧ�ܣ����򣬷��سɹ��򿪵Ĺ����ڴ�����Ľ���ӳ���ַ��
	/// ����  : const char *name ָ���ù����ڴ�����֣���name==NULLʱ������������������
	/// ����  : int shmsize ������򿪵Ĺ����ڴ��С
	/// ����  : �ñ���ʵ����һ�����̿ɱ��������ڴ�����  ע�⣺��ʹ�ñ�������ʱ�򣬽���������ȷ��shmsize;
    ///             ע�⣬���ﷵ�ص�ӳ���ַ����ϵͳ���صģ���Ҫ�����Ի�ָ��
	void * Open(const char *name, int shmsize);


	/// ������: myftok
	/// ���  : ������ 2007-4-25 11:07:57
	/// ����  : static key_t 
	/// ����  : const char *path
	/// ����  : int ID
	/// ����  : ��ȡ�ؼ���
	static key_t myftok(const char *path, int ID);



   /// ������: GetSHMPtr
   /// ���  : wms 2005-7-10 15:13:07
   /// ����  : �õ��������ڴ�ӳ�䵽�����̵��ڴ��ַ
   /// ����  : void * ���������ڴ�ӳ��ĵ�ַ����û��ʹ��Open�򿪻��Ѿ����ر�
   ///          ��ʱ�򣬷���ΪNULL��

	/// ������: GetSHMPtr
	/// ���  : ������ 2007-4-25 11:09:54
	/// ����  : void * 
	/// ����  : 
	void * GetSHMPtr();

   /// ������: GetMessage
   /// ���  : wms 2005-7-10 15:11:38
   /// ����  : �������������ڴ���Դ��ʱ��������ش��󣬿���ͨ����
   ///         �����õ�������Ϣ
   /// ����  : char * ���ڲ���������У����һ�δ�����Ϣ
	char * GetMessage();

   /// ������: Close
   /// ���  : wms 2005-7-9 0:13:21
   /// ����  : �رձ�������Դ���Ӵ˱����̾Ͳ���ʹ�ñ���Դ�ˣ�
   ///          ���ǲ�����ϵͳ��ɾ������Դ
   /// ����  : void 
	void Close();

   
   
   CShareMemory();
	virtual ~CShareMemory();

private:
	bool locktype(int ltype, unsigned int offset, unsigned int  len);


};
#undef KERNEL_EXPORT
#endif /// !defined(AFX_SHAREMEMORY_H__385E9967_54D4_4A8A_AD31_0445E3EF56CC__INCLUDED_)

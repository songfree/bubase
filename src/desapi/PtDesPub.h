#if !defined(AFX_PTDES_PUB_H__)
#define AFX_PTDES_PUB_H__


#ifdef MYKERNEL_EXPORTS
#define KERNEL_EXPORT __declspec(dllexport)
#else
#ifdef WIN_DLL
#define KERNEL_EXPORT __declspec(dllimport)
#else
#define KERNEL_EXPORT 
#endif
#endif


extern "C"
{

	// ������: softgenmac
	// ���  : ������ 2013-1-27 8:37:16
	// ����  : int  0�ɹ�
	// �������  : char *makkey   mackey ���� 
	// �������  : char *macblock   Ҫ����MAC�ı��Ŀ�
	// �������  : char *returnmac  ���ص�MAC����
	// �������  : int blocklen  Ҫ����MAC�ı��Ŀ鳤��
	// ����  : ����MAC
	 KERNEL_EXPORT  int  softgenmac(char *makkey,char *macblock,char *returnmac,int blocklen);


	// ������: softendesc
	// ���  : ������ 2013-1-27 8:39:33
	// ����  : int  0�ɹ�
	// ����  : char *mkey   ��Կ   ����Ϊ16����   16Ϊdes  32Ϊ2des 48Ϊ3des
	// ����  : char *pin    Ҫ���ܵ������ (һ���ȵ���getpinblock���˺����õ������) ����Ϊ16
	// ����  : char *outpin  ���ܵ����� ���Ȳ�����Ϊ16
	// ����  : �����
	 KERNEL_EXPORT  int  softendesc(char *mkey, char *pin, char *outpin);

	// ������: softdedesc
	// ���  : ������ 2013-1-27 8:50:44
	// ����  : int  0�ɹ�
	// �������  : char *mkey ��Կ   ����Ϊ16����   16Ϊdes  32Ϊ2des 48Ϊ3des
	// �������  : char *pin   Ҫ���ܵ������  ����Ϊ16
	// �������  : char *outpin  ��������� ����getbin�õ���������
	// ����  : ����
	 KERNEL_EXPORT  int  softdedesc(char *mkey, char *pin, char *outpin);
	 

	// ������: getpinblock
	// ���  : ������ 2016-1-27 8:52:29
	// ����  : void 
	// �������  : char *zh  �˺�
	// �������  : char *pin  ��������
	// �������  : char *pinblock  �����
	// ����  : ������˺����õ������
	 KERNEL_EXPORT  void getpinblock(char *zh,char *pin,char *pinblock );

	// ������: getpin
	// ���  : ������ 2013-1-27 8:53:40
	// ����  : void 
	// �������  : char *zh  �˺�
	// �������  : char *pinblock  �����
	// �������  : char *pin  ��������
	// ����  : �õ���������
	 KERNEL_EXPORT  void getpin(char *zh,char *pinblock,char *pin);

	 KERNEL_EXPORT  void change_key(char *key,int len);



	// ������: softencrypt
	// ���  : ������ 2016-1-27 8:55:36
	// ����  : int  
	// �������  : const char *pin  Ҫ���ܵ������ (һ���ȵ���getpinblock���˺����õ������) ����Ϊ16
	// �������  : char *outpin     ���ܵ����� ���Ȳ�����Ϊ16
	// ����  : �����׿ͻ��˵��õ�����ܷ�������Կ�̶�
	 KERNEL_EXPORT  int  softencrypt(const char *pin, char *outpin);


	// ������: softdecrypt
	// ���  : ������ 2016-1-27 8:57:37
	// ����  : int  
	// �������  : const char *pin Ҫ���ܵ������  ����Ϊ16
	// �������  : char *outpin    ��������� ����getbin�õ���������
	// ����  : �����׿ͻ��˵��õ�����ܷ�������Կ�̶�
	 KERNEL_EXPORT  int  softdecrypt(const char *pin, char *outpin);


	// ������: softde2pwd
	// ���  : ������ 2016-1-27 9:00:22
	// ����  : int  
	// �������  : const char *zh   �˺�
	// �������  : const char *pin  Ҫ���ܵ������ ����Ϊ16
	// �������  : char *outpin  ��������
	// ����  : �����׿ͻ��˵��õ�����ܷ�������Կ�̶�,����ֱ�ӵ���getbin�õ�����������
	 KERNEL_EXPORT  int  softde2pwd(const char *zh,const char *pin, char *outpin);
};



#endif /* !defined(AFX_PTDES_PUB_H__E3B5E504_19B5_11D3_8A76_00A00C12E824__INCLUDED_)*/

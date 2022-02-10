// dreberrcode.h: interface for the dreberrcode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREBERRCODE_H__FD1208CA_2314_4920_98EA_D003257FD554__INCLUDED_)
#define AFX_DREBERRCODE_H__FD1208CA_2314_4920_98EA_D003257FD554__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define  SUCCESS              0   //�ɹ�
#define  ERR_SVRID            1   //����Ų���
#define  ERR_PRIVATEID        2   //˽�з���Ų���
#define  ERR_REGISTER         3   //������ע��
#define  ERR_SVRNOTREGISTER   4   //����δע��
#define  ERR_DREBID           5   //ͨѶ�ڵ�ID����
#define  ERR_DREBPRIVATEID    6   //ͨѶ˽�нڵ�ID����
#define  ERR_DREBREGISTER     7   //ͨѶ�ڵ���ע��
#define  ERR_DREBNODE         8   //ͨѶ�ڵ��ظ�
#define  ERR_QUEUE            9    //�����ʧ��
#define  ERR_DREBROUTE        10   //ͨѶ�ڵ�·�ɲ�����
#define  ERR_VERSION          11   //���Ӱ汾����
#define  ERR_SERVICENO        12   //ע�Ṧ�ܺŲ���
#define  ERR_FUNCNOTFUND      13   //���ܺŲ�����
#define  ERR_NEXTFILENOT      14   //�������ļ�������
#define  ERR_NOBPUGROUP       15   //�޴�BPU��
#define  ERR_NOBPUFREE        16   //��BPU����������
#define  ERR_TIMEOUT          17   //�����ڷ�������ѳ�ʱ
#define  ERR_DATA_ENCRYPTZIP  18   //���ݴ�������ܽ�ѹ����
#define  ERR_SYSTEM           19   //ϵͳ��

#define  ERR_BPC_REGTX          21   //ע�ύ�״���
#define  ERR_BPC_NOGROUP        22   //ע�ύ�״���,��BPU��
#define  ERR_BPC_NOREGISTER     23   //ע�ύ�״���,����ע��
#define  ERR_BPC_BPULIMIT       24   //ע�ύ�״���,bpu��������,����
#define  ERR_BPC_TXFIND         25   //ע�ύ�״���,������ע��
#define  ERR_START_DREBTIMER     26   //����dreb��ض�ʱ��ʧ��
#define  ERR_STOP_DREBTIMER      27   //ֹͣdreb��ض�ʱ��ʧ��

#define  ERR_BPU_INIT             40   //BPU��ʼ������
#define  ERR_BPU_CATCH            41   //BPU�����쳣����
#define  ERR_BPU_NOTFUNC          42   //BPU�޴˽���
#define  ERR_BPU_DATA             43   //BPU�������ݲ���
#define  ERR_BPU_MONITORDATA      44   //BPU������ݲ���
#define  ERR_MONITOR_DOWNLOAD     45   //�����ļ�����
#define  ERR_MONITOR_UPLOAD       46   //�ϴ��ļ�����
#endif // !defined(AFX_DREBERRCODE_H__FD1208CA_2314_4920_98EA_D003257FD554__INCLUDED_)

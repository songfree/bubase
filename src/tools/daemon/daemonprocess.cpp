
#include "public.h"
#include "BF_Tools.h"
#include "BF_SocketTcp.h"
#include "BF_Xml.h"
#include "KvData.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "bf_kerneld.lib") 
#pragma message("Automatically linking with  bf_kerneld.lib")
#else
#pragma comment(lib, "bf_kernel.lib") 
#pragma message("Automatically linking with  bf_kernel.lib")
#endif
#endif


int main(int argc, char* argv[])
{
	CBF_Xml xml ;
	if (!xml.FromFile("testdaemon.xml"))
	{
		printf("�����ļ�testdaemon.xml�����ڻ��xml\n");
		return -1;
	}

	
	std::string sIp;
	int  nPort;
	std::string node_id;
	CBF_SocketTcp pSocket;
	CKvData data;
	std::string  sendbuffer;
	int ret;
	

	if (xml.GetNodeValueByPath("daemon/�����ػ�IP",false,sIp)== NULL)
	{
		printf("daemon��IPδ����\n");
		return -1;
	}

	if (xml.GetNodeValueByPath("daemon/�����ػ��˿�",false,nPort) == NULL)
	{
		printf("daemon�Ķ˿�δ����\n");
		return -1;
	}

	if (xml.GetNodeValueByPath("daemon/�ڵ��ʶ",false,node_id)== NULL)
	{
		printf("�ڵ��ʶδ����\n");
		return -1;
	}
	CKVNode *tmpnode = data.SetNodeByName("node_id",22);
	if (tmpnode == NULL)
	{
		printf("���ýڵ��ʶ����\n");
		return -1;
	}
	tmpnode->m_sKvValue = node_id;
	data.ToString(sendbuffer);
	char sendData[8192];
	int  sendLen = sendbuffer.length()+8;
	sprintf(sendData,"%08d#%s",sendbuffer.length(),sendbuffer.c_str());
	while (true)
	{
		if (!pSocket.GetConnectStatus())
		{
			pSocket.Close();
			pSocket.Create(AF_INET);
			if (!pSocket.ConnectServer(sIp.c_str(),nPort,5))
			{
				printf("����daemon�����˿�ʧ��\n");
				SLEEP_SECONDS(2);
				continue;
			}
		}
		ret = pSocket.Send(sendData,sendLen);
		if (ret != sendLen)
		{
			pSocket.Close();
		}
		SLEEP_SECONDS(5);
	}

	return 0;
}


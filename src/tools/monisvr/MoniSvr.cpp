/**
* Copyright (c) 2008 songfree
* All rights reserved.
*
* �ļ�����:MoniSvr.cpp
* ժҪ:
*       
*        
*
* ��ǰ�汾��:1.0.0
*
* �汾��ʷ��Ϣ:
* �汾��            ����/�޸���       �������                 ԭ��
* 1.0.0                AUTO           20131127                  ����
**/
// MoniSvr.cpp: implementation of the CMoniSvr class.
//
//////////////////////////////////////////////////////////////////////



#include "MoniSvr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CMoniSvr::~CMoniSvr()
{

}
bool CMoniSvr::BuInit()
{
	m_bIsInit = true;
	if (m_pSqlCmd == NULL)
	{
		m_bIsInit = false;
		return false;
	}
	sprintf(m_sSql,"select para_value from PFM_PARAMETER where para_name=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = "SAVELOGNUM";
	CSqlData data = m_pSqlCmd->LoadResultSql();
	if (data.GetSqlCount() <1)
	{
		sprintf(m_sSql,"insert into  PFM_PARAMETER (para_name,para_note,para_value) values(:1,:2,:3)");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = "SAVELOGNUM";
		m_pSqlCmd->Param(2).setAsString() = "������־��¼��";
		m_pSqlCmd->Param(3).setAsString() = "1000";
		if (!m_pSqlCmd->Execute())
		{
			m_pSqlCmd->Rollback();
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ִ��[%s]���� %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			m_bIsInit = false;
			return false;
		}
		m_pSqlCmd->Commit();
		m_nSaveLogNum = 1000;
		return m_bIsInit;
	}
	m_nSaveLogNum = data.GetAtI(0,0);
	sprintf(m_sSql,"select para_value from PFM_PARAMETER where para_name=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = "STATUSTIMEOUT";
	data = m_pSqlCmd->LoadResultSql();
	if (data.GetSqlCount() <1)
	{
		sprintf(m_sSql,"insert into  PFM_PARAMETER (para_name,para_note,para_value) values(:1,:2,:3)");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = "STATUSTIMEOUT";
		m_pSqlCmd->Param(2).setAsString() = "״̬��ʱʱ��(��)";
		m_pSqlCmd->Param(3).setAsString() = "20";
		if (!m_pSqlCmd->Execute())
		{
			m_pSqlCmd->Rollback();
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ִ��[%s]���� %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			m_bIsInit = false;
			return false;
		}
		m_pSqlCmd->Commit();
		m_nStatusTimeOut = 20;
		return m_bIsInit;
	}
	m_nStatusTimeOut = data.GetAtI(0,0);

	sprintf(m_sSql,"select para_value from PFM_PARAMETER where para_name=:1");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsString() = "WRITEBFMLOG";
	data = m_pSqlCmd->LoadResultSql();
	if (data.GetSqlCount() <1)
	{
		sprintf(m_sSql,"insert into  PFM_PARAMETER (para_name,para_note,para_value) values(:1,:2,:3)");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = "WRITEBFMLOG";
		m_pSqlCmd->Param(2).setAsString() = "�Ƿ�д�����־";
		m_pSqlCmd->Param(3).setAsString() = "1";
		if (!m_pSqlCmd->Execute())
		{
			m_pSqlCmd->Rollback();
			m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"ִ��[%s]���� %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			m_bIsInit = false;
			return false;
		}
		m_pSqlCmd->Commit();
		m_nWriteBFMLog = 1;
		return m_bIsInit;
	}
	m_nWriteBFMLog = data.GetAtI(0,0);
	
	return m_bIsInit;
}


void CMoniSvr::SetBuPara(CBF_Xml *conxml,CIPcLink *pclink,CDbApi *sqldb,CBF_SimpleLog *bulog)
{
	if (conxml == NULL || pclink == NULL || bulog == NULL)
	{
		m_bIsSetPara = false;
		return;
	}
	m_pLog = bulog;
	m_pConfigXml = conxml;
	m_pPcLink = pclink;
	m_pSqlCmd = sqldb;
	m_bIsSetPara = true;
}
//ע�⣺����Ķ�����ÿ�������涼Ҫ�У���������ݴ˷�����ȡ�����ʵ��
CMoniSvr * getInstance() 
{ 
    return new CMoniSvr(); 
}
CMoniSvr::CMoniSvr()
{
	m_nWriteBFMLog = 1;
	m_nCurTime =0;
	bzero(m_sSql,sizeof(m_sSql));
	//���ܺ� ����ָ��  �������� �汾 ����  ���ȼ�  ������־ �Ƿ��׵�λ  �Ƿ�ͣ��
	//InitFuncInfo(1000,(FuncPointer )&CSample::test01,"����",1.0,"����",1,false,true,false);
	//�����ӹ���Ҫ�ڴ˽�������
	InitFuncInfo(99001,(FuncPointer )&CMoniSvr::NodeReport,"��ؽڵ㱨��",1.0,"songfree",1,false,false,false);
}



int CMoniSvr::NodeReport(PBPCCOMMSTRU data)
{
	CBF_Xml pXml;
	data->sBuffer[data->sDBHead.nLen]=0;
	m_pLog->LogBin(LOG_DEBUG+1,__FILE__,__LINE__,"���յ�������",data->sBuffer,data->sDBHead.nLen);
	if (!pXml.FromBuffer(data->sBuffer))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�������ݷ�xml");
		return -1;
	}
	if (pXml.GetNodeValueByPath("Monitor/�ڵ�����",false,m_nNodeType) == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��[Monitor/�ڵ�����]");
		return -1;
	}
	if (pXml.GetNodeValueByPath("Monitor/�����ڵ�",false,m_nNodeId) == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��[Monitor/�����ڵ�]");
		return -1;
	}
	if (pXml.GetNodeValueByPath("Monitor/˽�нڵ�",false,m_nNodePrivateId) == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��[Monitor/˽�нڵ�]");
		return -1;
	}
	if (pXml.GetNodeValueByPath("Monitor/��������",false,m_nReportType) == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��[Monitor/��������]");
		return -1;
	}
	if (pXml.GetNodeValueByPath("Monitor/����ʱ��",false,m_sStartTime) == NULL)
	{
		m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"��[Monitor/����ʱ��]");
		m_sStartTime = "";
	}
	
	if (m_pSqlCmd == NULL)
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"δ�������ݿ�");
		return -1;
	}
	int nRet;
	//ȡ���ݿ⵱ǰʱ��



	//����Ӧ�ýڵ��
	sprintf(m_sSql,"select pc_node_id from BFM_APP_NODE where public_id=:1 and private_id=:2 ");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
	m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
	CSqlData sqldata = m_pSqlCmd->LoadResultSql();
	if (sqldata.GetSqlCount() != 1)
	{
		m_nPcNodeId = 0;//�����ڵ�Ĭ��ֵ
		//δ�ҵ�,����
		//��������ڵ���Ƿ���Ĭ�ϵĽڵ�
		sprintf(m_sSql,"select count(*) from PFM_PC_NODE where pc_node_id=:1 ");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsLong() = 0;
		sqldata = m_pSqlCmd->LoadResultSql();
		if (sqldata.GetSqlCount()<1)
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]���� %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			return -1;
		}
		nRet = sqldata.GetAtI(0,0);
		if (nRet <1)
		{
			//����Ĭ�ϻ����ڵ�
			sprintf(m_sSql,"insert into PFM_PC_NODE (pc_node_id,pc_node_name,pc_ip1,pc_ip2,pc_area,pc_mem_warn,pc_cpu_warn,pc_disk_warn) values(:1,:2,:3,:4,:5,:6,:7,:8)");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsLong() = 0;
			m_pSqlCmd->Param(2).setAsString() = "Ĭ�ϻ����ڵ�";
			m_pSqlCmd->Param(3).setAsString() = "127.0.0.1";
			m_pSqlCmd->Param(4).setAsString() = "127.0.0.1";
			m_pSqlCmd->Param(5).setAsString() = "Ĭ�ϻ���λ��";
			m_pSqlCmd->Param(6).setAsLong() = 99;//�ڴ�ʹ�þ��� %ʱ�澯
			m_pSqlCmd->Param(7).setAsLong() = 99;//cpuʹ�þ��� %ʱ�澯
			m_pSqlCmd->Param(8).setAsLong() = 90;//����ʹ�þ��� %ʱ�澯
			if (!m_pSqlCmd->Execute())
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]���� %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
				m_pSqlCmd->Rollback();
				return -1;
			}
			m_pSqlCmd->Commit();
		}
		//���ӽڵ��
		m_pDate.Update();
		sprintf(m_sSql,"insert into BFM_APP_NODE (public_id,private_id,app_name,app_type,pc_node_id,rpt_date,rpt_time,last_time,node_status,start_datetime) values(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10)");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
		m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
		m_pSqlCmd->Param(3).setAsString() = "δ֪�����޸�";
		m_pSqlCmd->Param(4).setAsLong() = m_nNodeType; //�����ڵ㣬Ĭ��
		m_pSqlCmd->Param(5).setAsLong() = 0; //�����ڵ㣬Ĭ��
		m_pSqlCmd->Param(6).setAsString() = m_pDate.ToStringD8().c_str();//��������
		m_pSqlCmd->Param(7).setAsLong() = atoi(m_pDate.ToStringT6().c_str());//����ʱ��
		m_pSqlCmd->Param(8).setAsLong() = time(NULL);//������ʱ��
		m_pSqlCmd->Param(9).setAsLong() = 0;//״̬
		m_pSqlCmd->Param(10).setAsString() = m_sStartTime.c_str();//����ʱ��
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]���� %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			m_pSqlCmd->Rollback();
			return -1;
		}
		m_pSqlCmd->Commit();
	}
	else //���½ڵ��
	{
		m_nPcNodeId = sqldata.GetAtI(0,0);
		m_pDate.Update();
		sprintf(m_sSql,"update BFM_APP_NODE set rpt_date=:1,rpt_time=:2,last_time=:3,node_status=:4,start_datetime=:5 where public_id=:6 and private_id=:7");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = m_pDate.ToStringD8().c_str();//��������
		m_pSqlCmd->Param(2).setAsLong() = atoi(m_pDate.ToStringT6().c_str());//����ʱ��
		m_pSqlCmd->Param(3).setAsLong() = time(NULL);//������ʱ��
		m_pSqlCmd->Param(4).setAsLong() = 0;//״̬
		m_pSqlCmd->Param(5).setAsString() = m_sStartTime.c_str();//����ʱ��
		m_pSqlCmd->Param(6).setAsLong() = m_nNodeId;
		m_pSqlCmd->Param(7).setAsLong() = m_nNodePrivateId;
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]���� %s",m_sSql,m_pSqlCmd->GetCmdErrInfo());
			m_pSqlCmd->Rollback();
			return -1;
		}
		m_pSqlCmd->Commit();
	}
	//д������־��
	if (m_nWriteBFMLog == 1)
	{
		char sLogSerial[18];
		if (!GetSerial(sLogSerial))
		{
			return -1;
		}
		sprintf(m_sSql,"insert into BFM_LOG (serial_no,app_type,public_id,private_id,rpt_time,rpt_type,rtp_txt) values(:1,:2,:3,:4,:5,:6,:7)");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsString() = sLogSerial;
		m_pSqlCmd->Param(2).setAsLong() = m_nNodeType;
		m_pSqlCmd->Param(3).setAsLong() = m_nNodeId;
		m_pSqlCmd->Param(4).setAsLong() = m_nNodePrivateId;
		m_pSqlCmd->Param(5).setAsLong() = time(NULL);
		m_pSqlCmd->Param(6).setAsLong() = m_nReportType;
		m_pSqlCmd->Param(7).setAsString() = data->sBuffer;
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д����־����� %s",m_pSqlCmd->GetCmdErrInfo());
			m_pSqlCmd->Rollback();
			return -1;
		}
		m_pSqlCmd->Commit();
	}
	
	//����cpu �ڴ� ���̵�
	int nCpuRate=0;
	int nRamTotal=0;
	int nRamUsed=0;
	CXmlAttribute *tmpattr = NULL;
	std::string   sDiskDrive;
	int           nDiskTotal;
	int           nDiskUsed;
	CXmlNode *tmpnode=NULL;
	if (pXml.GetNodeValueByPath("Monitor/������Դ/CPU",false,nCpuRate) != NULL)
	{
		//������Դ
		sprintf(m_sSql,"select count(*) from PFM_PC_NODE_CPU where pc_node_id=:1");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsLong() = m_nPcNodeId;
		sqldata = m_pSqlCmd->LoadResultSql();
		nRet = sqldata.GetAtI(0,0);
		if (nRet !=1 ) //û�������
		{
			sprintf(m_sSql,"insert into  PFM_PC_NODE_CPU (pc_node_id,pc_node_cpu) values(:1,:2)");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsLong() = m_nPcNodeId;
			m_pSqlCmd->Param(2).setAsLong() = nCpuRate;
			if (!m_pSqlCmd->Execute())
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_PC_NODE_CPU����� %s",m_pSqlCmd->GetCmdErrInfo());
				m_pSqlCmd->Rollback();
				return -1;
			}
		}
		else //����
		{
			sprintf(m_sSql,"update PFM_PC_NODE_CPU set pc_node_cpu=:1 where pc_node_id=:2");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsLong() = nCpuRate;
			m_pSqlCmd->Param(2).setAsLong() = m_nPcNodeId;
			if (!m_pSqlCmd->Execute())
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����PFM_PC_NODE_CPU����� %s",m_pSqlCmd->GetCmdErrInfo());
				m_pSqlCmd->Rollback();
				return -1;
			}
		}
		//�ڴ�
		pXml.GetNodeAttribute("Monitor/������Դ/�ڴ�",false,"total",false,nRamTotal);
		pXml.GetNodeAttribute("Monitor/������Դ/�ڴ�",false,"used",false,nRamUsed);
		sprintf(m_sSql,"select count(*) from PFM_PC_NODE_RAM where pc_node_id=:1");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsLong() = m_nPcNodeId;
		sqldata = m_pSqlCmd->LoadResultSql();
		nRet = sqldata.GetAtI(0,0);
		if (nRet !=1 ) //û�������
		{
			sprintf(m_sSql,"insert into  PFM_PC_NODE_RAM (pc_node_id,pc_total_ram,pc_used_ram) values(:1,:2,:3)");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsLong() = m_nPcNodeId;
			m_pSqlCmd->Param(2).setAsLong() = nRamTotal;
			m_pSqlCmd->Param(3).setAsLong() = nRamUsed;
			if (!m_pSqlCmd->Execute())
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_PC_NODE_RAM����� %s",m_pSqlCmd->GetCmdErrInfo());
				m_pSqlCmd->Rollback();
				return -1;
			}
		}
		else
		{
			sprintf(m_sSql,"update PFM_PC_NODE_RAM set pc_total_ram=:1,pc_used_ram=:2 where pc_node_id=:3");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsLong() = nRamTotal;
			m_pSqlCmd->Param(2).setAsLong() = nRamUsed;
			m_pSqlCmd->Param(3).setAsLong() = m_nPcNodeId;
			if (!m_pSqlCmd->Execute())
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����PFM_PC_NODE_RAM����� %s",m_pSqlCmd->GetCmdErrInfo());
				m_pSqlCmd->Rollback();
				return -1;
			}
		}
		//����
		tmpnode = pXml.GetNodeByPath("Monitor/������Դ/����",false);
		if (tmpnode != NULL)
		{
			tmpnode = (CXmlNode *)tmpnode->GetFirstChild();
			while (tmpnode != NULL)
			{
				tmpattr = tmpnode->GetAttribute("drive",false);
				if (tmpattr == NULL)
				{
					tmpnode = (CXmlNode *)tmpnode->getNextSibling();
					continue;
				}
				sDiskDrive = tmpattr->GetAttrValue(false);
				CBF_Tools::LRtrim(sDiskDrive);
				tmpattr = tmpnode->GetAttribute("total",false);
				if (tmpattr == NULL)
				{
					tmpnode = (CXmlNode *)tmpnode->getNextSibling();
					continue;
				}
				nDiskTotal = atol(tmpattr->GetAttrValue(false).c_str());
				tmpattr = tmpnode->GetAttribute("used",false);
				if (tmpattr == NULL)
				{
					tmpnode = (CXmlNode *)tmpnode->getNextSibling();
					continue;
				}
				nDiskUsed = atol(tmpattr->GetAttrValue(false).c_str());
				//д����̱�
				sprintf(m_sSql,"select count(*) from PFM_PC_NODE_DISK where pc_node_id=:1 and pc_disk_name=:2");
				m_pSqlCmd->setCommandText(m_sSql);
				m_pSqlCmd->Param(1).setAsLong() = m_nPcNodeId;
				m_pSqlCmd->Param(2).setAsString() = sDiskDrive.c_str();
				sqldata = m_pSqlCmd->LoadResultSql();
				nRet = sqldata.GetAtI(0,0);
				if (nRet !=1 ) //û�������
				{
					sprintf(m_sSql,"insert into  PFM_PC_NODE_DISK (pc_node_id,pc_disk_name,pc_disk_total,pc_disk_used) values(:1,:2,:3,:4)");
					m_pSqlCmd->setCommandText(m_sSql);
					m_pSqlCmd->Param(1).setAsLong() = m_nPcNodeId;
					m_pSqlCmd->Param(2).setAsString() = sDiskDrive.c_str();
					m_pSqlCmd->Param(3).setAsLong() = nDiskTotal;
					m_pSqlCmd->Param(4).setAsLong() = nDiskUsed;
					if (!m_pSqlCmd->Execute())
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_PC_NODE_DISK����� %s",m_pSqlCmd->GetCmdErrInfo());
						m_pSqlCmd->Rollback();
						return -1;
					}
				}
				else
				{
					sprintf(m_sSql,"update PFM_PC_NODE_DISK set pc_disk_total=:1,pc_disk_used=:2 where pc_node_id=:3 and pc_disk_name=:4");
					m_pSqlCmd->setCommandText(m_sSql);
					m_pSqlCmd->Param(1).setAsLong() = nDiskTotal;
					m_pSqlCmd->Param(2).setAsLong() = nDiskUsed;
					m_pSqlCmd->Param(3).setAsLong() = m_nPcNodeId;
					m_pSqlCmd->Param(4).setAsString() = sDiskDrive.c_str();
					if (!m_pSqlCmd->Execute())
					{
						m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����PFM_PC_NODE_DISK����� %s",m_pSqlCmd->GetCmdErrInfo());
						m_pSqlCmd->Rollback();
						return -1;
					}
				}
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
			}
		}
		

	}
	std::string sIp;
	int nPort=0;
	int nPubId=0;
	int nPrivateId=0;
	std::string sStatus;
	int nStatus=0;
	//������Ϣ
	tmpnode = pXml.GetNodeByPath("Monitor/������Ϣ",false);
	if (tmpnode != NULL)
	{
		tmpnode = (CXmlNode *)tmpnode->GetFirstChild();
		while (tmpnode != NULL)
		{
			tmpattr = tmpnode->GetAttribute("ip",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sIp = tmpattr->GetAttrValue(false);
			tmpattr = tmpnode->GetAttribute("port",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nPort = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("�����ڵ�",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nPubId = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("˽�нڵ�",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nPrivateId = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("status",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sStatus = tmpattr->GetAttrValue(false);
			if (sStatus.compare("����") == 0)
			{
				nStatus = 0;
			}
			else
			{
				nStatus = 1;
			}
			//���±�
			sprintf(m_sSql,"select count(*) from PFM_APP_CONNECT where public_id=:1 and private_id=:2 and dst_ip=:3 and dst_port=:4");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
			m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
			m_pSqlCmd->Param(3).setAsString() = sIp.c_str();
			m_pSqlCmd->Param(4).setAsLong() = nPort;
			sqldata = m_pSqlCmd->LoadResultSql();
			nRet = sqldata.GetAtI(0,0);
			if (nRet !=1 ) //û�������
			{
				sprintf(m_sSql,"insert into  PFM_APP_CONNECT (public_id,private_id,dst_public_id,dst_private_id,dst_ip,dst_port,conn_type,conn_status,lasttime) values(:1,:2,:3,:4,:5,:6,:7,:8,:9)");
				m_pSqlCmd->setCommandText(m_sSql);
				m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
				m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
				m_pSqlCmd->Param(3).setAsLong() = nPubId;
				m_pSqlCmd->Param(4).setAsLong() = nPrivateId;
				m_pSqlCmd->Param(5).setAsString() = sIp.c_str();
				m_pSqlCmd->Param(6).setAsLong() = nPort;
				m_pSqlCmd->Param(7).setAsLong() = m_nNodeType;
				m_pSqlCmd->Param(8).setAsLong() = nStatus;
				m_pSqlCmd->Param(9).setAsLong() = time(NULL);
				if (!m_pSqlCmd->Execute())
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_APP_CONNECT����� %s",m_pSqlCmd->GetCmdErrInfo());
					m_pSqlCmd->Rollback();
					return -1;
				}
			}
			else //����
			{
				if (nStatus == 0) //����Ŀ��ڵ��
				{
					sprintf(m_sSql,"update PFM_APP_CONNECT set dst_public_id=:1,dst_private_id=:2,conn_status=:3,lasttime=:4 where public_id=:5 and private_id=:6 and dst_ip=:7 and dst_port=:8");
					m_pSqlCmd->setCommandText(m_sSql);
					m_pSqlCmd->Param(1).setAsLong() = nPubId;
					m_pSqlCmd->Param(2).setAsLong() = nPrivateId;
					m_pSqlCmd->Param(3).setAsLong() = nStatus;
					m_pSqlCmd->Param(4).setAsLong() = time(NULL);
					m_pSqlCmd->Param(5).setAsLong() = m_nNodeId;
					m_pSqlCmd->Param(6).setAsLong() = m_nNodePrivateId;
					m_pSqlCmd->Param(7).setAsString() = sIp.c_str();
					m_pSqlCmd->Param(8).setAsLong() = nPort;
				}
				else
				{
					sprintf(m_sSql,"update PFM_APP_CONNECT set conn_status=:1,lasttime=:2 where public_id=:3 and private_id=:4 and dst_ip=:5 and dst_port=:6");
					m_pSqlCmd->setCommandText(m_sSql);
					m_pSqlCmd->Param(1).setAsLong() = nStatus;
					m_pSqlCmd->Param(2).setAsLong() = time(NULL);
					m_pSqlCmd->Param(3).setAsLong() = m_nNodeId;
					m_pSqlCmd->Param(4).setAsLong() = m_nNodePrivateId;
					m_pSqlCmd->Param(5).setAsString() = sIp.c_str();
					m_pSqlCmd->Param(6).setAsLong() = nPort;
				}
				if (!m_pSqlCmd->Execute())
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����PFM_APP_CONNECT����� %s",m_pSqlCmd->GetCmdErrInfo());
					m_pSqlCmd->Rollback();
					return -1;
				}
			}
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
	}
	m_pSqlCmd->Commit();

	switch (m_nNodeType)
	{
		case 0: //dreb
			nRet = DrebReport(&pXml);
			break;
		case 1://bpc
			nRet = BpcReport(&pXml);
			break;
		case 2://��bpc����cgate
			nRet = Bpc2Report(&pXml);
			break;
		default:
			m_pLog->LogMp(LOG_WARNNING,__FILE__,__LINE__,"�ڵ�����[%d]����",m_nNodeType);
			return -1;
	}
	UpdateByLastTime();
	return nRet;
}

int CMoniSvr::DrebReport(CBF_Xml *xml)
{
	int nRet;
	CXmlAttribute *tmpattr=NULL;
	CSqlData sqldata;
	CXmlNode *tmpnode = xml->GetNodeByPath("Monitor/DREB��Ϣ/DREBLIST",false);
	int nId=0;
	int nPid=0;
	int nIndex=0;
	int nStatus=0;
	if (tmpnode != NULL)
	{
		tmpnode = (CXmlNode *)tmpnode->GetFirstChild();
		while (tmpnode != NULL)
		{
			tmpattr = tmpnode->GetAttribute("id",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nId = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("pid",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nPid = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("index",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nIndex = atoi(tmpattr->GetAttrValue(false).c_str());
			sprintf(m_sSql,"select count(*) from PFM_DREB_CONNLIST where public_id=:1 and private_id=:2 and dst_public_id=:3 and dst_private_id=:4");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
			m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
			m_pSqlCmd->Param(3).setAsLong() = nId;
			m_pSqlCmd->Param(4).setAsLong() = nPid;
			sqldata = m_pSqlCmd->LoadResultSql();
			nRet = sqldata.GetAtI(0,0);
			if (nRet !=1 ) //û�������
			{
				sprintf(m_sSql,"insert into  PFM_DREB_CONNLIST (public_id,private_id,dst_public_id,dst_private_id,conn_index,conn_status,lasttime) values(:1,:2,:3,:4,:5,:6,:7)");
				m_pSqlCmd->setCommandText(m_sSql);
				m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
				m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
				m_pSqlCmd->Param(3).setAsLong() = nId;
				m_pSqlCmd->Param(4).setAsLong() = nPid;
				m_pSqlCmd->Param(5).setAsLong() = nIndex;
				m_pSqlCmd->Param(6).setAsLong() = 0;
				m_pSqlCmd->Param(7).setAsLong() = time(NULL);
				if (!m_pSqlCmd->Execute())
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_DREB_CONNLIST����� %s",m_pSqlCmd->GetCmdErrInfo());
					m_pSqlCmd->Rollback();
					return -1;
				}
			}
			else //����
			{
				sprintf(m_sSql,"update PFM_DREB_CONNLIST set conn_index=:1,conn_status=:2,lasttime=:3 where public_id=:4 and private_id=:5 and dst_public_id=:6 and dst_private_id=:7");
				m_pSqlCmd->setCommandText(m_sSql);
				m_pSqlCmd->Param(1).setAsLong() = nIndex;
				m_pSqlCmd->Param(2).setAsLong() = nStatus;
				m_pSqlCmd->Param(3).setAsLong() = time(NULL);
				m_pSqlCmd->Param(4).setAsLong() = m_nNodeId;
				m_pSqlCmd->Param(5).setAsLong() = m_nNodePrivateId;
				m_pSqlCmd->Param(6).setAsLong() = nId;
				m_pSqlCmd->Param(7).setAsLong() = nPid;
				if (!m_pSqlCmd->Execute())
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_DREB_CONNLIST����� %s",m_pSqlCmd->GetCmdErrInfo());
					m_pSqlCmd->Rollback();
					return -1;
				}
			}
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
	}
	tmpnode = xml->GetNodeByPath("Monitor/DREB��Ϣ/SVRLIST",false);
	if (tmpnode != NULL)
	{
		tmpnode = (CXmlNode *)tmpnode->GetFirstChild();
		while (tmpnode != NULL)
		{
			tmpattr = tmpnode->GetAttribute("id",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nId = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("pid",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nPid = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("index",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nIndex = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("status",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			if (tmpattr->GetAttrValue(false).compare("����") == 0)
			{
				nStatus = 0;
			}
			else
			{
				nStatus = 1;
			}
			sprintf(m_sSql,"select count(*) from PFM_DREB_CONNLIST where public_id=:1 and private_id=:2 and dst_public_id=:3 and dst_private_id=:4");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
			m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
			m_pSqlCmd->Param(3).setAsLong() = nId;
			m_pSqlCmd->Param(4).setAsLong() = nPid;
			sqldata = m_pSqlCmd->LoadResultSql();
			nRet = sqldata.GetAtI(0,0);
			if (nRet !=1 ) //û�������
			{
				sprintf(m_sSql,"insert into  PFM_DREB_CONNLIST (public_id,private_id,dst_public_id,dst_private_id,conn_index,conn_status,lasttime) values(:1,:2,:3,:4,:5,:6,:7)");
				m_pSqlCmd->setCommandText(m_sSql);
				m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
				m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
				m_pSqlCmd->Param(3).setAsLong() = nId;
				m_pSqlCmd->Param(4).setAsLong() = nPid;
				m_pSqlCmd->Param(5).setAsLong() = nIndex;
				m_pSqlCmd->Param(6).setAsLong() = 0;
				m_pSqlCmd->Param(7).setAsLong() = time(NULL);
				if (!m_pSqlCmd->Execute())
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_DREB_CONNLIST����� %s",m_pSqlCmd->GetCmdErrInfo());
					m_pSqlCmd->Rollback();
					return -1;
				}
			}
			else //����
			{
				sprintf(m_sSql,"update PFM_DREB_CONNLIST set conn_index=:1,conn_status=:2,lasttime=:3 where public_id=:4 and private_id=:5 and dst_public_id=:6 and dst_private_id=:7");
				m_pSqlCmd->setCommandText(m_sSql);
				m_pSqlCmd->Param(1).setAsLong() = nIndex;
				m_pSqlCmd->Param(2).setAsLong() = nStatus;
				m_pSqlCmd->Param(3).setAsLong() = time(NULL);
				m_pSqlCmd->Param(4).setAsLong() = m_nNodeId;
				m_pSqlCmd->Param(5).setAsLong() = m_nNodePrivateId;
				m_pSqlCmd->Param(6).setAsLong() = nId;
				m_pSqlCmd->Param(7).setAsLong() = nPid;
				if (!m_pSqlCmd->Execute())
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_DREB_CONNLIST����� %s",m_pSqlCmd->GetCmdErrInfo());
					m_pSqlCmd->Rollback();
					return -1;
				}
			}
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
	}
	m_pSqlCmd->Commit();
	return 0;
}

int CMoniSvr::BpcReport(CBF_Xml *xml)
{
	int nRet;
	CXmlAttribute *tmpattr=NULL;
	CSqlData sqldata;
	std::string sName;
	int nBpuNum=0;
	int nRealNum=0;
	int nHtFlag=0;
	int nDpMode;
	int nAtStart;
	std::string sPrgName;
	std::string sPrgPath;
	std::string sRunPath;
	std::string sPara;
	int nStatus=0;

	CXmlNode *tmpnode = xml->GetNodeByPath("Monitor/BPU��Ϣ/BPULIST",false);
	if (tmpnode != NULL)
	{
		tmpnode = (CXmlNode *)tmpnode->GetFirstChild();
		while (tmpnode != NULL)
		{
			tmpattr = tmpnode->GetAttribute("����",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sName = tmpattr->GetAttrValue(false);
			tmpattr = tmpnode->GetAttribute("��Ŀ",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nBpuNum = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("ʵ����",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nRealNum = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("����Ӧ��",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nHtFlag = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("����ģʽ",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nDpMode = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("�Զ�����",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			nAtStart = atoi(tmpattr->GetAttrValue(false).c_str());
			tmpattr = tmpnode->GetAttribute("������",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sPrgName = tmpattr->GetAttrValue(false);
			tmpattr = tmpnode->GetAttribute("����Ŀ¼",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sPrgPath = tmpattr->GetAttrValue(false);
			tmpattr = tmpnode->GetAttribute("����Ŀ¼",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sRunPath = tmpattr->GetAttrValue(false);
			tmpattr = tmpnode->GetAttribute("����",false);
			if (tmpattr == NULL)
			{
				tmpnode = (CXmlNode *)tmpnode->getNextSibling();
				continue;
			}
			sPara = tmpattr->GetAttrValue(false);
			if (nRealNum < nBpuNum  )
			{
				nStatus = 1;
			}
			else
			{
				nStatus = 0;
			}
			sprintf(m_sSql,"select count(*) from PFM_BPULIST where public_id=:1 and private_id=:2 and bpu_name=:3");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
			m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
			m_pSqlCmd->Param(3).setAsString() = sName.c_str();
			sqldata = m_pSqlCmd->LoadResultSql();
			nRet = sqldata.GetAtI(0,0);
			if (nRet !=1 ) //û�������
			{
				sprintf(m_sSql,"insert into  PFM_BPULIST (public_id,private_id,bpu_name,bpu_num,real_num,htflag,dpmode,autostart,prgname,prgpath,runpath,para,lasttime,bpu_status) values(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14)");
				m_pSqlCmd->setCommandText(m_sSql);
				m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
				m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
				m_pSqlCmd->Param(3).setAsString() = sName.c_str();
				m_pSqlCmd->Param(4).setAsLong() = nBpuNum;
				m_pSqlCmd->Param(5).setAsLong() = nRealNum;
				m_pSqlCmd->Param(6).setAsLong() = nHtFlag;
				m_pSqlCmd->Param(7).setAsLong() = nDpMode;
				m_pSqlCmd->Param(8).setAsLong() = nAtStart;
				m_pSqlCmd->Param(9).setAsString() = sPrgName.c_str();
				m_pSqlCmd->Param(10).setAsString() = sPrgPath.c_str();
				m_pSqlCmd->Param(11).setAsString() = sRunPath.c_str();
				m_pSqlCmd->Param(12).setAsString() = sPara.c_str();
				m_pSqlCmd->Param(13).setAsLong() = time(NULL);
				m_pSqlCmd->Param(14).setAsLong() = nStatus;
				if (!m_pSqlCmd->Execute())
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_BPULIST����� %s",m_pSqlCmd->GetCmdErrInfo());
					m_pSqlCmd->Rollback();
					return -1;
				}
			}
			else //����
			{
				sprintf(m_sSql,"update PFM_BPULIST set bpu_num=:1,real_num=:2,htflag=:3,dpmode=:4,autostart=:5,prgname=:6,prgpath=:7,runpath=:8,para=:9,lasttime=:10,bpu_status=:14 where public_id=:11 and private_id=:12 and bpu_name=:13");
				m_pSqlCmd->setCommandText(m_sSql);
				m_pSqlCmd->Param(1).setAsLong() = nBpuNum;
				m_pSqlCmd->Param(2).setAsLong() = nRealNum;
				m_pSqlCmd->Param(3).setAsLong() = nHtFlag;
				m_pSqlCmd->Param(4).setAsLong() = nDpMode;
				m_pSqlCmd->Param(5).setAsLong() = nAtStart;
				m_pSqlCmd->Param(6).setAsString() = sPrgName.c_str();
				m_pSqlCmd->Param(7).setAsString() = sPrgPath.c_str();
				m_pSqlCmd->Param(8).setAsString() = sRunPath.c_str();
				m_pSqlCmd->Param(9).setAsString() = sPara.c_str();
				m_pSqlCmd->Param(10).setAsLong() = time(NULL);
				m_pSqlCmd->Param(11).setAsLong() = m_nNodeId;
				m_pSqlCmd->Param(12).setAsLong() = m_nNodePrivateId;
				m_pSqlCmd->Param(13).setAsString() = sName.c_str();
				m_pSqlCmd->Param(14).setAsLong() = nStatus;
				if (!m_pSqlCmd->Execute())
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_BPULIST����� %s",m_pSqlCmd->GetCmdErrInfo());
					m_pSqlCmd->Rollback();
					return -1;
				}
			}
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
	}
	
	m_pSqlCmd->Commit();
	return 0;
}

int CMoniSvr::Bpc2Report(CBF_Xml *xml)
{
	int nRet;
	CXmlAttribute *tmpattr=NULL;
	CSqlData sqldata;
	std::string sRptName;
	std::string sRptTxt;
	int nStatus=0;
	std::string sxmlbuf;
	
	bool isCgate=true;
	int  nConnectNum=0;
	if (xml->GetNodeAttribute("/Monitor/���ӳ�",false,"�ͻ�������",false,nConnectNum) == NULL )
	{
		isCgate = false;
	}
	//��cgate���أ���������д�����ݿ��
	if (isCgate)
	{
		sRptName = "CGATE";
		m_pDate.Update();
		sprintf(m_sSql,"select count(*) from PFM_CGATE_INFO where public_id=:1 and private_id=:2");
		m_pSqlCmd->setCommandText(m_sSql);
		m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
		m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
		sqldata = m_pSqlCmd->LoadResultSql();
		nRet = sqldata.GetAtI(0,0);
		if (nRet !=1 ) //û�������
		{
			sprintf(m_sSql,"insert into  PFM_CGATE_INFO (public_id,private_id,report_name,report_date,report_time,connect_num) values(:1,:2,:3,:4,:5,:6)");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
			m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
			m_pSqlCmd->Param(3).setAsString() = sRptName.c_str();
			m_pSqlCmd->Param(4).setAsString() = m_pDate.ToStringD8().c_str();
			m_pSqlCmd->Param(5).setAsLong() = atoi(m_pDate.ToStringT6().c_str());;
			m_pSqlCmd->Param(6).setAsLong() = nConnectNum;
			if (!m_pSqlCmd->Execute())
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_CGATE_INFO����� %s",m_pSqlCmd->GetCmdErrInfo());
				m_pSqlCmd->Rollback();
			}
			else
			{
				m_pSqlCmd->Commit();
			}
		}
		else
		{	
			sprintf(m_sSql,"update PFM_CGATE_INFO set report_date=:1,report_time=:2,connect_num=:3 where public_id=:4 and private_id=:5");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsString() = m_pDate.ToStringD8().c_str();
			m_pSqlCmd->Param(2).setAsLong() =  atoi(m_pDate.ToStringT6().c_str());
			m_pSqlCmd->Param(3).setAsLong() = nConnectNum;
			m_pSqlCmd->Param(4).setAsLong() = m_nNodeId;
			m_pSqlCmd->Param(5).setAsLong() = m_nNodePrivateId;
			if (!m_pSqlCmd->Execute())
			{
				m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����PFM_CGATE_INFO����� %s",m_pSqlCmd->GetCmdErrInfo());
				m_pSqlCmd->Rollback();
			}
			else
			{
				m_pSqlCmd->Commit();
			}
		}
		
	}


	CXmlNode *tmpnode = xml->GetNodeByPath("Monitor/��չ��Ϣ",false);
	if (tmpnode != NULL)
	{
		tmpnode = (CXmlNode *)tmpnode->GetFirstChild();
		while (tmpnode != NULL)
		{
			sRptName = tmpnode->GetNodeName(false);
			sRptTxt =  tmpnode->GetNodeValue(true);
			if (sRptTxt.length()<1)
			{
				sRptTxt = "NULL";
//				xml->ToString(sxmlbuf);
//				m_pLog->LogBin(LOG_ERROR_GENERAL,__FILE__,__LINE__,"������չ��ϢΪ��",(char *)sxmlbuf.c_str(),sxmlbuf.length());
			}
			tmpattr = tmpnode->GetAttribute("status");
			if (tmpattr != NULL)
			{
				nStatus = atoi(tmpattr->GetAttrValue(false).c_str());
			}
			else
			{
				nStatus = 0;
			}
			m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��BPC���� [%d %d ] status[%d] info[%s]",\
				m_nNodeId,m_nNodePrivateId,nStatus,sRptTxt.c_str());
			//д���
			sprintf(m_sSql,"select count(*) from PFM_BPC2_REPORT where public_id=:1 and private_id=:2 and report_name=:3");
			m_pSqlCmd->setCommandText(m_sSql);
			m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
			m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
			m_pSqlCmd->Param(3).setAsString() = sRptName.c_str();
			sqldata = m_pSqlCmd->LoadResultSql();
			nRet = sqldata.GetAtI(0,0);
			if (nRet !=1 ) //û�������
			{
				sprintf(m_sSql,"insert into  PFM_BPC2_REPORT (public_id,private_id,report_name,report_status,report_txt,lasttime) values(:1,:2,:3,:4,:5,:6)");
				m_pSqlCmd->setCommandText(m_sSql);
				m_pSqlCmd->Param(1).setAsLong() = m_nNodeId;
				m_pSqlCmd->Param(2).setAsLong() = m_nNodePrivateId;
				m_pSqlCmd->Param(3).setAsString() = sRptName.c_str();
				m_pSqlCmd->Param(4).setAsLong() = nStatus;
				m_pSqlCmd->Param(5).setAsString() = sRptTxt.c_str();
				m_pSqlCmd->Param(6).setAsLong() = time(NULL);
				if (!m_pSqlCmd->Execute())
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_BPC2_REPORT����� %s",m_pSqlCmd->GetCmdErrInfo());
					m_pSqlCmd->Rollback();
					return -1;
				}
			}
			else //����
			{
				sprintf(m_sSql,"update PFM_BPC2_REPORT set report_status=:1,report_txt=:2,lasttime=:3 where public_id=:4 and private_id=:5 and report_name=:6");
				m_pSqlCmd->setCommandText(m_sSql);
				m_pSqlCmd->Param(1).setAsLong() = nStatus;
				m_pSqlCmd->Param(2).setAsString() = sRptTxt.c_str();
				m_pSqlCmd->Param(3).setAsLong() = time(NULL);
				m_pSqlCmd->Param(4).setAsLong() = m_nNodeId;
				m_pSqlCmd->Param(5).setAsLong() = m_nNodePrivateId;
				m_pSqlCmd->Param(6).setAsString() = sRptName.c_str();
				
				if (!m_pSqlCmd->Execute())
				{
					m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"д��PFM_BPC2_REPORT����� %s",m_pSqlCmd->GetCmdErrInfo());
					m_pSqlCmd->Rollback();
					return -1;
				}
			}
			tmpnode = (CXmlNode *)tmpnode->getNextSibling();
		}
	}
	m_pSqlCmd->Commit();
	
	return 0;
}

bool CMoniSvr::GetSerial(char *logserial)
{
	char tmpsql[200];
	int  sno;
	if (m_pSqlCmd->m_dbconn.m_nDbType == 0) //oracle
	{
		sprintf(tmpsql,"select BFM_SEQ_MONITOR.nextval from dual");
		//m_pSqlCmd->setCommandText(m_sSql);
		if (!m_pSqlCmd->Execute(tmpsql))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]���� [%s]",\
				tmpsql,m_pSqlCmd->GetCmdErrInfo());
			return false;
		}
		if (!m_pSqlCmd->isResultSet())
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]�޽����",tmpsql);
			return false;
		}
		if (!m_pSqlCmd->FetchNext())
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]�޽����",tmpsql);
			return false;
		}
		sno = m_pSqlCmd->Field(0).asLong();
	}
	else if (m_pSqlCmd->m_dbconn.m_nDbType == 4) //db2
	{
		if (!GetSerialDb2(sno))
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"��DB2����ȡֵ����");
			return false;
		}
	}
	else
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"�ݲ�֧��ORACLE��DB2֮������ݿ⣬����ϵ��˾");
		return false;
	}
	
	m_pDate.Update();
	sprintf(logserial,"%s%09d",m_pDate.ToStringD8().c_str(),sno);
	char delserialno[20];
	if (sno >m_nSaveLogNum)
	{
		sprintf(delserialno,"%s%09d",m_pDate.ToStringD8().c_str(),sno-m_nSaveLogNum-1);
		sprintf(tmpsql,"delete from BFM_LOG where serial_no<:1");
		m_pSqlCmd->setCommandText(tmpsql);
		m_pSqlCmd->Param(1).setAsString() = delserialno;
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]����",tmpsql);
			m_pSqlCmd->Rollback();
		}
		else
		{
			m_pSqlCmd->Commit();
		}
	}
	else
	{
		m_pDate-=1;//ɾ��1��ǰ������
		sprintf(delserialno,"%s%09d",m_pDate.ToStringD8().c_str(),999999999-m_nSaveLogNum-1+sno);
		sprintf(tmpsql,"delete from BFM_LOG where serial_no<:1");
		m_pSqlCmd->setCommandText(tmpsql);
		m_pSqlCmd->Param(1).setAsString() = delserialno;
		if (!m_pSqlCmd->Execute())
		{
			m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]����",tmpsql);
			m_pSqlCmd->Rollback();
		}
		else
		{
			m_pSqlCmd->Commit();
		}
	}
	return true;
	
}

int CMoniSvr::UpdateByLastTime()
{
	//����Ӧ�ù�����Ӧ�ñ�
	int lasttime = time(NULL)-m_nStatusTimeOut;
	//���±��г�ʱ��״̬Ϊ������
	sprintf(m_sSql,"update BFM_APP_NODE set node_status=:1 where last_time<:2 and node_status=:3");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsLong() = 2;//��ʱ�쳣
	m_pSqlCmd->Param(2).setAsLong() = lasttime;
	m_pSqlCmd->Param(3).setAsLong() = 0;//ֻ����������
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����BFM_APP_NODE����� %s",m_pSqlCmd->GetCmdErrInfo());
		m_pSqlCmd->Rollback();
		return -1;
	}
	sprintf(m_sSql,"update PFM_APP_CONNECT set conn_status=:1 where lasttime<:2 and conn_status=:3");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsLong() = 2;//��ʱ�쳣
	m_pSqlCmd->Param(2).setAsLong() = lasttime;
	m_pSqlCmd->Param(3).setAsLong() = 0;//ֻ����������
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����PFM_APP_CONNECT����� %s",m_pSqlCmd->GetCmdErrInfo());
		m_pSqlCmd->Rollback();
		return -1;
	}
	//���±��г�ʱ��״̬Ϊ������
	sprintf(m_sSql,"update PFM_DREB_CONNLIST set conn_status=:1 where  lasttime<:2 and conn_status=:3");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsLong() = 2;//��ʱ�쳣
	m_pSqlCmd->Param(2).setAsLong() = lasttime;
	m_pSqlCmd->Param(3).setAsLong() = 0;//ֻ����������
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����PFM_DREB_CONNLIST����� %s",m_pSqlCmd->GetCmdErrInfo());
		m_pSqlCmd->Rollback();
		return -1;
	}
	
	//���±��г�ʱ��״̬Ϊ������
	sprintf(m_sSql,"update PFM_BPULIST set bpu_status=:1 where  lasttime<:2 and bpu_status=:3");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsLong() = 2;//״̬Ϊ��ʱ�쳣
	m_pSqlCmd->Param(2).setAsLong() = lasttime;
	m_pSqlCmd->Param(3).setAsLong() = 0;//ֻ����������
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����PFM_BPULIST����� %s",m_pSqlCmd->GetCmdErrInfo());
		m_pSqlCmd->Rollback();
		return -1;
	}
	//���±��г�ʱ��״̬Ϊ������
	sprintf(m_sSql,"update PFM_BPC2_REPORT set report_status=:1 where lasttime<:2 and report_status=:3");
	m_pSqlCmd->setCommandText(m_sSql);
	m_pSqlCmd->Param(1).setAsLong() = 2;//״̬Ϊ��ʱ�쳣
	m_pSqlCmd->Param(2).setAsLong() = lasttime;
	m_pSqlCmd->Param(3).setAsLong() = 0;//ֻ����������
	if (!m_pSqlCmd->Execute())
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"����PFM_BPC2_REPORT����� %s",m_pSqlCmd->GetCmdErrInfo());
		m_pSqlCmd->Rollback();
		return -1;
	}
	m_pSqlCmd->Commit();
}

int CMoniSvr::AddPcNode(PBPCCOMMSTRU data)
{
	return 0;
}

bool CMoniSvr::GetSerialDb2(int &no)
{
	char tmpsql[200];
	sprintf(tmpsql,"select (nextval for BFM_SEQ_MONITOR) from sysibm.sysdummy1");
	if (!m_pSqlCmd->Execute(tmpsql))
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]���� [%s]",\
			tmpsql,m_pSqlCmd->GetCmdErrInfo());
		return false;
	}
	if (!m_pSqlCmd->isResultSet())
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]�޽����",tmpsql);
		return false;
	}
	if (!m_pSqlCmd->FetchNext())
	{
		m_pLog->LogMp(LOG_ERROR_GENERAL,__FILE__,__LINE__,"ִ��[%s]�޽����",tmpsql);
		return false;
	}
	no = m_pSqlCmd->Field(0).asLong();
	return true;
}

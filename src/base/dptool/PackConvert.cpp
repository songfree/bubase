// PackConvert.cpp: implementation of the CPackConvert class.
//
//////////////////////////////////////////////////////////////////////

#include "PackConvert.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CPackConvert::CPackConvert()
{	
	memset(m_errMsg,0,sizeof(m_errMsg));
	memset(m_buf,0,sizeof(m_buf));
	m_buflen = 0;
	strcpy(m_logfile,"conv");
	m_isInit = false;
	m_resConv =0;
	m_publog.SetLogPara(LOG_DEBUG,"","packconv.log");
}

CPackConvert::CPackConvert(char *file)
{
	memset(m_errMsg,0,sizeof(m_errMsg));
	memset(m_buf,0,sizeof(m_buf));
	m_buflen = 0;
	strcpy(m_logfile,"conv");
	Init(file);
	m_isInit = true;
}
CPackConvert::~CPackConvert()
{

}

int CPackConvert::Init(char *file)
{
	//�������ļ�
	if (!m_isInit)
	{
		if (m_interfaceConf.fromFile(file)<0)
		{
			strcpy(m_errMsg,m_interfaceConf.m_errMsg);
			return -1;
		}
	}
	return 0;
}

//ת���ⲿ����Ϊ�ڲ�xmlpack��ʽ
bool CPackConvert::Conv2XmlReq(int &txcode, void *buf, int buflen, CXmlPack *xmlpack)
{
	int ret;
	if (txcode<1)
	{
		ret = m_interfaceConf.getTxCode((char *)buf,buflen,txcode);
		if (ret<0)
		{
			m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
			return -1;
		}
	}
	S_FUNC_CONF_INFO *funcinfo = m_interfaceConf.getFuncInfo(txcode);
	if (funcinfo==NULL)
	{
		sprintf(m_errMsg,"������[%d]�ڽӿ�������û�ж���",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 0 ) //xml��
	{
		xmlpack->fromBuf((char *)buf);
		return true;
	}
	if (funcinfo->funcAttr.conv_type == 2 ) //�ָ���
	{
		std::string tmpfield;
		xmlpack->newPackage();
		CBF_Slist publog;
		char separator[3];
		bzero(separator,sizeof(separator));
		if (CBF_Tools::IsNumber(funcinfo->funcAttr.separator))
		{
			separator[0] = atoi(funcinfo->funcAttr.separator);
		}
		else
		{
			separator[0] = funcinfo->funcAttr.separator[0];
		}
		publog.SetSeparateString(separator);
		if (publog.FillSepString((char *)buf)<0)
		{
			sprintf(m_errMsg,"���ݻ��岻Ϊ�ָ�������");
			return false;
		}
		//�ָ���
		for (int i=0;i<funcinfo->funcReq.funcReq.size();i++)
		{
			std::string name = (funcinfo->funcReq.funcReq[i]).name;//���� xml�ֶ�
			std::string value = (funcinfo->funcReq.funcReq[i]).value;
			if (strncmp(name.c_str(),"include",7)==0) //include�εĴ���
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"��ͷ[%s]�ڽӿ�������û�ж���",value.c_str());
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return false;
				}
				for (int j=0;j<headinfo->headinfo.size();j++)
				{
					name = (headinfo->headinfo[j]).name;//���� xml�ֶ�
					value = (headinfo->headinfo[j]).value;
					if (!getValueFromBufL(&publog,&headinfo->headinfo[j],tmpfield))
					{
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
						return false;
					}
					if (strncmp(name.c_str(),"������",6)==0 || strncmp(name.c_str(),"������",6)==0)
					{
						xmlpack->setHeadNameValue(name,tmpfield);
					}
					else
					{
						xmlpack->setPackNameValue(name,tmpfield);
					}
				}
			}
			else  //�ӿ��ﶨ��
			{
				if (!getValueFromBufL(&publog,&funcinfo->funcReq.funcReq[i],tmpfield))
				{
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
					return false;
				}
				if (strncmp(name.c_str(),"������",6)==0 || strncmp(name.c_str(),"������",6)==0)
				{
					xmlpack->setHeadNameValue(name,tmpfield);
				}
				else
				{
					xmlpack->setPackNameValue(name,tmpfield);
				}
			}
		}
		return true;
	}
	if (funcinfo->funcAttr.conv_type == 3 ) //������
	{
		std::string tmpfield;
		xmlpack->newPackage();
		//buf����Ϊ�����޷ָ�������
		//�����޷ָ�
		for (int i=0;i<funcinfo->funcReq.funcReq.size();i++)
		{
			std::string name = (funcinfo->funcReq.funcReq[i]).name;//���� xml�ֶ�
			std::string value = (funcinfo->funcReq.funcReq[i]).value;
			
			if (strncmp(name.c_str(),"include",7)==0) //include�εĴ���
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"��ͷ[%s]�ڽӿ�������û�ж���",value.c_str());
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return false;
				}
				for (int j=0;j<headinfo->headinfo.size();j++)
				{
					name = (headinfo->headinfo[j]).name;//���� xml�ֶ�
					value = (headinfo->headinfo[j]).value;
					if (!getValueFromBufS((char *)buf,buflen,&headinfo->headinfo[j],tmpfield))
					{
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
						return false;
					}
					if (strncmp(name.c_str(),"������",6)==0 || strncmp(name.c_str(),"������",6)==0)
					{
						xmlpack->setHeadNameValue(name,tmpfield);
					}
					else
					{
						xmlpack->setPackNameValue(name,tmpfield);
					}
					
				}
			}
			else  //�ӿ��ﶨ��
			{
				if (!getValueFromBufS((char *)buf,buflen,&funcinfo->funcReq.funcReq[i],tmpfield))
				{
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
					return false;
				}
				if (strncmp(name.c_str(),"������",6)==0 || strncmp(name.c_str(),"������",6)==0)
				{
					xmlpack->setHeadNameValue(name,tmpfield);
				}
				else
				{
					xmlpack->setPackNameValue(name,tmpfield);
				}
			}
		}
		return true;
	}
	return false;
}
//ת���ⲿ��Ӧ��Ϊ�ڲ�xmlpack��ʽ
bool CPackConvert::Conv2XmlAns(int txcode, void *buf, int buflen, CXmlPack *xmlpack)
{
	S_FUNC_CONF_INFO *funcinfo = m_interfaceConf.getFuncInfo(txcode);
	if (funcinfo==NULL)
	{
		sprintf(m_errMsg,"������[%d]�ڽӿ�������û�ж���",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 0 ) //xml��
	{
		CXmlPack tmpxml;
		tmpxml.fromBuf((char *)buf);
		xmlpack->setHeadNameValue("������",tmpxml.getHeadValue("������"));
		xmlpack->setHeadNameValue("��¼��",tmpxml.getHeadValue("��¼��"));
		//��tmpxml���pack���Ƶ�xmlpack,�ظ��Ĳ�����
		xmlpack->modiPack(&tmpxml,false);
		return true;
	}
	
	if (funcinfo->funcAttr.conv_type == 2 || funcinfo->funcAttr.conv_type == 3 ) //�ָ����򶨳���
	{
		std::string name ; //xml����
		std::string value ;//cpack�ֶ�
		std::string tmpfield;//xml���ƶ�Ӧ��ֵ
		CBF_Slist publog;
		char tmpchar[20];
		if (funcinfo->funcRes.size()<=1) //ֻ��һ��Ӧ��
		{
			if (funcinfo->funcRes[0].conv_type ==2 )
			{
				char separator[3];
				bzero(separator,sizeof(separator));
				if (CBF_Tools::IsNumber(funcinfo->funcRes[0].separator))
				{
					separator[0] = atoi(funcinfo->funcRes[0].separator);
				}
				else
				{
					separator[0] = funcinfo->funcRes[0].separator[0];
				}
				publog.SetSeparateString(separator);
				publog.FillSepString((char *)buf);
			}
			for (int i=0;i<funcinfo->funcRes[0].funcRes.size();i++)
			{
				name = funcinfo->funcRes[0].funcRes[i].name;//xml����
				value = funcinfo->funcRes[0].funcRes[i].value;//
				//��ͷ
				if (strncmp(name.c_str(),"include",7)==0)
				{
					S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
					if (headinfo==NULL)
					{
						sprintf(m_errMsg,"������[%d]�ڽӿ�������û�ж���",txcode);
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
						return false;
					}
					for (int j=0;j<headinfo->headinfo.size();j++)
					{
						name = (headinfo->headinfo[j]).name;//���� xml�ֶ�
						value = (headinfo->headinfo[j]).value;
						if (funcinfo->funcRes[0].conv_type == 2 )
						{
							if (!getValueFromBufL(&publog,&headinfo->headinfo[j],tmpfield))
							{
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
								return false;
							}
						}
						else
						{
							if (!getValueFromBufS((char *)buf,buflen,&headinfo->headinfo[j],tmpfield))
							{
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
								return false;
							}
						}
						//����ֵ��xmlpack
						if (strncmp(name.c_str(),"������",6)==0)
						{
							xmlpack->setHeadNameValue(name,tmpfield);
						}
						else
						{
							xmlpack->setPackNameValue(name,tmpfield,headinfo->headinfo[j].isupdate);
						}
					}
				} 
				else
				{
					if (funcinfo->funcRes[0].conv_type ==2 )
					{
						if (!getValueFromBufL(&publog,&funcinfo->funcRes[0].funcRes[i],tmpfield))
						{
							m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
							return false;
						}
					}
					else
					{
						if (!getValueFromBufS((char *)buf,buflen,&funcinfo->funcRes[0].funcRes[i],tmpfield))
						{
							m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
							return false;
						}
					}
					//����ֵ��xmlpack
					if (strncmp(name.c_str(),"������",6)==0)
					{
						xmlpack->setHeadNameValue(name,tmpfield);
					}
					else
					{
						xmlpack->setPackNameValue(name,tmpfield,funcinfo->funcRes[0].funcRes[i].isupdate);
					}
					
				}
			
			} //end for
			return true;

		}
		else   //���ݷ�����Ķ��巵��Ӧ��
		{
			for (int i=0;i<funcinfo->funcRes.size();i++)
			{
				int ret = xmlpack->getHeadValueI("������");
				if (ret == funcinfo->funcRes[i].retCode)
				{
					for (int ij=0;ij<funcinfo->funcRes[i].funcRes.size();ij++)
					{
						name = funcinfo->funcRes[i].funcRes[ij].name;//xml����
						value = funcinfo->funcRes[i].funcRes[ij].value;//
						//��ͷ
						if (strncmp(name.c_str(),"include",7)==0)
						{
							S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
							if (headinfo==NULL)
							{
								sprintf(m_errMsg,"������[%d]�ڽӿ�������û�ж���",txcode);
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
								return false;
							}
							for (int j=0;j<headinfo->headinfo.size();j++)
							{
								name = (headinfo->headinfo[j]).name;//���� xml�ֶ�
								value = (headinfo->headinfo[j]).value;
								if (funcinfo->funcRes[i].conv_type ==2 )
								{
									if (!getValueFromBufL(&publog,&headinfo->headinfo[j],tmpfield))
									{
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
										return false;
									}
								}
								else
								{
									if (!getValueFromBufS((char *)buf,buflen,&headinfo->headinfo[j],tmpfield))
									{
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
										return false;
									}
								}
								//����ֵ��xmlpack
								if (strncmp(name.c_str(),"������",6)==0)
								{
									xmlpack->setHeadNameValue(name,tmpfield);
								}
								else
								{
									xmlpack->setPackNameValue(name,tmpfield,headinfo->headinfo[j].isupdate);
								}
								
								
							}
						} 
						else
						{
							if (funcinfo->funcRes[i].conv_type ==2 )
							{
								if (!getValueFromBufL(&publog,&funcinfo->funcRes[i].funcRes[ij],tmpfield))
								{
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
									return false;
								}
							}
							else
							{
								if (!getValueFromBufS((char *)buf,buflen,&funcinfo->funcRes[ij].funcRes[i],tmpfield))
								{
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
									return false;
								}
							}
							//����ֵ��xmlpack
							if (strncmp(name.c_str(),"������",6)==0)
							{
								xmlpack->setHeadNameValue(name,tmpfield);
							}
							else
							{
								xmlpack->setPackNameValue(name,tmpfield,funcinfo->funcRes[i].funcRes[ij].isupdate);
							}
						}
					}
					
				}
				return true;
			}
		}
		return true;
	}
	
	return false;
}
//ת���ڲ�xmlpack��ʽΪ�ⲿ����
bool CPackConvert::Xml2Req(int txcode, CXmlPack *xmlpack, void *buf, int &buflen)
{
	S_FUNC_CONF_INFO *funcinfo = m_interfaceConf.getFuncInfo(txcode);
	if (funcinfo==NULL)
	{
		sprintf(m_errMsg,"������[%d]�ڽӿ�������û�ж���",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 0 ) //xml��
	{
		xmlpack->toBuf((char *)buf,buflen);//�����buf�����ж���������
		return true;
	}
	
	if (funcinfo->funcAttr.conv_type == 2 ) //�ָ���
	{
		CBF_Slist publog;
		char tt[50];
		char separator[3];
		std::string name;
		std::string value;
		std::string tmpfield;
		int i;
		bzero(separator,sizeof(separator));
		if (CBF_Tools::IsNumber(funcinfo->funcAttr.separator))
		{
			separator[0] = atoi(funcinfo->funcAttr.separator);
		}
		else
		{
			separator[0] = funcinfo->funcAttr.separator[0];
		}
		publog.SetSeparateString(separator);
// 		sprintf(tt,"131313%c",separator);
// 		publog.FillSepString(tt);
 		publog.Clear();
		//�ȸ����ֶ��������ɿյ��ִ�
		for (i=0;i<funcinfo->funcReq.funcReq.size();i++)
		{
			name = (funcinfo->funcReq.funcReq[i]).name;//xml����
			value = (funcinfo->funcReq.funcReq[i]).value;
			if (strncmp(name.c_str(),"include",7)==0) //include�εĴ���
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"������[%d]��ͷ[%s]�ڽӿ�������û�ж���",txcode,value.c_str());
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return false;
				}
				for (int j=0;j<headinfo->headinfo.size();j++)
				{
					if (j<headinfo->headinfo.size()-1)
					{
						publog.Add(" ");
					}
				}
			}
			else
			{
				publog.Add(" ");
			}
		}
		for (i=0;i<funcinfo->funcReq.funcReq.size();i++)
		{
			name = (funcinfo->funcReq.funcReq[i]).name;//xml����
			value = (funcinfo->funcReq.funcReq[i]).value;
			
			if (strncmp(name.c_str(),"include",7)==0) //include�εĴ���
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"������[%d]��ͷ[%s]�ڽӿ�������û�ж���",txcode,value.c_str());
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return false;
				}
				
				for (int j=0;j<headinfo->headinfo.size();j++)
				{
					if (!setValueToBufL(xmlpack,&headinfo->headinfo[j],&publog))
					{
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
						return false;
					}
				}
			}
			else
			{
				if (!setValueToBufL(xmlpack,&funcinfo->funcReq.funcReq[i],&publog))
				{
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
					return false;
				}

			}
		}
		strcpy((char *)buf,publog.ToString().c_str());
//		m_publog.errlog(m_logfile,__FILE__,__LINE__,txcode,(char *)buf);
		return true;
	}
	if (funcinfo->funcAttr.conv_type == 3 ) //������
	{
		for (int i=0;i<funcinfo->funcReq.funcReq.size();i++)
		{
			std::string name = (funcinfo->funcReq.funcReq[i]).name;//xml����
			std::string value = (funcinfo->funcReq.funcReq[i]).value;
			std::string tmpfield;
			if (strncmp(name.c_str(),"include",7)==0) //include�εĴ���
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"������[%d]��ͷ[%s]�ڽӿ�������û�ж���",txcode,value.c_str());
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return false;
				}
				
				for (int j=0;j<headinfo->headinfo.size();j++)
				{
					if (!setValueToBufS(xmlpack,&headinfo->headinfo[j],(char *)buf))
					{
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
						return false;
					}
					buflen = headinfo->headinfo[j].fieldoffset + headinfo->headinfo[j].fieldlength;
				}
			}
			else
			{
				if (!setValueToBufS(xmlpack,&funcinfo->funcReq.funcReq[i],(char *)buf))
				{
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
					return false;
				}
				buflen = funcinfo->funcReq.funcReq[i].fieldoffset + funcinfo->funcReq.funcReq[i].fieldlength;
			}
		}
		((char *)buf)[buflen]=0;
		return true;
	}
	return false;
}
//xmlӦ��ת�����ⲿ��
bool CPackConvert::Xml2Ans(int txcode, CXmlPack *xmlpack, void *buf, int &buflen)
{
	S_FUNC_CONF_INFO *funcinfo = m_interfaceConf.getFuncInfo(txcode);
	if (funcinfo==NULL)
	{
		sprintf(m_errMsg,"������[%d]�ڽӿ�������û�ж���",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 0 ) //xml��
	{
		xmlpack->toBuf((char *)buf,buflen);//�����buf�����ж���������
		return true;
	}
	

	if (funcinfo->funcAttr.conv_type == 2 || funcinfo->funcAttr.conv_type == 3 ) //�ָ���������
	{
		std::string name ; //xml����
		std::string value ;//cpack�ֶ�
		std::string tmpfield;//xml���ƶ�Ӧ��ֵ
		CBF_Slist tmppublog;
		char tmpchar[90];
		if (funcinfo->funcRes.size()<=1) //ֻ��һ��Ӧ��
		{
			m_resConv = funcinfo->funcRes[0].conv_type;
			if (2 == m_resConv) //�ָ����ָ�
			{
				char separator[3];
				bzero(separator,sizeof(separator));
				if (CBF_Tools::IsNumber(funcinfo->funcRes[0].separator))
				{
					separator[0] = atoi(funcinfo->funcRes[0].separator);
				}
				else
				{
					separator[0] = funcinfo->funcRes[0].separator[0];
				}
				tmppublog.SetSeparateString(separator);
// 				sprintf(tmpchar,"113%c",separator);
// 				tmppublog.FillStrings(tmpchar,separator);
 				tmppublog.Clear();
				//�������ֶ�ֵ
				for (int iii=0;iii<funcinfo->funcRes[0].funcRes.size();iii++)
				{
					tmppublog.Add(" ");
				}
			}
			for (int i=0;i<funcinfo->funcRes[0].funcRes.size();i++)
			{
				name = funcinfo->funcRes[0].funcRes[i].name;//xml����
				value = funcinfo->funcRes[0].funcRes[i].value;//
				//��ͷ
				if (strncmp(name.c_str(),"include",7)==0)
				{
					S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
					if (headinfo==NULL)
					{
						sprintf(m_errMsg,"������[%d]��ͷ[%s]�ڽӿ�������û�ж���",txcode,value.c_str());
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
						return false;
					}
					for (int j=0;j<headinfo->headinfo.size();j++)
					{
						if (2==m_resConv)
						{
							if (j<headinfo->headinfo.size()-1)
							{
								tmppublog.Add(" ");
							}
							if (!setValueToBufL(xmlpack,&headinfo->headinfo[j],&tmppublog))
							{
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
								return false;
							}
								
						}
						else
						{
							if (strncmp(headinfo->headinfo[j].name.c_str(),"������",6)==0)
							{
								char tmpcharr[20];
								char tt[20];
								sprintf(tt,"%s0%d%s","%",headinfo->headinfo[j].fieldlength,"d");
								sprintf(tmpcharr,tt,txcode);
								memcpy((char *)buf+headinfo->headinfo[j].fieldoffset,tmpcharr,headinfo->headinfo[j].fieldlength);
							}
							else
							{
								if (!setValueToBufS(xmlpack,&headinfo->headinfo[j],(char *)buf))
								{
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
									return false;
								}
							}
							
							buflen = headinfo->headinfo[j].fieldoffset + headinfo->headinfo[j].fieldlength;
						}
						
					}
				} 
				else
				{
					if (2==m_resConv )
					{
						if (!setValueToBufL(xmlpack,&funcinfo->funcRes[0].funcRes[i],&tmppublog))
						{
							m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
							return false;
						}
					}
					else
					{
						if (strncmp(funcinfo->funcRes[0].funcRes[i].name.c_str(),"������",6)==0)
						{
							char tmpcharr[20];
							char tt[20];
							sprintf(tt,"%s0%d%s","%",funcinfo->funcRes[0].funcRes[i].fieldlength,"d");
							sprintf(tmpcharr,tt,txcode);
							memcpy((char *)buf+funcinfo->funcRes[0].funcRes[i].fieldoffset,tmpcharr,funcinfo->funcRes[0].funcRes[i].fieldlength);
						}
						else
						{
							if (!setValueToBufS(xmlpack,&funcinfo->funcRes[0].funcRes[i],(char *)buf))
							{
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
								return false;
							}
						}
						
						buflen = funcinfo->funcRes[0].funcRes[i].fieldoffset + funcinfo->funcRes[0].funcRes[i].fieldlength;
					}
					
				}
				
			
			} //end for
			if (2==m_resConv )
			{
				//ת��������ݷ���buf��
				strcpy((char *)buf,tmppublog.ToString().c_str());
//				tmppublog.GetBuff();
				tmppublog.Clear();
				buflen = strlen((char *)buf);
				
			}
			else
			{
				((char *)buf)[buflen]=0;
			}
			return true;
		}
		else //���Ӧ��
		{
			for (int i=0;i<funcinfo->funcRes.size();i++)
			{
				int ret = xmlpack->getHeadValueI("������");
				if (ret == funcinfo->funcRes[i].retCode)
				{
					m_resConv = funcinfo->funcRes[i].conv_type;
					if (2==m_resConv )
					{
						char separator[3];
						bzero(separator,sizeof(separator));
						if (CBF_Tools::IsNumber(funcinfo->funcRes[i].separator))
						{
							separator[0] = atoi(funcinfo->funcRes[i].separator);
						}
						else
						{
							separator[0] = funcinfo->funcRes[i].separator[0];
						}
						tmppublog.SetSeparateString(separator);
// 						sprintf(tmpchar,"113%c",separator);
// 						tmppublog.FillStrings(tmpchar,separator);
						tmppublog.Clear();
						//�������ֶ�ֵ
						for (int iii=0;iii<funcinfo->funcRes[i].funcRes.size();iii++)
						{
							tmppublog.Add(" ");
						}
					}
					for (int ij=0;ij<funcinfo->funcRes[i].funcRes.size();ij++)
					{
						name = funcinfo->funcRes[i].funcRes[ij].name;//xml����
						value = funcinfo->funcRes[i].funcRes[ij].value;//
						//��ͷ
						if (strncmp(name.c_str(),"include",7)==0)
						{
							S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
							if (headinfo==NULL)
							{
								sprintf(m_errMsg,"������[%d]��ͷ[%s]�ڽӿ�������û�ж���",txcode,value.c_str());
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
								return false;
							}
							for (int j=0;j<headinfo->headinfo.size();j++)
							{
								if (2==m_resConv )
								{
									if (j<headinfo->headinfo.size()-1)
									{
										tmppublog.Add(" ");
									}
									if (!setValueToBufL(xmlpack,&headinfo->headinfo[j],&tmppublog))
									{
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
										return false;
									}
								}
								else
								{
									if (strncmp(headinfo->headinfo[j].name.c_str(),"������",6)==0)
									{
										char tmpcharr[20];
										char tt[20];
										sprintf(tt,"%s0%d%s","%",headinfo->headinfo[j].fieldlength,"d");
										sprintf(tmpcharr,tt,txcode);
										memcpy((char *)buf+headinfo->headinfo[j].fieldoffset,tmpcharr,headinfo->headinfo[j].fieldlength);
									}
									else
									{
										if (!setValueToBufS(xmlpack,&headinfo->headinfo[j],(char *)buf))
										{
											m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
											return false;
										}
									}
									
									buflen = headinfo->headinfo[j].fieldoffset + headinfo->headinfo[j].fieldlength;
								}
								
								
							}
						} 
						else
						{
							if (2==m_resConv )
							{
								if (!setValueToBufL(xmlpack,&funcinfo->funcRes[i].funcRes[ij],&tmppublog))
								{
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
									return false;
								}
							}
							else
							{
								if (strncmp(funcinfo->funcRes[i].funcRes[ij].name.c_str(),"������",6)==0)
								{
									char tmpcharr[20];
									char tt[20];
									sprintf(tt,"%s0%d%s","%",funcinfo->funcRes[i].funcRes[ij].fieldlength,"d");
									sprintf(tmpcharr,tt,txcode);
									memcpy((char *)buf+funcinfo->funcRes[i].funcRes[ij].fieldoffset,tmpcharr,funcinfo->funcRes[i].funcRes[ij].fieldlength);
								}
								else
								{
									if (!setValueToBufS(xmlpack,&funcinfo->funcRes[i].funcRes[ij],(char *)buf))
									{
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
										return false;
									}
								}
								
								buflen = funcinfo->funcRes[i].funcRes[ij].fieldoffset + funcinfo->funcRes[i].funcRes[ij].fieldlength;
							}
							
						}
					
						
					}
					if (2==m_resConv )
					{
						//ת��������ݷ���buf��
						strcpy((char *)buf,tmppublog.ToString().c_str());
						//tmppublog.GetBuff((char *)buf);
						tmppublog.Clear();	
						buflen = strlen((char *)buf);
					}
					else
					{
						((char *)buf)[buflen]=0;
					}
					return true;

				}
			}
		}
	}
	return false;	
}


std::string CPackConvert::getTransValue(std::string transname,CXmlPack *xmlpack,int fieldtype)
{
	for (int i=0;i<m_interfaceConf.m_trans_info.size();i++)
	{
		if (m_interfaceConf.m_trans_info[i].transName.compare(transname)==0)
		{
			double tmp1 = xmlpack->getPackValueD(m_interfaceConf.m_trans_info[i].transinfo[0].value);
			double tmp2 = xmlpack->getPackValueD(m_interfaceConf.m_trans_info[i].transinfo[2].value);
			if (strncmp(m_interfaceConf.m_trans_info[i].transinfo[2].value.c_str(),"+",1)==0)
			{
				char tmpchar[40];
				if (fieldtype==2) //integer
				{
					sprintf(tmpchar,"%ld",(int)(tmp1+tmp2));
				}
				else
				{
					sprintf(tmpchar,"%lf",tmp1+tmp2);
				}
				
				return std::string(tmpchar);
			}
			else if (strncmp(m_interfaceConf.m_trans_info[i].transinfo[2].value.c_str(),"-",1)==0)
			{
				char tmpchar[40];
				if (fieldtype==2) //integer
				{
					sprintf(tmpchar,"%ld",(int)(tmp1-tmp2));
				}
				else
				{
					sprintf(tmpchar,"%lf",tmp1-tmp2);
				}
				return std::string(tmpchar);
			}
			else if (strncmp(m_interfaceConf.m_trans_info[i].transinfo[2].value.c_str(),"*",1)==0)
			{
				char tmpchar[40];
				if (fieldtype==2) //integer
				{
					sprintf(tmpchar,"%ld",(int)(tmp1*tmp2));
				}
				else
				{
					sprintf(tmpchar,"%lf",tmp1*tmp2);
				}
				
				return std::string(tmpchar);
			}
			else if (strncmp(m_interfaceConf.m_trans_info[i].transinfo[2].value.c_str(),"/",1)==0)
			{
				char tmpchar[40];
				if (fieldtype==2) //integer
				{
					sprintf(tmpchar,"%ld",(int)(tmp1/tmp2));
				}
				else
				{
					sprintf(tmpchar,"%lf",tmp1/tmp2);
				}
				return std::string(tmpchar);
			}
		}
	}
	return "";
}
std::string CPackConvert::getTransValue(std::string transname,char *buf,int flag,int fieldtype)
{
	for (int i=0;i<m_interfaceConf.m_trans_info.size();i++)
	{
		if (m_interfaceConf.m_trans_info[i].transName.compare(transname)==0)
		{
			char tmpfield[800];
			double tmp1;
			double tmp2;
			memset(tmpfield,0,sizeof(tmpfield));
			if (flag==0) //����
			{	
				memcpy(tmpfield,buf+m_interfaceConf.m_trans_info[i].transinfo[0].fieldoffset,m_interfaceConf.m_trans_info[i].transinfo[0].fieldlength);
				tmpfield[m_interfaceConf.m_trans_info[i].transinfo[0].fieldlength]=0;
				tmp1 = atof(tmpfield);
				memset(tmpfield,0,sizeof(tmpfield));
				memcpy(tmpfield,buf+m_interfaceConf.m_trans_info[i].transinfo[2].fieldoffset,m_interfaceConf.m_trans_info[i].transinfo[2].fieldlength);
				tmpfield[m_interfaceConf.m_trans_info[i].transinfo[2].fieldlength]=0;
				tmp2 = atof(tmpfield);
			}
			else //�ָ�
			{
				CBF_Slist publog;
				publog.SetSeparateString("|");
				publog.FillSepString(buf);
				if (m_interfaceConf.m_trans_info[i].transinfo[0].fieldoffset > publog.GetCount())
				{
					sprintf(m_errMsg,"buf���ݺͶ��岻��");
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return "";
				}
				CBF_Tools::StringCopy(tmpfield,799,publog.GetAt(m_interfaceConf.m_trans_info[i].transinfo[0].fieldoffset).c_str());
				tmp1 = atof(tmpfield);
				if (m_interfaceConf.m_trans_info[i].transinfo[2].fieldoffset > publog.GetCount())
				{
					sprintf(m_errMsg,"buf���ݺͶ��岻��");
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return "";
				}
				CBF_Tools::StringCopy(tmpfield,799,publog.GetAt(m_interfaceConf.m_trans_info[i].transinfo[2].fieldoffset).c_str());
				tmp2 = atof(tmpfield);
			}
			if (strncmp(m_interfaceConf.m_trans_info[i].transinfo[2].value.c_str(),"+",1)==0)
			{
				char tmpchar[40];
				if (fieldtype==2) //integer
				{
					sprintf(tmpchar,"%ld",(int)(tmp1+tmp2));
				}
				else
				{
					sprintf(tmpchar,"%lf",tmp1+tmp2);
				}
				return std::string(tmpchar);
			}
			else if (strncmp(m_interfaceConf.m_trans_info[i].transinfo[2].value.c_str(),"-",1)==0)
			{
				char tmpchar[40];
				if (fieldtype==2) //integer
				{
					sprintf(tmpchar,"%ld",(int)(tmp1-tmp2));
				}
				else
				{
					sprintf(tmpchar,"%lf",tmp1-tmp2);
				}
				return std::string(tmpchar);
			}
			else if (strncmp(m_interfaceConf.m_trans_info[i].transinfo[2].value.c_str(),"*",1)==0)
			{
				char tmpchar[40];
				if (fieldtype==2) //integer
				{
					sprintf(tmpchar,"%ld",(int)(tmp1*tmp2));
				}
				else
				{
					sprintf(tmpchar,"%lf",tmp1*tmp2);
				}
				
				return std::string(tmpchar);
			}
			else if (strncmp(m_interfaceConf.m_trans_info[i].transinfo[2].value.c_str(),"/",1)==0)
			{
				char tmpchar[40];
				if (fieldtype==2) //integer
				{
					sprintf(tmpchar,"%ld",(int)(tmp1/tmp2));	
				}
				else
				{
					sprintf(tmpchar,"%lf",tmp1/tmp2);
				}
				
				return std::string(tmpchar);
			}
		}
	}
	return "";
}

//����
bool CPackConvert::getValueFromBufS(char *buf,int buflen,S_FUNC_FIELD_INFO *fieldconf,std::string &value)
{
	char tmpfield[800];
	memset(tmpfield,0,sizeof(tmpfield));
	if (fieldconf->isTrans)
	{
		if (fieldconf->isVar) //�����򷽷�
		{
			//ȡ��������
			std::string ss = m_interfaceConf.getTransConv(fieldconf->transValue);
			if (atoi(ss.c_str())==0)
			{
				if (fieldconf->fieldoffset + fieldconf->fieldlength > buflen)
				{
					strcpy(tmpfield,"");
				}
				else
				{
					memcpy(tmpfield,buf+fieldconf->fieldoffset,fieldconf->fieldlength);
					tmpfield[fieldconf->fieldlength]=0;
					CBF_Tools::LRtrim(tmpfield);
					ss = m_interfaceConf.getTransString(fieldconf->transValue,tmpfield);
				}
				
			}
			else
			{
				ss = getTransValue(fieldconf->transValue,buf,0,fieldconf->fieldtype);
			}
			strcpy(tmpfield,ss.c_str());
		}
		else //����
		{
			strcpy(tmpfield,fieldconf->transValue.c_str());
		}
	}
	else
	{
		//�������ж�Ӧbuf��ֵȡ��tmpfield
		if (fieldconf->fieldoffset + fieldconf->fieldlength > buflen)
		{
		    if (buflen - fieldconf->fieldoffset <1)
		    {
		        strcpy(tmpfield,"");
		    }
			else
			{
			    memcpy(tmpfield,buf+fieldconf->fieldoffset,buflen - fieldconf->fieldoffset);
			    tmpfield[buflen - fieldconf->fieldoffset]=0;
		    }
		}
		else
		{
			memcpy(tmpfield,buf+fieldconf->fieldoffset,fieldconf->fieldlength);
			tmpfield[fieldconf->fieldlength]=0;
		}
	}
	//�ж�ȡֵ�Ƿ�Ͷ���������������
	if (fieldconf->fieldtype == 0)
	{
		if (fieldconf->isTrim)
		{
			CBF_Tools::LRtrim(tmpfield);
		}
	}
	else if (fieldconf->fieldtype == 1)
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!CBF_Tools::IsNumber(tmpfield))
		{
			sprintf(m_errMsg,"�ֶ�[%s]��integer",fieldconf->name.c_str());
			return false;
		}
	}
	else
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!IsDouble(tmpfield))
		{
			sprintf(m_errMsg,"�ֶ�[%s]��double",fieldconf->name.c_str());
			return false;
		}
	}
	value = tmpfield;
	return true;
}
//�ָ���
bool CPackConvert::getValueFromBufL(CBF_Slist *publog,S_FUNC_FIELD_INFO *fieldconf,std::string &value)
{
	char tmpfield[800];
	char buf[15000];
	int  fieldCount;
	fieldCount = publog->GetCount();
	memset(buf,0,sizeof(buf));
	memset(tmpfield,0,sizeof(tmpfield));
	if (fieldconf->isTrans)
	{
		if (fieldconf->isVar) //�����򷽷�
		{
			//ȡ��������
			std::string ss = m_interfaceConf.getTransConv(fieldconf->transValue);
			if (atoi(ss.c_str())==0)
			{
				if (fieldconf->fieldoffset >fieldCount-1)
				{
					strcpy(tmpfield,"");
				}
				else
				{
					CBF_Tools::StringCopy(tmpfield,799,publog->GetAt(fieldconf->fieldoffset,true).c_str());
					CBF_Tools::LRtrim(tmpfield);
					ss = m_interfaceConf.getTransString(fieldconf->transValue,tmpfield);
				}
			}
			else
			{
				strcpy(buf,publog->ToString().c_str());
				ss = getTransValue(fieldconf->transValue,buf,1,fieldconf->fieldtype);
			}
			strcpy(tmpfield,ss.c_str());
		}
		else //����
		{
			strcpy(tmpfield,fieldconf->transValue.c_str());
		}
	}
	else
	{
		//�������ж�Ӧbuf��ֵȡ��tmpfield
		if (fieldconf->fieldoffset >fieldCount-1)
		{
			strcpy(tmpfield,"");
		}
		else
		{
			CBF_Tools::StringCopy(tmpfield,799,publog->GetAt(fieldconf->fieldoffset).c_str());
		}
	}
	//�ж�ȡֵ�Ƿ�Ͷ���������������
	if (fieldconf->fieldtype == 0)
	{
		if (fieldconf->isTrim)
		{
			CBF_Tools::LRtrim(tmpfield);
		}
	}
	else if (fieldconf->fieldtype == 1)
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!CBF_Tools::IsNumber(tmpfield))
		{
			sprintf(m_errMsg,"�ֶ�[%s]��integer",fieldconf->name.c_str());
			return false;
		}
	}
	else
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!IsDouble(tmpfield))
		{
			sprintf(m_errMsg,"�ֶ�[%s]��double",fieldconf->name.c_str());
			return false;
		}
	}
	value = tmpfield;
	return true;
}

bool CPackConvert::setValueToBufS(CXmlPack *xmlpack, S_FUNC_FIELD_INFO *fieldconf, char *buf)
{
	std::string tmpfield; 
	char tmpchar[16384];
	char tt[50];
	memset(tmpchar,0,sizeof(tmpchar));
	memset(tt,0,sizeof(tt));
	if (fieldconf->isTrans)
	{
		if (fieldconf->isVar) //�����򷽷�
		{
			//ȡ��������
			std::string ss = m_interfaceConf.getTransConv(fieldconf->transValue);
			if (atoi(ss.c_str())==0)
			{
				if (strncmp(fieldconf->name.c_str(),"������",6)==0 || strncmp(fieldconf->name.c_str(),"������",6)==0)
				{
					xmlpack->getHeadValue(fieldconf->name,tmpfield);
				}
				else
				{
					xmlpack->getPackValue(fieldconf->name,tmpfield);
				}
				tmpfield = m_interfaceConf.getTransString(fieldconf->transValue,tmpfield);
			}
			else
			{
//				tmpfield = getTransValue(fieldconf->transValue,xmlpack);
			}
		}
		else //����
		{
			tmpfield = fieldconf->transValue;
		}
	}
	else
	{
		//�������ж�Ӧxmlpack��ֵȡ��tmpfield
		if (strncmp(fieldconf->name.c_str(),"������",6)==0 || strncmp(fieldconf->name.c_str(),"������",6)==0)
		{
			xmlpack->getHeadValue(fieldconf->name,tmpfield);
		}
		else
		{
			xmlpack->getPackValue(fieldconf->name,tmpfield);
		}
	}
//	m_publog.errlog(m_logfile,__FILE__,__LINE__,0,(char *)tmpfield.c_str());
	//�ж�ȡֵ�Ƿ�Ͷ���������������
	if (fieldconf->fieldtype == 0)
	{
		if (fieldconf->isTrim)
		{
			CBF_Tools::LRtrim(tmpfield);
		}
		if (fieldconf->left == 1) //�Ҷ���
		{
			sprintf(tt,"%s%d%s","%",fieldconf->fieldlength,"s");
			sprintf(tmpchar,tt,tmpfield.c_str());
		}
		else //�����
		{
			sprintf(tmpchar,"%s",tmpfield.c_str());	
		}
	}
	else if (fieldconf->fieldtype == 1)
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!CBF_Tools::IsNumber((char *)tmpfield.c_str()))
		{
			sprintf(m_errMsg,"�ֶ�[%s]��integer",fieldconf->name.c_str());
			return false;
		}
		if (fieldconf->left == 1) //�Ҷ���
		{
			sprintf(tt,"%s0%d%s","%",fieldconf->fieldlength,"ld");
			sprintf(tmpchar,tt,atoi(tmpfield.c_str()));
		}
		else //�����
		{
			sprintf(tmpchar,"%ld",atoi(tmpfield.c_str()));	
		}
	}
	else if (fieldconf->fieldtype == 2)
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!IsDouble((char *)tmpfield.c_str()))
		{
			sprintf(m_errMsg,"�ֶ�[%s]��double",fieldconf->name.c_str());
			return false;
		}
		if (fieldconf->left == 1) //�Ҷ���
		{
			if (fieldconf->dec >= 0)
			{
				sprintf(tt,"%s0%d.%d%s","%",fieldconf->fieldlength,fieldconf->dec,"lf");
			}
			else
			{
				sprintf(tt,"%s0%d%s","%",fieldconf->fieldlength,"lf");
			}
			sprintf(tmpchar,tt,atof(tmpfield.c_str()));
		}
		else //�����
		{
			if (fieldconf->dec >= 0)
			{
				sprintf(tt,"%s.%d%s","%",fieldconf->dec,"lf");
				sprintf(tmpchar,tt,atof(tmpfield.c_str()));	
			}
			else
			{
				sprintf(tmpchar,"%lf",atof(tmpfield.c_str()));	
			}
			
		}
	}
	else
	{
		sprintf(m_errMsg,"�ֶ�[%s]�Ƿ�",fieldconf->name.c_str());
		return false;
	}
	memcpy(buf+fieldconf->fieldoffset,tmpchar,fieldconf->fieldlength);
	return true;
}
bool CPackConvert::setValueToBufL(CXmlPack *xmlpack, S_FUNC_FIELD_INFO *fieldconf, CBF_Slist *publog)
{
	std::string tmpfield; 
	char tmpchar[800];
	char tt[20];
	memset(tt,0,sizeof(tt));
	memset(tmpchar,0,sizeof(tmpchar));
	if (fieldconf->isTrans)
	{
		if (fieldconf->isVar) //�����򷽷�
		{
			//ȡ��������
			std::string ss = m_interfaceConf.getTransConv(fieldconf->transValue);
			if (atoi(ss.c_str())==0)
			{
				if (strncmp(fieldconf->name.c_str(),"������",6)==0)
				{
					xmlpack->getHeadValue(fieldconf->name,tmpfield);
				}
				else
				{
					xmlpack->getPackValue(fieldconf->name,tmpfield);
				}
				tmpfield = m_interfaceConf.getTransString(fieldconf->transValue,tmpfield);
			}
			//�����������ӷ��뷽��
		}
		else //����
		{
			tmpfield = fieldconf->transValue;
		}
	}
	else
	{
		//�������ж�Ӧxmlpack��ֵȡ��tmpfield
		if (strncmp(fieldconf->name.c_str(),"������",6)==0)
		{
			xmlpack->getHeadValue(fieldconf->name,tmpfield);
		}
		else
		{
			xmlpack->getPackValue(fieldconf->name,tmpfield);
		}
	}
	//�ж�ȡֵ�Ƿ�Ͷ���������������
	if (fieldconf->fieldtype == 0)
	{
		if (fieldconf->isTrim)
		{
			CBF_Tools::LRtrim(tmpfield);
		}
	}
	else if (fieldconf->fieldtype == 1)
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!CBF_Tools::IsNumber((char *)tmpfield.c_str()))
		{
			sprintf(m_errMsg,"�ֶ�[%s]��integer",fieldconf->name.c_str());
			return false;
		}
	}
	else if (fieldconf->fieldtype == 2)
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!IsDouble((char *)tmpfield.c_str()))
		{
			sprintf(m_errMsg,"�ֶ�[%s]��double",fieldconf->name.c_str());
			return false;
		}
			
		if (fieldconf->dec >= 0)
		{
			sprintf(tt,"%s.%d%s","%",fieldconf->dec,"lf");
		}
	
		sprintf(tmpchar,tt,atof(tmpfield.c_str()));
		tmpfield = tmpchar;
		CBF_Tools::LRtrim(tmpfield);
	}
	else
	{
		sprintf(m_errMsg,"�ֶ�[%s]�Ƿ�",fieldconf->name.c_str());
		return false;
	}
	if (fieldconf->fieldoffset > publog->GetCount()-1)
	{
		sprintf(m_errMsg,"�����ֶ�[%d]����[%d]",fieldconf->fieldoffset,publog->GetCount()-1);
		return false;
	}
//	memset(tmpchar,0,sizeof(tmpchar));
//	strcpy(tmpchar,tmpfield.c_str());
	publog->SetAt((char *)tmpfield.c_str(),fieldconf->fieldoffset);
//	publog->GetBuff(tmpchar);
	return true;
}

bool CPackConvert::getResAttr(int txcode, int retcode, int &fileattr)
{
	S_FUNC_CONF_INFO *funcinfo = m_interfaceConf.getFuncInfo(txcode);
	if (funcinfo==NULL)
	{
		sprintf(m_errMsg,"������[%d]�ڽӿ�������û�ж���",txcode);
		return false;
	}
	if (funcinfo->funcRes.size()<=1) //ֻ��һ��Ӧ��
	{
		fileattr = funcinfo->funcRes[0].fileFlag;
		return true;
	}
	else
	{
		for (int i=0;i<funcinfo->funcRes.size();i++)
		{
			if (retcode == funcinfo->funcRes[i].retCode)
			{
				fileattr = funcinfo->funcRes[i].fileFlag;
				return true;
			}
		}
	}
	return false;
}

bool CPackConvert::conv2BufS(int txcode,S_PACKHEAD_INFO *headinfo,CXmlPack *xmlpack,char *buf,int &buflen)
{
	if (headinfo==NULL)
	{
		buflen= 0;
		return true;
	}
	for (int i=0;i<headinfo->headinfo.size();i++)
	{
		if (strncmp(headinfo->headinfo[i].name.c_str(),"������",6)==0)
		{	
			char tmpcharr[20];
			char tt[20];
			sprintf(tt,"%s0%d%s","%",headinfo->headinfo[i].fieldlength,"d");
			sprintf(tmpcharr,tt,txcode);
			memcpy((char *)buf+headinfo->headinfo[i].fieldoffset,tmpcharr,headinfo->headinfo[i].fieldlength);
		}
		else
		{
			if (!setValueToBufS(xmlpack,&headinfo->headinfo[i],(char *)buf))
			{
				m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
				return false;
			}
		}
		buflen = headinfo->headinfo[i].fieldoffset+headinfo->headinfo[i].fieldlength;
	}

	return true;
}

bool CPackConvert::Xml2AnsExt(int txcode, CXmlPack *xmlpack, char *buf, int &buflen)
{
	S_FUNC_CONF_INFO *funcinfo = m_interfaceConf.getFuncInfo(txcode);
	if (funcinfo==NULL)
	{
		sprintf(m_errMsg,"������[%d]�ڽӿ�������û�ж���",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 3 ) //����
	{
		if (funcinfo->funcRes.size()<=1) //ֻ��һ��Ӧ��
		{
			if (strlen(funcinfo->funcRes[0].include)>0 && funcinfo->funcRes[0].conv_type !=funcinfo->funcAttr.conv_type)
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(funcinfo->funcRes[0].include);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"������[%d]���õİ�ͷ[%s]�ڽӿ�������û�ж���",txcode,funcinfo->funcRes[0].include);
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return false;
				}
				return conv2BufS(txcode,headinfo,xmlpack,buf,buflen);
			}
		}
		else
		{
			for (int i=0;i<funcinfo->funcRes.size();i++)
			{
				int ret = xmlpack->getHeadValueI("������");
				if (ret == funcinfo->funcRes[i].retCode)
				{
					if (strlen(funcinfo->funcRes[i].include)>0 && funcinfo->funcRes[i].conv_type !=funcinfo->funcAttr.conv_type)
					{
						S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(funcinfo->funcRes[i].include);
						if (headinfo==NULL)
						{
							sprintf(m_errMsg,"������[%d]���õİ�ͷ[%s]�ڽӿ�������û�ж���",txcode,funcinfo->funcRes[i].include);
							m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
							return false;
						}
						return conv2BufS(txcode,headinfo,xmlpack,(char *)buf,buflen);
					}
				}
			}
		}
	}
	return false;
}

bool CPackConvert::Xml2AnsExt(int txcode, CXmlPack *xmlpack, char *buf, int &buflen, int type)
{
	S_FUNC_CONF_INFO *funcinfo = m_interfaceConf.getFuncInfo(txcode);
	if (funcinfo==NULL)
	{
		sprintf(m_errMsg,"������[%d]�ڽӿ�������û�ж���",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 3 ) //����
	{
		if (funcinfo->funcRes.size()<=1) //ֻ��һ��Ӧ��
		{
			if (strlen(funcinfo->funcRes[0].include)>0)
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(funcinfo->funcRes[0].include);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"������[%d]���õİ�ͷ[%s]�ڽӿ�������û�ж���",txcode,funcinfo->funcRes[0].include);
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return false;
				}
				return conv2BufS(txcode,headinfo,xmlpack,buf,buflen);
			}
		}
		else
		{
			int ret = type;
			for (int i=0;i<funcinfo->funcRes.size();i++)
			{
				if (ret == funcinfo->funcRes[i].retCode)
				{
					if (strlen(funcinfo->funcRes[i].include)>0)
					{
						S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(funcinfo->funcRes[i].include);
						if (headinfo==NULL)
						{
							sprintf(m_errMsg,"������[%d]���õİ�ͷ[%s]�ڽӿ�������û�ж���",txcode,funcinfo->funcRes[i].include);
							m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
							return false;
						}
						return conv2BufS(txcode,headinfo,xmlpack,(char *)buf,buflen);
					}
				}
			}
		}
	}
	return false;
}

bool CPackConvert::Xml2Ans(int txcode, CXmlPack *xmlpack, void *buf, int &buflen, int type)
{
	S_FUNC_CONF_INFO *funcinfo = m_interfaceConf.getFuncInfo(txcode);
	if (funcinfo==NULL)
	{
		sprintf(m_errMsg,"������[%d]�ڽӿ�������û�ж���",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 0 ) //xml��
	{
		xmlpack->toBuf((char *)buf,buflen);//�����buf�����ж���������
		return true;
	}
	

	if (funcinfo->funcAttr.conv_type == 2 || funcinfo->funcAttr.conv_type == 3 ) //�ָ���������
	{
		std::string name ; //xml����
		std::string value ;//cpack�ֶ�
		std::string tmpfield;//xml���ƶ�Ӧ��ֵ
		CBF_Slist tmppublog;
		char tmpchar[90];
		if (funcinfo->funcRes.size()<=1) //ֻ��һ��Ӧ��
		{
			m_resConv = funcinfo->funcRes[0].conv_type;
			if (2 == m_resConv) //�ָ����ָ�
			{
				char separator[3];
				bzero(separator,sizeof(separator));
				if (CBF_Tools::IsNumber(funcinfo->funcRes[0].separator))
				{
					separator[0] = atoi(funcinfo->funcRes[0].separator);
				}
				else
				{
					separator[0] = funcinfo->funcRes[0].separator[0];
				}
				tmppublog.SetSeparateString(separator);
//				sprintf(tmpchar,"113%c",separator);
//				tmppublog.FillStrings(tmpchar,separator);
				tmppublog.Clear();
				//�������ֶ�ֵ
				for (int iii=0;iii<funcinfo->funcRes[0].funcRes.size();iii++)
				{
					tmppublog.Add(" ");
				}
			}
			for (int i=0;i<funcinfo->funcRes[0].funcRes.size();i++)
			{
				name = funcinfo->funcRes[0].funcRes[i].name;//xml����
				value = funcinfo->funcRes[0].funcRes[i].value;//
				//��ͷ
				if (strncmp(name.c_str(),"include",7)==0)
				{
					S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
					if (headinfo==NULL)
					{
						sprintf(m_errMsg,"������[%d]��ͷ[%s]�ڽӿ�������û�ж���",txcode,value.c_str());
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
						return false;
					}
					for (int j=0;j<headinfo->headinfo.size();j++)
					{
						if (2==m_resConv)
						{
							if (j<headinfo->headinfo.size()-1)
							{
								tmppublog.Add(" ");
							}
							if (!setValueToBufL(xmlpack,&headinfo->headinfo[j],&tmppublog))
							{
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
								return false;
							}
								
						}
						else
						{
							if (strncmp(headinfo->headinfo[j].name.c_str(),"������",6)==0)
							{
								char tmpcharr[20];
								char tt[20];
								sprintf(tt,"%s0%d%s","%",headinfo->headinfo[j].fieldlength,"d");
								sprintf(tmpcharr,tt,txcode);
								memcpy((char *)buf+headinfo->headinfo[j].fieldoffset,tmpcharr,headinfo->headinfo[j].fieldlength);
							}
							else
							{
								if (!setValueToBufS(xmlpack,&headinfo->headinfo[j],(char *)buf))
								{
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
									return false;
								}
							}
							
							buflen = headinfo->headinfo[j].fieldoffset + headinfo->headinfo[j].fieldlength;
						}
						
					}
				} 
				else
				{
					if (2==m_resConv )
					{
						if (!setValueToBufL(xmlpack,&funcinfo->funcRes[0].funcRes[i],&tmppublog))
						{
							m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
							return false;
						}
					}
					else
					{
						if (strncmp(funcinfo->funcRes[0].funcRes[i].name.c_str(),"������",6)==0)
						{
							char tmpcharr[20];
							char tt[20];
							sprintf(tt,"%s0%d%s","%",funcinfo->funcRes[0].funcRes[i].fieldlength,"d");
							sprintf(tmpcharr,tt,txcode);
							memcpy((char *)buf+funcinfo->funcRes[0].funcRes[i].fieldoffset,tmpcharr,funcinfo->funcRes[0].funcRes[i].fieldlength);
						}
						else
						{
							if (!setValueToBufS(xmlpack,&funcinfo->funcRes[0].funcRes[i],(char *)buf))
							{
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
								return false;
							}
						}
						
						buflen = funcinfo->funcRes[0].funcRes[i].fieldoffset + funcinfo->funcRes[0].funcRes[i].fieldlength;
					}
					
				}
				
			
			} //end for
			if (2==m_resConv )
			{
				//ת��������ݷ���buf��
				strcpy((char *)buf,tmppublog.ToString().c_str());
				tmppublog.Clear();
				buflen = strlen((char *)buf);
				
			}
			else
			{
				((char *)buf)[buflen]=0;
			}
			return true;
		}
		else //���Ӧ��
		{
			int ret = type;
			for (int i=0;i<funcinfo->funcRes.size();i++)
			{
				if (ret == funcinfo->funcRes[i].retCode)
				{
					m_resConv = funcinfo->funcRes[i].conv_type;
					if (2==m_resConv )
					{
						char separator[3];
						bzero(separator,sizeof(separator));
						if (CBF_Tools::IsNumber(funcinfo->funcRes[i].separator))
						{
							separator[0] = atoi(funcinfo->funcRes[i].separator);
						}
						else
						{
							separator[0] = funcinfo->funcRes[i].separator[0];
						}
						tmppublog.SetSeparateString(separator);
//						sprintf(tmpchar,"113%c",separator);
//						tmppublog.FillStrings(tmpchar,separator);
						tmppublog.Clear();
						//�������ֶ�ֵ
						for (int iii=0;iii<funcinfo->funcRes[i].funcRes.size();iii++)
						{
							tmppublog.Add(" ");
						}
					}
					for (int ij=0;ij<funcinfo->funcRes[i].funcRes.size();ij++)
					{
						name = funcinfo->funcRes[i].funcRes[ij].name;//xml����
						value = funcinfo->funcRes[i].funcRes[ij].value;//
						//��ͷ
						if (strncmp(name.c_str(),"include",7)==0)
						{
							S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
							if (headinfo==NULL)
							{
								sprintf(m_errMsg,"������[%d]��ͷ[%s]�ڽӿ�������û�ж���",txcode,value.c_str());
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
								return false;
							}
							for (int j=0;j<headinfo->headinfo.size();j++)
							{
								if (2==m_resConv )
								{
									if (j<headinfo->headinfo.size()-1)
									{
										tmppublog.Add(" ");
									}
									if (!setValueToBufL(xmlpack,&headinfo->headinfo[j],&tmppublog))
									{
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
										return false;
									}
								}
								else
								{
									if (strncmp(headinfo->headinfo[j].name.c_str(),"������",6)==0)
									{
										char tmpcharr[20];
										char tt[20];
										sprintf(tt,"%s0%d%s","%",headinfo->headinfo[j].fieldlength,"d");
										sprintf(tmpcharr,tt,txcode);
										memcpy((char *)buf+headinfo->headinfo[j].fieldoffset,tmpcharr,headinfo->headinfo[j].fieldlength);
									}
									else
									{
										if (!setValueToBufS(xmlpack,&headinfo->headinfo[j],(char *)buf))
										{
											m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
											return false;
										}
									}
									
									buflen = headinfo->headinfo[j].fieldoffset + headinfo->headinfo[j].fieldlength;
								}
								
								
							}
						} 
						else
						{
							if (2==m_resConv )
							{
								if (!setValueToBufL(xmlpack,&funcinfo->funcRes[i].funcRes[ij],&tmppublog))
								{
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
									return false;
								}
							}
							else
							{
								if (strncmp(funcinfo->funcRes[i].funcRes[ij].name.c_str(),"������",6)==0)
								{
									char tmpcharr[20];
									char tt[20];
									sprintf(tt,"%s0%d%s","%",funcinfo->funcRes[i].funcRes[ij].fieldlength,"d");
									sprintf(tmpcharr,tt,txcode);
									memcpy((char *)buf+funcinfo->funcRes[i].funcRes[ij].fieldoffset,tmpcharr,funcinfo->funcRes[i].funcRes[ij].fieldlength);
								}
								else
								{
									if (!setValueToBufS(xmlpack,&funcinfo->funcRes[i].funcRes[ij],(char *)buf))
									{
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"������%d %s",txcode,m_errMsg);
										return false;
									}
								}
								
								buflen = funcinfo->funcRes[i].funcRes[ij].fieldoffset + funcinfo->funcRes[i].funcRes[ij].fieldlength;
							}
							
						}
					
						
					}
					if (2==m_resConv )
					{
						//ת��������ݷ���buf��
						strcpy((char *)buf,tmppublog.ToString().c_str());
						tmppublog.Clear();	
						buflen = strlen((char *)buf);
					}
					else
					{
						((char *)buf)[buflen]=0;
					}
					return true;

				}
			}
		}
	}
	return false;	
}
bool CPackConvert::IsDouble(char *str)
{
	int len;
	int sop=0;
	int ret=0;
	len = strlen(str);
	if (len<1) return true;
	for (int i=0;i<len;i++)
	{
		if (str[i]=='-' && i==0)
		{
			continue;
		}
		if (str[i]=='.')
		{
			ret++;
			if (ret>1)
				return false;
		}
		if ((str[i]<'0' || str[i]>'9') && str[i]!='.')
		{
			return false;
		}
		if (str[i]=='.')
		{
			if (i==0 || i==len-1)
			{
				return false;
			}
			sop++;
		}
	}
	if (sop>1)
	{
		return false;
	}
	return true;	
}
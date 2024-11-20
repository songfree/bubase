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
	//读配置文件
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

//转换外部请求为内部xmlpack格式
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
		sprintf(m_errMsg,"交易码[%d]在接口配置中没有定义",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 0 ) //xml包
	{
		xmlpack->fromBuf((char *)buf);
		return true;
	}
	if (funcinfo->funcAttr.conv_type == 2 ) //分隔符
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
			sprintf(m_errMsg,"数据缓冲不为分隔符数据");
			return false;
		}
		//分隔符
		for (int i=0;i<funcinfo->funcReq.funcReq.size();i++)
		{
			std::string name = (funcinfo->funcReq.funcReq[i]).name;//名称 xml字段
			std::string value = (funcinfo->funcReq.funcReq[i]).value;
			if (strncmp(name.c_str(),"include",7)==0) //include段的处理
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"包头[%s]在接口配置中没有定义",value.c_str());
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return false;
				}
				for (int j=0;j<headinfo->headinfo.size();j++)
				{
					name = (headinfo->headinfo[j]).name;//名称 xml字段
					value = (headinfo->headinfo[j]).value;
					if (!getValueFromBufL(&publog,&headinfo->headinfo[j],tmpfield))
					{
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
						return false;
					}
					if (strncmp(name.c_str(),"返回码",6)==0 || strncmp(name.c_str(),"交易码",6)==0)
					{
						xmlpack->setHeadNameValue(name,tmpfield);
					}
					else
					{
						xmlpack->setPackNameValue(name,tmpfield);
					}
				}
			}
			else  //接口里定义
			{
				if (!getValueFromBufL(&publog,&funcinfo->funcReq.funcReq[i],tmpfield))
				{
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
					return false;
				}
				if (strncmp(name.c_str(),"返回码",6)==0 || strncmp(name.c_str(),"交易码",6)==0)
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
	if (funcinfo->funcAttr.conv_type == 3 ) //定长包
	{
		std::string tmpfield;
		xmlpack->newPackage();
		//buf内容为定长无分隔符内容
		//定长无分隔
		for (int i=0;i<funcinfo->funcReq.funcReq.size();i++)
		{
			std::string name = (funcinfo->funcReq.funcReq[i]).name;//名称 xml字段
			std::string value = (funcinfo->funcReq.funcReq[i]).value;
			
			if (strncmp(name.c_str(),"include",7)==0) //include段的处理
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"包头[%s]在接口配置中没有定义",value.c_str());
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return false;
				}
				for (int j=0;j<headinfo->headinfo.size();j++)
				{
					name = (headinfo->headinfo[j]).name;//名称 xml字段
					value = (headinfo->headinfo[j]).value;
					if (!getValueFromBufS((char *)buf,buflen,&headinfo->headinfo[j],tmpfield))
					{
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
						return false;
					}
					if (strncmp(name.c_str(),"返回码",6)==0 || strncmp(name.c_str(),"交易码",6)==0)
					{
						xmlpack->setHeadNameValue(name,tmpfield);
					}
					else
					{
						xmlpack->setPackNameValue(name,tmpfield);
					}
					
				}
			}
			else  //接口里定义
			{
				if (!getValueFromBufS((char *)buf,buflen,&funcinfo->funcReq.funcReq[i],tmpfield))
				{
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
					return false;
				}
				if (strncmp(name.c_str(),"返回码",6)==0 || strncmp(name.c_str(),"交易码",6)==0)
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
//转换外部的应答为内部xmlpack格式
bool CPackConvert::Conv2XmlAns(int txcode, void *buf, int buflen, CXmlPack *xmlpack)
{
	S_FUNC_CONF_INFO *funcinfo = m_interfaceConf.getFuncInfo(txcode);
	if (funcinfo==NULL)
	{
		sprintf(m_errMsg,"交易码[%d]在接口配置中没有定义",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 0 ) //xml包
	{
		CXmlPack tmpxml;
		tmpxml.fromBuf((char *)buf);
		xmlpack->setHeadNameValue("返回码",tmpxml.getHeadValue("返回码"));
		xmlpack->setHeadNameValue("记录数",tmpxml.getHeadValue("记录数"));
		//把tmpxml里的pack复制到xmlpack,重复的不更新
		xmlpack->modiPack(&tmpxml,false);
		return true;
	}
	
	if (funcinfo->funcAttr.conv_type == 2 || funcinfo->funcAttr.conv_type == 3 ) //分隔符或定长包
	{
		std::string name ; //xml名称
		std::string value ;//cpack字段
		std::string tmpfield;//xml名称对应的值
		CBF_Slist publog;
		char tmpchar[20];
		if (funcinfo->funcRes.size()<=1) //只有一个应答
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
				name = funcinfo->funcRes[0].funcRes[i].name;//xml名称
				value = funcinfo->funcRes[0].funcRes[i].value;//
				//包头
				if (strncmp(name.c_str(),"include",7)==0)
				{
					S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
					if (headinfo==NULL)
					{
						sprintf(m_errMsg,"交易码[%d]在接口配置中没有定义",txcode);
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
						return false;
					}
					for (int j=0;j<headinfo->headinfo.size();j++)
					{
						name = (headinfo->headinfo[j]).name;//名称 xml字段
						value = (headinfo->headinfo[j]).value;
						if (funcinfo->funcRes[0].conv_type == 2 )
						{
							if (!getValueFromBufL(&publog,&headinfo->headinfo[j],tmpfield))
							{
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
								return false;
							}
						}
						else
						{
							if (!getValueFromBufS((char *)buf,buflen,&headinfo->headinfo[j],tmpfield))
							{
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
								return false;
							}
						}
						//设置值到xmlpack
						if (strncmp(name.c_str(),"返回码",6)==0)
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
							m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
							return false;
						}
					}
					else
					{
						if (!getValueFromBufS((char *)buf,buflen,&funcinfo->funcRes[0].funcRes[i],tmpfield))
						{
							m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
							return false;
						}
					}
					//设置值到xmlpack
					if (strncmp(name.c_str(),"返回码",6)==0)
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
		else   //根据返回码的定义返回应答
		{
			for (int i=0;i<funcinfo->funcRes.size();i++)
			{
				int ret = xmlpack->getHeadValueI("返回码");
				if (ret == funcinfo->funcRes[i].retCode)
				{
					for (int ij=0;ij<funcinfo->funcRes[i].funcRes.size();ij++)
					{
						name = funcinfo->funcRes[i].funcRes[ij].name;//xml名称
						value = funcinfo->funcRes[i].funcRes[ij].value;//
						//包头
						if (strncmp(name.c_str(),"include",7)==0)
						{
							S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
							if (headinfo==NULL)
							{
								sprintf(m_errMsg,"交易码[%d]在接口配置中没有定义",txcode);
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
								return false;
							}
							for (int j=0;j<headinfo->headinfo.size();j++)
							{
								name = (headinfo->headinfo[j]).name;//名称 xml字段
								value = (headinfo->headinfo[j]).value;
								if (funcinfo->funcRes[i].conv_type ==2 )
								{
									if (!getValueFromBufL(&publog,&headinfo->headinfo[j],tmpfield))
									{
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
										return false;
									}
								}
								else
								{
									if (!getValueFromBufS((char *)buf,buflen,&headinfo->headinfo[j],tmpfield))
									{
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
										return false;
									}
								}
								//设置值到xmlpack
								if (strncmp(name.c_str(),"返回码",6)==0)
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
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
									return false;
								}
							}
							else
							{
								if (!getValueFromBufS((char *)buf,buflen,&funcinfo->funcRes[ij].funcRes[i],tmpfield))
								{
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
									return false;
								}
							}
							//设置值到xmlpack
							if (strncmp(name.c_str(),"返回码",6)==0)
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
//转换内部xmlpack格式为外部请求
bool CPackConvert::Xml2Req(int txcode, CXmlPack *xmlpack, void *buf, int &buflen)
{
	S_FUNC_CONF_INFO *funcinfo = m_interfaceConf.getFuncInfo(txcode);
	if (funcinfo==NULL)
	{
		sprintf(m_errMsg,"交易码[%d]在接口配置中没有定义",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 0 ) //xml包
	{
		xmlpack->toBuf((char *)buf,buflen);//这里的buf可能有二进制数据
		return true;
	}
	
	if (funcinfo->funcAttr.conv_type == 2 ) //分隔符
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
		//先根据字段数据生成空的字串
		for (i=0;i<funcinfo->funcReq.funcReq.size();i++)
		{
			name = (funcinfo->funcReq.funcReq[i]).name;//xml名称
			value = (funcinfo->funcReq.funcReq[i]).value;
			if (strncmp(name.c_str(),"include",7)==0) //include段的处理
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"交易码[%d]包头[%s]在接口配置中没有定义",txcode,value.c_str());
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
			name = (funcinfo->funcReq.funcReq[i]).name;//xml名称
			value = (funcinfo->funcReq.funcReq[i]).value;
			
			if (strncmp(name.c_str(),"include",7)==0) //include段的处理
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"交易码[%d]包头[%s]在接口配置中没有定义",txcode,value.c_str());
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return false;
				}
				
				for (int j=0;j<headinfo->headinfo.size();j++)
				{
					if (!setValueToBufL(xmlpack,&headinfo->headinfo[j],&publog))
					{
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
						return false;
					}
				}
			}
			else
			{
				if (!setValueToBufL(xmlpack,&funcinfo->funcReq.funcReq[i],&publog))
				{
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
					return false;
				}

			}
		}
		strcpy((char *)buf,publog.ToString().c_str());
//		m_publog.errlog(m_logfile,__FILE__,__LINE__,txcode,(char *)buf);
		return true;
	}
	if (funcinfo->funcAttr.conv_type == 3 ) //定长包
	{
		for (int i=0;i<funcinfo->funcReq.funcReq.size();i++)
		{
			std::string name = (funcinfo->funcReq.funcReq[i]).name;//xml名称
			std::string value = (funcinfo->funcReq.funcReq[i]).value;
			std::string tmpfield;
			if (strncmp(name.c_str(),"include",7)==0) //include段的处理
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"交易码[%d]包头[%s]在接口配置中没有定义",txcode,value.c_str());
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return false;
				}
				
				for (int j=0;j<headinfo->headinfo.size();j++)
				{
					if (!setValueToBufS(xmlpack,&headinfo->headinfo[j],(char *)buf))
					{
						m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
						return false;
					}
					buflen = headinfo->headinfo[j].fieldoffset + headinfo->headinfo[j].fieldlength;
				}
			}
			else
			{
				if (!setValueToBufS(xmlpack,&funcinfo->funcReq.funcReq[i],(char *)buf))
				{
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
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
//xml应答转换给外部包
bool CPackConvert::Xml2Ans(int txcode, CXmlPack *xmlpack, void *buf, int &buflen)
{
	S_FUNC_CONF_INFO *funcinfo = m_interfaceConf.getFuncInfo(txcode);
	if (funcinfo==NULL)
	{
		sprintf(m_errMsg,"交易码[%d]在接口配置中没有定义",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 0 ) //xml包
	{
		xmlpack->toBuf((char *)buf,buflen);//这里的buf可能有二进制数据
		return true;
	}
	

	if (funcinfo->funcAttr.conv_type == 2 || funcinfo->funcAttr.conv_type == 3 ) //分隔符及定长
	{
		std::string name ; //xml名称
		std::string value ;//cpack字段
		std::string tmpfield;//xml名称对应的值
		CBF_Slist tmppublog;
		char tmpchar[90];
		if (funcinfo->funcRes.size()<=1) //只有一个应答
		{
			m_resConv = funcinfo->funcRes[0].conv_type;
			if (2 == m_resConv) //分隔符分隔
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
				//置所有字段值
				for (int iii=0;iii<funcinfo->funcRes[0].funcRes.size();iii++)
				{
					tmppublog.Add(" ");
				}
			}
			for (int i=0;i<funcinfo->funcRes[0].funcRes.size();i++)
			{
				name = funcinfo->funcRes[0].funcRes[i].name;//xml名称
				value = funcinfo->funcRes[0].funcRes[i].value;//
				//包头
				if (strncmp(name.c_str(),"include",7)==0)
				{
					S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
					if (headinfo==NULL)
					{
						sprintf(m_errMsg,"交易码[%d]包头[%s]在接口配置中没有定义",txcode,value.c_str());
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
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
								return false;
							}
								
						}
						else
						{
							if (strncmp(headinfo->headinfo[j].name.c_str(),"交易码",6)==0)
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
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
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
							m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
							return false;
						}
					}
					else
					{
						if (strncmp(funcinfo->funcRes[0].funcRes[i].name.c_str(),"交易码",6)==0)
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
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
								return false;
							}
						}
						
						buflen = funcinfo->funcRes[0].funcRes[i].fieldoffset + funcinfo->funcRes[0].funcRes[i].fieldlength;
					}
					
				}
				
			
			} //end for
			if (2==m_resConv )
			{
				//转换后的数据放至buf中
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
		else //多个应答
		{
			for (int i=0;i<funcinfo->funcRes.size();i++)
			{
				int ret = xmlpack->getHeadValueI("返回码");
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
						//置所有字段值
						for (int iii=0;iii<funcinfo->funcRes[i].funcRes.size();iii++)
						{
							tmppublog.Add(" ");
						}
					}
					for (int ij=0;ij<funcinfo->funcRes[i].funcRes.size();ij++)
					{
						name = funcinfo->funcRes[i].funcRes[ij].name;//xml名称
						value = funcinfo->funcRes[i].funcRes[ij].value;//
						//包头
						if (strncmp(name.c_str(),"include",7)==0)
						{
							S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
							if (headinfo==NULL)
							{
								sprintf(m_errMsg,"交易码[%d]包头[%s]在接口配置中没有定义",txcode,value.c_str());
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
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
										return false;
									}
								}
								else
								{
									if (strncmp(headinfo->headinfo[j].name.c_str(),"交易码",6)==0)
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
											m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
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
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
									return false;
								}
							}
							else
							{
								if (strncmp(funcinfo->funcRes[i].funcRes[ij].name.c_str(),"交易码",6)==0)
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
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
										return false;
									}
								}
								
								buflen = funcinfo->funcRes[i].funcRes[ij].fieldoffset + funcinfo->funcRes[i].funcRes[ij].fieldlength;
							}
							
						}
					
						
					}
					if (2==m_resConv )
					{
						//转换后的数据放至buf中
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
			if (flag==0) //定长
			{	
				memcpy(tmpfield,buf+m_interfaceConf.m_trans_info[i].transinfo[0].fieldoffset,m_interfaceConf.m_trans_info[i].transinfo[0].fieldlength);
				tmpfield[m_interfaceConf.m_trans_info[i].transinfo[0].fieldlength]=0;
				tmp1 = atof(tmpfield);
				memset(tmpfield,0,sizeof(tmpfield));
				memcpy(tmpfield,buf+m_interfaceConf.m_trans_info[i].transinfo[2].fieldoffset,m_interfaceConf.m_trans_info[i].transinfo[2].fieldlength);
				tmpfield[m_interfaceConf.m_trans_info[i].transinfo[2].fieldlength]=0;
				tmp2 = atof(tmpfield);
			}
			else //分隔
			{
				CBF_Slist publog;
				publog.SetSeparateString("|");
				publog.FillSepString(buf);
				if (m_interfaceConf.m_trans_info[i].transinfo[0].fieldoffset > publog.GetCount())
				{
					sprintf(m_errMsg,"buf内容和定义不符");
					m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,m_errMsg);
					return "";
				}
				CBF_Tools::StringCopy(tmpfield,799,publog.GetAt(m_interfaceConf.m_trans_info[i].transinfo[0].fieldoffset).c_str());
				tmp1 = atof(tmpfield);
				if (m_interfaceConf.m_trans_info[i].transinfo[2].fieldoffset > publog.GetCount())
				{
					sprintf(m_errMsg,"buf内容和定义不符");
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

//定长
bool CPackConvert::getValueFromBufS(char *buf,int buflen,S_FUNC_FIELD_INFO *fieldconf,std::string &value)
{
	char tmpfield[800];
	memset(tmpfield,0,sizeof(tmpfield));
	if (fieldconf->isTrans)
	{
		if (fieldconf->isVar) //变量或方法
		{
			//取翻译属性
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
		else //常量
		{
			strcpy(tmpfield,fieldconf->transValue.c_str());
		}
	}
	else
	{
		//把配置中对应buf的值取到tmpfield
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
	//判断取值是否和定义的数据类型相符
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
			sprintf(m_errMsg,"字段[%s]非integer",fieldconf->name.c_str());
			return false;
		}
	}
	else
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!IsDouble(tmpfield))
		{
			sprintf(m_errMsg,"字段[%s]非double",fieldconf->name.c_str());
			return false;
		}
	}
	value = tmpfield;
	return true;
}
//分隔符
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
		if (fieldconf->isVar) //变量或方法
		{
			//取翻译属性
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
		else //常量
		{
			strcpy(tmpfield,fieldconf->transValue.c_str());
		}
	}
	else
	{
		//把配置中对应buf的值取到tmpfield
		if (fieldconf->fieldoffset >fieldCount-1)
		{
			strcpy(tmpfield,"");
		}
		else
		{
			CBF_Tools::StringCopy(tmpfield,799,publog->GetAt(fieldconf->fieldoffset).c_str());
		}
	}
	//判断取值是否和定义的数据类型相符
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
			sprintf(m_errMsg,"字段[%s]非integer",fieldconf->name.c_str());
			return false;
		}
	}
	else
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!IsDouble(tmpfield))
		{
			sprintf(m_errMsg,"字段[%s]非double",fieldconf->name.c_str());
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
		if (fieldconf->isVar) //变量或方法
		{
			//取翻译属性
			std::string ss = m_interfaceConf.getTransConv(fieldconf->transValue);
			if (atoi(ss.c_str())==0)
			{
				if (strncmp(fieldconf->name.c_str(),"返回码",6)==0 || strncmp(fieldconf->name.c_str(),"交易码",6)==0)
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
		else //常量
		{
			tmpfield = fieldconf->transValue;
		}
	}
	else
	{
		//把配置中对应xmlpack的值取到tmpfield
		if (strncmp(fieldconf->name.c_str(),"返回码",6)==0 || strncmp(fieldconf->name.c_str(),"交易码",6)==0)
		{
			xmlpack->getHeadValue(fieldconf->name,tmpfield);
		}
		else
		{
			xmlpack->getPackValue(fieldconf->name,tmpfield);
		}
	}
//	m_publog.errlog(m_logfile,__FILE__,__LINE__,0,(char *)tmpfield.c_str());
	//判断取值是否和定义的数据类型相符
	if (fieldconf->fieldtype == 0)
	{
		if (fieldconf->isTrim)
		{
			CBF_Tools::LRtrim(tmpfield);
		}
		if (fieldconf->left == 1) //右对齐
		{
			sprintf(tt,"%s%d%s","%",fieldconf->fieldlength,"s");
			sprintf(tmpchar,tt,tmpfield.c_str());
		}
		else //左对齐
		{
			sprintf(tmpchar,"%s",tmpfield.c_str());	
		}
	}
	else if (fieldconf->fieldtype == 1)
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!CBF_Tools::IsNumber((char *)tmpfield.c_str()))
		{
			sprintf(m_errMsg,"字段[%s]非integer",fieldconf->name.c_str());
			return false;
		}
		if (fieldconf->left == 1) //右对齐
		{
			sprintf(tt,"%s0%d%s","%",fieldconf->fieldlength,"ld");
			sprintf(tmpchar,tt,atoi(tmpfield.c_str()));
		}
		else //左对齐
		{
			sprintf(tmpchar,"%ld",atoi(tmpfield.c_str()));	
		}
	}
	else if (fieldconf->fieldtype == 2)
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!IsDouble((char *)tmpfield.c_str()))
		{
			sprintf(m_errMsg,"字段[%s]非double",fieldconf->name.c_str());
			return false;
		}
		if (fieldconf->left == 1) //右对齐
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
		else //左对齐
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
		sprintf(m_errMsg,"字段[%s]非法",fieldconf->name.c_str());
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
		if (fieldconf->isVar) //变量或方法
		{
			//取翻译属性
			std::string ss = m_interfaceConf.getTransConv(fieldconf->transValue);
			if (atoi(ss.c_str())==0)
			{
				if (strncmp(fieldconf->name.c_str(),"返回码",6)==0)
				{
					xmlpack->getHeadValue(fieldconf->name,tmpfield);
				}
				else
				{
					xmlpack->getPackValue(fieldconf->name,tmpfield);
				}
				tmpfield = m_interfaceConf.getTransString(fieldconf->transValue,tmpfield);
			}
			//后续可以增加翻译方法
		}
		else //常量
		{
			tmpfield = fieldconf->transValue;
		}
	}
	else
	{
		//把配置中对应xmlpack的值取到tmpfield
		if (strncmp(fieldconf->name.c_str(),"返回码",6)==0)
		{
			xmlpack->getHeadValue(fieldconf->name,tmpfield);
		}
		else
		{
			xmlpack->getPackValue(fieldconf->name,tmpfield);
		}
	}
	//判断取值是否和定义的数据类型相符
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
			sprintf(m_errMsg,"字段[%s]非integer",fieldconf->name.c_str());
			return false;
		}
	}
	else if (fieldconf->fieldtype == 2)
	{
		CBF_Tools::LRtrim(tmpfield);
		if (!IsDouble((char *)tmpfield.c_str()))
		{
			sprintf(m_errMsg,"字段[%s]非double",fieldconf->name.c_str());
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
		sprintf(m_errMsg,"字段[%s]非法",fieldconf->name.c_str());
		return false;
	}
	if (fieldconf->fieldoffset > publog->GetCount()-1)
	{
		sprintf(m_errMsg,"设置字段[%d]大于[%d]",fieldconf->fieldoffset,publog->GetCount()-1);
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
		sprintf(m_errMsg,"交易码[%d]在接口配置中没有定义",txcode);
		return false;
	}
	if (funcinfo->funcRes.size()<=1) //只有一个应答
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
		if (strncmp(headinfo->headinfo[i].name.c_str(),"交易码",6)==0)
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
				m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
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
		sprintf(m_errMsg,"交易码[%d]在接口配置中没有定义",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 3 ) //定长
	{
		if (funcinfo->funcRes.size()<=1) //只有一个应答
		{
			if (strlen(funcinfo->funcRes[0].include)>0 && funcinfo->funcRes[0].conv_type !=funcinfo->funcAttr.conv_type)
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(funcinfo->funcRes[0].include);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"交易码[%d]配置的包头[%s]在接口配置中没有定义",txcode,funcinfo->funcRes[0].include);
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
				int ret = xmlpack->getHeadValueI("返回码");
				if (ret == funcinfo->funcRes[i].retCode)
				{
					if (strlen(funcinfo->funcRes[i].include)>0 && funcinfo->funcRes[i].conv_type !=funcinfo->funcAttr.conv_type)
					{
						S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(funcinfo->funcRes[i].include);
						if (headinfo==NULL)
						{
							sprintf(m_errMsg,"交易码[%d]配置的包头[%s]在接口配置中没有定义",txcode,funcinfo->funcRes[i].include);
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
		sprintf(m_errMsg,"交易码[%d]在接口配置中没有定义",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 3 ) //定长
	{
		if (funcinfo->funcRes.size()<=1) //只有一个应答
		{
			if (strlen(funcinfo->funcRes[0].include)>0)
			{
				S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(funcinfo->funcRes[0].include);
				if (headinfo==NULL)
				{
					sprintf(m_errMsg,"交易码[%d]配置的包头[%s]在接口配置中没有定义",txcode,funcinfo->funcRes[0].include);
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
							sprintf(m_errMsg,"交易码[%d]配置的包头[%s]在接口配置中没有定义",txcode,funcinfo->funcRes[i].include);
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
		sprintf(m_errMsg,"交易码[%d]在接口配置中没有定义",txcode);
		return false;
	}
	if (funcinfo->funcAttr.conv_type == 0 ) //xml包
	{
		xmlpack->toBuf((char *)buf,buflen);//这里的buf可能有二进制数据
		return true;
	}
	

	if (funcinfo->funcAttr.conv_type == 2 || funcinfo->funcAttr.conv_type == 3 ) //分隔符及定长
	{
		std::string name ; //xml名称
		std::string value ;//cpack字段
		std::string tmpfield;//xml名称对应的值
		CBF_Slist tmppublog;
		char tmpchar[90];
		if (funcinfo->funcRes.size()<=1) //只有一个应答
		{
			m_resConv = funcinfo->funcRes[0].conv_type;
			if (2 == m_resConv) //分隔符分隔
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
				//置所有字段值
				for (int iii=0;iii<funcinfo->funcRes[0].funcRes.size();iii++)
				{
					tmppublog.Add(" ");
				}
			}
			for (int i=0;i<funcinfo->funcRes[0].funcRes.size();i++)
			{
				name = funcinfo->funcRes[0].funcRes[i].name;//xml名称
				value = funcinfo->funcRes[0].funcRes[i].value;//
				//包头
				if (strncmp(name.c_str(),"include",7)==0)
				{
					S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
					if (headinfo==NULL)
					{
						sprintf(m_errMsg,"交易码[%d]包头[%s]在接口配置中没有定义",txcode,value.c_str());
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
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
								return false;
							}
								
						}
						else
						{
							if (strncmp(headinfo->headinfo[j].name.c_str(),"交易码",6)==0)
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
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
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
							m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
							return false;
						}
					}
					else
					{
						if (strncmp(funcinfo->funcRes[0].funcRes[i].name.c_str(),"交易码",6)==0)
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
								m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
								return false;
							}
						}
						
						buflen = funcinfo->funcRes[0].funcRes[i].fieldoffset + funcinfo->funcRes[0].funcRes[i].fieldlength;
					}
					
				}
				
			
			} //end for
			if (2==m_resConv )
			{
				//转换后的数据放至buf中
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
		else //多个应答
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
						//置所有字段值
						for (int iii=0;iii<funcinfo->funcRes[i].funcRes.size();iii++)
						{
							tmppublog.Add(" ");
						}
					}
					for (int ij=0;ij<funcinfo->funcRes[i].funcRes.size();ij++)
					{
						name = funcinfo->funcRes[i].funcRes[ij].name;//xml名称
						value = funcinfo->funcRes[i].funcRes[ij].value;//
						//包头
						if (strncmp(name.c_str(),"include",7)==0)
						{
							S_PACKHEAD_INFO *headinfo = m_interfaceConf.getHeadInfo(value);
							if (headinfo==NULL)
							{
								sprintf(m_errMsg,"交易码[%d]包头[%s]在接口配置中没有定义",txcode,value.c_str());
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
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
										return false;
									}
								}
								else
								{
									if (strncmp(headinfo->headinfo[j].name.c_str(),"交易码",6)==0)
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
											m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
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
									m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
									return false;
								}
							}
							else
							{
								if (strncmp(funcinfo->funcRes[i].funcRes[ij].name.c_str(),"交易码",6)==0)
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
										m_publog.LogMp(LOG_ERROR,__FILE__,__LINE__,"交易码%d %s",txcode,m_errMsg);
										return false;
									}
								}
								
								buflen = funcinfo->funcRes[i].funcRes[ij].fieldoffset + funcinfo->funcRes[i].funcRes[ij].fieldlength;
							}
							
						}
					
						
					}
					if (2==m_resConv )
					{
						//转换后的数据放至buf中
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
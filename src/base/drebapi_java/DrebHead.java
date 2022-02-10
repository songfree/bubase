package cheetah.core.net.bubase;

import java.lang.reflect.Field;
import com.sun.jna.Structure;

/**
 * dreb ���� DrebPubDefine.h
 *
 */
public class DrebHead extends Structure
{
	public DrebHead()
	{
		setAlignType(Structure.ALIGN_NONE);
		this.setNBodyLen(0);
	}
	
	//�ṹ�崫ָ��  
//    public static class ByReference extends DrebHead implements Structure.ByReference { } 
//    
//    public static class ByValue extends DrebHead implements Structure.ByValue {}
    
	//////////////////////////////////////////////////////////////// ͷ
	/** ѹ����־          0����  1des������Կ����16�����ܺ󳤶����� 2ѹ�� 3DES������Կ����8�����ܺ󳤶Ȳ��� 4DES���ܲ�ѹ��  */
	public byte              h_cZip = '\0';         
	/** �������߽ڵ����������֣�Ӧ��ʱ�����任    ͬ������8DPCALL �㲥��11CMD_DPABC��������12CMD_DPPUSH*/
	public byte              h_cCmd = '\0' ;         
	/** У��λ  */
	public  int               h_nCrc = 0;   
	/** ����Ӧ���־  0���� 1Ӧ��  */
	public byte              h_cRaflag = '\0' ;      
	/** ��������־  dreb֮�����·��ʹ��ʱ0�װ� 1������  ����ʱ0�װ� 1��������Ӧ��ʱ0�޺����� 1�к�����  */
	public byte              h_cNextFlag = '\0';    
	/** �������߽ڵ�ȷ�ϱ�־ 0�� 1�������߽ڵ�ȷ��  */
	public byte              h_cDrebAffirm = '\0';  
	/** Դ��Ϣ           �����󷽼������������߽ڵ���д  */
	
	/////////////////////////////////////////////////////////////// Դ�ڵ�
	/** �������߽ڵ��� �����ã��㲥Ϊ0 unsigned*/
	public  int s_nNodeId = 0 ;
	/** �������߽ڵ�˽�����  �����ã��㲥Ϊ0 */
	public byte  s_cNodePrivateId = '\0' ; 
	/** ע�����������߽ڵ��ϵ��������ܺ�   ֻ�з������ʱ������д �����ã��㲥Ϊ0  unsigned*/ 
	public  int   s_nSvrMainId = 0;  
	/** ע�����������߽ڵ��ϵ�˽����� 0��ʾ���� >0Ϊÿ���������ţ�s_nSvrMainId+s_cSvrPrivateId ��Ϊ˽�й��ܺ�   ֻ�з������ʱ������д �����ã��㲥Ϊ0*/
	public byte  s_cSvrPrivateId = '\0' ;  
	/** �ͻ���ʹ�õ��ֶΣ��������߽ڵ㲻�ã�ԭ�����ء�  �����ã�Ϊʱ���*/
	public  int   s_nHook = 0; 
	/** �첽ʱ��д����ˮ���������߽ڵ㲻�ã�ԭ������  �����ã�Ϊindex*/
	public  int   s_nSerial = 0; 
	/** �������߽ڵ��������ˮ����־Ψһ���������ݰ�  unsigned*/    
	public  int   s_nDrebSerial = 0;    
	/** ���ʹ�������������߽ڵ���������  unsigned*/
	public  int s_nIndex = 0;
	/** ������������ʱ�������Ӧ��ʱ��ͬ����Ϊ�����Ѷϡ����ǿͻ���Ӧ���������  */
	public  int   s_nTimestamp = 0;
	///////////////////////////////////////////////////////////// Ŀ��ڵ� 
	/** Ŀ���������߽ڵ��� �����ã��㲥Ϊ0  unsigned*/ 
	public  int d_nNodeId = 0;
	/** �������߽ڵ�˽����� �����ã��㲥Ϊ0*/ 
	public byte  d_cNodePrivateId = '\0'; 
	/** Ŀ������ܺ� ,������д �����ã��㲥Ϊ0  unsigned*/      
	public  int   d_nSvrMainId = 0;
	/** Ŀ��˽����� 0��ʾ���� >0Ϊÿ���������ţ�������д �����ã��㲥Ϊ0*/ 
	public byte  d_cSvrPrivateId = '\0';  
	/** Ҫ���õķ����ܺ� ������ unsigned*/  
	public  int   d_nServiceNo = 0;
	
	////////////////////////////////////////////////////////////  Ӧ����Ϣ 
	/** Ӧ����������߽ڵ�ڵ�ID unsigned*/
	public  int a_nNodeId = 0;
	/** �������߽ڵ�˽����� */
	public byte  a_cNodePrivateId = 0 ;
	/** Ӧ��ķ����� unsigned*/      
	public  int a_nRetCode = 0;	
	/////////////////////////////////////////////////////////// ��������־  
	/**��������� unsigned*/
	public  int n_nNextNo = 0;
	/**������λ�� unsigned*/     
	public  int n_nNextOffset = 0; 
	
	/////////////////////////////////////////////////////////// BPC��Ϣ
	/** BPC ʱ��� */
	public  int bpc_nTimestamp = 0;
	/** BPC ���� unsigned */
	public  int bpc_cIndex = 0;
	
	/** ��������ݳ��� unsigned */
	public   int nBodyLen=0;
	
	
    private short _iMsgFlag=BubaseConstant.MSG_FLAG_LITTLE_ENDIAN;
	
//	public int size()
//	{
//		return BubaseConstant.LEN_DREB_HEAD;		
//	}
    
	public void init()
	{
		this.h_cZip = '\0';
		this.h_cCmd = '\0' ;
		this.setH_nCrc(0);
		this.h_cRaflag = '\0' ;
		this.h_cNextFlag = '\0';  
		this.h_cDrebAffirm = '\0';  
		this.setS_nNodeId(0);
		this.s_cNodePrivateId = '\0' ;
		this.setS_nSvrMainId(0);
		this.s_cSvrPrivateId = '\0' ;
		this.setS_nHook(0);
		this.setS_nSerial(0);
		this.setS_nDrebSerial(0);
		this.setS_nIndex((short)0);
		this.setS_nTimestamp(0);
		this.setD_nNodeId((short)0);
		this.d_cNodePrivateId = '\0'; 
		this.setD_nSvrMainId(0);
		this.d_cSvrPrivateId = '\0';  
		this.setD_nServiceNo(0);
		this.setA_nNodeId((short)0);
		this.a_cNodePrivateId = 0 ;
		this.setA_nRetCode((short)0);
		this.setN_nNextNo(0);
		this.setN_nNextOffset(0);
		this.setBpc_nTimestamp(0);
		this.setBpc_cIndex((short)0);
		this.setNBodyLen(0);
		
	}
	
	public void initReq(DrebHead head)
	{
		this.h_cZip = head.h_cZip;
		this.h_cCmd = head.h_cCmd ;
		this.setH_nCrc(head.getH_nCrc());
		this.h_cRaflag = BubaseConstant.DREB_HEAD_RA_FLAG_RSP ;
		this.h_cNextFlag = BubaseConstant.DREB_HEAD_NEXT_FLAG_FIRST;  
		this.h_cDrebAffirm = head.h_cDrebAffirm;  
		this.setS_nNodeId(head.getS_nNodeId());
		this.s_cNodePrivateId = head.s_cNodePrivateId ;
		this.setS_nSvrMainId(head.getS_nSvrMainId());
		this.s_cSvrPrivateId = head.s_cSvrPrivateId ;
		this.setS_nHook(head.getS_nHook());
		this.setS_nSerial(head.getS_nSerial());
		this.setS_nDrebSerial(head.getS_nDrebSerial());
		this.setS_nIndex(head.getS_nIndex());
		this.setS_nTimestamp(head.getS_nTimestamp());
		this.setD_nNodeId(head.getD_nNodeId());
		this.d_cNodePrivateId = head.d_cNodePrivateId; 
		this.setD_nSvrMainId(head.getD_nSvrMainId());
		this.d_cSvrPrivateId = head.d_cSvrPrivateId;  
		this.setD_nServiceNo(head.getD_nServiceNo());
		this.setA_nNodeId(head.getA_nNodeId());
		this.a_cNodePrivateId = head.a_cNodePrivateId ;
		this.setA_nRetCode(head.getA_nRetCode());
		this.setN_nNextNo(head.getN_nNextNo());
		this.setN_nNextOffset(head.getN_nNextOffset());
		this.setBpc_nTimestamp(head.getBpc_nTimestamp());
		this.setBpc_cIndex(head.getBpc_cIndex());
	}
	
    /**
	 * ���ɱ���
	 * @param tagMsgs     ��ű��ĵ�����
	 * @param iStartIdx   ��ʼ���λ��
	 * return  ����
	 */
	public  int toBytes(byte[] tagMsgs,int iStartIdx) throws Exception
	{
		int i=iStartIdx;
		tagMsgs[i++]=h_cZip;
		tagMsgs[i++]=h_cCmd;
		System.arraycopy(h_nCrc, 0, tagMsgs, i, 4);
		i+=4;
		tagMsgs[i++]=h_cRaflag;
		tagMsgs[i++]=h_cNextFlag;
		tagMsgs[i++]=h_cDrebAffirm;
		System.arraycopy(s_nNodeId, 0, tagMsgs, i, 4);
		i+=4;
		tagMsgs[i++]=s_cNodePrivateId;
		System.arraycopy(s_nSvrMainId, 0, tagMsgs, i, 4);
		i+=4;
		tagMsgs[i++]=s_cSvrPrivateId;
		System.arraycopy(s_nHook, 0, tagMsgs, i, 4);
		i+=4;
		System.arraycopy(s_nSerial, 0, tagMsgs, i, 4);
		i+=4;
		System.arraycopy(s_nDrebSerial, 0, tagMsgs, i, 4);
		i+=4;
		System.arraycopy(s_nIndex, 0, tagMsgs, i, 4);
		i+=4;
		System.arraycopy(s_nTimestamp, 0, tagMsgs, i, 4);
		i+=4;
		System.arraycopy(d_nNodeId, 0, tagMsgs, i, 4);
		i+=4;
		tagMsgs[i++]=d_cNodePrivateId;
		System.arraycopy(d_nSvrMainId, 0, tagMsgs, i, 4);
		i+=4;
		tagMsgs[i++]=d_cSvrPrivateId;
		System.arraycopy(d_nServiceNo, 0, tagMsgs, i, 4);
		i+=4;
		System.arraycopy(a_nNodeId, 0, tagMsgs, i, 4);
		i+=4;
		tagMsgs[i++]=a_cNodePrivateId;
		System.arraycopy(a_nRetCode, 0, tagMsgs, i, 4);
		i+=4;
		System.arraycopy(n_nNextNo, 0, tagMsgs, i, 4);
		i+=4;
		System.arraycopy(n_nNextOffset, 0, tagMsgs, i, 4);
		i+=4;
		System.arraycopy(bpc_nTimestamp, 0, tagMsgs, i, 4);
		i+=4;
		System.arraycopy(bpc_cIndex, 0, tagMsgs, i, 4);
		i+=4;
		System.arraycopy(nBodyLen, 0, tagMsgs, i, 4);
		i+=4;
		return i-iStartIdx;
	}
	
	/**
	 * ��������
	 * @param msg
	 * @throws Exception
	 */
	public int parse(short iMsgFlag,byte[] msg,int iStartIdx) //throws Exception
	{
		_iMsgFlag=iMsgFlag;
		int i=iStartIdx;
		this.h_cZip=msg[i++];
		this.h_cCmd=msg[i++];
		System.arraycopy(msg, i, this.h_nCrc, 0, 4);
		i+=4;
		this.h_cRaflag=msg[i++];
		this.h_cNextFlag=msg[i++];
		this.h_cDrebAffirm=msg[i++];
		System.arraycopy(msg, i, this.s_nNodeId, 0, 4);
		i+=4;
		this.s_cNodePrivateId=msg[i++];
		System.arraycopy(msg, i, this.s_nSvrMainId, 0, 4);
		i+=4;
		this.s_cSvrPrivateId=msg[i++];
		System.arraycopy(msg, i, this.s_nHook, 0, 4);
		i+=4;
		System.arraycopy(msg, i, this.s_nSerial, 0, 4);
		i+=4;
		System.arraycopy(msg, i, this.s_nDrebSerial, 0, 4);
		i+=4;
		System.arraycopy(msg, i, this.s_nIndex, 0, 4);
		i+=4;
		System.arraycopy(msg, i, this.s_nTimestamp, 0, 4);
		i+=4;
		System.arraycopy(msg, i, this.d_nNodeId, 0, 4);
		i+=4;
		this.d_cNodePrivateId=msg[i++];
		System.arraycopy(msg, i, this.d_nSvrMainId, 0, 4);
		i+=4;
		this.d_cSvrPrivateId=msg[i++];
		System.arraycopy(msg, i, this.d_nServiceNo, 0, 4);
		i+=4;
		System.arraycopy(msg, i, this.a_nNodeId, 0, 4);
		i+=4;
		this.a_cNodePrivateId=msg[i++];
		System.arraycopy(msg, i, this.a_nRetCode, 0, 4);
		i+=4;
		System.arraycopy(msg, i, this.n_nNextNo, 0, 4);
		i+=4;
		System.arraycopy(msg, i, this.n_nNextOffset, 0, 4);
		i+=4;
		System.arraycopy(msg, i, this.bpc_nTimestamp, 0, 4);
		i+=4;
		System.arraycopy(msg, i, this.bpc_cIndex, 0, 4);
		i+=4;
		System.arraycopy(msg, i, this.nBodyLen, 0, 4);
		i+=4;
		return i-iStartIdx;
	}
	
	
	/**
	 * ת�����׶����ַ���
	 */
	public String toString()
	{
		StringBuffer sb = new StringBuffer();
		sb.append("h_cZip==>[" + h_cZip + "]")
		.append(",h_cCmd==>[" + this.h_cCmd + "]")
		.append(",h_nCrc=[" + this.getH_nCrc() + "]")
		.append(",h_cRaflag==>[" + this.h_cRaflag + "]")
		.append(",h_cNextFlag==>[" + this.h_cNextFlag + "]")
		.append(",h_cDrebAffirm==>[" + this.h_cDrebAffirm + "]")
		.append(",s_nNodeId==>[" + this.getS_nNodeId() + "]")
		.append(",s_cNodePrivateId==>[" + this.s_cNodePrivateId + "]")
		.append(",s_nSvrMainId==>[" + this.getS_nSvrMainId() + "]")
		.append(",s_cSvrPrivateId==>[" + this.s_cSvrPrivateId + "]")
		.append(",s_nHook==>[" + this.getS_nHook() + "]")
		.append(",s_nSerial==>[" + this.getS_nSerial() + "]")
		.append(",s_nDrebSerial==>[" + this.getS_nDrebSerial() + "]")
		.append(",s_nIndex==>[" + this.getS_nIndex() + "]")
		.append(",s_nTimestamp==>[" + this.getS_nTimestamp() + "]")
		.append(",d_nNodeId==>[" + this.getD_nNodeId() + "]")
		.append(",d_cNodePrivateId==>[" + this.getD_cNodePrivateId() + "]")
		.append(",d_nSvrMainId==>[" + this.getD_nSvrMainId() + "]")
		.append(",d_cSvrPrivateId==>[" + this.getD_cSvrPrivateId() + "]")
		.append(",d_nServiceNo==>[" + this.getD_nServiceNo() + "]")
		.append(",a_nNodeId==>[" + this.getA_nNodeId() + "]")
		.append(",a_cNodePrivateId==>[" + this.getA_cNodePrivateId() + "]")
		.append(",a_nRetCode==>[" + this.getA_nRetCode() + "]")
		.append(",n_nNextNo==>[" + this.getN_nNextNo() + "]")
		.append(",n_nNextOffset==>[" + this.getN_nNextOffset() + "]")
		.append(",bpc_nTimestamp==>[" + this.getBpc_nTimestamp() + "]")
		.append(",bpc_cIndex==>[" + this.getBpc_cIndex() + "]")
		.append(",nBodyLen==>[" + this.getNBodyLen() + "]")
		;
		return sb.toString();
	}
	
	
	public byte getH_cZip()
	{
		return h_cZip;
	}

	public void setH_cZip(byte zip)
	{
		h_cZip = zip;
	}

	public byte getH_cCmd()
	{
		return h_cCmd;
	}

	public void setH_cCmd(byte cmd)
	{
		h_cCmd = cmd;
	}

	public int getH_nCrc()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag,h_nCrc,0);
		return h_nCrc;
	}

	public void setH_nCrc(int crc)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag,crc,this.h_nCrc,0);
		this.h_nCrc = crc;
	}

	public byte getH_cRaflag()
	{
		return h_cRaflag;
	}

	public void setH_cRaflag(byte raflag)
	{
		h_cRaflag = raflag;
	}

	public byte getH_cNextFlag()
	{
		return h_cNextFlag;
	}

	public void setH_cNextFlag(byte nextFlag)
	{
		h_cNextFlag = nextFlag;
	}

	public byte getH_cDrebAffirm()
	{
		return h_cDrebAffirm;
	}

	public void setH_cDrebAffirm(byte drebAffirm)
	{
		h_cDrebAffirm = drebAffirm;
	}

	public int getS_nNodeId()
	{
//		return BubaseUtil.byteToUShort(_iMsgFlag,s_nNodeId,0);
		return s_nNodeId;
	}

	public void setS_nNodeId(int nodeId)
	{
//		BubaseUtil.setUShortValueToBytes(_iMsgFlag,nodeId,this.s_nNodeId,0);
		this.s_nNodeId = nodeId;
	}

	public byte getS_cNodePrivateId()
	{
		return s_cNodePrivateId;
	}

	public void setS_cNodePrivateId(byte nodePrivateId)
	{
		s_cNodePrivateId = nodePrivateId;
	}

	public int getS_nSvrMainId()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag,s_nSvrMainId,0);
		return s_nSvrMainId;
	}

	public void setS_nSvrMainId(int svrMainId)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag,svrMainId,this.s_nSvrMainId,0);
		this.s_nSvrMainId = svrMainId;
	}

	public byte getS_cSvrPrivateId()
	{
		return s_cSvrPrivateId;
	}

	public void setS_cSvrPrivateId(byte svrPrivateId)
	{
		s_cSvrPrivateId = svrPrivateId;
	}

	public int getS_nHook()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag,s_nHook,0);
		return s_nHook;
	}

	public void setS_nHook(int hook)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag,hook,this.s_nHook,0);
		this.s_nHook = hook;
	}

	public int getS_nSerial()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag,s_nSerial,0);
		return s_nSerial;
	}

	public void setS_nSerial(int serial)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag,serial,this.s_nSerial,0);
		this.s_nSerial = serial;
	}

	public int getS_nDrebSerial()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag,s_nDrebSerial,0);
		return this.s_nDrebSerial;
	}

	public void setS_nDrebSerial(int drebSerial)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag,drebSerial,this.s_nDrebSerial,0);
		this.s_nDrebSerial = drebSerial;
	}

	public short getS_nIndex()
	{
//		return BubaseUtil.byteToShort(_iMsgFlag,s_nIndex,0);
		return (short)this.s_nIndex;
	}

	public void setS_nIndex(short index)
	{
//		BubaseUtil.setShortValueToBytes(_iMsgFlag,index,this.s_nIndex,0);
		this.s_nIndex = index;
	}

	public int getS_nTimestamp()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag,s_nTimestamp,0);
		return this.s_nTimestamp;
	}

	public void setS_nTimestamp(int timestamp)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag,timestamp,this.s_nTimestamp,0);
		this.s_nTimestamp = timestamp;
	}

	public short getD_nNodeId()
	{
//		return BubaseUtil.byteToShort(_iMsgFlag,d_nNodeId,0);
		return (short)d_nNodeId;
	}

	public void setD_nNodeId(short nodeId)
	{
//		BubaseUtil.setShortValueToBytes(_iMsgFlag,nodeId,this.d_nNodeId,0);
		d_nNodeId = nodeId;
	}

	public byte getD_cNodePrivateId()
	{
		return d_cNodePrivateId;
	}

	public void setD_cNodePrivateId(byte nodePrivateId)
	{
		d_cNodePrivateId = nodePrivateId;
	}

	public int getD_nSvrMainId()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag,d_nSvrMainId,0);
		return d_nSvrMainId;
	}

	public void setD_nSvrMainId(int svrMainId)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag,svrMainId,this.d_nSvrMainId,0);
		this.d_nSvrMainId = svrMainId;
	}

	public byte getD_cSvrPrivateId()
	{
		return d_cSvrPrivateId;
	}

	public void setD_cSvrPrivateId(byte svrPrivateId)
	{
		d_cSvrPrivateId = svrPrivateId;
	}

	public int getD_nServiceNo()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag,d_nServiceNo,0);
		return d_nServiceNo;
	}

	public void setD_nServiceNo(int serviceNo)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag,serviceNo,this.d_nServiceNo,0);
		d_nServiceNo = serviceNo;
	}

	public short getA_nNodeId()
	{
//		return BubaseUtil.byteToShort(_iMsgFlag,a_nNodeId,0);
		return (short)a_nNodeId;
	}

	public void setA_nNodeId(short nodeId)
	{
//		BubaseUtil.setShortValueToBytes(_iMsgFlag,nodeId,this.a_nNodeId,0);
		this.a_nNodeId = nodeId;
	}

	public byte getA_cNodePrivateId()
	{
		return a_cNodePrivateId;
	}

	public void setA_cNodePrivateId(byte nodePrivateId)
	{
		a_cNodePrivateId = nodePrivateId;
	}

	public short getA_nRetCode()
	{
//		return BubaseUtil.byteToShort(_iMsgFlag,a_nRetCode,0);
		return (short)a_nRetCode;
	}

	public void setA_nRetCode(short retCode)
	{
//		BubaseUtil.setShortValueToBytes(_iMsgFlag,retCode,this.a_nRetCode,0);
		this.a_nRetCode = retCode;
	}

	public int getN_nNextNo()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag,n_nNextNo,0);
		return n_nNextNo;
	}

	public void setN_nNextNo(int nextNo)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag,nextNo,this.n_nNextNo,0);
		this.n_nNextNo = nextNo;
	}

	public int getN_nNextOffset()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag,n_nNextOffset,0);
		return n_nNextOffset;
	}

	public void setN_nNextOffset(int nextOffset)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag,nextOffset,this.n_nNextOffset,0);
		this.n_nNextOffset = nextOffset;
	}

	public int getBpc_nTimestamp()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag,bpc_nTimestamp,0);
		return bpc_nTimestamp;
	}

	public void setBpc_nTimestamp(int bpc_nTimestamp)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag,bpc_nTimestamp,this.bpc_nTimestamp,0);
		this.bpc_nTimestamp = bpc_nTimestamp;
	}

	public short getBpc_cIndex()
	{
//		return BubaseUtil.byteToShort(_iMsgFlag,bpc_cIndex,0);
		return (short)bpc_cIndex;
	}

	public void setBpc_cIndex(short bpc_cIndex)
	{
//		BubaseUtil.setShortValueToBytes(_iMsgFlag,bpc_cIndex,this.bpc_cIndex,0);
		this.bpc_cIndex = bpc_cIndex;
	}

	public int getNBodyLen()
	{
//		return BubaseUtil.byteToUShort(_iMsgFlag,nBodyLen,0);
		return nBodyLen;
	}

	public void setNBodyLen(int bodyLen)
	{
//		BubaseUtil.setUShortValueToBytes(_iMsgFlag,bodyLen,this.nBodyLen,0);
		this.nBodyLen = bodyLen;
	}

	public short get_iMsgFlag()
	{
		return _iMsgFlag;
	}

	public void set_iMsgFlag(short msgFlag)
	{
		_iMsgFlag = msgFlag;
	}
	
	public static void main(String [] args)
	{
		Field[] fields = DrebHead.class.getFields();
		for(Field f : fields)
		{
			System.out.println(f.getName());
		}
	}
}

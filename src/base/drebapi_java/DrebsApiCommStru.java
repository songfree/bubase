package cheetah.core.net.drebapi.jni;

import java.lang.reflect.Field;

import cheetah.core.net.NetMsg;
import cheetah.core.net.bubase.BubaseConstant;
import cheetah.core.net.bubase.DrebHead;

public class DrebsApiCommStru
{

	public  byte		cMsgType	= '\0';											; //��Ϣ����  �����߷���������Ϣ���ͣ�����ʱ���á�
	public  int	index		= 0;									//��dreb���߹���������������ָ��Ҫ�������ݵ���������(�������ӵ�������˳����ѡ��)
	public  DrebHead	sDBHead		= new DrebHead();								//drebͷ
	public  byte[]	sBuffer		= new byte[BubaseConstant.LEN_BPU_DATA_SIZE];
	
	private short	_iMsgFlag	= BubaseConstant.MSG_FLAG_LITTLE_ENDIAN;

	public DrebsApiCommStru()
	{
	}


	
//	public int size()
//	{
//		int size=0;
//		size=1+4+68+sBuffer.length;
//		return size;
//		
//	}
	
	public void init()
	{
		this.cMsgType= '\0';		
		this.setIndex(0);
		this.sDBHead.init();
	}
	
	
	/**
	 * ���ɱ���
	 * @param tagMsgs     ��ű��ĵ�����
	 * @param iStartIdx   ��ʼ���λ��
	 * return  ����
	 */
	public int toBytes(byte[] tagMsgs, int iStartIdx) throws Exception
	{

//		int nBodyLen = BubaseUtil.byteToUShort(_iMsgFlag, sDBHead.nBodyLen, 0);
		int nBodyLen = sDBHead.nBodyLen;
		int i = iStartIdx;
		tagMsgs[i++] = cMsgType;
		System.arraycopy(index, 0, tagMsgs, i, 4);
		i += 4;
		int len = sDBHead.toBytes(tagMsgs, i);
		i += len;
		if(nBodyLen>0)
		{
			System.arraycopy(sBuffer, 0, tagMsgs, i, nBodyLen);
			i += nBodyLen;
		}

		return i - iStartIdx;
	}

	/**
	 * ת�����׶����ַ���
	 */
	public String toString()
	{
		StringBuffer sb = new StringBuffer();
		sb.append("cMsgType==>[" + cMsgType + "]")
		.append(",index==>[" + this.getIndex() + "]")
		.append(",sDBHead=[" + sDBHead.toString() + "]");
//		int nBodyLen = BubaseUtil.byteToUShort(_iMsgFlag, sDBHead.nBodyLen, 0);
		int nBodyLen = sDBHead.nBodyLen;
		if(nBodyLen>0)
		{
			sb.append(",body=[" + new String(this.sBuffer,0,nBodyLen) + "]");
		}
			
		return sb.toString();
	}

	/**
	 * ��������
	 * @param msg
	 * @throws Exception
	 */
	public int parse(short iMsgFlag, byte[] msg, int iStartIdx) //throws Exception
	{
		_iMsgFlag = iMsgFlag;
		sDBHead.set_iMsgFlag(iMsgFlag);
		int i = iStartIdx;
		this.cMsgType = msg[i++];
		System.arraycopy(msg, i, this.index, 0, 4);
		i += 4;
		int len = sDBHead.parse(iMsgFlag, msg, i);
		i += len;
//		int nBodyLen = BubaseUtil.byteToUShort(iMsgFlag, sDBHead.nBodyLen, 0);
		int nBodyLen = sDBHead.nBodyLen;
		if(nBodyLen>0)
		{
			System.arraycopy(msg, i, this.sBuffer, 0, nBodyLen);
			i += nBodyLen;	
		}else
		{
			
		}
		return i - iStartIdx;
	}

	public byte getCMsgType()
	{
		return cMsgType;
	}

	public void setCMsgType(byte msgType)
	{
		cMsgType = msgType;
	}

	public int getIndex()
	{
//		return BubaseUtil.byteToInt(_iMsgFlag, index, 0);
		return this.index;
	}

	public void setIndex(int index)
	{
//		BubaseUtil.setIntValueToBytes(_iMsgFlag, index, this.index, 0);
		this.index = index;
	}

	public DrebHead getSDBHead()
	{
		return sDBHead;
	}

	public void setSDBHead(DrebHead head)
	{
		sDBHead.initReq(head) ;
	}

	public byte[] getSBuffer()
	{
		return sBuffer;
	}

	public void setSBuffer(byte[] buffer)
	{
		System.arraycopy(buffer, 0, this.sBuffer, 0, buffer.length);
//		BubaseUtil.setUShortValueToBytes(_iMsgFlag,buffer.length,this.sDBHead.nBodyLen,0);
		this.sDBHead.nBodyLen = buffer.length;
	}
	
	/**
	 * ���Ψһ��Key
	 */
	public String getUniqueKey()
	{
		return this.sDBHead.getS_nHook()+":"+this.sDBHead.getS_nSerial();
	}
	
	public void toNetMsg(NetMsg netMsg)
	{
		int len=this.sDBHead.getNBodyLen();
		netMsg.srcMsgConent=new byte[len];
		System.arraycopy(this.sBuffer, 0, netMsg.srcMsgConent, 0, len);
		int type=this.getCMsgType();
		if(type==BubaseConstant.MSG_REQ) //������������͡��㲥
		{
			if(this.sDBHead.h_cCmd==BubaseConstant.DREB_HEAD_CMD_DPABC)
			{
				netMsg.cMsgType=NetMsg.MSG_TYPE_BRO;
			}
			else if(this.sDBHead.h_cCmd==BubaseConstant.DREB_HEAD_CMD_DPPUSH)
			{
				netMsg.cMsgType=NetMsg.MSG_TYPE_PUSH;
			}
			else 
			{
				netMsg.cMsgType=NetMsg.MSG_TYPE_REQ;
			}
		}
		else if(type==BubaseConstant.MSG_DREBANS)
		{
			netMsg.cMsgType=NetMsg.MSG_TYPE_RSP;	
		}
		else 
		{
			netMsg.cMsgType=NetMsg.MSG_TYPE_REQ;
		}
		
		netMsg.iRetCode=this.sDBHead.getA_nRetCode();
		netMsg.d_nServiceNo=this.sDBHead.getD_nServiceNo();
		netMsg.setUniquekey(getUniqueKey());
	}
	
	public void fromNetMsg(NetMsg netMsg)
	{
		this.sDBHead.setD_nServiceNo(netMsg.d_nServiceNo);
		System.arraycopy(netMsg.srcMsgConent, 0, this.sBuffer, 0, netMsg.srcMsgConent.length);
		this.sDBHead.setNBodyLen(netMsg.srcMsgConent.length);
		netMsg.setUniquekey(getUniqueKey());

	}

	public static void main(String [] args)
	{
		Field[] fields = DrebsApiCommStru.class.getFields();
		for(Field f : fields)
		{
			System.out.println(f.getName());
		}
	}
}

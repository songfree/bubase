#include "public.h"
#ifdef _WINDOWS
#include "jni_w.h"
#include "jni_md_w.h"
#else
#include "jni.h"
#include "jni_md.h"
#endif

#include "DrebApi.h"
#include "BF_DrebServerApi.h"
#include "IErrLog.h"
#include "jni_DrebApiInterface.h"
#include "DrebApiML.h"


#ifdef _WINDOWS
#ifdef _DEBUG
#pragma comment(lib, "bf_drebapid.lib") 
#pragma comment(lib, "bf_kerneld.lib") 
#pragma comment(lib, "bf_dbpubd.lib") 
#pragma message("Automatically linking with  bf_dbpubd.lib bf_kerneld.lib bf_drebapid.lib")
#else
#pragma comment(lib, "bf_drebapi.lib") 
#pragma comment(lib, "bf_kernel.lib") 
#pragma comment(lib, "bf_dbpub.lib") 
#pragma message("Automatically linking with  bf_dbpub.lib bf_kernel.lib bf_drebapi.lib")
#endif

#endif


CDrebApiML  g_drebapi_list_poing;

//��java������Ӧjava�ķ���Ϊ drebApiSendMsg(Object pClass,DrebsApiCommStru data); ��Ӧ��JNI Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiSendMsg(JNIEnv * env, jobject thiz, jobject pClass, jobject data)
//java���ݽṹת����c�ṹ
int setJavaDataToCData(JNIEnv* env, jobject objdata, DREBSAPI_COMMSTRU &data,CIErrlog *log )
{
	
    jmethodID methodId;
    //���objdata����ľ��
    jclass cls_objClass = env->FindClass(
        "cheetah/core/net/drebapi/jni/DrebsApiCommStru");
    if (0 == cls_objClass)
    {
		log->LogMp(LOG_ERROR,__FILE__,__LINE__,"GetObjectClass failed ");
        return -1;
    }
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���cMsgType����");  
    //���cMsgType����
    jfieldID cMsgFieldID = env->GetFieldID(cls_objClass, "cMsgType", "B");
    data.cMsgType = (char) env->GetByteField(objdata, cMsgFieldID);
//  __android_log_print(ANDROID_LOG_INFO, gszTAG,
//   "data.head.stComm.cMsgType = %d", data.head.stComm.cMsgType);  

    //atoi(char*) ת����int���ͣ� atol(char*)ת����long���ͣ�atof(char*)ת����double����
    //���index����
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���index����");  
    jfieldID indexFieldID = env->GetFieldID(cls_objClass, "index", "I");
    data.index = (unsigned int)env->GetIntField(objdata, indexFieldID);

    
//###################��ȡsDBHead����
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���sDBHead����");  
    //���sDBHead����
    jfieldID sDBHeadFieldID = env->GetFieldID(cls_objClass, "sDBHead", "Lcheetah/core/net/bubase/DrebHead;");
    jobject  sDBHeadObj = env->GetObjectField(objdata, sDBHeadFieldID);
    if(sDBHeadObj == NULL)
    {
    	printf("GetObjectClass Lcheetah/core/net/bubase/DrebHead failed \n");
    	return -1;
    }
    jclass sDBHeadCls = env->GetObjectClass(sDBHeadObj);
	
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���cZip����");  
    //���cZip����
    jfieldID cZipFieldID = env->GetFieldID(sDBHeadCls, "h_cZip", "B");
    data.sDBHead.cZip = (char) env->GetByteField(sDBHeadObj, cZipFieldID);

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���cCmd����");  
    //���cCmd����
    jfieldID cCmdFieldID = env->GetFieldID(sDBHeadCls, "h_cCmd", "B");
    data.sDBHead.cCmd = (char) env->GetByteField(sDBHeadObj, cCmdFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG, "data.head.stComm.cCmd = %d",
//        data.head.stComm.cCmd);

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���nCrc����");  
    //���nCrc����
    jfieldID nCrcFieldID = env->GetFieldID(sDBHeadCls, "h_nCrc", "I");
    data.sDBHead.nCrc = env->GetIntField(sDBHeadObj,nCrcFieldID);


	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���cRaflag����");
    //���cRaflag����
    jfieldID cRaflagFieldID = env->GetFieldID(sDBHeadCls, "h_cRaflag", "B");
    data.sDBHead.cRaflag = (char) env->GetByteField(sDBHeadObj, cRaflagFieldID);


	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���cNextFlag����");
    //���cNextFlag����
    jfieldID cNextFlagFieldID = env->GetFieldID(sDBHeadCls, "h_cNextFlag", "B");
    data.sDBHead.cNextFlag = (char) env->GetByteField(sDBHeadObj, cNextFlagFieldID);

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���cDrebAffirm����");
    //���cDrebAffirm����
    jfieldID cDrebAffirmFieldID = env->GetFieldID(sDBHeadCls, "h_cDrebAffirm", "B");
    data.sDBHead.cDrebAffirm = (char) env->GetByteField(sDBHeadObj, cDrebAffirmFieldID);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���s_nNodeId����");
    //���s_nNodeId����
    jfieldID s_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "s_nNodeId", "I");
    data.sDBHead.s_Sinfo.s_nNodeId = (unsigned short)env->GetIntField(sDBHeadObj,s_nNodeIdFieldID);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���s_cNodePrivateId����");
    //���s_cNodePrivateId����
    jfieldID s_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "s_cNodePrivateId", "B");
    data.sDBHead.s_Sinfo.s_cNodePrivateId = (char) env->GetByteField(sDBHeadObj, s_cNodePrivateIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���s_nSvrMainId�������");
    //���s_nSvrMainId�������
    jfieldID s_nSvrMainIdFieldID = env->GetFieldID(sDBHeadCls, "s_nSvrMainId", "I");
    data.sDBHead.s_Sinfo.s_nSvrMainId = (unsigned int)env->GetIntField(sDBHeadObj,s_nSvrMainIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���s_cSvrPrivateId�������");
    //���s_cSvrPrivateId�������
    jfieldID s_cSvrPrivateIdFieldID = env->GetFieldID(sDBHeadCls, "s_cSvrPrivateId", "B");
    data.sDBHead.s_Sinfo.s_cSvrPrivateId = (char) env->GetByteField(sDBHeadObj, s_cSvrPrivateIdFieldID);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���s_nHook�������");
    //���s_nHook�������
    jfieldID s_nHookFieldID = env->GetFieldID(sDBHeadCls, "s_nHook", "I");
    data.sDBHead.s_Sinfo.s_nHook = env->GetIntField(sDBHeadObj,s_nHookFieldID);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���s_nSerial�������");
    //���s_nSerial�������
    jfieldID s_nSerialFieldID = env->GetFieldID(sDBHeadCls, "s_nSerial", "I");
    data.sDBHead.s_Sinfo.s_nSerial = env->GetIntField(sDBHeadObj,s_nSerialFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���s_nIndex�������");
    //���s_nIndex�������
    jfieldID s_nIndexFieldID = env->GetFieldID(sDBHeadCls, "s_nIndex", "I");
    data.sDBHead.s_Sinfo.s_nIndex = env->GetIntField(sDBHeadObj,s_nIndexFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���s_nTimestamp�������");
    //���s_nTimestamp�������
    jfieldID s_nTimestampFieldID = env->GetFieldID(sDBHeadCls, "s_nTimestamp", "I");
    data.sDBHead.s_Sinfo.s_nTimestamp = env->GetIntField(sDBHeadObj,s_nTimestampFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���d_nNodeId�������");
    //���d_nNodeId�������
    jfieldID d_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "d_nNodeId", "I");
    data.sDBHead.d_Dinfo.d_nNodeId = env->GetIntField(sDBHeadObj,d_nNodeIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���d_cNodePrivateId�������");
    //���d_cNodePrivateId�������
    jfieldID d_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "d_cNodePrivateId", "B");
    data.sDBHead.d_Dinfo.d_cNodePrivateId = (char) env->GetByteField(sDBHeadObj, d_cNodePrivateIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���d_nSvrMainId�������");
    //���d_nSvrMainId�������
    jfieldID d_nSvrMainIdFieldID = env->GetFieldID(sDBHeadCls, "d_nSvrMainId", "I");
    data.sDBHead.d_Dinfo.d_nSvrMainId = env->GetIntField(sDBHeadObj,d_nSvrMainIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���d_cSvrPrivateId�������");
    //���d_cSvrPrivateId�������
    jfieldID d_cSvrPrivateIdFieldID = env->GetFieldID(sDBHeadCls, "d_cSvrPrivateId", "B");
    data.sDBHead.d_Dinfo.d_cSvrPrivateId = (char) env->GetByteField(sDBHeadObj, d_cSvrPrivateIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���d_nServiceNo�������");
    //���d_nServiceNo�������
    jfieldID d_nServiceNoFieldID = env->GetFieldID(sDBHeadCls, "d_nServiceNo", "I");
    data.sDBHead.d_Dinfo.d_nServiceNo = env->GetIntField(sDBHeadObj,d_nServiceNoFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���a_nNodeId�������");
    //���a_nNodeId�������
    jfieldID a_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "a_nNodeId", "I");
    data.sDBHead.a_Ainfo.a_nNodeId = env->GetIntField(sDBHeadObj,a_nNodeIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���a_cNodePrivateId�������");
    //���a_cNodePrivateId�������
    jfieldID a_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "a_cNodePrivateId", "B");
    data.sDBHead.a_Ainfo.a_cNodePrivateId = (char) env->GetByteField(sDBHeadObj, a_cNodePrivateIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���a_nRetCode�������");
    //���a_nRetCode�������
    jfieldID a_nRetCodeFieldID = env->GetFieldID(sDBHeadCls, "a_nRetCode", "I");
    data.sDBHead.a_Ainfo.a_nRetCode = env->GetIntField(sDBHeadObj,a_nRetCodeFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���n_nNextNo�������");
    //���n_nNextNo�������
    jfieldID n_nNextNoFieldID = env->GetFieldID(sDBHeadCls, "n_nNextNo", "I");
    data.sDBHead.n_Ninfo.n_nNextNo = env->GetIntField(sDBHeadObj,n_nNextNoFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���n_nNextOffset�������");
    //���n_nNextOffset�������
    jfieldID n_nNextOffsetFieldID = env->GetFieldID(sDBHeadCls, "n_nNextOffset", "I");
    data.sDBHead.n_Ninfo.n_nNextOffset = (unsigned int)env->GetIntField(sDBHeadObj,n_nNextOffsetFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���b_nSerial�������");
    //���b_nSerial�������
    jfieldID b_nSerialFieldID = env->GetFieldID(sDBHeadCls, "bpc_nTimestamp", "I");
    data.sDBHead.b_Cinfo.b_nSerial = env->GetIntField(sDBHeadObj,b_nSerialFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���b_cIndex�������");
    //���b_cIndex�������
    jfieldID b_cIndexFieldID = env->GetFieldID(sDBHeadCls, "bpc_cIndex", "I");
    data.sDBHead.b_Cinfo.b_cIndex = (unsigned short)env->GetIntField(sDBHeadObj,b_cIndexFieldID);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���nLen�������");
    //���nLen�������
    jfieldID nLenFieldID = env->GetFieldID(sDBHeadCls, "nBodyLen", "I");
    data.sDBHead.nLen = (unsigned short)env->GetIntField(sDBHeadObj,nLenFieldID);

    
    //#####################����nBodyLen��ȡsBuffer�ĳ���
        // ���sBuffer����
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"���sBuffer����");  
    jfieldID bufferFieldID = env->GetFieldID(cls_objClass, "sBuffer", "[B");
    jbyteArray sBuffer = (jbyteArray) env->GetObjectField(objdata,
        bufferFieldID);

    //��sBuffer���и�ֵ
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��sBuffer���и�ֵ");  
    jbyte * olddata = (jbyte*) env->GetByteArrayElements(sBuffer, 0);
   // int nArrLen = env->GetArrayLength(sBuffer);  ����ֱ������ȡ����ΪsBuffer��������С�ǹ̶���
    int nArrLen = data.sDBHead.nLen;
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"nArrLen:%d",nArrLen); 
	if (nArrLen >0 && nArrLen <BPUDATASIZE )
	{
		memcpy(data.sBuffer, (const char*) olddata, nArrLen);
	}
	else if (nArrLen >=BPUDATASIZE ) 
	{
		log->LogMp(LOG_ERROR,__FILE__,__LINE__,"���ݳ��ȷǷ�");
		return -1;
	}
	else
	{
		data.sBuffer[0]=0;
	}
//	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʼReleaseByteArrayElements");  
    env->ReleaseByteArrayElements(sBuffer, olddata, 0);

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�ͷ�");
    //�ͷ�
    env->DeleteLocalRef(sDBHeadCls);
    env->DeleteLocalRef(cls_objClass);
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"setJavaDataToCData���");
    return 0;
}
  
    
  //c���ݽṹת����java���ݽṹ
int setCDataToJavaData(JNIEnv* env, jobject objdata, DREBSAPI_COMMSTRU data,CIErrlog *log)
{
    jmethodID methodId;
    //���objdata����ľ��
    jclass cls_objClass = env->FindClass(
        "cheetah/core/net/drebapi/jni/DrebsApiCommStru");
    if (0 == cls_objClass)
    {
        printf("GetObjectClass failed \n");
		log->LogMp(LOG_ERROR,__FILE__,__LINE__,"GetObjectClass failed");
        return -1;
    }

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����cMsgType����");
    //����cMsgType����
    jfieldID cMsgFieldID = env->GetFieldID(cls_objClass, "cMsgType", "B");
    env->SetByteField(objdata, cMsgFieldID, (jbyte) data.cMsgType);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����index����");
    //����index����
    jfieldID indexFieldID = env->GetFieldID(cls_objClass, "index", "I");
    env->SetIntField(objdata, indexFieldID, (jint) data.index);
    

    //#######################################################33
    //���濪ʼ�����Զ�������sDBHead��ֵ

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"�Ȼ��sDBHead����");

    //�Ȼ��sDBHead����
    jfieldID sDBHeadFieldID = env->GetFieldID(cls_objClass, "sDBHead", "Lcheetah/core/net/bubase/DrebHead;");
    jobject  sDBHeadObj = env->GetObjectField(objdata, sDBHeadFieldID);
    if(sDBHeadObj == NULL)
    {
    	printf("GetObjectClass Lcheetah/core/net/bubase/DrebHead failed \n");
		log->LogMp(LOG_ERROR,__FILE__,__LINE__,"GetObjectClass Lcheetah/core/net/bubase/DrebHead failed");
    	return -1;
    }
    jclass sDBHeadCls = env->GetObjectClass(sDBHeadObj);
    
    //����h_cZip�������
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����h_cZip�������");
    jfieldID h_cZipFieldID = env->GetFieldID(sDBHeadCls, "h_cZip", "B");
    env->SetByteField(sDBHeadObj, h_cZipFieldID, (jbyte) data.sDBHead.cZip);

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����cCmd����");
    //���cCmd����
    jfieldID cCmdFieldID = env->GetFieldID(sDBHeadCls, "h_cCmd", "B");
    env->SetByteField(sDBHeadObj, h_cZipFieldID, (jbyte) data.sDBHead.cCmd);


    //���nCrc����
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����nCrc����");
    jfieldID nCrcFieldID = env->GetFieldID(sDBHeadCls, "h_nCrc", "I");
    env->SetIntField(sDBHeadObj, nCrcFieldID, (jint) data.sDBHead.nCrc);


    //���cRaflag����
    jfieldID cRaflagFieldID = env->GetFieldID(sDBHeadCls, "h_cRaflag", "B");
    env->SetByteField(sDBHeadObj, cRaflagFieldID, (jbyte) data.sDBHead.cRaflag);


    //���cNextFlag����
    jfieldID cNextFlagFieldID = env->GetFieldID(sDBHeadCls, "h_cNextFlag", "B");
    env->SetByteField(sDBHeadObj, cNextFlagFieldID, (jbyte) data.sDBHead.cNextFlag);


    //���cDrebAffirm����
    jfieldID cDrebAffirmFieldID = env->GetFieldID(sDBHeadCls, "h_cDrebAffirm", "B");
    env->SetByteField(sDBHeadObj, cDrebAffirmFieldID, (jbyte) data.sDBHead.cDrebAffirm);
    
    //���s_nNodeId����
    jfieldID s_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "s_nNodeId", "I");
    env->SetIntField(sDBHeadObj, s_nNodeIdFieldID, (jint) data.sDBHead.s_Sinfo.s_nNodeId);

    
    //���s_cNodePrivateId����
    jfieldID s_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "s_cNodePrivateId", "B");
    env->SetByteField(sDBHeadObj, s_cNodePrivateIdFieldID, (jbyte) data.sDBHead.s_Sinfo.s_cNodePrivateId);
    
    
    //���s_nSvrMainId�������
    jfieldID s_nSvrMainIdFieldID = env->GetFieldID(sDBHeadCls, "s_nSvrMainId", "I");
    env->SetIntField(sDBHeadObj, s_nSvrMainIdFieldID, (jint) data.sDBHead.s_Sinfo.s_nSvrMainId);
    
    
    //���s_cSvrPrivateId�������
    jfieldID s_cSvrPrivateIdFieldID = env->GetFieldID(sDBHeadCls, "s_cSvrPrivateId", "B");
    env->SetByteField(sDBHeadObj, s_cSvrPrivateIdFieldID, (jbyte) data.sDBHead.s_Sinfo.s_cSvrPrivateId);
    
    //���s_nHook�������
    jfieldID s_nHookFieldID = env->GetFieldID(sDBHeadCls, "s_nHook", "I");
    env->SetIntField(sDBHeadObj, s_nHookFieldID, (jint) data.sDBHead.s_Sinfo.s_nHook);
    
    //���s_nSerial�������
    jfieldID s_nSerialFieldID = env->GetFieldID(sDBHeadCls, "s_nSerial", "I");
    env->SetIntField(sDBHeadObj, s_nSerialFieldID, (jint) data.sDBHead.s_Sinfo.s_nSerial);  
    
    
    //���s_nDrebSerial�������                                                                           
    jfieldID s_nDrebSerialFieldID = env->GetFieldID(sDBHeadCls, "s_nDrebSerial", "I"); 
    env->SetIntField(sDBHeadObj, s_nDrebSerialFieldID, (jint) data.sDBHead.s_Sinfo.s_nDrebSerial);                       

      
    //���s_nIndex�������
    jfieldID s_nIndexFieldID = env->GetFieldID(sDBHeadCls, "s_nIndex", "I");
    env->SetIntField(sDBHeadObj, s_nIndexFieldID, (jint) data.sDBHead.s_Sinfo.s_nIndex);  
    
    
    //���s_nTimestamp�������
    jfieldID s_nTimestampFieldID = env->GetFieldID(sDBHeadCls, "s_nTimestamp", "I");
    env->SetIntField(sDBHeadObj, s_nTimestampFieldID, (jint) data.sDBHead.s_Sinfo.s_nTimestamp);  
    
    
    //���d_nNodeId�������
    jfieldID d_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "d_nNodeId", "I");
    env->SetIntField(sDBHeadObj, d_nNodeIdFieldID, (jint) data.sDBHead.d_Dinfo.d_nNodeId);  
    
    
    //���d_cNodePrivateId�������
    jfieldID d_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "d_cNodePrivateId", "B");
    env->SetByteField(sDBHeadObj, d_cNodePrivateIdFieldID, (jbyte) data.sDBHead.d_Dinfo.d_cNodePrivateId);
    
    
    //���d_nSvrMainId�������
    jfieldID d_nSvrMainIdFieldID = env->GetFieldID(sDBHeadCls, "d_nSvrMainId", "I");
    env->SetIntField(sDBHeadObj, d_nSvrMainIdFieldID, (jint) data.sDBHead.d_Dinfo.d_nSvrMainId); 
    
    
    //���d_cSvrPrivateId�������
    jfieldID d_cSvrPrivateIdFieldID = env->GetFieldID(sDBHeadCls, "d_cSvrPrivateId", "B");
    env->SetByteField(sDBHeadObj, d_cSvrPrivateIdFieldID, (jbyte) data.sDBHead.d_Dinfo.d_cSvrPrivateId);
    
    
    //���d_nServiceNo�������
    jfieldID d_nServiceNoFieldID = env->GetFieldID(sDBHeadCls, "d_nServiceNo", "I");
    env->SetIntField(sDBHeadObj, d_nServiceNoFieldID, (jint) data.sDBHead.d_Dinfo.d_nServiceNo); 
    
    
    //���a_nNodeId�������
    jfieldID a_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "a_nNodeId", "I");
    env->SetIntField(sDBHeadObj, a_nNodeIdFieldID, (jint) data.sDBHead.a_Ainfo.a_nNodeId); 
    
    
    //���a_cNodePrivateId�������
    jfieldID a_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "a_cNodePrivateId", "B");
    env->SetByteField(sDBHeadObj, a_cNodePrivateIdFieldID, (jbyte) data.sDBHead.a_Ainfo.a_cNodePrivateId);
    
    
    //���a_nRetCode�������
    jfieldID a_nRetCodeFieldID = env->GetFieldID(sDBHeadCls, "a_nRetCode", "I");
    env->SetIntField(sDBHeadObj, a_nRetCodeFieldID, (jint) data.sDBHead.a_Ainfo.a_nRetCode); 
    
    
    //���n_nNextNo�������
    jfieldID n_nNextNoFieldID = env->GetFieldID(sDBHeadCls, "n_nNextNo", "I");
    env->SetIntField(sDBHeadObj, n_nNextNoFieldID, (jint) data.sDBHead.n_Ninfo.n_nNextNo);
    
    
    //���n_nNextOffset�������
    jfieldID n_nNextOffsetFieldID = env->GetFieldID(sDBHeadCls, "n_nNextOffset", "I");
    env->SetIntField(sDBHeadObj, n_nNextOffsetFieldID, (jint) data.sDBHead.n_Ninfo.n_nNextOffset);
    
    
    //���b_nSerial�������
    jfieldID b_nSerialFieldID = env->GetFieldID(sDBHeadCls, "bpc_nTimestamp", "I");
    env->SetIntField(sDBHeadObj, b_nSerialFieldID, (jint) data.sDBHead.b_Cinfo.b_nSerial);
    
    
    //���b_cIndex�������
    jfieldID b_cIndexFieldID = env->GetFieldID(sDBHeadCls, "bpc_cIndex", "I");
    env->SetIntField(sDBHeadObj, b_cIndexFieldID, (jint) data.sDBHead.b_Cinfo.b_cIndex);

    
    //���nLen�������
    jfieldID nLenFieldID = env->GetFieldID(sDBHeadCls, "nBodyLen", "I");
    env->SetIntField(sDBHeadObj, nLenFieldID, (jint) data.sDBHead.nLen);
    
    
    //###############################����nBodyLen������sBuffer�ĳ���
    	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����sBuffer����");
    // ����sBuffer����
    jfieldID sBufferFieldID = env->GetFieldID(cls_objClass, "sBuffer", "[B");
    jbyteArray sBuffer = (jbyteArray) env->GetObjectField(objdata,
        sBufferFieldID);

    jbyte *jb = env->GetByteArrayElements(sBuffer, 0);
    
    if (jb != NULL)
    {
//    	memcpy(jb, data.sBuffer, sizeof(data.sBuffer));
//      env->SetByteArrayRegion(sBuffer, 0, sizeof(data.sBuffer), jb);
		memcpy(jb, data.sBuffer, data.sDBHead.nLen);
		env->SetByteArrayRegion(sBuffer, 0, data.sDBHead.nLen, jb);
		env->SetObjectField(objdata, sBufferFieldID, sBuffer);
    }
    else
    {
		printf("nativeUartframeTransform jb is NULL!\n");
    }
    env->ReleaseByteArrayElements(sBuffer, jb, 0);
    
    //����java��byte[]�����ֵ
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"����java��byte[]�����ֵ");

// 	char dataBuff[data.sDBHead.nLen];
// 	bzero(dataBuff,sizeof(dataBuff));
// 	memcpy(dataBuff, data.sBuffer, sizeof(dataBuff));
//    setJavaByteArrayValue(env, objdata, sBufferFieldID, sBuffer, jb, data.sBuffer,data.sDBHead.nLen);
   
    //�ͷ�
	env->DeleteLocalRef(sDBHeadCls);
    env->DeleteLocalRef(cls_objClass);
	
	return 0;
}


//java ���ַ�������ת����C�е�string
int   jstringTostring(JNIEnv*   env,   jstring   jstr, char *str)  
{  
	jclass   clsstring   = (env)->FindClass("java/lang/String");
	jstring   strencode   =   (env)->NewStringUTF("GB2312");  
	jmethodID   mid   =   (env)->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");   
	jbyteArray   barr=   (jbyteArray)(env)->CallObjectMethod(jstr,mid,strencode);  
	jsize   alen   =   (env)->GetArrayLength(barr);  
	jbyte*   ba   =   (env)->GetByteArrayElements(barr,JNI_FALSE);  
	if(alen   >   0)  
	{  
		memcpy(str,ba,alen);  
		str[alen]=0;  
	}  
	(env)->ReleaseByteArrayElements(barr,ba,0);  
	//printf("jstring2string�����д�ӡ��  str=%s\n", str);
	return   0;  
}

//C�е�stringת����java�е�String����
jstring char2jstring( JNIEnv* env, const char* pat )
{
	jstring   rtn  ;   
	jsize   len   =   strlen(pat);   
	jclass   clsstring   =   (env)->FindClass("java/lang/String");   
	jstring   strencode   =   (env)->NewStringUTF("GB2312");   
	jmethodID   mid   =   (env)->GetMethodID(clsstring,   "<init>",   "([BLjava/lang/String;)V");   
	jbyteArray   barr   =   (env)->NewByteArray(len);   
	(env)->SetByteArrayRegion(barr,0,len,(jbyte*)pat);   
	rtn   =   (jstring)(env)->NewObject(clsstring,mid,barr,strencode);   
	return   rtn;   
}

JNIEXPORT jint JNICALL Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiInitApi(JNIEnv *env, jobject jenv, jstring strapixml)
{
	const char *apixml = env->GetStringUTFChars(strapixml, NULL);
	void *apiclass=NULL;
	int ret;
	ret = BFDREBAPI_InitApi(apixml,&apiclass);
	env->ReleaseStringUTFChars(strapixml, apixml);
	if (ret != 0)
	{
		return (jint)ret;
	}
	//��apiclass������������
	ret = g_drebapi_list_poing.Insert((CBF_DrebServerApi *)apiclass);
	return (jint)ret;
}

JNIEXPORT void JNICALL Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiFree(JNIEnv *env, jobject jenv, jint apipoint)
{
	int ret = (int)apipoint;
	void *point = (void *)g_drebapi_list_poing.Find(ret);
	if (point !=NULL)
	{
		BFDREBAPI_Free(point);
		g_drebapi_list_poing.Delete(ret);
	}
	return;
}

JNIEXPORT jint JNICALL Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiStartApi(JNIEnv *env, jobject jenv, jint apipoint)
{
	int ret = (int)apipoint;
	void *point = (void *)g_drebapi_list_poing.Find(ret);
	if (point == NULL)
	{
		return (jint) -1;
	}
	return (jint)BFDREBAPI_StartApi(point);
	
}
JNIEXPORT void JNICALL Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiStopApi(JNIEnv *env, jobject jenv, jint apipoint)
{
	int ret = (int)apipoint;
	void *point = (void *)g_drebapi_list_poing.Find(ret);
	if (point == NULL)
	{
		return ;
	}
	BFDREBAPI_StopApi(point);
	return;
}

JNIEXPORT void JNICALL Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiMonitorApi(JNIEnv *env, jobject jenv, jint apipoint)
{
	int ret = (int)apipoint;
	void *point = (void *)g_drebapi_list_poing.Find(ret);
	if (point == NULL)
	{
		return ;
	}
	BFDREBAPI_MonitorApi(point);
	return;
}
JNIEXPORT jint JNICALL Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiSendMsg(JNIEnv *env, jobject jenv, jint apipoint, jobject senddata)
{
	int ret = (int)apipoint;
	void *point = (void *)g_drebapi_list_poing.Find(ret);
	if (point == NULL)
	{
		printf("���������[%d]ȡAPIʵ������\n",ret);
		return (jint) -1;
	}
	CBF_DrebServerApi *api = (CBF_DrebServerApi *)point;
	api->m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʼsetJavaDataToCDataת��");
	DREBSAPI_COMMSTRU data;
	bzero(&data,sizeof(DREBSAPI_COMMSTRU));
	if (setJavaDataToCData(env,senddata,data,api->m_pLog) != 0)
	{
		api->m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"setJavaDataToCDataת������");
		return (jint) -1;
	}
	api->m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"��ʼ����BFDREBAPI_SendMsg");
	return (jint)BFDREBAPI_SendMsg(point,(const char *)&data);
	
}

JNIEXPORT jint JNICALL Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiGetMsg(JNIEnv *env, jobject jenv, jint apipoint, jobject msgdata)
{
	int ret = (int)apipoint;
	void *point = (void *)g_drebapi_list_poing.Find(ret);
	if (point == NULL)
	{
		return (jint) -1;
	}
	DREBSAPI_COMMSTRU data;
	bzero(&data,sizeof(DREBSAPI_COMMSTRU));
	CBF_DrebServerApi *api = (CBF_DrebServerApi *)point;
	api->m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"��ʼBFDREBAPI_GetMsg");
	ret = BFDREBAPI_GetMsg(point,(char *)&data);
	if (ret == 1)
	{
		api->m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"ȡ�����ݣ���ʼsetCDataToJavaData");
		if (setCDataToJavaData(env,msgdata,data,api->m_pLog) != 0)
		{
			api->m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"setCDataToJavaDataת������");
			return (jint)-1;
		}
		api->m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"setCDataToJavaData���");
		return (jint)ret;
	}
	api->m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"BFDREBAPI_GetMsg������");
	return (jint)ret;
	
}

JNIEXPORT jstring JNICALL Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiGetErrMsg(JNIEnv *env, jobject jenv, jint apipoint)
{
	int ret = (int)apipoint;
	void *point = (void *)g_drebapi_list_poing.Find(ret);
	if (point == NULL)
	{
		jstring str = env->NewStringUTF("");
		return str;
	}
	char errmsg[256];
	bzero(errmsg,sizeof(errmsg));
	BFDREBAPI_GetErrMsg(point,errmsg);
	jstring str = env->NewStringUTF(errmsg);
	return str;
}
JNIEXPORT void JNICALL Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiUnRegisterDreb(JNIEnv *env, jobject jenv, jint apipoint)
{
	int ret = (int)apipoint;
	void *point = (void *)g_drebapi_list_poing.Find(ret);
	if (point == NULL)
	{
		return ;
	}
	BFDREBAPI_UnRegisterDreb(point);
	return;
}

JNIEXPORT void JNICALL Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiRegisterDreb(JNIEnv *env, jobject jenv, jint apipoint, jint drebindex, jstring txlist)
{
	int ret = (int)apipoint;
	void *point = (void *)g_drebapi_list_poing.Find(ret);
	if (point == NULL)
	{
		return ;
	}
	int index = (int)drebindex;
	const char *strlist = env->GetStringUTFChars(txlist, NULL);
	BFDREBAPI_RegisterDreb(point,index,(char *)strlist);
	env->ReleaseStringUTFChars(txlist, strlist);
	return;
}
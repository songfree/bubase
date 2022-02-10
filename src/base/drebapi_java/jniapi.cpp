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

//把java方法对应java的方法为 drebApiSendMsg(Object pClass,DrebsApiCommStru data); 对应的JNI Java_cheetah_core_net_drebapi_jni_DrebApiInterface_drebApiSendMsg(JNIEnv * env, jobject thiz, jobject pClass, jobject data)
//java数据结构转化成c结构
int setJavaDataToCData(JNIEnv* env, jobject objdata, DREBSAPI_COMMSTRU &data,CIErrlog *log )
{
	
    jmethodID methodId;
    //获得objdata对象的句柄
    jclass cls_objClass = env->FindClass(
        "cheetah/core/net/drebapi/jni/DrebsApiCommStru");
    if (0 == cls_objClass)
    {
		log->LogMp(LOG_ERROR,__FILE__,__LINE__,"GetObjectClass failed ");
        return -1;
    }
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得cMsgType属性");  
    //获得cMsgType属性
    jfieldID cMsgFieldID = env->GetFieldID(cls_objClass, "cMsgType", "B");
    data.cMsgType = (char) env->GetByteField(objdata, cMsgFieldID);
//  __android_log_print(ANDROID_LOG_INFO, gszTAG,
//   "data.head.stComm.cMsgType = %d", data.head.stComm.cMsgType);  

    //atoi(char*) 转化成int类型， atol(char*)转化成long类型，atof(char*)转化成double类型
    //获得index属性
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得index属性");  
    jfieldID indexFieldID = env->GetFieldID(cls_objClass, "index", "I");
    data.index = (unsigned int)env->GetIntField(objdata, indexFieldID);

    
//###################获取sDBHead属性
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得sDBHead属性");  
    //获得sDBHead属性
    jfieldID sDBHeadFieldID = env->GetFieldID(cls_objClass, "sDBHead", "Lcheetah/core/net/bubase/DrebHead;");
    jobject  sDBHeadObj = env->GetObjectField(objdata, sDBHeadFieldID);
    if(sDBHeadObj == NULL)
    {
    	printf("GetObjectClass Lcheetah/core/net/bubase/DrebHead failed \n");
    	return -1;
    }
    jclass sDBHeadCls = env->GetObjectClass(sDBHeadObj);
	
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得cZip属性");  
    //获得cZip属性
    jfieldID cZipFieldID = env->GetFieldID(sDBHeadCls, "h_cZip", "B");
    data.sDBHead.cZip = (char) env->GetByteField(sDBHeadObj, cZipFieldID);

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得cCmd属性");  
    //获得cCmd属性
    jfieldID cCmdFieldID = env->GetFieldID(sDBHeadCls, "h_cCmd", "B");
    data.sDBHead.cCmd = (char) env->GetByteField(sDBHeadObj, cCmdFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG, "data.head.stComm.cCmd = %d",
//        data.head.stComm.cCmd);

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得nCrc属性");  
    //获得nCrc属性
    jfieldID nCrcFieldID = env->GetFieldID(sDBHeadCls, "h_nCrc", "I");
    data.sDBHead.nCrc = env->GetIntField(sDBHeadObj,nCrcFieldID);


	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得cRaflag属性");
    //获得cRaflag属性
    jfieldID cRaflagFieldID = env->GetFieldID(sDBHeadCls, "h_cRaflag", "B");
    data.sDBHead.cRaflag = (char) env->GetByteField(sDBHeadObj, cRaflagFieldID);


	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得cNextFlag属性");
    //获得cNextFlag属性
    jfieldID cNextFlagFieldID = env->GetFieldID(sDBHeadCls, "h_cNextFlag", "B");
    data.sDBHead.cNextFlag = (char) env->GetByteField(sDBHeadObj, cNextFlagFieldID);

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得cDrebAffirm属性");
    //获得cDrebAffirm属性
    jfieldID cDrebAffirmFieldID = env->GetFieldID(sDBHeadCls, "h_cDrebAffirm", "B");
    data.sDBHead.cDrebAffirm = (char) env->GetByteField(sDBHeadObj, cDrebAffirmFieldID);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得s_nNodeId属性");
    //获得s_nNodeId属性
    jfieldID s_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "s_nNodeId", "I");
    data.sDBHead.s_Sinfo.s_nNodeId = (unsigned short)env->GetIntField(sDBHeadObj,s_nNodeIdFieldID);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得s_cNodePrivateId属性");
    //获得s_cNodePrivateId属性
    jfieldID s_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "s_cNodePrivateId", "B");
    data.sDBHead.s_Sinfo.s_cNodePrivateId = (char) env->GetByteField(sDBHeadObj, s_cNodePrivateIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得s_nSvrMainId类的属性");
    //获得s_nSvrMainId类的属性
    jfieldID s_nSvrMainIdFieldID = env->GetFieldID(sDBHeadCls, "s_nSvrMainId", "I");
    data.sDBHead.s_Sinfo.s_nSvrMainId = (unsigned int)env->GetIntField(sDBHeadObj,s_nSvrMainIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得s_cSvrPrivateId类的属性");
    //获得s_cSvrPrivateId类的属性
    jfieldID s_cSvrPrivateIdFieldID = env->GetFieldID(sDBHeadCls, "s_cSvrPrivateId", "B");
    data.sDBHead.s_Sinfo.s_cSvrPrivateId = (char) env->GetByteField(sDBHeadObj, s_cSvrPrivateIdFieldID);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得s_nHook类的属性");
    //获得s_nHook类的属性
    jfieldID s_nHookFieldID = env->GetFieldID(sDBHeadCls, "s_nHook", "I");
    data.sDBHead.s_Sinfo.s_nHook = env->GetIntField(sDBHeadObj,s_nHookFieldID);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得s_nSerial类的属性");
    //获得s_nSerial类的属性
    jfieldID s_nSerialFieldID = env->GetFieldID(sDBHeadCls, "s_nSerial", "I");
    data.sDBHead.s_Sinfo.s_nSerial = env->GetIntField(sDBHeadObj,s_nSerialFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得s_nIndex类的属性");
    //获得s_nIndex类的属性
    jfieldID s_nIndexFieldID = env->GetFieldID(sDBHeadCls, "s_nIndex", "I");
    data.sDBHead.s_Sinfo.s_nIndex = env->GetIntField(sDBHeadObj,s_nIndexFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得s_nTimestamp类的属性");
    //获得s_nTimestamp类的属性
    jfieldID s_nTimestampFieldID = env->GetFieldID(sDBHeadCls, "s_nTimestamp", "I");
    data.sDBHead.s_Sinfo.s_nTimestamp = env->GetIntField(sDBHeadObj,s_nTimestampFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得d_nNodeId类的属性");
    //获得d_nNodeId类的属性
    jfieldID d_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "d_nNodeId", "I");
    data.sDBHead.d_Dinfo.d_nNodeId = env->GetIntField(sDBHeadObj,d_nNodeIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得d_cNodePrivateId类的属性");
    //获得d_cNodePrivateId类的属性
    jfieldID d_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "d_cNodePrivateId", "B");
    data.sDBHead.d_Dinfo.d_cNodePrivateId = (char) env->GetByteField(sDBHeadObj, d_cNodePrivateIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得d_nSvrMainId类的属性");
    //获得d_nSvrMainId类的属性
    jfieldID d_nSvrMainIdFieldID = env->GetFieldID(sDBHeadCls, "d_nSvrMainId", "I");
    data.sDBHead.d_Dinfo.d_nSvrMainId = env->GetIntField(sDBHeadObj,d_nSvrMainIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得d_cSvrPrivateId类的属性");
    //获得d_cSvrPrivateId类的属性
    jfieldID d_cSvrPrivateIdFieldID = env->GetFieldID(sDBHeadCls, "d_cSvrPrivateId", "B");
    data.sDBHead.d_Dinfo.d_cSvrPrivateId = (char) env->GetByteField(sDBHeadObj, d_cSvrPrivateIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得d_nServiceNo类的属性");
    //获得d_nServiceNo类的属性
    jfieldID d_nServiceNoFieldID = env->GetFieldID(sDBHeadCls, "d_nServiceNo", "I");
    data.sDBHead.d_Dinfo.d_nServiceNo = env->GetIntField(sDBHeadObj,d_nServiceNoFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得a_nNodeId类的属性");
    //获得a_nNodeId类的属性
    jfieldID a_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "a_nNodeId", "I");
    data.sDBHead.a_Ainfo.a_nNodeId = env->GetIntField(sDBHeadObj,a_nNodeIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得a_cNodePrivateId类的属性");
    //获得a_cNodePrivateId类的属性
    jfieldID a_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "a_cNodePrivateId", "B");
    data.sDBHead.a_Ainfo.a_cNodePrivateId = (char) env->GetByteField(sDBHeadObj, a_cNodePrivateIdFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得a_nRetCode类的属性");
    //获得a_nRetCode类的属性
    jfieldID a_nRetCodeFieldID = env->GetFieldID(sDBHeadCls, "a_nRetCode", "I");
    data.sDBHead.a_Ainfo.a_nRetCode = env->GetIntField(sDBHeadObj,a_nRetCodeFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得n_nNextNo类的属性");
    //获得n_nNextNo类的属性
    jfieldID n_nNextNoFieldID = env->GetFieldID(sDBHeadCls, "n_nNextNo", "I");
    data.sDBHead.n_Ninfo.n_nNextNo = env->GetIntField(sDBHeadObj,n_nNextNoFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得n_nNextOffset类的属性");
    //获得n_nNextOffset类的属性
    jfieldID n_nNextOffsetFieldID = env->GetFieldID(sDBHeadCls, "n_nNextOffset", "I");
    data.sDBHead.n_Ninfo.n_nNextOffset = (unsigned int)env->GetIntField(sDBHeadObj,n_nNextOffsetFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得b_nSerial类的属性");
    //获得b_nSerial类的属性
    jfieldID b_nSerialFieldID = env->GetFieldID(sDBHeadCls, "bpc_nTimestamp", "I");
    data.sDBHead.b_Cinfo.b_nSerial = env->GetIntField(sDBHeadObj,b_nSerialFieldID);
    
    log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得b_cIndex类的属性");
    //获得b_cIndex类的属性
    jfieldID b_cIndexFieldID = env->GetFieldID(sDBHeadCls, "bpc_cIndex", "I");
    data.sDBHead.b_Cinfo.b_cIndex = (unsigned short)env->GetIntField(sDBHeadObj,b_cIndexFieldID);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得nLen类的属性");
    //获得nLen类的属性
    jfieldID nLenFieldID = env->GetFieldID(sDBHeadCls, "nBodyLen", "I");
    data.sDBHead.nLen = (unsigned short)env->GetIntField(sDBHeadObj,nLenFieldID);

    
    //#####################根据nBodyLen获取sBuffer的长度
        // 获得sBuffer属性
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"获得sBuffer属性");  
    jfieldID bufferFieldID = env->GetFieldID(cls_objClass, "sBuffer", "[B");
    jbyteArray sBuffer = (jbyteArray) env->GetObjectField(objdata,
        bufferFieldID);

    //对sBuffer进行赋值
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"对sBuffer进行赋值");  
    jbyte * olddata = (jbyte*) env->GetByteArrayElements(sBuffer, 0);
   // int nArrLen = env->GetArrayLength(sBuffer);  不能直接这样取，因为sBuffer缓冲区大小是固定的
    int nArrLen = data.sDBHead.nLen;
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"nArrLen:%d",nArrLen); 
	if (nArrLen >0 && nArrLen <BPUDATASIZE )
	{
		memcpy(data.sBuffer, (const char*) olddata, nArrLen);
	}
	else if (nArrLen >=BPUDATASIZE ) 
	{
		log->LogMp(LOG_ERROR,__FILE__,__LINE__,"数据长度非法");
		return -1;
	}
	else
	{
		data.sBuffer[0]=0;
	}
//	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"开始ReleaseByteArrayElements");  
    env->ReleaseByteArrayElements(sBuffer, olddata, 0);

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"释放");
    //释放
    env->DeleteLocalRef(sDBHeadCls);
    env->DeleteLocalRef(cls_objClass);
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"setJavaDataToCData完成");
    return 0;
}
  
    
  //c数据结构转化成java数据结构
int setCDataToJavaData(JNIEnv* env, jobject objdata, DREBSAPI_COMMSTRU data,CIErrlog *log)
{
    jmethodID methodId;
    //获得objdata对象的句柄
    jclass cls_objClass = env->FindClass(
        "cheetah/core/net/drebapi/jni/DrebsApiCommStru");
    if (0 == cls_objClass)
    {
        printf("GetObjectClass failed \n");
		log->LogMp(LOG_ERROR,__FILE__,__LINE__,"GetObjectClass failed");
        return -1;
    }

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"设置cMsgType属性");
    //设置cMsgType属性
    jfieldID cMsgFieldID = env->GetFieldID(cls_objClass, "cMsgType", "B");
    env->SetByteField(objdata, cMsgFieldID, (jbyte) data.cMsgType);
    
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"设置index属性");
    //设置index属性
    jfieldID indexFieldID = env->GetFieldID(cls_objClass, "index", "I");
    env->SetIntField(objdata, indexFieldID, (jint) data.index);
    

    //#######################################################33
    //下面开始设置自定义属性sDBHead的值

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"先获得sDBHead属性");

    //先获得sDBHead属性
    jfieldID sDBHeadFieldID = env->GetFieldID(cls_objClass, "sDBHead", "Lcheetah/core/net/bubase/DrebHead;");
    jobject  sDBHeadObj = env->GetObjectField(objdata, sDBHeadFieldID);
    if(sDBHeadObj == NULL)
    {
    	printf("GetObjectClass Lcheetah/core/net/bubase/DrebHead failed \n");
		log->LogMp(LOG_ERROR,__FILE__,__LINE__,"GetObjectClass Lcheetah/core/net/bubase/DrebHead failed");
    	return -1;
    }
    jclass sDBHeadCls = env->GetObjectClass(sDBHeadObj);
    
    //设置h_cZip类的属性
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"设置h_cZip类的属性");
    jfieldID h_cZipFieldID = env->GetFieldID(sDBHeadCls, "h_cZip", "B");
    env->SetByteField(sDBHeadObj, h_cZipFieldID, (jbyte) data.sDBHead.cZip);

	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"设置cCmd属性");
    //获得cCmd属性
    jfieldID cCmdFieldID = env->GetFieldID(sDBHeadCls, "h_cCmd", "B");
    env->SetByteField(sDBHeadObj, h_cZipFieldID, (jbyte) data.sDBHead.cCmd);


    //获得nCrc属性
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"设置nCrc属性");
    jfieldID nCrcFieldID = env->GetFieldID(sDBHeadCls, "h_nCrc", "I");
    env->SetIntField(sDBHeadObj, nCrcFieldID, (jint) data.sDBHead.nCrc);


    //获得cRaflag属性
    jfieldID cRaflagFieldID = env->GetFieldID(sDBHeadCls, "h_cRaflag", "B");
    env->SetByteField(sDBHeadObj, cRaflagFieldID, (jbyte) data.sDBHead.cRaflag);


    //获得cNextFlag属性
    jfieldID cNextFlagFieldID = env->GetFieldID(sDBHeadCls, "h_cNextFlag", "B");
    env->SetByteField(sDBHeadObj, cNextFlagFieldID, (jbyte) data.sDBHead.cNextFlag);


    //获得cDrebAffirm属性
    jfieldID cDrebAffirmFieldID = env->GetFieldID(sDBHeadCls, "h_cDrebAffirm", "B");
    env->SetByteField(sDBHeadObj, cDrebAffirmFieldID, (jbyte) data.sDBHead.cDrebAffirm);
    
    //获得s_nNodeId属性
    jfieldID s_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "s_nNodeId", "I");
    env->SetIntField(sDBHeadObj, s_nNodeIdFieldID, (jint) data.sDBHead.s_Sinfo.s_nNodeId);

    
    //获得s_cNodePrivateId属性
    jfieldID s_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "s_cNodePrivateId", "B");
    env->SetByteField(sDBHeadObj, s_cNodePrivateIdFieldID, (jbyte) data.sDBHead.s_Sinfo.s_cNodePrivateId);
    
    
    //获得s_nSvrMainId类的属性
    jfieldID s_nSvrMainIdFieldID = env->GetFieldID(sDBHeadCls, "s_nSvrMainId", "I");
    env->SetIntField(sDBHeadObj, s_nSvrMainIdFieldID, (jint) data.sDBHead.s_Sinfo.s_nSvrMainId);
    
    
    //获得s_cSvrPrivateId类的属性
    jfieldID s_cSvrPrivateIdFieldID = env->GetFieldID(sDBHeadCls, "s_cSvrPrivateId", "B");
    env->SetByteField(sDBHeadObj, s_cSvrPrivateIdFieldID, (jbyte) data.sDBHead.s_Sinfo.s_cSvrPrivateId);
    
    //获得s_nHook类的属性
    jfieldID s_nHookFieldID = env->GetFieldID(sDBHeadCls, "s_nHook", "I");
    env->SetIntField(sDBHeadObj, s_nHookFieldID, (jint) data.sDBHead.s_Sinfo.s_nHook);
    
    //获得s_nSerial类的属性
    jfieldID s_nSerialFieldID = env->GetFieldID(sDBHeadCls, "s_nSerial", "I");
    env->SetIntField(sDBHeadObj, s_nSerialFieldID, (jint) data.sDBHead.s_Sinfo.s_nSerial);  
    
    
    //获得s_nDrebSerial类的属性                                                                           
    jfieldID s_nDrebSerialFieldID = env->GetFieldID(sDBHeadCls, "s_nDrebSerial", "I"); 
    env->SetIntField(sDBHeadObj, s_nDrebSerialFieldID, (jint) data.sDBHead.s_Sinfo.s_nDrebSerial);                       

      
    //获得s_nIndex类的属性
    jfieldID s_nIndexFieldID = env->GetFieldID(sDBHeadCls, "s_nIndex", "I");
    env->SetIntField(sDBHeadObj, s_nIndexFieldID, (jint) data.sDBHead.s_Sinfo.s_nIndex);  
    
    
    //获得s_nTimestamp类的属性
    jfieldID s_nTimestampFieldID = env->GetFieldID(sDBHeadCls, "s_nTimestamp", "I");
    env->SetIntField(sDBHeadObj, s_nTimestampFieldID, (jint) data.sDBHead.s_Sinfo.s_nTimestamp);  
    
    
    //获得d_nNodeId类的属性
    jfieldID d_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "d_nNodeId", "I");
    env->SetIntField(sDBHeadObj, d_nNodeIdFieldID, (jint) data.sDBHead.d_Dinfo.d_nNodeId);  
    
    
    //获得d_cNodePrivateId类的属性
    jfieldID d_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "d_cNodePrivateId", "B");
    env->SetByteField(sDBHeadObj, d_cNodePrivateIdFieldID, (jbyte) data.sDBHead.d_Dinfo.d_cNodePrivateId);
    
    
    //获得d_nSvrMainId类的属性
    jfieldID d_nSvrMainIdFieldID = env->GetFieldID(sDBHeadCls, "d_nSvrMainId", "I");
    env->SetIntField(sDBHeadObj, d_nSvrMainIdFieldID, (jint) data.sDBHead.d_Dinfo.d_nSvrMainId); 
    
    
    //获得d_cSvrPrivateId类的属性
    jfieldID d_cSvrPrivateIdFieldID = env->GetFieldID(sDBHeadCls, "d_cSvrPrivateId", "B");
    env->SetByteField(sDBHeadObj, d_cSvrPrivateIdFieldID, (jbyte) data.sDBHead.d_Dinfo.d_cSvrPrivateId);
    
    
    //获得d_nServiceNo类的属性
    jfieldID d_nServiceNoFieldID = env->GetFieldID(sDBHeadCls, "d_nServiceNo", "I");
    env->SetIntField(sDBHeadObj, d_nServiceNoFieldID, (jint) data.sDBHead.d_Dinfo.d_nServiceNo); 
    
    
    //获得a_nNodeId类的属性
    jfieldID a_nNodeIdFieldID = env->GetFieldID(sDBHeadCls, "a_nNodeId", "I");
    env->SetIntField(sDBHeadObj, a_nNodeIdFieldID, (jint) data.sDBHead.a_Ainfo.a_nNodeId); 
    
    
    //获得a_cNodePrivateId类的属性
    jfieldID a_cNodePrivateIdFieldID = env->GetFieldID(sDBHeadCls, "a_cNodePrivateId", "B");
    env->SetByteField(sDBHeadObj, a_cNodePrivateIdFieldID, (jbyte) data.sDBHead.a_Ainfo.a_cNodePrivateId);
    
    
    //获得a_nRetCode类的属性
    jfieldID a_nRetCodeFieldID = env->GetFieldID(sDBHeadCls, "a_nRetCode", "I");
    env->SetIntField(sDBHeadObj, a_nRetCodeFieldID, (jint) data.sDBHead.a_Ainfo.a_nRetCode); 
    
    
    //获得n_nNextNo类的属性
    jfieldID n_nNextNoFieldID = env->GetFieldID(sDBHeadCls, "n_nNextNo", "I");
    env->SetIntField(sDBHeadObj, n_nNextNoFieldID, (jint) data.sDBHead.n_Ninfo.n_nNextNo);
    
    
    //获得n_nNextOffset类的属性
    jfieldID n_nNextOffsetFieldID = env->GetFieldID(sDBHeadCls, "n_nNextOffset", "I");
    env->SetIntField(sDBHeadObj, n_nNextOffsetFieldID, (jint) data.sDBHead.n_Ninfo.n_nNextOffset);
    
    
    //获得b_nSerial类的属性
    jfieldID b_nSerialFieldID = env->GetFieldID(sDBHeadCls, "bpc_nTimestamp", "I");
    env->SetIntField(sDBHeadObj, b_nSerialFieldID, (jint) data.sDBHead.b_Cinfo.b_nSerial);
    
    
    //获得b_cIndex类的属性
    jfieldID b_cIndexFieldID = env->GetFieldID(sDBHeadCls, "bpc_cIndex", "I");
    env->SetIntField(sDBHeadObj, b_cIndexFieldID, (jint) data.sDBHead.b_Cinfo.b_cIndex);

    
    //获得nLen类的属性
    jfieldID nLenFieldID = env->GetFieldID(sDBHeadCls, "nBodyLen", "I");
    env->SetIntField(sDBHeadObj, nLenFieldID, (jint) data.sDBHead.nLen);
    
    
    //###############################根据nBodyLen来设置sBuffer的长度
    	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"设置sBuffer属性");
    // 设置sBuffer属性
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
    
    //设置java的byte[]数组的值
	log->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"设置java的byte[]数组的值");

// 	char dataBuff[data.sDBHead.nLen];
// 	bzero(dataBuff,sizeof(dataBuff));
// 	memcpy(dataBuff, data.sBuffer, sizeof(dataBuff));
//    setJavaByteArrayValue(env, objdata, sBufferFieldID, sBuffer, jb, data.sBuffer,data.sDBHead.nLen);
   
    //释放
	env->DeleteLocalRef(sDBHeadCls);
    env->DeleteLocalRef(cls_objClass);
	
	return 0;
}


//java 的字符串对象转换成C中的string
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
	//printf("jstring2string方法中打印出  str=%s\n", str);
	return   0;  
}

//C中的string转换成java中的String对象
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
	//将apiclass加入表，返回序号
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
		printf("按连接序号[%d]取API实例出错\n",ret);
		return (jint) -1;
	}
	CBF_DrebServerApi *api = (CBF_DrebServerApi *)point;
	api->m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"开始setJavaDataToCData转换");
	DREBSAPI_COMMSTRU data;
	bzero(&data,sizeof(DREBSAPI_COMMSTRU));
	if (setJavaDataToCData(env,senddata,data,api->m_pLog) != 0)
	{
		api->m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"setJavaDataToCData转换出错");
		return (jint) -1;
	}
	api->m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"开始调用BFDREBAPI_SendMsg");
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
	api->m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"开始BFDREBAPI_GetMsg");
	ret = BFDREBAPI_GetMsg(point,(char *)&data);
	if (ret == 1)
	{
		api->m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"取到数据，开始setCDataToJavaData");
		if (setCDataToJavaData(env,msgdata,data,api->m_pLog) != 0)
		{
			api->m_pLog->LogMp(LOG_ERROR,__FILE__,__LINE__,"setCDataToJavaData转换出错");
			return (jint)-1;
		}
		api->m_pLog->LogMp(LOG_DEBUG,__FILE__,__LINE__,"setCDataToJavaData完成");
		return (jint)ret;
	}
	api->m_pLog->LogMp(LOG_DEBUG+1,__FILE__,__LINE__,"BFDREBAPI_GetMsg无数据");
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
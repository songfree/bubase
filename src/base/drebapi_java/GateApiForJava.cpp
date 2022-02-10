#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <unistd.h>

#include "com_ylink_bubasequotition_socket_GateApi.h"
#include "src/cgateapi/GateApi.h"
#include "include/gateway/GateHead.h"
#include "src/cgateapi/GateLink.h"

extern "C"
{
  const char* gszTAG = "CATEAPIFORJAVA";
  const char* gsz1TAG = "CDATATOJAVADATA";
  //CGateLink *pCgateApiClass1 = new CGateLink();
  //CGateLink *pCgateApiClass2 = new CGateLink();
  CGateLink pCgateApiClass1;
  CGateLink pCgateApiClass2;
  int isTest = 1;
  int sendType = 1;

  //1���ӳɹ�  0δ����   2��������
  jint
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1GetConnectStatus__(
      JNIEnv* env, jclass clazz)
  {
    if (isTest == 0)
      {
        return (jint) (BFCGATE_GetConnectStatus(sendType));
      }
    else
      {
        return (jint) BFCGATE_TS_GetConnectStatus(&pCgateApiClass1);
      }
  }

  // ����  : ��������
  jint
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1ConnectGate__(
      JNIEnv* env, jclass clazz)
  {
    if (isTest == 0)
      {
        return (jint) (BFCGATE_ConnectGate(sendType));
      }
    else
      {
        return (jint) BFCGATE_TS_ConnectGate(&pCgateApiClass1);
      }

  }

  // ����  : �رձ߽�,�˳��߳�
  void
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1Logout__(JNIEnv* env,
      jclass clazz)
  {

    if (isTest == 0)
      {
        BFCGATE_Logout(sendType);
      }
    else
      {
        BFCGATE_TS_Logout(&pCgateApiClass1);
      }

  }

  //java���ݽṹת����c�ṹ
  CGATE_COMMSTRU
  setJavaDataToCData(JNIEnv* env, jobject objdata, CGATE_COMMSTRU data)
  {
    jmethodID methodId;
    //���objdata����ľ��
    jclass cls_objClass = env->FindClass(
        "com/ylink/bubasequotition/bean/PCgeteCommStru");
    if (0 == cls_objClass)
      {
        __android_log_print(ANDROID_LOG_INFO, gszTAG,
            "GetObjectClass failed \n");
      }

    // ���buffer����
    jfieldID bufferFieldID = env->GetFieldID(cls_objClass, "buffer", "[B");
    jbyteArray buffer = (jbyteArray) env->GetObjectField(objdata,
        bufferFieldID);

    //��buffer���и�ֵ
    jbyte * olddata = (jbyte*) env->GetByteArrayElements(buffer, 0);
    int nArrLen = env->GetArrayLength(buffer);
    memcpy(data.buffer, (const char*) olddata, nArrLen);
    env->ReleaseByteArrayElements(buffer, olddata, 0);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG,
//        "setJavaDataToCData buffer= %s", data.buffer);

    //���cMsgType�������
    jfieldID cMsgFieldID = env->GetFieldID(cls_objClass, "cMsgType", "B");
    data.head.stComm.cMsgType = (char) env->GetByteField(objdata, cMsgFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG,
//        "data.head.stComm.cMsgType = %d", data.head.stComm.cMsgType);

    //���cZipType�������
    jfieldID cZipFieldID = env->GetFieldID(cls_objClass, "cZip", "B");
    data.head.stComm.cZip = (char) env->GetByteField(objdata, cZipFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG, "data.head.stComm.cZip = %d",
//        data.head.stComm.cZip);

    //���cCmdType�������
    jfieldID cCmdFieldID = env->GetFieldID(cls_objClass, "cCmd", "B");
    data.head.stComm.cCmd = (char) env->GetByteField(objdata, cCmdFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG, "data.head.stComm.cCmd = %d",
//        data.head.stComm.cCmd);

    //���nCrcType�������
    jfieldID nCrcFieldID = env->GetFieldID(cls_objClass, "nCrc", "S");
    data.head.stComm.nCrc = (unsigned short) env->GetShortField(objdata,
        nCrcFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG, "data.head.stComm.nCrc = %d",
//        data.head.stComm.nCrc);

    //���cRaflagType�������
    jfieldID cRaflagFieldID = env->GetFieldID(cls_objClass, "cRaflag", "B");
    data.head.stComm.cRaflag = (char) env->GetByteField(objdata,
        cRaflagFieldID);
    __android_log_print(ANDROID_LOG_INFO, gszTAG,
        "data.head.stComm.cRaflag = %d", data.head.stComm.cRaflag);

    //���cNextFlag�������
    jfieldID cNextFlagFieldID = env->GetFieldID(cls_objClass, "cNextFlag", "B");
    data.head.stComm.cNextFlag = (char) env->GetByteField(objdata,
        cNextFlagFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG,
//        "data.head.stComm.cNextFlag = %d", data.head.stComm.cNextFlag);

    //���nNodeIdType�������
    jfieldID nNodeIdFieldID = env->GetFieldID(cls_objClass, "nNodeId", "S");
    data.head.stDest.nNodeId = (unsigned short) env->GetShortField(objdata,
        nNodeIdFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG,
//        "data.head.stDest.nNodeId = %u", data.head.stDest.nNodeId);

    //���cNodePrivateId�������
    jfieldID cNodePrivateIdFieldID = env->GetFieldID(cls_objClass,
        "cNodePrivateId", "B");
    data.head.stDest.cNodePrivateId = (char) env->GetByteField(objdata,
        cNodePrivateIdFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG,
//        "data.head.stDest.cNodePrivateId = %d",
//        data.head.stDest.cNodePrivateId);

    //���nSvrMainId�������
    jfieldID nSvrMainIdFieldID = env->GetFieldID(cls_objClass, "nSvrMainId",
        "I");
    data.head.stDest.nSvrMainId = (unsigned int) env->GetIntField(objdata,
        nSvrMainIdFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG,
//        "data.head.stDest.nSvrMainId = %d", data.head.stDest.nSvrMainId);

    //���cSvrPrivateId�������
    jfieldID cSvrPrivateIdFieldID = env->GetFieldID(cls_objClass,
        "cSvrPrivateId", "B");
    data.head.stDest.cSvrPrivateId = (char) env->GetByteField(objdata,
        cSvrPrivateIdFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG,
//        "data.head.stDest.cSvrPrivateId = %d", data.head.stDest.cSvrPrivateId);

    //���nSerial�������
    jfieldID nSerialFieldID = env->GetFieldID(cls_objClass, "nSerial", "S");
    data.head.stDest.nSerial = (unsigned short) env->GetShortField(objdata,
        nSerialFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG,
//        "data.head.stDest.nSerial = %u", data.head.stDest.nSerial);

    //���nServiceNo�������
    jfieldID nServiceNoFieldID = env->GetFieldID(cls_objClass, "nServiceNo",
        "I");
    data.head.stDest.nServiceNo = (unsigned int) env->GetIntField(objdata,
        nServiceNoFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG,
//        "data.head.stDest.nServiceNo = %d", data.head.stDest.nServiceNo);

    //���nRetCode�������
    jfieldID nRetCodeFieldID = env->GetFieldID(cls_objClass, "nRetCode", "S");
    data.head.nRetCode = (unsigned short) env->GetShortField(objdata,
        nRetCodeFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG, "data.head.nRetCode = %u",
//        data.head.nRetCode);

    //���n_nNextNo�������
    jfieldID n_nNextNoFieldID = env->GetFieldID(cls_objClass, "n_nNextNo", "I");
    data.head.stNext.n_nNextNo = (unsigned int) env->GetIntField(objdata,
        n_nNextNoFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG,
//        "data.head.stNext.n_nNextNo = %d", data.head.stNext.n_nNextNo);

    //���n_nNextOffset�������
    jfieldID n_nNextOffsetFieldID = env->GetFieldID(cls_objClass,
        "n_nNextOffset", "I");
    data.head.stNext.n_nNextOffset = (unsigned int) env->GetIntField(objdata,
        n_nNextOffsetFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG,
//        "data.head.stNext.n_nNextOffset = %d", data.head.stNext.n_nNextOffset);

    //���nRetCode�������
    jfieldID nLenFieldID = env->GetFieldID(cls_objClass, "nLen", "S");
    data.head.nLen = (unsigned short) env->GetShortField(objdata, nLenFieldID);
//    __android_log_print(ANDROID_LOG_INFO, gszTAG, "data.head.nLen = %u",
//        data.head.nLen);

    //�ͷ�
    env->DeleteLocalRef(cls_objClass);
    return data;
  }

  //c���ݽṹת����java���ݽṹ
  void
  setCDataToJavaData(JNIEnv* env, jobject objdata, CGATE_COMMSTRU data)
  {
    jmethodID methodId;
    //���objdata����ľ��
    jclass cls_objClass = env->FindClass(
        "com/ylink/bubasequotition/bean/PCgeteCommStru");
    if (0 == cls_objClass)
      {
        __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
            "GetObjectClass failed \n");
      }

    // ���buffer����
    jfieldID bufferFieldID = env->GetFieldID(cls_objClass, "buffer", "[B");
    jbyteArray buffer = (jbyteArray) env->GetObjectField(objdata,
        bufferFieldID);

    jbyte *jb = env->GetByteArrayElements(buffer, 0);
    if (jb != NULL)
      {
        memcpy(jb, data.buffer, sizeof(data.buffer));
        env->SetByteArrayRegion(buffer, 0, sizeof(data.buffer), jb);
        env->SetObjectField(objdata, bufferFieldID, buffer);
      }
    else
      {
        __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
            "nativeUartframeTransform jb is NULL!\n");
      }
    env->ReleaseByteArrayElements(buffer, jb, 0);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG, "buffer= %s", data.buffer);

    //���cMsgType�������
    jfieldID cMsgFieldID = env->GetFieldID(cls_objClass, "cMsgType", "B");
    env->SetByteField(objdata, cMsgFieldID, (jbyte) data.head.stComm.cMsgType);
    __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
        "data.head.stComm.cMsgType = %d", data.head.stComm.cMsgType);

    //���cZipType�������
    jfieldID cZipFieldID = env->GetFieldID(cls_objClass, "cZip", "B");
    env->SetByteField(objdata, cZipFieldID, (jbyte) data.head.stComm.cZip);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG, "data.head.stComm.cZip = %d",
//        data.head.stComm.cZip);

    //���cCmdType�������
    jfieldID cCmdFieldID = env->GetFieldID(cls_objClass, "cCmd", "B");
    env->SetByteField(objdata, cCmdFieldID, (jbyte) data.head.stComm.cCmd);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG, "data.head.stComm.cCmd = %d",
//        data.head.stComm.cCmd);

    //���nCrcType�������
    jfieldID nCrcFieldID = env->GetFieldID(cls_objClass, "nCrc", "S");
    env->SetShortField(objdata, nCrcFieldID, (jshort) data.head.stComm.nCrc);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG, "data.head.stComm.nCrc = %u",
//        data.head.stComm.nCrc);

    //���cRaflagType�������
    jfieldID cRaflagFieldID = env->GetFieldID(cls_objClass, "cRaflag", "B");
    env->SetByteField(objdata, cRaflagFieldID,
        (jbyte) data.head.stComm.cRaflag);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
//        "data.head.stComm.cRaflag = %d", data.head.stComm.cRaflag);

    //���cNextFlag�������
    jfieldID cNextFlagFieldID = env->GetFieldID(cls_objClass, "cNextFlag", "B");
    env->SetByteField(objdata, cNextFlagFieldID,
        (jbyte) data.head.stComm.cNextFlag);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
//        "data.head.stComm.cNextFlag = %d", data.head.stComm.cNextFlag);

    //���nNodeIdType�������
    jfieldID nNodeIdFieldID = env->GetFieldID(cls_objClass, "nNodeId", "S");
    env->SetShortField(objdata, nNodeIdFieldID,
        (jshort) data.head.stDest.nNodeId);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
//        "data.head.stDest.nNodeId = %u", data.head.stDest.nNodeId);

    //���cNodePrivateId�������
    jfieldID cNodePrivateIdFieldID = env->GetFieldID(cls_objClass,
        "cNodePrivateId", "B");
    env->SetByteField(objdata, cNodePrivateIdFieldID,
        (jbyte) data.head.stDest.cNodePrivateId);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
//        "data.head.stDest.cNodePrivateId = %d",
//        data.head.stDest.cNodePrivateId);

    //���nSvrMainId�������
    jfieldID nSvrMainIdFieldID = env->GetFieldID(cls_objClass, "nSvrMainId",
        "I");
    env->SetIntField(objdata, nSvrMainIdFieldID,
        (jint) data.head.stDest.nSvrMainId);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
//        "data.head.stDest.nSvrMainId = %d", data.head.stDest.nSvrMainId);

    //���cSvrPrivateId�������
    jfieldID cSvrPrivateIdFieldID = env->GetFieldID(cls_objClass,
        "cSvrPrivateId", "B");
    env->SetByteField(objdata, cSvrPrivateIdFieldID,
        (jbyte) data.head.stDest.cSvrPrivateId);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
//        "data.head.stDest.cSvrPrivateId = %d", data.head.stDest.cSvrPrivateId);

    //���nSerial�������
    jfieldID nSerialFieldID = env->GetFieldID(cls_objClass, "nSerial", "S");
    env->SetShortField(objdata, nSerialFieldID,
        (jshort) data.head.stDest.nSerial);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
//        "data.head.stDest.nSerial = %u", data.head.stDest.nSerial);

    //���nServiceNo�������
    jfieldID nServiceNoFieldID = env->GetFieldID(cls_objClass, "nServiceNo",
        "I");
    env->SetIntField(objdata, nServiceNoFieldID,
        (jint) data.head.stDest.nServiceNo);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
//        "data.head.stDest.nServiceNo = %d", data.head.stDest.nServiceNo);

    //���nRetCode�������
    jfieldID nRetCodeFieldID = env->GetFieldID(cls_objClass, "nRetCode", "S");
    env->SetShortField(objdata, nRetCodeFieldID, (jshort) data.head.nRetCode);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG, "data.head.nRetCode = %d",
//        (int) data.head.nRetCode);

    //���n_nNextNo�������
    jfieldID n_nNextNoFieldID = env->GetFieldID(cls_objClass, "n_nNextNo", "I");
    env->SetIntField(objdata, n_nNextNoFieldID,
        (jint) data.head.stNext.n_nNextNo);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
//        "data.head.stNext.n_nNextNo = %d", data.head.stNext.n_nNextNo);

    //���n_nNextOffset�������
    jfieldID n_nNextOffsetFieldID = env->GetFieldID(cls_objClass,
        "n_nNextOffset", "I");
    env->SetIntField(objdata, n_nNextOffsetFieldID,
        (jint) data.head.stNext.n_nNextOffset);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG,
//        "data.head.stNext.n_nNextOffset = %d", data.head.stNext.n_nNextOffset);

    //���nLen�������
    jfieldID nLenFieldID = env->GetFieldID(cls_objClass, "nLen", "S");
    env->SetShortField(objdata, nLenFieldID, (jshort) data.head.nLen);
//    __android_log_print(ANDROID_LOG_INFO, gsz1TAG, "data.head.nLen = %u",
//        data.head.nLen);
    //�ͷ�
    env->DeleteLocalRef(cls_objClass);
  }

  // ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
  jint
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1InitApi__Ljava_lang_String_2(
      JNIEnv* env, jclass clazz, jstring strapixml)
  {
    const char *apixml = env->GetStringUTFChars(strapixml, NULL);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "apixml:%s", apixml);
    int ret = 0;
    if (isTest == 0)
      {
        ret = BFCGATE_InitApi(apixml, sendType);
      }
    else
      {
        ret = BFCGATE_TS_InitApi(apixml, (void **) &pCgateApiClass1);
      }

    __android_log_print(ANDROID_LOG_INFO, gszTAG, "ret:%d", ret);

    env->ReleaseStringUTFChars(strapixml, apixml);
    return (jint) ret;
  }

  // ����  : �������󲢽���Ӧ��
  jint
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1SendReqData__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2I(
      JNIEnv* env, jclass clazz, jobject objdata, jint timeout)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "sendrequ1 data.buffer= %s",
        data.buffer);
    int ret = 0;
    if (isTest == 0)
      {
        ret = (jint) ((BFCGATE_SendReqData(&data, (unsigned int) ((timeout)),
            sendType)));
      }
    else
      {
        ret = (jint) ((BFCGATE_TS_SendReqData(&pCgateApiClass1, &data,
            (unsigned int) ((timeout)))));
      }
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "sendrequ2 data.buffer= %s",
        data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "sendrequ3 data.buffer= %s",
        data.buffer);
    return ret;
  }

  // ����  : �������ض�������
  JNIEXPORT jint JNICALL
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1Subscribe__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2I(
      JNIEnv *env, jclass jclzss, jobject objdata, jint timeout)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "subscribe data.buffer= %s",
        data.buffer);
    int ret = 0;
    if (isTest == 0)
      {
        ret = (jint) ((BFCGATE_Subscribe(&data, (unsigned int) ((timeout)),
            sendType)));
      }
    else
      {
        ret = (jint) ((BFCGATE_TS_Subscribe(&pCgateApiClass1, &data,
            (unsigned int) ((timeout)))));
      }

    __android_log_print(ANDROID_LOG_INFO, gszTAG, "subscribe data.buffer= %s",
        data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);

    return ret;
  }

  // ����  : ȡ���ͻ�㲥����
  JNIEXPORT jint JNICALL
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1GetReqData__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2I(
      JNIEnv *env, jclass jclzss, jobject objdata, jint timeout)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG,
        "(GB)getreqdata data.buffer= %s", data.buffer);
    int ret = 0;
    if (isTest == 0)
      {
        ret = (jint) ((BFCGATE_GetReqData(&data, (unsigned int) ((timeout)),
            sendType)));
      }
    else
      {
        ret = (jint) ((BFCGATE_TS_GetReqData(&pCgateApiClass1, &data,
            (unsigned int) ((timeout)))));
      }
    __android_log_print(ANDROID_LOG_INFO, gszTAG,
        "(GB)getreqdata data.buffer= %s", data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);

    return ret;
  }

  // ����  : ȡ��ָ����ˮ��Ӧ��
  JNIEXPORT jint JNICALL
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1GetAnsData__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2I(
      JNIEnv *env, jclass jclzss, jobject objdata, jint timeout)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "getAnsdata data.buffer= %s",
        data.buffer);
    int ret = 0;
    if (isTest == 0)
      {
        ret = (jint) ((BFCGATE_GetAnsData(&data, (unsigned int) ((timeout)),
            sendType)));
      }
    else
      {
        ret = (jint) ((BFCGATE_TS_GetAnsData(&pCgateApiClass1, &data,
            (unsigned int) ((timeout)))));
      }

    __android_log_print(ANDROID_LOG_INFO, gszTAG, "getAnsdata data.buffer= %s",
        data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);

    return ret;
  }

  // ����  : �������󲢽���Ӧ��
  JNIEXPORT jint JNICALL
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1SendLogin__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2I(
      JNIEnv *env, jclass jclzss, jobject objdata, jint timeout)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "SendLogin1 data.buffer= %s",
        data.buffer);
    int ret = 0;
    if (isTest == 0)
      {
        ret = (jint) ((BFCGATE_SendLogin(&data, (unsigned int) ((timeout)),
            sendType)));
      }
    else
      {
        ret = (jint) ((BFCGATE_TS_SendLogin(&pCgateApiClass1, &data,
            (unsigned int) ((timeout)))));
      }
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "SendLogin2 data.buffer= %s",
        data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "SendLogin3 data.buffer= %s",
        data.buffer);
    return ret;
  }

  // ����  : ���Ͷ������󲢽���Ӧ��
  JNIEXPORT jint JNICALL
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1SendSms__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2I(
      JNIEnv *env, jclass jclzss, jobject objdata, jint timeout)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "SendSms data.buffer= %s",
        data.buffer);

    int ret = 0;
    if (isTest == 0)
      {
        ret = (jint) ((BFCGATE_SendSms(&data, (unsigned int) ((timeout)),
            sendType)));
      }
    else
      {
        ret = (jint) ((BFCGATE_TS_SendSms(&pCgateApiClass1, &data,
            (unsigned int) ((timeout)))));
      }
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "SendSms data.buffer= %s",
        data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);

    return ret;
  }

  //1���ӳɹ�  0δ����   2��������
  jint
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1GetConnectStatus__I(
      JNIEnv* env, jclass clazz, jint type)
  {
    return (jint) (BFCGATE_GetConnectStatus(type));
  }

  // ����  : ��������
  jint
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1ConnectGate__I(
      JNIEnv* env, jclass clazz, jint type)
  {
    return (jint) (BFCGATE_ConnectGate(type));
  }

  // ����  : �رձ߽�,�˳��߳�
  void
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1Logout__I(JNIEnv* env,
      jclass clazz, jint type)
  {
    BFCGATE_Logout(type);
  }

  // ����  : ��ʼ��api����ȡ�����ļ����õ�֤��Ŀ¼�����ص�ַ�˿�
  jint
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1InitApi__Ljava_lang_String_2I(
      JNIEnv* env, jclass clazz, jstring strapixml, jint type)
  {
    const char *apixml = env->GetStringUTFChars(strapixml, NULL);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "apixml:%s", apixml);
    int ret = BFCGATE_InitApi(apixml, type);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "ret:%d", ret);

    env->ReleaseStringUTFChars(strapixml, apixml);
    return (jint) ret;
  }

  // ����  : �������󲢽���Ӧ��
  jint
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1SendReqData__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2II(
      JNIEnv* env, jclass clazz, jobject objdata, jint timeout, jint type)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "sendrequ1 data.buffer= %s",
        data.buffer);
    int ret = (jint) ((BFCGATE_SendReqData(&data, (unsigned int) ((timeout)),
        type)));
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "sendrequ2 data.buffer= %s",
        data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "sendrequ3 data.buffer= %s",
        data.buffer);
    return ret;
  }

  // ����  : �������ض�������
  JNIEXPORT jint JNICALL
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1Subscribe__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2II(
      JNIEnv *env, jclass jclzss, jobject objdata, jint timeout, int type)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "subscribe data.buffer= %s",
        data.buffer);
    int ret = (jint) ((BFCGATE_Subscribe(&data, (unsigned int) ((timeout)),
        type)));

    __android_log_print(ANDROID_LOG_INFO, gszTAG, "subscribe data.buffer= %s",
        data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);

    return ret;
  }

  // ����  : ȡ���ͻ�㲥����
  JNIEXPORT jint JNICALL
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1GetReqData__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2II(
      JNIEnv *env, jclass jclzss, jobject objdata, jint timeout, jint type)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG,
        "(TS)getreqdata data.buffer= %s", data.buffer);
    int ret = (jint) ((BFCGATE_GetReqData(&data, (unsigned int) ((timeout)),
        type)));

    __android_log_print(ANDROID_LOG_INFO, gszTAG,
        "(TS)getreqdata data.buffer= %s", data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);

    return ret;
  }

  // ����  : ȡ��ָ����ˮ��Ӧ��
  JNIEXPORT jint JNICALL
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1GetAnsData__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2II(
      JNIEnv *env, jclass jclzss, jobject objdata, jint timeout, jint type)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "getAnsdata data.buffer= %s",
        data.buffer);
    int ret = (jint) ((BFCGATE_GetAnsData(&data, (unsigned int) ((timeout)),
        type)));

    __android_log_print(ANDROID_LOG_INFO, gszTAG, "getAnsdata data.buffer= %s",
        data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);

    return ret;
  }

  // ����  : �������󲢽���Ӧ��
  JNIEXPORT jint JNICALL
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1SendLogin__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2II(
      JNIEnv *env, jclass jclzss, jobject objdata, jint timeout, jint type)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "SendLogin1 data.buffer= %s",
        data.buffer);
    int ret = (jint) ((BFCGATE_SendLogin(&data, (unsigned int) ((timeout)),
        type)));

    __android_log_print(ANDROID_LOG_INFO, gszTAG, "SendLogin2 data.buffer= %s",
        data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "SendLogin3 data.buffer= %s",
        data.buffer);
    return ret;
  }

  // ����  : ���Ͷ������󲢽���Ӧ��
  JNIEXPORT jint JNICALL
  Java_com_ylink_bubasequotition_socket_GateApi_BFCGATE_1SendSms__Lcom_ylink_bubasequotition_bean_PCgeteCommStru_2II(
      JNIEnv *env, jclass jclzss, jobject objdata, jint timeout, jint type)
  {
    //��ʼ��API������ͨѶ�ṹ
    CGATE_COMMSTRU data;
    bzero(&data, sizeof(CGATE_COMMSTRU));
    //java���ݹ���������ת����c++�ṹ
    data = setJavaDataToCData(env, objdata, data);
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "SendSms data.buffer= %s",
        data.buffer);
    int ret =
        (jint) ((BFCGATE_SendSms(&data, (unsigned int) ((timeout)), type)));
    __android_log_print(ANDROID_LOG_INFO, gszTAG, "SendSms data.buffer= %s",
        data.buffer);
    // C++�ṹ�ش���Java
    setCDataToJavaData(env, objdata, data);

    return ret;
  }

}


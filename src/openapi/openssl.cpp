    
    OpenSSL_add_all_algorithms();
    EVP_PKEY *pPriKey;
    BIO *p_Bio_File;
    char * usr="root";
    p_Bio_File = BIO_new_file("990724.pem", "r");
    if(!p_Bio_File)
    {
        cout<<"file error"<<endl;
        return 0;
    }
    pPriKey = PEM_read_bio_PrivateKey(p_Bio_File, NULL, NULL,NULL);
    if (NULL == pPriKey)
    {
        cout<<"get key error"<<endl;
        return 0;
    }
    //fclose(pFile);
    BIO_free(p_Bio_File);
    char *mPackStr="{\"phonecode\":\"000000000000000\",\"mark\":\"1\"}";
    int mPackLen =strlen(mPackStr);
    EVP_MD_CTX mdctx;
    EVP_MD_CTX_init(&mdctx);
    if(!EVP_SignInit_ex(&mdctx,EVP_sha1(),NULL)) //签名初始化，设置摘要算法
    {
        printf("err\n");
        EVP_PKEY_free(pPriKey);
        return 0;
    }
    if(!EVP_SignUpdate(&mdctx,mPackStr,mPackLen)) //计算签名（摘要）Update
    {
        printf("err\n");
        EVP_PKEY_free(pPriKey);
        return 0;
    }
    unsigned int mSignLen = EVP_PKEY_size(pPriKey);
    unsigned char * pSignBuf = (unsigned char*)calloc(mSignLen+1, sizeof(char));
    if(!EVP_SignFinal(&mdctx,pSignBuf,&mSignLen,pPriKey))  //签名输出
    {
        printf("err\n");
        EVP_PKEY_free(pPriKey);
        return 0;
    }
    char * b=base64_encode((char *)pSignBuf);
    EVP_MD_CTX_cleanup(&mdctx);
    cout<<"签名结果:"<<endl;
    cout<<b<<endl;
    
    
    
    
    最近需要用到数字签名的相关技术，但是网络上对这方面的文章说的含糊，所以自己把这段时间在学习OpenSSL过程中得到心得发表出来，供大家讨论，欢迎大家联系我，互相交流。

以下是实现PFX证书的读取和应用证书进行数字签名与签名验证：

环境：VC6 + WIN2003 + OPENSSL

代码：

#include "stdafx.h"
#include <windows.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/pkcs12.h>

#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")

bool DoSignData(const char* szPKCS12FileName, const char* szPKCS12Password,
    const char* szUnSignData, char* szSignData)
{
 if (szPKCS12FileName == NULL || szUnSignData == NULL || szSignData == NULL) {
  return false;
 }
 /*变量*/
 int            err;
 unsigned int   sig_len;
 unsigned char  sig_buf[4096];
 EVP_MD_CTX     md_ctx;
 EVP_PKEY *     pkey = NULL;
 FILE *     fp   = NULL;
 X509 *     x509 = NULL;
 PKCS12*     p12  = NULL;
 STACK_OF(X509) *ca  = NULL;
 /*初始化*/
 SSLeay_add_all_algorithms();
 ERR_load_crypto_strings();
 /*读取个人信息证书并分解出密钥和证书*/
 if (!(fp = fopen(szPKCS12FileName, "rb"))) 
    {
  return false;
 }
 p12 = d2i_PKCS12_fp(fp, NULL);
 fclose (fp);
 if (!p12) {
  fprintf(stderr, "Error reading PKCS#12 file/n");
  ERR_print_errors_fp(stderr);
  return false;
 }
 if (!PKCS12_parse(p12, szPKCS12Password, &pkey, &x509, &ca)) {
  fprintf(stderr, "Error parsing PKCS#12 file/n");
  ERR_print_errors_fp(stderr);

  PKCS12_free(p12);
  return false;
 }
 PKCS12_free(p12);

 if (pkey == NULL) {
  ERR_print_errors_fp (stderr);
  
  return false;
 }
 /*签名数据*/
 EVP_SignInit   (&md_ctx, EVP_sha1());
 EVP_SignUpdate (&md_ctx, szUnSignData, strlen(szUnSignData));
 sig_len = sizeof(sig_buf);
 err = EVP_SignFinal (&md_ctx, sig_buf, &sig_len, pkey);

 if (err != 1) {
  ERR_print_errors_fp(stderr);
  /*释放相关变量*/
  if (pkey) {
   EVP_PKEY_free (pkey);
  }
  if (x509) {
   X509_free(x509);
  }
  return false;
 }
 sig_buf[sig_len] = 0;
 memcpy(szSignData, sig_buf, sig_len);
 /*释放相关变量*/
 if (pkey) {
  EVP_PKEY_free (pkey);
 }
 if (x509) {
  X509_free(x509);
 }

 return true;
}

bool DoVerifyData(const char* szPKCS12FileName, const char* szPKCS12Password,
      const char* szUnSignData, const char* szSignData)
{
 if (szPKCS12FileName == NULL || szSignData == NULL) {
  return false;
 }
 /*变量*/
 int            err;
 unsigned int   sig_len;
 EVP_MD_CTX     md_ctx;
 EVP_PKEY *     pkey = NULL;
 FILE *     fp   = NULL;
 X509 *     x509 = NULL;
 PKCS12*     p12  = NULL;
 STACK_OF(X509) *ca  = NULL;
 /*初始化*/
 SSLeay_add_all_algorithms();
 ERR_load_crypto_strings();
 /*读取个人信息证书并分解出密钥和证书*/
 if (!(fp = fopen(szPKCS12FileName, "rb"))) {
  return false;
 }
 p12 = d2i_PKCS12_fp(fp, NULL);
 fclose (fp);
 if (!p12) {
  fprintf(stderr, "Error reading PKCS#12 file/n");
  ERR_print_errors_fp(stderr);
  return false;
 }
 if (!PKCS12_parse(p12, szPKCS12Password, &pkey, &x509, &ca)) {
  fprintf(stderr, "Error parsing PKCS#12 file/n");
  ERR_print_errors_fp(stderr);

  PKCS12_free(p12);
  return false;
 }
 PKCS12_free(p12);

 if (x509 == NULL) {
  ERR_print_errors_fp (stderr);
  
  return false;
 }
 /*验证签名*/
 pkey=X509_get_pubkey(x509);
 if (pkey == NULL) {
  ERR_print_errors_fp (stderr);
  if (x509) {
   X509_free(x509);
  }
  return false;
 }

 /* Verify the signature */
 sig_len = 0;
 sig_len = strlen(szSignData);
 EVP_VerifyInit(&md_ctx, EVP_sha1());
 EVP_VerifyUpdate(&md_ctx, szUnSignData, strlen(szUnSignData));
 err = EVP_VerifyFinal (&md_ctx, (const BYTE*)szSignData, sig_len, pkey);
 EVP_PKEY_free (pkey);

 if (err != 1) {
  ERR_print_errors_fp (stderr);
  /*释放相关变量*/
  if (pkey) {
   EVP_PKEY_free(pkey);
  }
  return false;
 }
 /*释放相关变量*/
 if (pkey) {
  EVP_PKEY_free(pkey);
 }

 return true;
}

int main(int argc, char* argv[])
{
 char sig_buf [4096];
 memset(sig_buf, 0, sizeof(sig_buf));
 if (!DoSignData("F://项目文件//10002-password=123456.pfx", "123456", "i love china.", sig_buf)) {
  printf("Signature Data Failed.");
 }else{
  printf("Signature Data Success.");
  printf(">------------after sign data--------------begin/n");
  printf((char*)sig_buf);
  printf("/n>------------after sign data--------------end/n");
  if (!DoVerifyData("F://项目文件//10002-password=123456.pfx", "123456", "i love china.", sig_buf)) {
   printf ("Signature Verified Failed./n");
  }else{
   printf ("Signature Verified Ok./n");
  }
 }
 Sleep(10000);
 return 0;
}




从pfx提取密钥信息，并转换为key格式（pfx使用pkcs12模式补足）

1、提取密钥对（如果pfx证书已加密，会提示输入密码。）
       openssl pkcs12 -in 1.pfx -nocerts -nodes -out 1.key

2、从密钥对提取私钥
       openssl rsa -in  1.key -out 1_pri.key

3、从密钥对提取公钥
       openssl rsa -in 1.key -pubout -out 1_pub.key

4、因为RSA算法使用的是pkcs8模式补足，需要对提取的私钥进一步处理

       openssl pkcs8 -in 1_pri.key -out 1_pri.p8 -outform der -nocrypt -topk8
       
       
       
       
       int main(int argc, char **argv)
{
    FILE *fp;
    EVP_PKEY *pkey;
    X509 *cert;
    STACK_OF(X509) *ca = NULL;
    PKCS12 *p12;
    int i;
    if (argc != 4) {
        fprintf(stderr, "Usage: pkread p12file password opfile\n");
        exit (1);
    }
    SSLeay_add_all_algorithms();
    ERR_load_crypto_strings();
    if (!(fp = fopen(argv[1], "rb"))) {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        exit(1);
    }
    p12 = d2i_PKCS12_fp(fp, NULL);
    fclose (fp);
    if (!p12) {
        fprintf(stderr, "Error reading PKCS#12 file\n");
        ERR_print_errors_fp(stderr);
        exit (1);
    }
    if (!PKCS12_parse(p12, argv[2], &pkey, &cert, &ca)) {
        fprintf(stderr, "Error parsing PKCS#12 file\n");
        ERR_print_errors_fp(stderr);
        exit (1);
    }
    PKCS12_free(p12);
    if (!(fp = fopen(argv[3], "w"))) {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        exit(1);
    }
    if (pkey) {
        fprintf(fp, "***Private Key***\n");
        PEM_write_PrivateKey(fp, pkey, NULL, NULL, 0, NULL, NULL);
    }
    if (cert) {
        fprintf(fp, "***User Certificate***\n");
        PEM_write_X509_AUX(fp, cert);
    }
    if (ca && sk_num(ca)) {
        fprintf(fp, "***Other Certificates***\n");
        for (i = 0; i < sk_X509_num(ca); i++) 
            PEM_write_X509_AUX(fp, sk_X509_value(ca, i));
    }
    fclose(fp);
    return 0;
}

/******************************************************************
 * File Name: 
 *		SafeEngine.h
 * Description: 
 *		Head file for Safe Engine.
 *		Definition of error code and data structure
 * Create Date:
 *		01/14/2009
 * Copy Right:
 *      SHECA
 ******************************************************************/
#ifndef _SAFEENGINE_H
#define _SAFEENGINE_H
#include <stdio.h>
#include <stdlib.h>

#ifdef _WINDOWS
#include <winsock.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WINDOWS
	#define SAFEENGINE_DLL_NAME	            	"SafeEngine.dll"
	#define READCERT_DLL_NAME 				"ReadCert.dll"
	
#else
	#ifdef _HP
	#define SAFEENGINE_DLL_NAME			"/usr/lib/libsafeengine.sl"
	#define READCERT_DLL_NAME				"/usr/lib/libreadcert.sl"
		
	#else
	#define SAFEENGINE_DLL_NAME			"libsafeengine.so"
	#define READCERT_DLL_NAME				"libreadcert.so"
		
	#endif
#endif

#define MAX_PATH_LENGTH 256
#define MAX_P10_LEGNTH	8192
#define MAX_PRIVKEY_LENGTH 4096
#define MAX_CERTCHAIN_LENGTH 4096	
#define MAX_DEVICE_NUM 256
#define SDBI_KEY_SIZE  16
#define DES_KEY_SIZE   8
#define DES3_KEY_SIZE  24
#define MAX_CRL_FILE_LENGTH 15360

/*PKCS#11 definition*/
#ifndef MODULE_LENGTH
#define MODULE_LENGTH 1024
#endif /* MODULE_LENGTH */

#define MAX_PRIVKEY_LOGON_LENGTH 8
#define PKCS11_CERT_SIG_ID   "SHECA_PKCS11_CERT_SIG"
#define PKCS11_CERT_ENV_ID   "SHECA_PKCS11_CERT_ENV"

#define PKCS11_CHAIN_SIG_ID  "SHECA_PKCS11_CHAIN_SIG" 
#define PKCS11_CHAIN_ENV_ID  "SHECA_PKCS11_CHAIN_ENV" 

#define PKCS11_VERS_SIG_ID   "SHECA_PKCS11_VERS_SIG" 
#define PKCS11_VERS_ENV_ID   "SHECA_PKCS11_VERS_ENV" 

#define PKCS11_PUB_SIG_ID    "SHECA_PKCS11_PUB_SIG"
#define PKCS11_PUB_ENV_ID    "SHECA_PKCS11_PUB_ENV"

#define PKCS11_PRI_SIG_ID    "SHECA_PKCS11_PRI_SIG"
#define PKCS11_PRI_ENV_ID    "SHECA_PKCS11_PRI_ENV"

#define PKCS11_SEAL_CERT_ID  "SHECA_PKCS11_SEAL_CERT"
#define PKCS11_SEAL_VERS_ID  "SHECA_PKCS11_SEAL_VERS"
#define PKCS11_SEAL_PIC_ID   "SHECA_PKCS11_SEAL_PIC"
#define PKCS11_SEAL_CHAIN_ID "SHECA_PKCS11_SEAL_CHAIN"

#define CKU_SHECA_SO							0x80000000
#define CKU_SHECA_SIG_USER						0x80000001
#define CKU_SHECA_ENV_USER						0x80000002
#define CKU_SHECA_SEAL_USER						0x80000003

#define CKM_SSF33_UNPAD							0x0000f001 
#define CKM_SSF33_PAD							0x0000f002 
#define CKK_SSF33								0x0000f000

/*MS CSP definition*/
#define SHECA_VERSION							0x000000f1
#define SHECA_CERTCHAIN							0x000000f2
#define SHECA_SEAL_VERSION						0x000000f3
#define SHECA_SEAL_CERT							0x000000f4
#define SHECA_SEAL_PIC							0x000000f5
#define SHECA_SEAL_CERTCHAIN					0x000000f6

#define SHECA_CRYPT_ONE							0x000000f1
#define SHECA_CRYPT_TWO							0x000000f2

#define ALG_SID_SSF33_UNPAD						0x000000f1 
#define ALG_SID_SSF33_PAD						0x000000f2 
#define CALG_SSF33_UNPAD   (ALG_CLASS_DATA_ENCRYPT|ALG_TYPE_BLOCK|ALG_SID_SSF33_UNPAD)
#define CALG_SSF33_PAD	   (ALG_CLASS_DATA_ENCRYPT|ALG_TYPE_BLOCK|ALG_SID_SSF33_PAD)

/*SafeEngine version*/
#define SE_MODULE 1
#define SE_VERSION 1


/* Error code definition */
#define	SE_SUCCESS                              0L
#define	SE_ERROR_LOAD_LIBRARY                   0x82040000
#define	SE_ERROR_MEMORY                         0x82040001
#define	SE_ERROR_PRIVATE_KEY_DEV                0x82040002
#define	SE_ERROR_PRIVATE_KEY_PASSWORD           0x82040003
#define	SE_ERROR_CERT_CHAIN_DEV                 0x82040004
#define	SE_ERROR_CERT_CHAIN_PASSWORD            0x82040005
#define	SE_ERROR_CERT_DEV                       0x82040006
#define	SE_ERROR_CERT_PASSWORD                  0x82040007
#define	SE_ERROR_PRIVATE_KEY_TIME_OUT           0x82040008
#define	SE_ERROR_NO_ENOUGH_BUFFER               0x82040009
#define	SE_ERROR_INITIAL_SESSION                0x8204000A
#define	SE_ERROR_CLEAR_SESSION                  0x8204000B
#define	SE_ERROR_SIGN                           0x8204000C
#define	SE_ERROR_VERIFY_SIGN                    0x8204000D
#define	SE_ERROR_DIGEST                         0x8204000E
#define	SE_ERROR_INVALID_CERT                   0x8204000F
#define	SE_ERROR_ENVELOPE                       0x82040010
#define	SE_ERROR_GET_CERT_FROM_LDAP             0x82040011
#define	SE_ERROR_CERT_EXPIRED                   0x82040012
#define	SE_ERROR_GET_CERT_CHAIN                 0x82040013
#define	SE_ERROR_INVALID_CERT_CHAIN             0x82040014
#define	SE_ERROR_VERIFY_CERT_CHAIN              0x82040015
#define	SE_ERROR_CERT_REVOKED                   0x82040016
#define	SE_ERROR_INVALID_CRL                    0x82040017
#define	SE_ERROR_CONNECT_OCSP                   0x82040018
#define SE_ERROR_OCSP_REQ_ENCODE                0x82040019
#define	SE_ERROR_OCSP_RES                       0x8204001A
#define	SE_ERROR_INVALID_OCSP_RES               0x8204001B
#define	SE_ERROR_OCSP_RES_EXPIRED               0x8204001C
#define	SE_ERROR_OCSP_RES_VERIFY_SIGN           0x8204001D
#define	SE_ERROR_CERT_STATUS_UNKNOWN            0x8204001E
#define	SE_ERROR_SYN_CRYPT                      0x8204001F
#define	SE_ERROR_GET_CERT_INFO                  0x82040020
#define	SE_ERROR_GET_CERT_DETAIL                0x82040021
#define	SE_ERROR_GET_CERT_UNIQUE_ID             0x82040022
#define	SE_ERROR_GET_CERT_EXTENSION             0x82040023
#define	SE_ERROR_PEM_ENCODE                     0x82040024
#define	SE_ERROR_PEM_DECODE                     0x82040025
#define	SE_ERROR_GEN_RANDOM                     0x82040026
#define	SE_ERROR_INVALID_P12_PARAM              0x82040027
#define	SE_ERROR_INVALID_PRIVATE_KEY            0x82040028
#define	SE_ERROR_KEY_PAIR                       0x82040029
#define	SE_ERROR_P12_ENCODE                     0x8204002A
#define	SE_ERROR_INVALID_P12                    0x8204002B
#define	SE_ERROR_P12_DECODE                     0x8204002C
#define	SE_ERROR_ASYN_CRYPT                     0x8204002D
#define	SE_ERROR_INVALID_OID                    0x8204002E
#define	SE_ERROR_INVALID_LDAP_ADDRESS           0x8204002F
#define SE_ERROR_LDAP_ADDRESS                   0x82040030
#define SE_ERROR_LDAP_CONNECT                   0x82040031
#define SE_ERROR_LDAP_BIND                      0x82040032
#define SE_ERROR_OID_NOT_FOUND                  0x82040033
#define SE_ERROR_GET_CERT_CLASS                 0x82040034
#define SE_ERROR_READ_CONFIG                    0x82040035
#define SE_ERROR_PRIKEY_NOT_LOAD                0x82040036
#define SE_ERROR_PRIVATE_KEY_PASSWORD_LEN       0x82040037

#define SE_ERROR_GEN_KEYPAIR                    0x82040040
#define SE_ERROR_GET_PUBKEY                     0x82040041
#define SE_ERROR_GET_PUBKEY_FROM_PRIKEY         0x82040041
#define SE_ERROR_ENCODE_PKCS10                  0x82040042
#define SE_ERROR_DECODE_PKCS10                  0x82040043
#define SE_ERROR_DECODE_NE_PKCS10               0x82040044
#define SE_ERROR_ENCODE_CERT                    0x82040045
#define SE_ERROR_DECODE_PUBKEY                  0x82040046
#define SE_ERROR_DECODE_PRIKEY                  0x82040047
#define SE_ERROR_GET_SESSION_KEY                0x82040049

#define SE_ERROR_DEVICE_PORT                    0x82040050
#define SE_ERROR_NO_DEVICE                      0x82040051
#define SE_ERROR_DEVICE_DRIVER                  0x82040052
#define SE_ERROR_CONNECT                        0x82040053
#define SE_ERROR_DEVICE_LOCKED                  0x82040054
#define SE_ERROR_GET_LOCK_COUNT                 0x82040055
#define SE_ERROR_GET_DEVICE_PORT                0x82040056
#define SE_ERROR_MORE_THAN_ONE_DEVICE           0x82040057
#define SE_ERROR_AUTO_DETECT_DEVICE             0x82040058
#define SE_ERROR_DEV_UNRECOGNIZE                0x82040059
#define SE_ERROR_NOT_SUPPORTED                  0x8204005A
#define SE_ERROR_PARAM							0x8204005B
#define SE_ERROR_CERTCHAIN_VERIFY_FAIL			0x8204005C
#define SE_ERROR_SEAL_NO_ENOUGH 			0x8204005D
#define SE_ERROR_CHANGE_PASS				0x8204005E

#define	SE_ERROR_API_EXPIRED                    0x820400FF

/*Error code for new functions*/
#define SE_ERROR_LIBRARY_LOAD                   0x80220000                 
#define SE_ERROR_USB_PORT       				0x80220001
#define SE_ERROR_DEVICE_INIT                    0x80220002
#define SE_ERROR_DEVICE_DETECT                  0x80220003
#define SE_ERROR_DRIVER_UNINST                  0x80220004
#define SE_ERROR_DEVICE_CONNECT                 0x80220005
#define SE_ERROR_FUNCTION_UNSUPPORT             0x80220006
#define SE_ERROR_DEVICE_LOCK                    0x80220007
#define SE_ERROR_ADMIN_PIN_SET                  0x80220101
#define SE_ERROR_ADMIN_PIN_VAL                  0x80220102
#define SE_ERROR_USER_PIN_SET                   0x80220103
#define SE_ERROR_ADMIN_LOCK_SET                 0x80220104
#define SE_ERROR_USER_LOCK_SET                  0x80220105
#define SE_ERROR_OLD_PASSWORD                   0x80220106
#define SE_ERROR_PASSWORD_CHANGE0				0x80220107



#define MAX_X500_NAME_LEN              256
#define MAX_SERIAL_LEN                 128
#define MAX_ALGO_PARA_LEN              1024
#define MAX_UNIQUE_ID_LEN              128
#define MAX_PUBLIC_KEY_LEN             1024
#define MAX_SIGNATURE_LEN              512

#define MAX_UNKNOWN_EXT                8
#define MAX_OID_SIZE                   32 
#define MAX_EXT_VAL_SIZE               1024


/*X500 Name String Type*/
#define ISSUER_NAME_TELETEX            0x01
#define ISSUER_NAME_PRINTABLE          0x02
#define ISSUER_NAME_UNIVERSAL          0x03
#define ISSUER_NAME_UTF8               0x04
#define ISSUER_NAME_BMP                0x05
#define ISSUER_NAME_MASK               0x0F
#define SUBJECT_NAME_TELETEX           0x10
#define SUBJECT_NAME_PRINTABLE         0x20
#define SUBJECT_NAME_UNIVERSAL         0x30
#define SUBJECT_NAME_UTF8              0x40
#define SUBJECT_NAME_BMP               0x50
#define SUBJECT_NAME_MASK              0xF0


/*Structure definition*/
/*typedef unsigned long long uint64;*/
typedef unsigned long   uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;
typedef  unsigned long APICONTEXT;

#ifndef _WINDOWS
#ifndef _UNIXFUNC_H /* unixfunc.h重复定义 */
typedef void* HINSTANCE;
#endif /* _UNIXFUNC_H */
#endif /* _WINDOWS */

typedef void* HSE;

typedef struct{
	unsigned short ReaderID;
	char ReaderDesc[128];
} READER_INFO;

typedef struct{
	char devDesc[128];
	int bAlgorithm;
	int bSaveCertEnable;
	int iSupportReaderNum;
	READER_INFO Reader[10];
	char CSPLib[128];
	char CardLib[128];
	char STDLib[128];
	char INTLib[128];
	char ProvName[128];
} DEVICE_INFO;

typedef struct SERIAL_INFO_{
    uint8              serialNumber[MAX_SERIAL_LEN];
    uint16             serialNumberLen;
} SERIAL_INFO;

typedef struct ALGORITHM_INFO_{
    uint16             algorithm;
    uint8              parameters[MAX_PUBLIC_KEY_LEN];
    uint16             parametersLen;
} ALGORITHM_INFO;

typedef  struct X500_NAME_INFO_{
    char               countryName[MAX_X500_NAME_LEN];
    char               organizationName[MAX_X500_NAME_LEN];
    char               organizationalUnitname[MAX_X500_NAME_LEN];
    char               stateOrProvinceName[MAX_X500_NAME_LEN];
    char               commonName[MAX_X500_NAME_LEN];
    char               localityName[MAX_X500_NAME_LEN];
    char               title[MAX_X500_NAME_LEN];
    char               surname[MAX_X500_NAME_LEN];
    char               givenName[MAX_X500_NAME_LEN];
    char               initials[MAX_X500_NAME_LEN];
    char               email[MAX_X500_NAME_LEN];
    char               postalAddress[MAX_X500_NAME_LEN];
    char               postalCode[MAX_X500_NAME_LEN];
    char               postalOfficeBox[MAX_X500_NAME_LEN];  /*邮政信箱号*/
    char               telephoneNumber[MAX_X500_NAME_LEN];
    char               telexNumber[MAX_X500_NAME_LEN];      
} X500_NAME_INFO;

typedef struct PUBLIC_KEY_INFO_{
    uint8              derPublicKey[MAX_ALGO_PARA_LEN];
    uint16             derPublicKeyLen;
    ALGORITHM_INFO     algorithmInfo;
} PUBLIC_KEY_INFO;

typedef struct UNIQUE_ID_INFO_{
    uint8              uniqueID[MAX_UNIQUE_ID_LEN];
    uint16             uniqueIDLen;
} UNIQUE_ID_INFO;

typedef struct CA_SIGNATURE_INFO_{
	uint8              caSignature[MAX_SIGNATURE_LEN];
	uint16             caSignatureLen;
} CA_SIGNATURE_INFO;

typedef struct{
    X500_NAME_INFO             directoryName;
    SERIAL_INFO                serialNumber;
} USER_AuthKeyIdentifier_INFO;

typedef struct{
    char        notBefore[64];   /*YYYYMMDDHHMMSS*/
    char        notAfter[64];    /*YYYYMMDDHHMMSS*/
} USER_PrivateKeyUsagePeriod_INFO;

typedef struct{
    uint8       policyIdentifier[4][32];
    uint32      policyIdentifierSize[4];
    char        CPS_url[4][MAX_X500_NAME_LEN];
} USER_CertificatePolicies_INFO;

typedef struct{
    uint8       cA;
    uint32      pathLenConstraint;
} USER_BasicConstraints_INFO;

typedef struct{
    uint32      keyUsage_bit;
} USER_KeyUsage_INFO;

typedef struct{
    uint32      neCertType_bit;
} USER_neCertType_INFO;

typedef struct{
   uint32      setCertType_bit;
} USER_setCertType_INFO;

typedef struct{
    uint8       hashedRootKey[64];
    uint32      hashedRootKeySize;
} USER_setHashedRootKey_INFO;

typedef struct{
    uint8       oid[MAX_UNKNOWN_EXT][MAX_OID_SIZE];
    uint32      oidSize[MAX_UNKNOWN_EXT];
    uint8       critical[MAX_UNKNOWN_EXT];
    uint8       value[MAX_UNKNOWN_EXT][MAX_EXT_VAL_SIZE]; 
    uint32      valueSize[MAX_UNKNOWN_EXT];
    uint32      extSize; 
} USER_privateExt_INFO;

typedef struct{
    USER_AuthKeyIdentifier_INFO       AuthKeyIdentifier;
    USER_CertificatePolicies_INFO     CertificatePolicies;
    USER_PrivateKeyUsagePeriod_INFO   PrivateKeyUsagePeriod;
    USER_BasicConstraints_INFO        BasicConstraints;
    USER_KeyUsage_INFO                KeyUsage;
    USER_neCertType_INFO              neCertType;
    USER_setCertType_INFO             setCertType;
    USER_setHashedRootKey_INFO        setHashedRootKey;
    uint32                            selfCertClass;
    char                              revocationURL[MAX_X500_NAME_LEN];
    USER_privateExt_INFO              privateExt;
} USER_EXTENSION_INFO;

typedef struct {
    uint8                 version;
    SERIAL_INFO           serialNumber;
    ALGORITHM_INFO        signature;
    X500_NAME_INFO        issuer;
    char                  notBefore[64];   /*YYYYMMDDHHMMSS*/
    char                  notAfter[64];    /*YYYYMMDDHHMMSS*/
    X500_NAME_INFO        subject;
    PUBLIC_KEY_INFO       subjectPublicKeyInfo;
    UNIQUE_ID_INFO        issuerUniqueID; 
    UNIQUE_ID_INFO        subjectUniqueID;
    CA_SIGNATURE_INFO     caSignature;

    uint32                extensionsFlag;
    uint32                criticalFlag;

    USER_EXTENSION_INFO   extensions;

} CERT_INFO_EX;

typedef struct{
	int   version;
	char  subject_countryName[MAX_X500_NAME_LEN];
	char  subject_organizationName[MAX_X500_NAME_LEN];
	char  subject_organizationalUnitname1[MAX_X500_NAME_LEN];

	char  subject_stateOrProvinceName[MAX_X500_NAME_LEN];
	char  subject_commonName[MAX_X500_NAME_LEN];
	char  subject_localityName[MAX_X500_NAME_LEN];
	char  subject_title[MAX_X500_NAME_LEN];
	char  subject_surname[MAX_X500_NAME_LEN];
	char  subject_givenName[MAX_X500_NAME_LEN];
	char  subject_initials[MAX_X500_NAME_LEN];
	char  subject_email[MAX_X500_NAME_LEN];
	char  subject_postalAddress[MAX_X500_NAME_LEN];
	char  subject_postalCode[MAX_X500_NAME_LEN];
	char  subject_postalOfficeBox[MAX_X500_NAME_LEN];
	char  subject_telephoneNumber[MAX_X500_NAME_LEN];
	char  subject_telexNumber[MAX_X500_NAME_LEN];
	unsigned char  subjectPublicKeyInfo[MAX_PUBLIC_KEY_LEN];
	int   subjectPublicKeyInfoSize;
} PKCS10_INFO;

typedef struct{
	int version;
	int serialNumberSize;
	int signatureAlgorithm;
	int parametersSize;
	unsigned char serialNumber[MAX_SERIAL_LEN];
	unsigned char parameters[1024];
	char issuer_countryName[MAX_X500_NAME_LEN];
	char issuer_organizationName[MAX_X500_NAME_LEN];
	char issuer_organizationalUnitname1[MAX_X500_NAME_LEN];
	char issuer_stateOrProvinceName[MAX_X500_NAME_LEN];
	char issuer_commonName[MAX_X500_NAME_LEN];
	char issuer_localityName[MAX_X500_NAME_LEN];
	char issuer_title[MAX_X500_NAME_LEN];
	char issuer_surname[MAX_X500_NAME_LEN];
	char issuer_givenName[MAX_X500_NAME_LEN];
	char issuer_initials[MAX_X500_NAME_LEN];
	char issuer_email[MAX_X500_NAME_LEN];
	char issuer_postalAddress[MAX_X500_NAME_LEN];
	char issuer_postalCode[MAX_X500_NAME_LEN];
	char issuer_postalOfficeBox[MAX_X500_NAME_LEN];
	char issuer_telephoneNumber[MAX_X500_NAME_LEN];
	char issuer_telexNumber[MAX_X500_NAME_LEN];
	char notBefore[MAX_X500_NAME_LEN];
	char notAfter[MAX_X500_NAME_LEN];
	char subject_countryName[MAX_X500_NAME_LEN];
	char subject_organizationName[MAX_X500_NAME_LEN];
	char subject_organizationalUnitname1[MAX_X500_NAME_LEN];
	char subject_stateOrProvinceName[MAX_X500_NAME_LEN];
	char subject_commonName[MAX_X500_NAME_LEN];
	char subject_localityName[MAX_X500_NAME_LEN];
	char subject_title[MAX_X500_NAME_LEN];
	char subject_surname[MAX_X500_NAME_LEN];
	char subject_givenName[MAX_X500_NAME_LEN];
	char subject_initials[MAX_X500_NAME_LEN];
	char subject_email[MAX_X500_NAME_LEN];
	char subject_postalAddress[MAX_X500_NAME_LEN];
	char subject_postalCode[MAX_X500_NAME_LEN];
	char subject_postalOfficeBox[MAX_X500_NAME_LEN];
	char subject_telephoneNumber[MAX_X500_NAME_LEN];
	char subject_telexNumber[MAX_X500_NAME_LEN];
	unsigned char ca_signature[MAX_SIGNATURE_LEN];
	int ca_signature_len;
	unsigned char subjectPublicKeyInfo[MAX_ALGO_PARA_LEN];
	unsigned char issuerUniqueID[MAX_UNIQUE_ID_LEN];
	unsigned char subjectUniqueID[MAX_UNIQUE_ID_LEN];
	int subjectPublicKeyInfoSize;
	int issuerUniqueIDSize;
	int subjectUniqueIDSize;
	unsigned int extensionsFlag;
	unsigned int criticalFlag;
	char authKeyId_countryName[MAX_X500_NAME_LEN];
	char authKeyId_organizationName[MAX_X500_NAME_LEN];
	char authKeyId_organizationalUnitname1[MAX_X500_NAME_LEN] ;
	char authKeyId_stateOrProvinceName[MAX_X500_NAME_LEN];
	char authKeyId_commonName[MAX_X500_NAME_LEN];
	char authKeyId_localityName[MAX_X500_NAME_LEN];
	char authKeyId_title[MAX_X500_NAME_LEN];
	char authKeyId_surname[MAX_X500_NAME_LEN];
	char authKeyId_givenName[MAX_X500_NAME_LEN];
	char authKeyId_initials[MAX_X500_NAME_LEN];
	char authKeyId_email[MAX_X500_NAME_LEN];
	char authKeyId_postalAddress[MAX_X500_NAME_LEN];
	char authKeyId_postalCode[MAX_X500_NAME_LEN];
	char authKeyId_postalOfficeBox[MAX_X500_NAME_LEN];
	char authKeyId_telephoneNumber[MAX_X500_NAME_LEN];
	char authKeyId_telexNumber[MAX_X500_NAME_LEN];
	unsigned char authKeyId_serialNumber[MAX_SERIAL_LEN];
	int authKeyId_serialNumberSize;
	unsigned char policyId1[64];
	unsigned char policyId2[64];
	unsigned char policyId3[64];
	unsigned char policyId4[64];
	int policyIdSize1;
	int policyIdSize2;
	int policyIdSize3;
	int policyIdSize4;
	char policyUrl1[MAX_X500_NAME_LEN];
	char policyUrl2[MAX_X500_NAME_LEN];
	char policyUrl3[MAX_X500_NAME_LEN];
	char policyUrl4[MAX_X500_NAME_LEN];
	char PrivateKeyUsagePeriod_notBefore[64];
	char PrivateKeyUsagePeriod_notAfter[64];
	int  BasicConstraints_cA;
	int BasicConstraints_pathLenConstraint;
	unsigned int KeyUsage;
	unsigned int neCertType;
	unsigned int setCertType;
	int  self_certClass;
	char revocationURL[MAX_X500_NAME_LEN] ;
	USER_privateExt_INFO  private_extension;
	unsigned char  hashedRootKey[64];
	int            hashedRootKeySize; 
} ENCODE_CERT_INFO;


typedef struct
{
	unsigned long device_type;
	HINSTANCE hdll_csp;
	HINSTANCE hdll_int;
	char*  logon_psw;

	char*  para;
	APICONTEXT ctx;	
}DEVICE_INFO_EX;

typedef struct
{
	time_t start_time;
	time_t end_time;
}TIME_INFO;

typedef struct
{
	unsigned char* data;
	unsigned long data_length;
}DATA_INFO;


typedef struct
{
	char proxy_address[MAX_PATH_LENGTH];
	int proxy_port;
	char user_name[MAX_PATH_LENGTH];
	char password[MAX_PATH_LENGTH];
}PROXY_INFO;

#ifdef __cplusplus
}
#endif

#endif	/* _SAFEENGINE_H */

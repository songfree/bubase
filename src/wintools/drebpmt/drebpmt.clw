; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDrebpmtDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "drebpmt.h"

ClassCount=5
Class1=CDrebpmtApp
Class2=CDrebpmtDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_DIALOG_SETVAR
Class4=CVarConfig
Resource4=IDD_DREBPMT_DIALOG
Class5=CViewErrInfo
Resource5=IDD_DIALOG_ERRINFO

[CLS:CDrebpmtApp]
Type=0
HeaderFile=drebpmt.h
ImplementationFile=drebpmt.cpp
Filter=N

[CLS:CDrebpmtDlg]
Type=0
HeaderFile=drebpmtDlg.h
ImplementationFile=drebpmtDlg.cpp
Filter=D
LastObject=IDC_COMBO_CMD
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=drebpmtDlg.h
ImplementationFile=drebpmtDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_DREBPMT_DIALOG]
Type=1
Class=CDrebpmtDlg
ControlCount=91
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_RUNTIME,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT_TIMEOUT,edit,1350631552
Control8=IDC_STATIC,static,1342308352
Control9=IDC_EDIT_TIMESEP,edit,1350631552
Control10=IDC_STATIC,static,1342308352
Control11=IDC_EDIT_TRYNUM,edit,1350631552
Control12=IDC_CHECK_AUTO,button,1342242819
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_EDIT_ALLTIME,edit,1484849280
Control21=IDC_EDIT_CURTIME,edit,1484849280
Control22=IDC_EDIT_MINRESP,edit,1484849280
Control23=IDC_EDIT_TOTALTHREAD,edit,1484849280
Control24=IDC_EDIT_ALLTHREAD,edit,1484849280
Control25=IDC_EDIT_SENDNUM,edit,1484849280
Control26=IDC_EDIT_RCVNUM,edit,1484849280
Control27=IDC_STATIC,static,1342308352
Control28=IDC_STATIC,static,1342308352
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,static,1342308352
Control31=IDC_STATIC,static,1342308352
Control32=IDC_STATIC,static,1342308352
Control33=IDC_EDIT_MAXRESP,edit,1484849280
Control34=IDC_EDIT_AVRESP,edit,1484849280
Control35=IDC_EDIT_SENDBYTE,edit,1484849280
Control36=IDC_EDIT_CURNUMSEC,edit,1484849280
Control37=IDC_EDIT_MAXNUMSEC,edit,1484849280
Control38=IDC_EDIT_AVNUMSEC,edit,1484849280
Control39=IDC_STATIC,static,1342308352
Control40=IDC_STATIC,static,1342308352
Control41=IDC_STATIC,static,1342308352
Control42=IDC_STATIC,static,1342308352
Control43=IDC_STATIC,static,1342308352
Control44=IDC_EDIT_RCVBYTE,edit,1484849280
Control45=IDC_EDIT_AVR_SENDBYTE,edit,1484849280
Control46=IDC_EDIT_AVR_RCVBYTE,edit,1484849280
Control47=IDC_EDIT_ERRNUM,edit,1484849280
Control48=IDC_BUTTON_VIEW,button,1342242816
Control49=IDC_STATIC,button,1342177287
Control50=IDC_LIST_SC,SysListView32,1350631425
Control51=IDC_STATIC,button,1342177287
Control52=IDC_STATIC,static,1342308352
Control53=IDC_STATIC,static,1342308352
Control54=IDC_IPADDRESS,SysIPAddress32,1342242816
Control55=IDC_EDIT_PORT,edit,1350639744
Control56=IDC_STATIC,static,1342308352
Control57=IDC_COMBO_CONNECT,combobox,1344339987
Control58=IDC_STATIC,static,1342308352
Control59=IDC_EDIT_BEGINTIME,edit,1350639744
Control60=IDC_STATIC,static,1342308352
Control61=IDC_EDIT_THREADNUM,edit,1350639744
Control62=IDC_STATIC,static,1342308352
Control63=IDC_STATIC,static,1342308352
Control64=IDC_STATIC,static,1342308352
Control65=IDC_STATIC,static,1342308352
Control66=IDC_STATIC,static,1342308352
Control67=IDC_STATIC,static,1342308352
Control68=IDC_EDIT_NODEID,edit,1350639744
Control69=IDC_EDIT_NODEPRIVATE,edit,1350639744
Control70=IDC_EDIT_SVRID,edit,1350639744
Control71=IDC_EDIT_SVRPRIVATE,edit,1350639744
Control72=IDC_EDIT_SERVICEFUNC,edit,1350639744
Control73=IDC_EDIT_SCFILE,edit,1350631552
Control74=IDC_BUTTON_FILE,button,1342242816
Control75=IDC_BUTTON_SETVAR,button,1342242816
Control76=IDC_BUTTON_STOP,button,1342242816
Control77=IDC_BUTTON_RESTAR,button,1342242816
Control78=IDC_STATIC,button,1342177287
Control79=IDC_BUTTON_ADD,button,1342242816
Control80=IDC_BUTTON_DEL,button,1342242816
Control81=IDC_BUTTON_SAVELIST,button,1342242816
Control82=IDC_BUTTON_OPENLIST,button,1342242816
Control83=IDC_BUTTON_SAVETEST,button,1342242816
Control84=IDC_BUTTON_OPENTEST,button,1342242816
Control85=IDC_STATIC,static,1342308352
Control86=IDC_COMBO_CZIP,combobox,1344340226
Control87=IDC_STATIC,static,1342308352
Control88=IDC_EDIT_LOGLEVEL,edit,1350631552
Control89=IDC_EDIT_RRTIME,edit,1350631552
Control90=IDC_STATIC,static,1342308352
Control91=IDC_COMBO_CMD,combobox,1344339987

[DLG:IDD_DIALOG_SETVAR]
Type=1
Class=CVarConfig
ControlCount=20
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT_VARNAME,edit,1350631552
Control4=IDC_COMBO_TYPE,combobox,1344339987
Control5=IDC_EDIT_VARLEN,edit,1350639744
Control6=IDC_EDIT_FMT,edit,1350631552
Control7=IDC_EDIT_INITVALUE,edit,1350631552
Control8=IDC_EDIT_VALUE,edit,1350631552
Control9=IDC_BUTTON_ADDVALUE,button,1342242816
Control10=IDC_LIST_VAR,SysListView32,1350631425
Control11=IDC_LIST_VARVALUE,SysListView32,1350631425
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,button,1342177287
Control17=IDC_STATIC_LISTVALUE,button,1342177287
Control18=IDC_STATIC,static,1342308352
Control19=IDC_BUTTON_DELVALUE,button,1342242816
Control20=IDC_BUTTON_DEL,button,1342242816

[CLS:CVarConfig]
Type=0
HeaderFile=VarConfig.h
ImplementationFile=VarConfig.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CVarConfig

[DLG:IDD_DIALOG_ERRINFO]
Type=1
Class=CViewErrInfo
ControlCount=2
Control1=IDCANCEL,button,1342242816
Control2=IDC_RICHEDIT_ERR,RICHEDIT,1353779396

[CLS:CViewErrInfo]
Type=0
HeaderFile=ViewErrInfo.h
ImplementationFile=ViewErrInfo.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_RICHEDIT_ERR
VirtualFilter=dWC


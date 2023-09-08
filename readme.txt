此框架是借鉴KS的开发框架独立研发的面向服务的分布式开发框架 支持windows/linux等平台

感兴趣可加QQ:738651沟通。
202309：版本2.0.12  cgate在连上总线，进行订阅时，使用的总线连接索引错误(连多个总线发现问题),统一修改cgate drebapi,2个index是同样的值
202305：版本2.0.11  修改kernel库的日期时间类，增加linux下面取ns时间
202305：版本2.0.10  修改kernel库的共享内存类，支持共享内存大小超过2G
202304：版本2.0.9  修改总线api库，针对广播数据过滤重复的内存表定期清除，增加配置过期时间默认10秒，即当内存表记录超过此时间后删除。防止数据量太大占用内存过多
202304：版本2.0.8  修改bumodule库，支持推送
202302：版本2.0.7  优化bpc/sap，当sap接监控spu时，此sap的监控信息不经过总线内部直接发给监控spu
202212：版本2.0.6  优化日志信息
202211：版本2.0.5  修改网关取消订阅的接口，和订阅一致，通过flag标志区分是订阅还是取消订阅,修改总线，支持广播给固定节点，针对行情全域广播带宽不足时。
202211：版本2.0.4  修改网关订阅的bug
202211：版本2.0.3  修改网关及网关api，订阅改为功能号+key的方式 key使用s_nHook字段
202207：版本2.0.2  修改xdp协议，支持INT64,和protobuf性能比较，优于protobuf，可见examples/teestxdppb下的测试代码。同样examples/testboost是框架的内存表和boost的multi_index构建的内存表的比较，在linux下面优于boost的。
202206：版本2.0.1  总线增加广播订阅模式，总线api增加订阅接口； bpc和cgate改为使用总线api连接总线。




目录结构
BUBASE
---------｜
---------｜etc 配置文件目录
---------｜examples 例子程序目录
                ｜------samplenew    bpu动态库例子
                ｜------samplene2    bpu动态库例子
                ｜------.....
---------｜include   框架头文件目录
                ｜------kernel  框架公共类库头文件目录
                ｜------dreb    总线api头文件目录
                ｜------aio     封装了EPOLL,SELECT,IOCP等IO操作的类库头文件目录
                ｜------bumodule  bpu业务公共库头文件目录
                ｜------cgateapi  网关api的头文件目录
                ｜------dbapi  数据库操作的头文件目录
                ｜------dbpub  总线bpc字节序报文日志类库头文件目录
                ｜------dptool  报文转换rtf生成等头文件目录
                ｜------gateway  cgate网关头文件目录
                ｜------gtp  金交所GTP协议解析器头文件目录
                ｜------kvdata  kv报文解析头文件目录
                ｜------xdp  XDP数据传输协议解析器头文件目录
                ｜------xml  老版的xml解析器头文件目录  新版的解析器在kernel里。
---------｜设计   设计文档目录 
---------｜手册   框架使用文档目录                                      
---------｜src   源代码目录     
                ｜------base     框架基础源代码目录  
                        ｜------aio  封装了EPOLL,SELECT,IOCP等IO操作的类库源码
                        ｜------arb  主备仲裁源码 
                        ｜------bpc  bpc主程序源码 
                        ｜------bpu  bpu主程序源码 
                        ｜------bumodule  bpu类库源码 
                        ｜------cgate  网关源码 
                        ｜------cgateapi  网关api源码 
                        ｜------dbapi  数据库api 
                        ｜------dbpub  dreb bpc公共库源码 
                        ｜------dptool  报文转换等工具源码 
                        ｜------dreb  总线源码 
                        ｜------drebapi  总线api源码 
                        ｜------drebapi_java  java的总线api源码，使用jni调用。 
                        ｜------fpc  前置中心源码 
                        ｜------fpu  前置处理单元源码 
                        ｜------kernel  框架公共类库源码 
                        ｜------xdp  xdp报文解析器源码 
                        ｜------xml  旧版xml解析器源码 
                ｜------cfcaapi  网关CFCA认证动态库源码目录
                ｜------openapi  网关openssl认证动态库源码目录
                ｜------shcaapi  网关SHCA认证动态库源码目录
                ｜------tools    工具类程序源码目录
                        ｜------bfgw  透传网关源码 
                        ｜------cgatepwd  网关配置密码修改工具源码 
                        ｜------changedbpwd  修改bpu数据库配置工具源码
                        ｜------daemon  进程守护程序源码 
                        ｜------GTP  GTP协议解析器源码 
                        ｜------kvdata  KV报文解析器源码 
                        ｜------monisvr  监控bpu动态库源码 
                ｜------vcprj    vc工程目录  
                ｜------wintools  windows界面工具源码目录    
                        ｜------AppWizard  bpu动态库生成向导程序源码         
                        ｜------drebclient  总线单元测试工具源码         
                        ｜------drebpmt  总线压力测试工具源码         
                        ｜------gatepmt  网关压力测试工具源码         
                        ｜------logview  总线/bpc/网关 报文日志查看工具源码         
                        ｜------RegBuPath  注册框架路径程序源码         
                        ｜------testcomm  前置单元测试工具源码         
                        ｜------testfront  前置压力测试工具源码    
                ｜------trade  基于总线的内存交易框架    主程序  
                        ｜------busidll    内存交易业务动态库         
                        ｜------exchdll    交易所连接动态库         
                        ｜------initdll    初始化内存数据动态库         
                        ｜------include    框架公共头文件目录  上述的动态库需要用到此目录下的头文件        
                        
BUBASE_BIN

---------｜编译器xx     vc编译器xx的框架运行包
                ｜x86   32位程序目录
                        ｜------bin  可执行程序目录
                        ｜------lib    框架库文件目录 
                        ｜------libvc  vc2010的库文件目录
                        ｜------libapp  应用库目录，包括监控bpu动态库，例子动态库等
                        ｜------tool   windows界面工具程序目录
                ｜x64   64位程序目录
                        ｜------bin  可执行程序目录
                        ｜------lib    框架库文件目录 
                        ｜------libvc  vc2010的库文件目录
                        ｜------libapp  应用库目录，包括监控bpu动态库，例子动态库等
                        ｜------tool   windows界面工具程序目录
 
                                                
VC编译说明：
     设置系统的环境变量   D:\BUBASE_BIN是例子目录，要根据实际的目录替换
     BUBASE_INCLUDE  = D:\BUBASE_BIN\include\kernel;D:\BUBASE_BIN\include\xdp;D:\BUBASE_BIN\include\xml;D:\BUBASE_BIN\include\gateway;D:\BUBASE_BIN\include\dreb;D:\BUBASE_BIN\include\dbpub;D:\BUBASE_BIN\include\dbapi;D:\BUBASE_BIN\include\cgateapi;D:\BUBASE_BIN\include\bumodule;D:\BUBASE_BIN\include\aio;D:\BUBASE_BIN\include\gtp;D:\BUBASE_BIN\include\kvdata   
          即实际的框架头文件目录
     BUBASE_LIB_X64  = D:\BUBASE_BIN\vc2019\x64\lib
          即编译好的64位库文件存放目录，库依赖时要用到。
     BUBASE_LIB_X86  = D:\BUBASE_BIN\vc2019\x86\lib
          即编译好的32位库文件存放目录，库依赖时要用到。
     BUBASE_BIN_X86 =  D:\BUBASE_BIN\vc2019\x86\bin     
          即编译好的32位可执行程序存放目录。
     BUBASE_BIN_X64 =  D:\BUBASE_BIN\vc2019\x64\bin     
          即编译好的64位可执行程序存放目录。
     BUBASE_LIBAPP_X64  = D:\BUBASE_BIN\vc2019\x64\libapp
          即编译好的64位应用库文件存放目录，库依赖时要用到。
     BUBASE_LIBAPP_X86  = D:\BUBASE_BIN\vc2019\x86\libapp
          即编译好的32位应用库文件存放目录，库依赖时要用到。
     BUBASE_TOOLS_X86 =  D:\BUBASE_BIN\vc2019\x86\tools     
          即编译好的32位mfc界面程序存放目录。
     BUBASE_TOOLS_X64 =  D:\BUBASE_BIN\vc2019\x64\tools
          即编译好的64位mfc界面程序存放目录
     
VS 64位库目录
$(VCInstallDir)lib\amd64;$(VCInstallDir)atlmfc\lib\amd64;$(WindowsSDK_LibraryPath_x64);$(BUBASE_LIB_X64);  
VS 64位包含目录 
$(VCInstallDir)include;$(VCInstallDir)atlmfc\include;$(WindowsSDK_IncludePath);$(BUBASE_INCLUDE);


VS 32位库目录
$(VCInstallDir)lib;$(VCInstallDir)atlmfc\lib;$(WindowsSDK_LibraryPath_x86);$(BUBASE_LIB_X86);    
VS 32位包含目录 
$(VCInstallDir)include;$(VCInstallDir)atlmfc\include;$(WindowsSDK_IncludePath);$(BUBASE_INCLUDE);


LINUX编译说明：
     将源码放入用户的HOME目录，结构如下：
     bin
     etc
     lib
     libapp
     src
     include
     examples
     
     在.bash_profile里面加入
     export LD_LIBRARY_PATH=$HOME/lib:$HOME/libapp:$LD_LIBRARY_PATH:.
     export LANG=C                 源码编码采用GBK,非utf-8
     PATH=$PATH:$HOME/bin:
     
     执行. .bash_profile使修改生效，然后进到src的各子目录下面make即可
     


VC调试运行说明：
     调试的时候，需要找到BUBASE的库文件目录，所以要在windows的环境变量的PATH里面设置所使用BUBASE库目录
     同样以D:\BUBASE_BIN\vc2019\为例进行说明，这里使用X64的库，修改PATH变量增加如下：
     D:\BUBASE_BIN\vc2019\x64\bin;D:\BUBASE_BIN\vc2019\x64\lib;D:\BUBASE_BIN\vc2019\x64\libapp;
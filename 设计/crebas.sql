/*==============================================================*/
/* DBMS name:      ORACLE Version 11g                           */
/* Created on:     2014/1/4 15:06:47                            */
/*==============================================================*/


alter table BFM_APP_NODE
   drop constraint FK_BFM_APP__机器节点_PFM_PC_N;

alter table BFM_LOG
   drop constraint FK_BFM_LOG_REFERENCE_BFM_APP_;

alter table BFM_LOG_HIS
   drop constraint FK_BFM_LOG__REFERENCE_BFM_APP_;

alter table PFM_WARNNING
   drop constraint FK_PFM_WARN_REFERENCE_BFM_APP_;

drop index APP_NODE_INDEX_LASTTIME;

drop index BF_APP_PNODE_FK;

drop table BFM_APP_NODE cascade constraints;

drop index Index_node;

drop table BFM_LOG cascade constraints;

drop table BFM_LOG_HIS cascade constraints;

drop index INDEX_APP_CONNECT_LASTTIME;

drop index INDEX_APP_CONNECT_IP;

drop table PFM_APP_CONNECT cascade constraints;

drop index index_bpc2_lasttime;

drop table PFM_BPC2_REPORT cascade constraints;

drop index Index_PFM_BPULIST_LASTTIME;

drop table PFM_BPULIST cascade constraints;

drop index INDEX_DREB_CONNLIST;

drop table PFM_DREB_CONNLIST cascade constraints;

drop table PFM_PARAMETER cascade constraints;

drop table PFM_PC_NODE cascade constraints;

drop table PFM_PC_NODE_CPU cascade constraints;

drop table PFM_PC_NODE_DISK cascade constraints;

drop table PFM_PC_NODE_RAM cascade constraints;

drop table PFM_WARNNING cascade constraints;

drop sequence BFM_SEQ_MONITOR;

create sequence BFM_SEQ_MONITOR
maxvalue 999999999
 minvalue 1
 cache 20;

/*==============================================================*/
/* Table: BFM_APP_NODE                                          */
/*==============================================================*/
create table BFM_APP_NODE 
(
   public_id            NUMBER(8)            not null,
   private_id           NUMBER(3)            not null,
   app_name             VARCHAR2(100)        not null,
   app_type             NUMBER(2)            not null,
   pc_node_id           NUMBER(8)            not null,
   rpt_date             VARCHAR2(8),
   rpt_time             NUMBER(8),
   last_time            NUMBER(17),
   node_status          NUMBER(2),
   start_datetime       VARCHAR(20),
   constraint PK_BFM_APP_NODE primary key (public_id, private_id)
)
tablespace BFM_TBS_MONITOR;

comment on column BFM_APP_NODE.app_type is
'0：dreb
1： bpc
2：类bpc，如网关cgate等';

comment on column BFM_APP_NODE.node_status is
'0正常
1报告异常
2超时异常';

/*==============================================================*/
/* Index: BF_APP_PNODE_FK                                       */
/*==============================================================*/
create index BF_APP_PNODE_FK on BFM_APP_NODE (
   pc_node_id ASC
)
tablespace BFM_TBS_MONITOR;

/*==============================================================*/
/* Index: APP_NODE_INDEX_LASTTIME                               */
/*==============================================================*/
create index APP_NODE_INDEX_LASTTIME on BFM_APP_NODE (
   last_time ASC,
   node_status ASC
);

/*==============================================================*/
/* Table: BFM_LOG                                               */
/*==============================================================*/
create table BFM_LOG 
(
   serial_no            VARCHAR2(17)         not null,
   app_type             NUMBER(2)            not null,
   public_id            NUMBER(8)            not null,
   private_id           NUMBER(3)            not null,
   rpt_time             NUMBER(17)           not null,
   rpt_type             NUMBER(1)            not null,
   rtp_txt              CLOB,
   constraint PK_BFM_LOG primary key (serial_no)
)
tablespace BFM_TBS_MONITOR;

comment on table BFM_LOG is
'记录各节点推送的信息
只保存前24小时的数据';

comment on column BFM_LOG.app_type is
'0：dreb
1： bpc
2：类bpc，如网关cgate等';

comment on column BFM_LOG.rpt_type is
'0：正常
1：警告
2：错误';

/*==============================================================*/
/* Index: Index_node                                            */
/*==============================================================*/
create index Index_node on BFM_LOG (
   public_id ASC,
   private_id ASC
)
tablespace BFM_TBS_MONITOR;

/*==============================================================*/
/* Table: BFM_LOG_HIS                                           */
/*==============================================================*/
create table BFM_LOG_HIS 
(
   serila_no            VARCHAR2(17)         not null,
   app_type             NUMBER(2)            not null,
   public_id            NUMBER(8)            not null,
   private_id           NUMBER(3)            not null,
   rpt_time             NUMBER(17)           not null,
   rpt_type             NUMBER(1)            not null,
   rtp_txt              CLOB,
   constraint PK_BFM_LOG_HIS primary key (serila_no)
)
tablespace BFM_TBS_MONITOR;

comment on table BFM_LOG_HIS is
'记录各节点推送的信息
';

comment on column BFM_LOG_HIS.app_type is
'0：dreb
1： bpc
2：类bpc，如网关cgate等';

comment on column BFM_LOG_HIS.rpt_type is
'0：正常
1：警告
2：错误';

/*==============================================================*/
/* Table: PFM_APP_CONNECT                                       */
/*==============================================================*/
create table PFM_APP_CONNECT 
(
   public_id            NUMBER(8)            not null,
   private_id           NUMBER(3)            not null,
   dst_public_id        NUMBER(8)            not null,
   dst_private_id       NUMBER(3)            not null,
   dst_ip               VARCHAR(20)          not null,
   dst_port             NUMBER(8)            not null,
   conn_type            NUMBER(2)            not null,
   conn_status          NUMBER(2)            not null,
   lasttime             NUMBER(17),
   constraint PK_PFM_APP_CONNECT primary key (public_id, private_id, dst_ip, dst_port)
)
tablespace BFM_TBS_MONITOR;

comment on column PFM_APP_CONNECT.conn_type is
'0：DREB之间的连接  TODREB
1：BPC连接DREB，即DREB服务连接
2：BPU连接BPC';

comment on column PFM_APP_CONNECT.conn_status is
'0正常
1报告异常
2超时异常';

/*==============================================================*/
/* Index: INDEX_APP_CONNECT_IP                                  */
/*==============================================================*/
create index INDEX_APP_CONNECT_IP on PFM_APP_CONNECT (
   public_id ASC,
   private_id ASC,
   dst_public_id ASC,
   dst_private_id ASC
)
tablespace BFM_TBS_MONITOR;

/*==============================================================*/
/* Index: INDEX_APP_CONNECT_LASTTIME                            */
/*==============================================================*/
create index INDEX_APP_CONNECT_LASTTIME on PFM_APP_CONNECT (
   lasttime ASC,
   conn_status ASC
)
tablespace BFM_TBS_MONITOR;

/*==============================================================*/
/* Table: PFM_BPC2_REPORT                                       */
/*==============================================================*/
create table PFM_BPC2_REPORT 
(
   public_id            NUMBER(8)            not null,
   private_id           NUMBER(3)            not null,
   report_name          VARCHAR(40)          not null,
   report_status        NUMBER(3)            not null,
   report_txt           VARCHAR(2048)        not null,
   lasttime             NUMBER(17)           not null,
   constraint PK_PFM_BPC2_REPORT primary key (public_id, private_id, report_name)
)
tablespace BFM_TBS_MONITOR;

comment on column PFM_BPC2_REPORT.report_status is
'0正常
1报告异常
2超时异常';

/*==============================================================*/
/* Index: index_bpc2_lasttime                                   */
/*==============================================================*/
create index index_bpc2_lasttime on PFM_BPC2_REPORT (
   lasttime ASC,
   report_status ASC
)
tablespace BFM_TBS_MONITOR;

/*==============================================================*/
/* Table: PFM_BPULIST                                           */
/*==============================================================*/
create table PFM_BPULIST 
(
   public_id            NUMBER(8)            not null,
   private_id           NUMBER(3)            not null,
   bpu_name             VARCHAR(20)          not null,
   bpu_num              NUMBER(3)            not null,
   real_num             NUMBER(3)            not null,
   htflag               NUMBER(1)            not null,
   dpmode               NUMBER(1)            not null,
   autostart            NUMBER(1)            not null,
   prgname              VARCHAR(40)          not null,
   prgpath              VARCHAR(200)         not null,
   runpath              VARCHAR(200)         not null,
   para                 VARCHAR(2048)        not null,
   lasttime             NUMBER(17)           not null,
   bpu_status           NUMBER(1),
   constraint PK_PFM_BPULIST primary key (public_id, private_id, bpu_name)
)
tablespace BFM_TBS_MONITOR;

comment on column PFM_BPULIST.bpu_status is
'0正常
1报告异常
2超时异常';

/*==============================================================*/
/* Index: Index_PFM_BPULIST_LASTTIME                            */
/*==============================================================*/
create index Index_PFM_BPULIST_LASTTIME on PFM_BPULIST (
   lasttime ASC,
   bpu_status ASC
)
tablespace BFM_TBS_MONITOR;

/*==============================================================*/
/* Table: PFM_DREB_CONNLIST                                     */
/*==============================================================*/
create table PFM_DREB_CONNLIST 
(
   public_id            NUMBER(8)            not null,
   private_id           NUMBER(3)            not null,
   dst_public_id        NUMBER(8)            not null,
   dst_private_id       NUMBER(3)            not null,
   conn_index           NUMBER(8),
   conn_status          NUMBER(3)            not null,
   lasttime             NUMBER(17)           not null,
   constraint PK_PFM_DREB_CONNLIST primary key (public_id, private_id, dst_public_id, dst_private_id)
)
tablespace BFM_TBS_MONITOR;

comment on column PFM_DREB_CONNLIST.conn_status is
'0正常
1报告异常
2超时异常';

/*==============================================================*/
/* Index: INDEX_DREB_CONNLIST                                   */
/*==============================================================*/
create index INDEX_DREB_CONNLIST on PFM_DREB_CONNLIST (
   lasttime ASC,
   conn_status ASC
)
tablespace BFM_TBS_MONITOR;

/*==============================================================*/
/* Table: PFM_PARAMETER                                         */
/*==============================================================*/
create table PFM_PARAMETER 
(
   para_name            VARCHAR(20)          not null,
   para_note            VARCHAR(100)         not null,
   para_value           VARCHAR(20)          not null,
   constraint PK_PFM_PARAMETER primary key (para_name)
)
tablespace BFM_TBS_MONITOR;

/*==============================================================*/
/* Table: PFM_PC_NODE                                           */
/*==============================================================*/
create table PFM_PC_NODE 
(
   pc_node_id           NUMBER(8)            not null,
   pc_node_name         VARCHAR2(100)        not null,
   pc_ip1               VARCHAR2(20),
   pc_ip2               VARCHAR2(20),
   pc_area              VARCHAR2(400),
   pc_mem_warn          NUMBER(3),
   pc_cpu_warn          NUMBER(3),
   pc_disk_warn         NUMBER(3),
   constraint PK_PFM_PC_NODE primary key (pc_node_id)
)
tablespace BFM_TBS_MONITOR;

comment on table PFM_PC_NODE is
'实际的硬件机器列表';

/*==============================================================*/
/* Table: PFM_PC_NODE_CPU                                       */
/*==============================================================*/
create table PFM_PC_NODE_CPU 
(
   pc_node_id           NUMBER(8)            not null,
   pc_node_cpu          NUMBER(3)            not null,
   constraint PK_PFM_PC_NODE_CPU primary key (pc_node_id)
)
tablespace BFM_TBS_MONITOR;

/*==============================================================*/
/* Table: PFM_PC_NODE_DISK                                      */
/*==============================================================*/
create table PFM_PC_NODE_DISK 
(
   pc_node_id           NUMBER(8)            not null,
   pc_disk_name         VARCHAR(200)         not null,
   pc_disk_total        NUMBER(8)            not null,
   pc_disk_used         NUMBER(8)            not null,
   constraint PK_PFM_PC_NODE_DISK primary key (pc_node_id, pc_disk_name)
)
tablespace BFM_TBS_MONITOR;

/*==============================================================*/
/* Table: PFM_PC_NODE_RAM                                       */
/*==============================================================*/
create table PFM_PC_NODE_RAM 
(
   pc_node_id           NUMBER(8)            not null,
   pc_total_ram         NUMBER(8)            not null,
   pc_used_ram          NUMBER(8)            not null,
   constraint PK_PFM_PC_NODE_RAM primary key (pc_node_id)
)
tablespace BFM_TBS_MONITOR;

/*==============================================================*/
/* Table: PFM_WARNNING                                          */
/*==============================================================*/
create table PFM_WARNNING 
(
   public_id            NUMBER(8)            not null,
   private_id           NUMBER(3)            not null,
   warn_flag            NUMBER(1)            not null,
   warn_txt             VARCHAR2(2048)       not null,
   process_txt          VARCHAR2(2048)       not null,
   constraint PK_PFM_WARNNING primary key (public_id, private_id)
)
tablespace BFM_TBS_MONITOR;

comment on column PFM_WARNNING.warn_flag is
'0：正常
1：提示  
2：警告
3：错误';

alter table BFM_APP_NODE
   add constraint FK_BFM_APP__机器节点_PFM_PC_N foreign key (pc_node_id)
      references PFM_PC_NODE (pc_node_id);

alter table BFM_LOG
   add constraint FK_BFM_LOG_REFERENCE_BFM_APP_ foreign key (public_id, private_id)
      references BFM_APP_NODE (public_id, private_id);

alter table BFM_LOG_HIS
   add constraint FK_BFM_LOG__REFERENCE_BFM_APP_ foreign key (public_id, private_id)
      references BFM_APP_NODE (public_id, private_id);

alter table PFM_WARNNING
   add constraint FK_PFM_WARN_REFERENCE_BFM_APP_ foreign key (public_id, private_id)
      references BFM_APP_NODE (public_id, private_id);


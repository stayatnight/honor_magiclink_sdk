/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slProbe.h                                                     *
* Description :                                                               *
*               smart lamp probe header file                                  *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2021-10-08                                                    *
* Mod History : (none)                                                        *
******************************************************************************/
#ifndef __SL_PROBE_H__
#define __SL_PROBE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include <stdint.h>
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define  LWPOWER_MSG_HEAD  (0xbebebebe)
#define  LWPOWER_MSG_TAIL   (0xeded)
#define  LWPOWER_MSG_DEVSTATE_LEN  (16)

#define BIT6	0x0040
#define BIT7	0x0080

//#define LWPOWER_MSG_HEAD (0xadbcaebf)
//#define LWPOWER_MSG_TAIL (0xbeedbeed)
#define  probeRspMsgBody

#define  PROBERSPMSGBODY_BASE       58


#define  LWPOWER_MSG_HEAD_OFFSET  (4+LWPOWER_MSG_DEVSTATE_LEN)
#define  LWPOWER_MSG_PAIRST_OFFSET  (4)
#define  LWPOWER_MSG_VER_OFFSET     (5)
#define  LWPOWER_MSG_PAIRST_YES     (1)
#define  LWPOWER_MSG_PAIRST_NO      (2)

#define  LWPOWER_MSG_LAMP_VER_LOW_OFFSET 5
#define  LWPOWER_MSG_LAMP_CAP_AND_VER_HIGH_OFFSET 6
//#define  LWPOWER_MSG_CLASS_OFFSET 7
//#define  LWPOWER_MSG_SKU_OFFSET 8
#define  LWPOWER_MSG_CLASS_OFFSET 2
#define  LWPOWER_MSG_SKU_OFFSET 3
#define  LWPOWER_MSG_SCN_OFFSET 9
#define  LWPOWER_MSG_BRI_OFFSET 10
#define  LWPOWER_MSG_CCT_OFFSET 11
#define  LWPOWER_MSG_RED_OFFSET 13
#define  LWPOWER_MSG_GREEN_OFFSET 14
#define  LWPOWER_MSG_BLUE_OFFSET 15
#define  LWPOWER_MSG_NET_STATUS_OFFSET 16

#define  LAMP_SUPPORT_RGB_AND_CW BIT7|BIT6
#define  LAMP_SUPPORT_ONLY_CW    BIT7
#define  LAMP_SUPPORT_ONLY_ONOFF BIT6



#define  LWPOWER_MSG_HEAD_LEN   (6)
#define  LWPOWER_MSG_TAIL_LEN    (2)

#define PAIR_CONFIG_VALID_HEAD (0xbebe)
#define PAIR_CONFIG_VLAID_TAIL (0xbeed)



#define CMD_PAIR_DEV      (0x7101)
#define CMD_ON_DEV        (0x7102)
#define CMD_OFF_DEV       (0x7103)

#define CMD_CALL_SCN_DEV  (0x7104)

#define CMD_DIM_ADD_DEV         (0x7105)
#define CMD_DIM_SUB_DEV         (0x7106)

#define CMD_CCT_ADD_DEV         (0x7107)
#define CMD_CCT_SUB_DEV         (0x7108)

#define CMD_ENTER_EASYLINK_DEV  (0x7109)
#define CMD_OFFMATCH_DEV        (0x7110)

#define CMD_MATCHWIFI_DEV        (0x7100)

#define CMD_TOGGLE_DEV           (0x7111)
#define CMD_BLINK_DEV            (0x7112)


#define MAX_PAIR_ITEM_NUM  8

#define   APFLAGON                 0
#define   APFLAGOFF                1
#define   JOIN_EASYLINK_ALREADY    1
#define   JOIN_EASYLINK_UNALREADY  0

#define   CMD_MSG_QUEUE_SIZE       2

#define   RET_QUEUE_FULL           1
#define   QUEUE_VALID_FLAG         0xabef

#define   MSG_HANDLE_FINISH        0
#define   MSG_HANDLE_CONTINUE      1


#define  MAC_ADDR_BROADCAST  (0x01)
#define  MAC_ADDR_SAME_SELF   (0x02)
#define  MAC_ADDR_DIF_SELF   (0x03)


#define OPP_SWTICH_ON       1
#define OPP_SWTICH_OFF     0

//led off status
#define  LAMP_STATUS_UNKNOW             (0x00)
#define  LAMP_STATUS_OFF                     (0x01)
#define  LAMP_STATUS_ON                       (0x02)
#define  LAMP_STATUS_PAIR                    (0x03)

#define  OPP_YES   (1)
#define  OPP_NO    (0)

#define  msleep  dna_task_sleep
#define  mico_get_time  dna_system_ticks
/************ controler dim cct var ****************/
#define   MAX_STEP_NUM             60
//单步周期 ms
#define   STEP_PERIOD                 8

#define DIMCCTTIMER_INTRVL  (50)
#define DIMCCTTIMER_ACT_PERIOD  (300)
#define DIMCCTTIMER_ACT_COUNT   (100)

#define DIM_CCT_FLAG_NO               (0)
#define DIM_CCT_FLAG_DIMADD       (1)
#define DIM_CCT_FLAG_DIMSUB       (2)
#define DIM_CCT_FLAG_CCTADD       (3)
#define DIM_CCT_FLAG_CCTSUB        (4)
//**************************************************//

/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
#pragma pack(1)
//***************************************/
typedef struct _linkStatus_t{
  int is_connected;       /**< The link to wlan is established or not, 0: disconnected, 1: connected. */
  int wifi_strength;      /**< Signal strength of the current connected AP */
  uint8_t  ssid[32];      /**< SSID of the current connected wlan */
  uint8_t  bssid[6];      /**< BSSID of the current connected wlan */
  int channel;
} OPP_LinkStatusTypeDef;

typedef struct {
  uint32_t head;
  uint16_t packLen;
  uint32_t cmd;
  uint8_t  cmdData[1];
}LOW_POWER_PNL_MSG_HEAD;

typedef struct {
  uint16_t crc;
  uint32_t tail;
}LOW_POWER_PNL_MSG_TAIL;

typedef struct {
  uint8_t pmac[6];
  uint16_t randomData;
  uint8_t lmac[6];
  uint8_t keyIdx;
  uint8_t pairedHash;
  uint16_t devNum;
}CDM_DATA_PAIR_DEV;

typedef struct {
  //uint16_t devNum;
  uint8_t targetMac[8];
  uint16_t scnNum;
}CDM_DATA_CALL_SCN;

typedef struct {
  //uint16_t devNum;
  uint8_t targetMac[8];
  uint16_t dimStep;
  uint16_t cmdNum;
}CDM_DATA_DIM_MSG;

typedef struct {
  uint8_t  targetMac[8];
  uint32_t cmdNum;
}CDM_DATA_TOGGLE_MSG;

typedef struct {
  //uint16_t devNum;
  uint8_t targetMac[8];
  uint16_t cctStep;
  uint16_t cmdNum;
}CDM_DATA_CCT_MSG;

typedef struct {

  uint16_t devNum;

}CDM_DATA_EASYLINK_MSG;

typedef struct {
  uint16_t devNum;
  uint16_t isallclear;
}CDM_DATA_OFFMATCH_MSG;

typedef struct{
  CDM_DATA_PAIR_DEV  pairPnlInfo;//配对的面板消息 
  uint16_t pad;
}PAIR_PNL_ITEM;

typedef struct{
  uint32_t tableHeadValid;
  uint32_t validItemNum;
  PAIR_PNL_ITEM items[MAX_PAIR_ITEM_NUM];
  uint32_t tableTailValid;
}PAIR_PNL_TABLE;


typedef struct{
   uint16_t cmdId;
   uint8_t  cmdData[108];
}LOW_PNL_CMD_MSG;
/******************************************************************************
 Function    :   
 Description :  低功耗面板命令处理函数
 Note        : (none)
 Para        :
               该命令的消息参数
 Return      : 
                0   命令执行完毕
                1   命令需要继续执行
 Other       : (none)
******************************************************************************/
typedef uint8_t (*LOW_PNL_CMD_MSG_PROC)( LOW_PNL_CMD_MSG* msg );

/******************************************************************************
 Function    :   
 Description :  低功耗面板命令处理函数
 Note        : (none)
 Para        :
               该命令的消息参数
 Return      : 
                0   命令执行完毕
                1   命令需要继续执行
 Other       : (none)
******************************************************************************/
typedef uint8_t (*LOW_PNL_CMD_MSG_ABORT)( LOW_PNL_CMD_MSG* msg );

typedef struct{
    uint32_t                     validFlag;
    LOW_PNL_CMD_MSG         msgData;
    LOW_PNL_CMD_MSG_PROC    procHandle;
    LOW_PNL_CMD_MSG_ABORT   abortHandle;
}LOW_PNL_CMD_MSG_UNIT;

typedef struct{
    LOW_PNL_CMD_MSG_UNIT list[CMD_MSG_QUEUE_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t isFull;
}LOW_PNL_CMD_MSG_QUEUE;

typedef struct{
   uint32_t    targetPwm[4];//目标PWM 值
   uint32_t    srcPwm[4];   //起始的PWM值
   float  fStepPwm[4]; //步进的PWM 值
   uint32_t    lastTick;    //上次调节时的tick值
   uint8_t     stepWay[4];  //每个通道的变换方式 1 增加 ；0 减少
   uint16_t    wStepTotal;  //总步数
   uint16_t    wStepCur;    //当前的步数
   uint8_t     isInit;      // 0 未初始化，1 初始化 初始化表示计算了目标亮度 对应的target Pwm
   uint8_t     targetBri;   //目标输出的亮度
   uint8_t     srcBri;      //设定目标时当时亮度值  ,初始化前 delta = |targetBri - srcBri| ；初始化时，根据系统当前亮度 修改 srcBri 表示此次希望调解的亮度变化值,初始化后，srcBri + 
   uint8_t     pad;
}MSG_DIMMER_DATA;

typedef struct{
   uint32_t    targetPwm[4];//目标PWM 值
   uint32_t    srcPwm[4];   //起始的PWM值
   float  fStepPwm[4]; //步进的PWM 值
   uint32_t    lastTick;    //上次调节时的tick值
   uint8_t     stepWay[4];  //每个通道的变换方式 1 增加 ；0 减少
   uint16_t    wStepTotal;  //总步数
   uint16_t    wStepCur;    //当前的步数
   uint16_t    targetCCT;   //目标输出的亮度
   uint16_t    srcCCT;      //设定目标时当时亮度值  ,初始化前 delta = |targetBri - srcBri| ；初始化时，根据系统当前亮度 修改 srcBri 表示此次希望调解的亮度变化值,初始化后，srcBri + 
   uint8_t     isInit;      // 0 未初始化，1 初始化 初始化表示计算了目标亮度 对应的target Pwm
   uint8_t     pad;
}MSG_CCT_DATA;

// headFlag  prtclId prtclV xxx tailFlag

typedef struct {
  //LWPOWER_MSG_HEAD
  uint32_t headFlag;
  //recall dev state
  uint8_t   devState[16];
  uint8_t  prtclId;
  uint8_t  prtclVersion;
  uint8_t  prtclBody[1];
}WIFI_LOW_PRTCL_MAIN_HEAD;


typedef struct {
  //LWPOWER_MSG_TAIL
  uint16_t tailFlag ;
}WIFI_LOW_PRTCL_MAIN_TAIL;

typedef struct {
  uint8_t  dataLen;
  uint8_t  pData[1] ;
}BROADCAST_ONOFF_MSG;


typedef struct
{
    uint8_t len;     /**< SSID length */
    uint8_t val[32]; /**< SSID name (AP name)  */
} scan_ssid_t;

typedef struct
{
    int32_t number_of_probes_per_channel;                     /**< Number of probes to send on each channel                                               */
    int32_t scan_active_dwell_time_per_channel_ms;            /**< Period of time to wait on each channel when active scanning                            */
    int32_t scan_passive_dwell_time_per_channel_ms;           /**< Period of time to wait on each channel when passive scanning                           */
    int32_t scan_home_channel_dwell_time_between_channels_ms; /**< Period of time to wait on the home channel when scanning. Only relevant if associated. */
} scan_extended_params_t;
#pragma pack()


#pragma pack(1)
typedef struct {
    uint16_t frame_control;
    uint16_t duration;
    uint8_t addr1[6];
    uint8_t addr2[6];
    uint8_t addr3[6];
    uint16_t seq;
} ieee80211_header_t;

typedef struct {
    ieee80211_header_t header;
    uint8_t ssid_id;
    uint8_t ssid_len;
} stProbeReqFrame_t;

typedef struct {
    ieee80211_header_t header;
    uint8_t timeStamp[8];
    uint16_t interval;
    uint16_t capability;
    uint8_t ssid_id;
    uint8_t ssid_len;
} stProbeRespFrame_t;

typedef struct {
    uint8_t version;
    uint8_t control;
    uint16_t duartion;
    uint8_t *pDestination;
    uint8_t *pSource;
    uint8_t *pBSSID;
    uint16_t seqFragNum;
    uint8_t *pSSID;
    uint8_t  ssid_len;
    uint8_t vendor_spec_id;
    uint8_t user_cmd_len;
    uint8_t *pUserCmd;
} probe_req_frame;

#pragma pack()

typedef  struct dna_wlan_mac_hdr {
    struct {
        unsigned short ver:2;
        unsigned short type:2;
        unsigned short subtype:4;
        unsigned short tods:1;
        unsigned short frds:1;
        unsigned short morefrag:1;
        unsigned short retry:1;
        unsigned short pwrmgmt:1;
        unsigned short moredata:1;
        unsigned short wep:1;
        unsigned short order:1;
    }fc;
    unsigned short duration;
    unsigned char addr1[6];
    unsigned char addr2[6];
    unsigned char addr3[6];
    unsigned short frag:4;
    unsigned short sequence:12;
}  dna_wlan_mac_hdr_t;


void startLowerPowerAp();
//void stopLowerPowerAp();
void Opp_SendOTAResultStatus(void);

void Opp_SendOTAProcessStatus(void);

void slProbeSendRequest(const char *ifname, char *ssid, int ssid_len, uint8_t *pData, int data_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */




#endif /*__SL_PROBE_H__*/




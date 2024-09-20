/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slProbe.c                                                     *
* Description :                                                               *
*               smart lamp probe source file                                  *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2021-10-08                                                    *
* Mod History : (none)                                                        *
******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "slProbe.h"
#include "stdio.h"
#include <stdlib.h>
#include <math.h>

#include "wifi_constants.h"
#include "wifi_conf.h"
#include "wifi_util.h"
#include "myHal.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define WLAN_FC_STYPE_PROBE_RSP 0x50
#define WLAN_FC_STYPE_PROBE_REQ 0x40

#define ALI_OUI                     "\xC8\x93\x46"   /** MXCHIP OUI C8-93-46*/
#define DOT11_MNG_VS_ID             221 /* d11 management Vendor Specific IE */
#define _SSID_IE_               0
#define _SUPPORTEDRATES_IE_ 1

#define SEARCH_CLASS_SKU_DEVICE  0x01
#define BIND_CLASS_SKU_DEVICE    0x03
#define READ_PIN_SN_CMD          0x05
#define CLOSE_WIFI_CMD           0x07
#define OPEN_WIFI_RANDOM_CMD     0x09
#define SEND_DTLOCK_RANDOM_CMD   0x0D
#define SET_SSID_KEY_CMD         0x11
#define SET_IP_PORT_CMD          0x13
#define START_OTA_CMD            0x15
#define CHECK_PROCESS_CMD        0x17
#define WRITE_SNPIN_CMD          0x19
#define WRITE_DTLOCKSALT_CMD     0x1B
#define FLASH_LIGHT_CMD          0x26

#define ONOFF_WIFI_CMD   0x02

//////////////////////////oppprobep01v01app.c/////////////////////////////////////////////
#define  PRTCL01ID              (0x01)
#define  PRTCL01V01            (0x01)

#define P01V01_HEAD_LEN  (24)

#define CRC_CHECK_MASK    (0x01)
#define PWD_CHECK_MASK   (0x10)

//no need to pair first
#define  CHECK_PAIR_NO         (0x00)
//need to pair
#define  CHECK_PAIR_YES       (0x01)
//whether pair depend on cmdId
#define  CHECK_PAIR_CMD      (0x02)

//#define  PRTCL_CMD_SPECIAL_ON_CMD    (0x7124)
//#define  PRTCL_CMD_SPECIAL_OFF_CMD  (0x7125)

#define  PRTCL_CMD_MULTI_FUNCTION  (0x7123)

#define  PRTCL_CMD_ON_DEV        (0x7103)
#define  PRTCL_CMD_OFF_DEV       (0x7104)


#define  PRTCL_CMD_SET_NETINFO_PINSN (0x7200)


#define Build_uint16(loByte, hiByte)                                 \
    ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

extern void oppDataReadSN(unsigned char *pSN, unsigned char inLen, unsigned char *outLen);
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
typedef enum
{
    OPP_SUCCESS = 0, 
    OPP_FAILURE, 
    OPP_OVERFLOW, 
    OPP_NO_PRIVILEGE, 
    OPP_EXISTED, 
    OPP_UNKNOWN
}OPP_ERROR_CODE;

#pragma pack(1)

typedef enum
{
    CMD_STATE_REQ = 1,
    CMD_STATE_ACK,
    CMD_STATE_ACK_RSP,
    CMD_STATE_RSP_ACK,
} EN_P01V01CMD_STATE;

typedef enum
{
    CMD_MODE_NOACK = 0,
    CMD_MODE_ACK,
    CMD_MODE_ACK_RSP,
    CMD_MODE_CONTINUE = 0xff,
} EN_P01V01CMD_MODE;


// 22
typedef struct
{

    //resever 2byte
    uint16_t reseverHead;
    //password crc valid
    uint8_t  pwdCrcValid;
    // total wifi_low_prtcl package len
    uint8_t  dataLen;
    // total pack crc ,inlcude headFlag tailFlag
    uint16_t crc ;
    uint8_t  sMac[6];
    //0xffffffffff  broadcast or multicast
    uint8_t  dMac[6];
    //whether need to check pair ; 0  no need
    uint8_t  isPairCheck;
    //grup num when dMac 0xffff valid ,0xff broadcast
    uint8_t  grupNum;
    uint32_t reserveTail;
    uint8_t  msgBody[1];
} WIFI_L_P01V01_HEAD;

//luan

typedef struct
{
    uint32_t PACK_HEAD;
    uint8_t  RESERVED[16];
    uint8_t  PROTOCOL_ID;
    uint8_t  PROTOCOL_VERSION;
    uint16_t BACKUP_FIRST;
    uint8_t  PACMODE;
    uint8_t  LEN;
    uint16_t CRC_VALUE;
    uint8_t  SMAC[6];
    uint8_t  DMAC[6];
    uint8_t  MATCHFLAG;
    uint8_t  GROUPNUMBER;
    uint32_t BACKUP_END;
} PROBE_MSG_HEAD;

typedef struct
{
    uint16_t PACK_END;
} PROBE_MSG_TAIL;

typedef struct
{
    uint16_t LampVer;
    uint8_t LampCap;
    uint8_t CurrentScn;
    uint8_t Bri;
    uint16_t CCT;
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint8_t RESERVE[4];
} PROBE_RSP_LAMPINFO;

typedef struct
{
    uint16_t Cmd;
    uint8_t SubCmd;
    uint8_t Len;
    uint8_t RESERVE[27];
} PROBE_RSP_CMDINFO;


typedef struct
{
    PROBE_MSG_HEAD                     msg_head;  //46 bytes
    PROBE_RSP_LAMPINFO                 probe_light_data_send; //14 bytes
    PROBE_RSP_CMDINFO                  probe_sub_cmd;         //31 bytes
    PROBE_MSG_TAIL                     msg_tail;              //2 bytes
} PROBE_LIGHT_MSG_SEND;


typedef struct
{
    //command id
    uint16_t  cmdId;
    //random value ,the same command id ;the random Num ,action once
    uint16_t  randomNum;
    // command state : 0x01 req , ack ,ack rsp ,rsp ack
    uint8_t   cmdState;
    // communication mode  0x00 no ack ;0x01 ack; 0x02 rsp ack
    uint8_t   communMode;
    uint8_t   cmdData[1];
} WIFI_L_P01V01_BODY;

typedef struct
{
    uint8_t  ucSubCMD;
    uint8_t  ucLen;
    uint8_t  aucPayload[6];
    uint8_t  aucReserve[4];
} SET_NETINFO_PINSN_MSG_REQ;
typedef struct
{
    uint8_t  ucSubCMD;
    uint8_t  ucLen;
    uint8_t  aucPayload[64+8+4];
} SET_NETINFO_DATALOCK_MSG_REQ;


typedef struct {
    uint8_t    dstMac[6][6];
    uint8_t    hopNum;
    uint32_t  resever;
} SPECIAL_ONOFF_MSG_REQ;
#pragma pack()
/******************************************************************************
*                                Globals                                      *
******************************************************************************/
static uint8_t  g_probeRspMsgBody[68+25] = {0xbe, 0xbe, 0xbe, 0xbe, 0x00};
static uint8_t  g_aucSNPIN[24];
//static uint8_t  g_aucPIN[8];

//static uint8_t  g_ucEnableBindAction = 0;
static uint8_t  g_ucSNPINReaded = 0;
//static uint8_t  g_ucKongKouFlow = 0;
//static uint8_t  g_ucOtaStart = 0;


uint8_t  g_IsLampInPairState = OPP_NO;
uint8_t  g_ucTimeOver = 0;
uint8_t  g_ucCloseWifiInterface = 0;
int  g_ucSNPINExistFlag = 0;



static uint8_t  g_ucWifiCmd = 0x00;
static uint8_t  g_aucPairedMacAddr[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
//static uint8_t  g_u8FlashLightResultCode = 0;



//uint16_t g_DevRandom[6];

static uint8_t *custom_ie = NULL;
static uint8_t custom_ie_len = 0;
uint8_t  g_RspData[120+35+15] = {0};
uint8_t   g_IsEverSndRsp = 0;

uint8_t  *g_pLowerRecvdata;
uint8_t    g_pLowerRecvLen;

uint16_t g_u16RandomData = 0; /*空口随机数*/
uint8_t  g_u8OTAProcess = 0;
uint8_t  g_aucDstMac[6] = {0,0,0,0,0,0};

uint16_t g_uwRemoterCmdSave = 0;

static uint8_t a_ucProbeBuf[1024];
static stProbeReqFrame_t *pstProbeReqFrame;
static stProbeRespFrame_t *pstProbeRespFrame;
/******************************************************************************
*                          Extern Declarations                                *
******************************************************************************/
#if 0
extern OPP_WIFI_STA_STATE g_Opp_Wifi_CurState;
extern ST_OPP_LAMP_INFO g_stThisLampInfo;
extern config_t configParas;
extern char g_acLocalHost[16]; //="192.168.3.43";
extern int  g_u32HttpPort; // = 80;
extern int  g_u8WifiSetParamFlag;
extern int  gCallSenceNum;
#endif

void RecvDataFromProble( uint8_t *pData, uint32_t dataLen, uint8_t *pSrcMac );
//extern void Opp_Disable_Uart0(void);

/******************************************************************************
*                            Implementations                                  *
******************************************************************************/
/******************************************************************************
 Function    : IsPrtclMainMsgValid
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t IsPrtclMainMsgValid( uint8_t *pData, uint32_t dataLen, uint8_t *pSrcMac )
{
    //????β
    WIFI_LOW_PRTCL_MAIN_HEAD    *pPrtclMainHead = ( WIFI_LOW_PRTCL_MAIN_HEAD * )pData;
    WIFI_LOW_PRTCL_MAIN_TAIL      *pPrtclMainTail =  ( WIFI_LOW_PRTCL_MAIN_TAIL * )( pData + dataLen - sizeof(
                                                                                                     WIFI_LOW_PRTCL_MAIN_TAIL ) );

    if( pPrtclMainHead->headFlag != LWPOWER_MSG_HEAD )
    {
        //    printf(" msg head invalid =%x",pPrtclMainHead->headFlag);
        return OPP_FAILURE;
    }

    if( pPrtclMainTail->tailFlag != LWPOWER_MSG_TAIL )
    {
        printf( " msg tail invalid =%x", pPrtclMainTail->tailFlag );
        return OPP_FAILURE;
    }

    //????Э???ж??????Ч??
    //  if((g_pPrtclMainHead->prtclId == PRTCL01ID) && (g_pPrtclMainHead->prtclVersion == PRTCL01V01)){
    //  return IsP01V01MsgValid(pData,dataLen,g_pPrtclMainHead->prtclBody,pSrcMac);
    // }
    return OPP_SUCCESS;
}

/******************************************************************************
 Function    : HandleP01V01SetNetInfoPINSNCmd
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void HandleP01V01SetNetInfoPINSNCmd( uint8_t *pPrtclData, uint8_t  *pSrcMac )
{
    WIFI_L_P01V01_BODY  *pReqCmdBody = ( WIFI_L_P01V01_BODY * )pPrtclData;
    SET_NETINFO_PINSN_MSG_REQ *pReqMsg = ( SET_NETINFO_PINSN_MSG_REQ * ) pReqCmdBody->cmdData;
    //SET_NETINFO_DATALOCK_MSG_REQ *pReqMsg_datalock = ( SET_NETINFO_DATALOCK_MSG_REQ * ) pReqCmdBody->cmdData;
    static uint16_t randomNum = 0xFFFF;
    //uint16_t uwTemp = 0;
    //uint32_t uwMs;

    //uint8_t ret = OPP_NO;


    if( randomNum ==  pReqCmdBody->randomNum )
    {
        return;
    }

    //      DEBUG_LOG("%s, ssid %s, key %s, class %x, sku %x\r\n", __FUNCTION__,
    //              pReqMsg->aucSSID, pReqMsg->acuKEY, pReqMsg->uwClass, pReqMsg->uwSKU);
    //      if ((pReqMsg->uwClass != PRODUCTCLASS) || (pReqMsg->uwSKU != PRODUCTSKU)) {
    //          DEBUG_LOG("incorrect class/sku,return\r\n");
    //
    //          g_ucUnderFCTSN = 0;
    //          return;
    //      }
    #if 0
    ret = IsInPairState();

    if ( ret = OPP_NO )
    {
        randomNum = pReqCmdBody->randomNum;
        g_ucUnderFCTSN = 0;
        return;
    }

    #endif

    #if 0
    memset( g_ConfigApInfo.ssid, 0, sizeof( g_ConfigApInfo.ssid ) );
    memset( g_ConfigApInfo.psk, 0, sizeof( g_ConfigApInfo.psk ) );
    memcpy( g_ConfigApInfo.ssid, pReqMsg->aucSSID, os_strlen( pReqMsg->aucSSID ) );
    memcpy( g_ConfigApInfo.psk, pReqMsg->acuKEY, os_strlen( pReqMsg->acuKEY ) );
    g_ConfigApInfo.ssid_len = os_strlen( pReqMsg->aucSSID );
    g_ConfigApInfo.psk_len = os_strlen( pReqMsg->acuKEY );
    DEBUG_LOG( "config ssid: %s\r\n", g_ConfigApInfo.ssid );
    OPP_Wifi_State_Change( ( -1 ), 4/*OPP_WIFI_CONNECTING*/ );
    #endif
    randomNum = pReqCmdBody->randomNum;
    DEBUG_LOG( "subCMD:%02X\n", pReqMsg->ucSubCMD );
    g_ucWifiCmd = 0;

    if(g_ucCloseWifiInterface == 1 && pReqMsg->ucSubCMD != 0x07)
        return;

    switch(pReqMsg->ucSubCMD) {
    case 0x05:
        if(g_ucCloseWifiInterface == 2)
        {
            break;
        }
        if(g_IsLampInPairState == OPP_NO)
        {
            DEBUG_LOG( "g_IsLampInPairState=%d,0x05\n", g_IsLampInPairState );
            break;
        }
        if (0 != memcmp( g_aucPairedMacAddr, pSrcMac, 6 ))
        {
            DEBUG_LOG( "pSrcMac=%02X,%02X,%02X,0x05\n", g_IsLampInPairState, *pSrcMac, *(pSrcMac+1), *(pSrcMac+2) );
            break;
        }
        g_ucWifiCmd = READ_PIN_SN_CMD;
        g_ucSNPINReaded = 1;
        break;
    }
#if 0
    switch(pReqMsg->ucSubCMD)
    {
        case 0x01:
            if(g_ucCloseWifiInterface == 2)
            {
                //g_ucWifiCmd = CLOSE_WIFI_CMD;
                break;
            }
            uwTemp = pReqMsg->aucPayload[1];
            uwTemp <<= 8;
            uwTemp |= pReqMsg->aucPayload[0];
            if(PRODUCTCLASS_OVERSEA != uwTemp)
            {
                DEBUG_LOG( "uwTemp=%04X\n", uwTemp );

                break;
            }

            uwTemp = pReqMsg->aucPayload[3];
            uwTemp <<= 8;
            uwTemp |= pReqMsg->aucPayload[2];

            if(PRODUCTSKU_OVERSEA != uwTemp)
            {
                DEBUG_LOG( "uwTemp=%04X\n", uwTemp );

                break;
            }
            g_ucWifiCmd = SEARCH_CLASS_SKU_DEVICE;
            g_ucTimeOver = 0;
            if(g_IsLampInPairState != OPP_NO)
            {
                DEBUG_LOG( "g_IsLampInPairState=%d,0x01\n", g_IsLampInPairState );
                break;
            }

            uwMs = LOS_Tick2MS(( UINT32 )LOS_TickCountGet());
            uwMs /= 10;
            if(uwMs > pReqMsg->aucPayload[5])
                uwMs -= pReqMsg->aucPayload[5];
            else
                uwMs = 0;
            uwMs /= 10;
            if(uwMs > pReqMsg->aucPayload[4])
            {
                DEBUG_LOG( "uwMs=%d, %d, %d \n", uwMs, pReqMsg->aucPayload[4], pReqMsg->aucPayload[5]);
                g_ucTimeOver = 1;
                break;
            }
            g_ucEnableBindAction = 1;
            break;
        case 0x03:
            if(g_ucCloseWifiInterface == 2)
            {
                //g_ucWifiCmd = CLOSE_WIFI_CMD;
                break;
            }
            if(g_ucEnableBindAction != 1)
            {
                DEBUG_LOG( "g_ucEnableBindAction=%d\n", g_ucEnableBindAction );
                break;
            }
            uwTemp = pReqMsg->aucPayload[1];
            uwTemp <<= 8;
            uwTemp |= pReqMsg->aucPayload[0];
            if(PRODUCTCLASS_OVERSEA != uwTemp)
            {
                DEBUG_LOG( "uwTemp=%04X\n", uwTemp );
                break;
            }

            uwTemp = pReqMsg->aucPayload[3];
            uwTemp <<= 8;
            uwTemp |= pReqMsg->aucPayload[2];

            if(PRODUCTSKU_OVERSEA != uwTemp)
            {
                DEBUG_LOG( "uwTemp=%04X\n", uwTemp );
                break;
            }
            g_ucWifiCmd = BIND_CLASS_SKU_DEVICE;
            if(g_IsLampInPairState != OPP_NO)
            {
                DEBUG_LOG( "g_IsLampInPairState=%d,0x03\n", g_IsLampInPairState );
                if (0 != memcmp( g_aucPairedMacAddr, pSrcMac, 6 ))
                    g_ucWifiCmd |= 0x80;
                break;
            }
            g_IsLampInPairState = OPP_YES;
            memcpy( g_aucPairedMacAddr, pSrcMac, 6 );
            DEBUG_LOG( "g_aucPairedMacAddr=%02X,%2X,%2X\n", g_aucPairedMacAddr[0], g_aucPairedMacAddr[1], g_aucPairedMacAddr[2]);

            break;
        case 0x05:
            if(g_ucCloseWifiInterface == 2)
            {
                //g_ucWifiCmd = CLOSE_WIFI_CMD;
                break;
            }
            if(g_IsLampInPairState == OPP_NO)
            {
                DEBUG_LOG( "g_IsLampInPairState=%d,0x05\n", g_IsLampInPairState );
                break;
            }
            if (0 != memcmp( g_aucPairedMacAddr, pSrcMac, 6 ))
            {
                DEBUG_LOG( "pSrcMac=%02X,%02X,%02X,0x05\n", g_IsLampInPairState, *pSrcMac, *(pSrcMac+1), *(pSrcMac+2) );
                break;
            }
            g_ucWifiCmd = READ_PIN_SN_CMD;
            g_ucSNPINReaded = 1;
            break;
        case 0x07:
            if(g_u8OTAProcess == 255)
            {
                DEBUG_LOG( "0x07:g_u8OTAProcess=255\n");
                //OppLampCtrlSimuCallSceneMsg( 1 );
            }
            else if(g_ucCloseWifiInterface == 2)
            {
                g_ucWifiCmd = CLOSE_WIFI_CMD;
                break;
            }
            else if(g_ucSNPINReaded == 0)
                break;
            if (0 != memcmp( g_aucPairedMacAddr, pSrcMac, 6 ))
            {
                DEBUG_LOG( "07pSrcMac=%02X,%02X,%02X,0x05\n", g_IsLampInPairState, *pSrcMac, *(pSrcMac+1), *(pSrcMac+2) );
                break;
            }
            if(g_u8OTAProcess == 255)
            {
                DEBUG_LOG( "CLOSE_WIFI_CMD:g_u8OTAProcess=255\n");
                g_u8OTAProcess = 0;
            }
            gCallSenceNum = 0;
            OppLampCtrlSimuCallSceneMsg( 0 );
            g_ucWifiCmd = CLOSE_WIFI_CMD;
            if(g_ucCloseWifiInterface == 1)
                g_ucWifiCmd |= 0x80;
            g_ucCloseWifiInterface = 1;
            break;

        case OPEN_WIFI_RANDOM_CMD:
            g_ucWifiCmd = OPEN_WIFI_RANDOM_CMD;
            g_ucKongKouFlow = 0x01;
            if(g_u16RandomData == 0)
            {
                g_u16RandomData = (uint16_t)LOS_TickCountGet();
                g_u16RandomData *= g_u16RandomData;
            }

            DEBUG_LOG( "OPEN_WIFI_RANDOM_CMD =%02x, %d,%02X\r\n", g_u16RandomData, g_ucCloseWifiInterface, g_ucKongKouFlow);
            break;
        case SEND_DTLOCK_RANDOM_CMD:
            if(g_ucKongKouFlow == 0)
                break;
            g_ucWifiCmd = SEND_DTLOCK_RANDOM_CMD;

            uwTemp = pReqMsg_datalock->aucPayload[1+64];
            uwTemp <<= 8;
            uwTemp |= pReqMsg_datalock->aucPayload[0+64];
            DEBUG_LOG( "SEND_DTLOCK_RANDOM_CMD1 = %04X,%04X,%02X\r\n", uwTemp, g_u16RandomData, pReqMsg_datalock->aucPayload[0]);
            if(uwTemp != g_u16RandomData)
            {
                g_ucWifiCmd |= 0x80;
                break;
            }
            g_ucKongKouFlow |= 0x80;
            if(0 != HILINK_CheckDatalock(pReqMsg_datalock->aucPayload, 70))
            {
                g_ucWifiCmd |= 0x40;
                for(uwTemp=0;uwTemp<70;uwTemp++)
                    DEBUG_PRINTF( "%02x",pReqMsg_datalock->aucPayload[uwTemp]);
                DEBUG_PRINTF( "\r\n");
            }
            else
            {
                if(g_ucCloseWifiInterface != 0)
                {
                    TestWriteSNPINFlagId(0); //开空口
                    g_ucCloseWifiInterface = 0;
                }
                g_ucSNPINReaded = 1; //允许关空口
                memcpy( g_aucPairedMacAddr, pSrcMac, 6 );
                g_ucKongKouFlow |= 0x02;
                gCallSenceNum = 1;
                OppLampCtrlSimuCallSceneMsg( 1 );
            }
            DEBUG_LOG( "SEND_DTLOCK_RANDOM_CMD2=%02X,%d,%02X\r\n", g_ucWifiCmd, g_ucCloseWifiInterface, g_ucKongKouFlow);
            break;
        case SET_SSID_KEY_CMD:
#if 0
            if(g_ucCloseWifiInterface == 2 || (g_ucKongKouFlow & 0x02) == 0 )
#else
            if (g_ucCloseWifiInterface == 2)
#endif
            {
                //g_ucWifiCmd = CLOSE_WIFI_CMD;
                break;
            }
            g_ucWifiCmd = SET_SSID_KEY_CMD;
            memset( configParas.wifi_ssid, 0x00, sizeof( configParas.wifi_ssid )  );
            memset( configParas.wifi_key, 0x00, sizeof( configParas.wifi_key )  );
            memcpy(configParas.wifi_ssid, pReqMsg_datalock->aucPayload, 16);
            memcpy(configParas.wifi_key, pReqMsg_datalock->aucPayload+16, 16);
            DEBUG_LOG( "SET_SSID_KEY_CMD =%s,%s\r\n", configParas.wifi_ssid, configParas.wifi_key);
            g_ucKongKouFlow |= 0x04;
            break;
        case SET_IP_PORT_CMD:
#if 0
            if(g_ucCloseWifiInterface == 2 || (g_ucKongKouFlow & 0x02) == 0 )
#else
            if (g_ucCloseWifiInterface == 2)
#endif
            {
                //g_ucWifiCmd = CLOSE_WIFI_CMD;
                break;
            }
            g_ucWifiCmd = SET_IP_PORT_CMD;
            snprintf( g_acLocalHost, sizeof( g_acLocalHost ), "%u.%u.%u.%u",
                  pReqMsg_datalock->aucPayload[3],pReqMsg_datalock->aucPayload[2],
                  pReqMsg_datalock->aucPayload[1],pReqMsg_datalock->aucPayload[0]);
            g_u32HttpPort = pReqMsg_datalock->aucPayload[5];
            g_u32HttpPort <<= 8;
            g_u32HttpPort |= pReqMsg_datalock->aucPayload[4];
            g_ucKongKouFlow |= 0x08;
            DEBUG_LOG( "***SET_IP_PORT_CMD=%s,%d,%02x\r\n", g_acLocalHost, g_u32HttpPort, g_ucKongKouFlow );
            break;
        case START_OTA_CMD:
#if 0
            if(g_ucCloseWifiInterface == 2 || (g_ucKongKouFlow & 0x0f) != 0x0f )
#else
            if (g_ucCloseWifiInterface == 2)
#endif
            {
                DEBUG_LOG( "***START_OTA_CMDbreak=%d,%02x\r\n", g_ucCloseWifiInterface, g_ucKongKouFlow );
                break;
            }
            if (1 == g_ucOtaStart)
            {
                g_ucWifiCmd = START_OTA_CMD;
                break;
            }
            g_ucWifiCmd = START_OTA_CMD;
            g_u8WifiSetParamFlag = 0x00aa;
            g_u8OTAProcess = 0;
            g_ucKongKouFlow |= 0x10;
            g_ucOtaStart = 1;
            DEBUG_LOG( "***START_OTA_CMD=%d\r\n", g_u8WifiSetParamFlag );
            break;
        case CHECK_PROCESS_CMD:
            if (g_ucCloseWifiInterface == 2)
            {
                DEBUG_LOG( "***CHECK_PROCESS_CMDbreak=%d,%02x\r\n", g_ucCloseWifiInterface, g_ucKongKouFlow );
                break;
            }
            g_ucWifiCmd = CHECK_PROCESS_CMD;
            DEBUG_LOG( "***CHECK_PROCESS_CMD\r\n");
            break;
        case WRITE_SNPIN_CMD:
            if(g_ucCloseWifiInterface == 2 || (g_ucKongKouFlow & 0x02) == 0 )
            {
                DEBUG_LOG( "***WRITE_SNPIN_CMD=%d,%02x\r\n", g_ucCloseWifiInterface, g_ucKongKouFlow );
                break;
            }
            g_ucWifiCmd = WRITE_SNPIN_CMD;
            uwTemp = 0;
            for(uwMs=1;uwMs<(pReqMsg_datalock->ucLen-2); uwMs++)
                uwTemp += pReqMsg_datalock->aucPayload[uwMs];

            uwMs = pReqMsg_datalock->aucPayload[pReqMsg_datalock->ucLen-1];
            uwMs <<= 8;
            uwMs |= pReqMsg_datalock->aucPayload[pReqMsg_datalock->ucLen-2];
            DEBUG_LOG( "***WRITE_SNPIN_CMD=%04x,%08x,%d\r\n", uwTemp, uwMs,pReqMsg_datalock->aucPayload[0]);
            if(uwTemp != uwMs)
                break;
            for(uwTemp=0;uwTemp<pReqMsg_datalock->ucLen;uwTemp++)
                DEBUG_PRINTF( "%02x",pReqMsg_datalock->aucPayload[uwTemp]);
            DEBUG_PRINTF( "\r\n");
            if(0 == TestWriteSNorPINId( &(pReqMsg_datalock->aucPayload[1]), pReqMsg_datalock->aucPayload[0] ))
            {
                g_ucWifiCmd |= (pReqMsg_datalock->aucPayload[0]<<6);
                DEBUG_LOG( "***TestWriteSNorPINId ok%d\r\n",g_ucWifiCmd);
            }
            else
            {
                DEBUG_LOG( "***TestWriteSNorPINId failed%d\r\n",g_ucWifiCmd);
            }

            break;
        case WRITE_DTLOCKSALT_CMD:
        {
            if(g_ucCloseWifiInterface == 2 || (g_ucKongKouFlow & 0x80) == 0 )
            {
                DEBUG_LOG( "***g_ucCloseWifiInterface=%d,%02x\r\n", g_ucCloseWifiInterface, g_ucKongKouFlow );
                break;
            }
            g_ucWifiCmd = WRITE_DTLOCKSALT_CMD;
            uwTemp = 0;
            for(uwMs=1;uwMs<(pReqMsg_datalock->ucLen-2); uwMs++)
                uwTemp += pReqMsg_datalock->aucPayload[uwMs];

            uwMs = pReqMsg_datalock->aucPayload[pReqMsg_datalock->ucLen-1];
            uwMs <<= 8;
            uwMs |= pReqMsg_datalock->aucPayload[pReqMsg_datalock->ucLen-2];
            DEBUG_LOG( "***WRITE_DTLOCKSALT_CMD=%04x,%08x,%d\r\n", uwTemp, uwMs,pReqMsg_datalock->ucLen);
            if(uwTemp != uwMs)
                break;
            for(uwTemp=0;uwTemp<pReqMsg_datalock->ucLen;uwTemp++)
                DEBUG_PRINTF( "%02x",pReqMsg_datalock->aucPayload[uwTemp]);
            DEBUG_PRINTF( "\r\n");


            if((pReqMsg_datalock->aucPayload[0]&0x03) == 0x03)
            {

                Opp_FactoryInfoForHW_UpdateDatalock(&(pReqMsg_datalock->aucPayload[1]));
                //if(pReqMsg_datalock->aucPayload[0]&0x02)
                Opp_FactoryInfoForHW_UpdateSalt(&(pReqMsg_datalock->aucPayload[65]));
                g_ucWifiCmd |= 0xC0;
                Opp_FactoryInfoForHW_WriteFlash();
            }
            else if(pReqMsg_datalock->aucPayload[0]&0x01)
            {
                Opp_FactoryInfoForHW_UpdateDatalock(&(pReqMsg_datalock->aucPayload[1]));
                g_ucWifiCmd |= 0x40;
                Opp_FactoryInfoForHW_WriteFlash();
            }
            else if(pReqMsg_datalock->aucPayload[0]&0x02)
            {
                Opp_FactoryInfoForHW_UpdateSalt(&(pReqMsg_datalock->aucPayload[1]));
                g_ucWifiCmd |= 0x80;
                Opp_FactoryInfoForHW_WriteFlash();
            }
            DEBUG_LOG( "***WRITE_DTLOCKSALT_CMD ok%d\r\n",g_ucWifiCmd);
            break;
        }
        case FLASH_LIGHT_CMD:
        {
            uint8_t u8Counts = 0;

            g_ucWifiCmd = FLASH_LIGHT_CMD;
            if(g_ucCloseWifiInterface == 2)
            {
                DEBUG_LOG( "***g_ucCloseWifiInterface=%d\r\n", g_ucCloseWifiInterface);
                g_u8FlashLightResultCode = 1;
                break;
            }
            u8Counts = pReqMsg->aucPayload[0];
            uwTemp = pReqMsg->aucPayload[2];
            uwTemp <<= 8;
            uwTemp |= pReqMsg->aucPayload[1];
            g_ucWifiCmd = FLASH_LIGHT_CMD;
            g_u8FlashLightResultCode = 0;
            DEBUG_LOG("Counts=%d, period=%d\r\n", u8Counts, uwTemp);
            INDC_LightBlinkCreate(u8Counts, uwTemp, uwTemp, NULL, NULL);
            break;
        }
    }
#endif
}

/******************************************************************************
 Function    : HandleP01V01SpcOnCmd
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void HandleP01V01SpcOnCmd(uint8_t *pPrtclData, uint8_t  *pSrcMac)
{
#if 0
    WIFI_L_P01V01_BODY  *pReqCmdBody = (WIFI_L_P01V01_BODY *)pPrtclData;
    SPECIAL_ONOFF_MSG_REQ *pReqMsg =(SPECIAL_ONOFF_MSG_REQ *) pReqCmdBody->cmdData;
    static uint16_t randomNum = 0xFFFF;
    uint8_t  i = 0, j = 0;
    uint8_t  ret = 0;
    uint8_t  isHandle = 0;
    uint8_t  pairItemId;


    if (randomNum ==  pReqCmdBody->randomNum) {
        return;
    }
#if 1
    OppLampCtrlSimuCallSceneMsg( 1 );//OppLampCtrlSimuSwitchOnOffMsg(1);
    g_ucWifiCmd = ONOFF_WIFI_CMD;

#else
    randomNum = pReqCmdBody->randomNum;
    OPP_Debug_Info(" spc on cmd \r\n");
    //OppLampCtrlSimuStepCCTMsg(1, pCCTMsg->cctStep, 2700, 6500);
    //check if my mac is in dstMac list
    for (i = 0; i < 6; i++) {
        ret = MACAddressType(&pReqMsg->dstMac[i][0]);
        if (ret == MAC_ADDR_SAME_SELF) {
            isHandle = 1;
            break;
        }
    }

    OPP_Debug_Warning("%s, pReqMsg->resever %lx, &0xFF %lx\r\n", __FUNCTION__, pReqMsg->resever, pReqMsg->resever&0xFF);
    if ((pReqMsg->resever&0xFF) == 0xEE)  // to disdinguish the spc on/off mechenism
        isHandle = 1;

    if (isHandle == 1) {
        for (i=0; i<6; i++) {
            for (j=0; j<g_PairPnlTable.validItemNum; j++) {
                OPP_Debug_Warning("item[%d] random %x, input random %x\n",
                                  j, g_PairPnlTable.items[j].pairPnlInfo.randomData, g_DevRandom[i]);
                if (g_PairPnlTable.items[j].pairPnlInfo.randomData == g_DevRandom[i]) {
                    OppProbeOnOffLight(OPP_SWTICH_ON);
                    goto out;
                }
            }
        }

    } else {
        OPP_Debug_Warning(" not target do nothing \r\n");
    }

out:
    // need to test befor open  broadcast hop
// ((WIFI_L_P01V01_HEAD*)(((WIFI_LOW_PRTCL_MAIN_HEAD*)g_pLowerRecvdata)->prtclBody))->isPairCheck = 0;
    ((WIFI_L_P01V01_HEAD *)(((WIFI_LOW_PRTCL_MAIN_HEAD *)g_pLowerRecvdata)->prtclBody))->isPairCheck = 0x01;
    PutOnOffBroadcastMsg(g_pLowerRecvdata, g_pLowerRecvLen);
#endif
#endif
}

/******************************************************************************
 Function    : HandleP01V01SpcOffCmd
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void HandleP01V01SpcOffCmd(uint8_t *pPrtclData, uint8_t  *pSrcMac)
{
#if 0
    WIFI_L_P01V01_BODY  *pReqCmdBody = (WIFI_L_P01V01_BODY *)pPrtclData;
    SPECIAL_ONOFF_MSG_REQ *pReqMsg =(SPECIAL_ONOFF_MSG_REQ *) pReqCmdBody->cmdData;
    static uint16_t randomNum = 0xFFFF;
    uint8_t  i = 0, j = 0;
    uint8_t  ret = 0;
    uint8_t  isHandle = 0;
    uint8_t  pairItemId;

    if (randomNum ==  pReqCmdBody->randomNum) {
        return;
    }
#if 1
    OppLampCtrlSimuSwitchOnOffMsg(0);
    g_ucWifiCmd = ONOFF_WIFI_CMD;

#else
    OPP_Debug_Info(" spc off cmd \r\n");
    randomNum = pReqCmdBody->randomNum;
    //OppLampCtrlSimuStepCCTMsg(1, pCCTMsg->cctStep, 2700, 6500);
    //check if my mac is in dstMac list
    for (i = 0; i < 6; i++) {
        ret = MACAddressType(&pReqMsg->dstMac[i][0]);
        if (ret == MAC_ADDR_SAME_SELF) {
            isHandle = 1;
            break;
        }
    }

    OPP_Debug_Warning("%s, pReqMsg->resever %lx\r\n", __FUNCTION__, pReqMsg->resever);
    if ((pReqMsg->resever&0xFF) == 0xEE)    // to disdinguish the spc on/off mechenism
        isHandle = 1;

    if (isHandle == 1) {
        for (i=0; i<6; i++) {
            for (j=0; j<g_PairPnlTable.validItemNum; j++) {
                //              OPP_Debug_Warning("item[%d] random %x, input random %x\n",
                //                  j, g_PairPnlTable.items[j].pairPnlInfo.randomData, g_DevRandom[i]);
                if (g_PairPnlTable.items[j].pairPnlInfo.randomData == g_DevRandom[i]) {
                    OppProbeOnOffLight(OPP_SWTICH_OFF);
                    goto out;
                }
            }
        }
        //return;
    } else {
        OPP_Debug_Warning(" not target do nothing \r\n");
    }

out:
    // need to test befor open  broadcast hop
    // ((WIFI_L_P01V01_HEAD*)(((WIFI_LOW_PRTCL_MAIN_HEAD*)g_pLowerRecvdata)->prtclBody))->isPairCheck = 0;
    ((WIFI_L_P01V01_HEAD *)(((WIFI_LOW_PRTCL_MAIN_HEAD *)g_pLowerRecvdata)->prtclBody))->isPairCheck = 0x01;
    PutOnOffBroadcastMsg(g_pLowerRecvdata, g_pLowerRecvLen);
#endif
#endif
}

/******************************************************************************
 Function    : HandleP01V01AppMsg
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t  HandleP01V01AppMsg( uint8_t *pData, uint32_t totalLen, uint8_t  *pPrtclData, uint8_t *pSrcMac )
{
#if 0
    WIFI_L_P01V01_BODY *pCmdBody = ( WIFI_L_P01V01_BODY * )pPrtclData;
    //--------------//
    //DEBUG_LOG("%s,type=%d\r\n",__FUNCTION__,pCmdBody->cmdId);
    /*
      if(LampInfoSave.IsMainLightFlag==ONLY_BACK_LIGHT)
    {
          DEBUG_LOG("type=%x\r\n",pCmdBody->cmdId);
       if( PRTCL_CMD_CCT_ADD_DEV == pCmdBody->cmdId ||PRTCL_CMD_CCT_SUB_DEV== pCmdBody->cmdId
           ||PRTCL_CMD_SPECIAL_CCT_ADD_DEV == pCmdBody->cmdId||PRTCL_CMD_SPECIAL_CCT_SUB_DEV ==pCmdBody->cmdId)
       {
           DEBUG_LOG("only back no cct adjust\r\n");
           return ;
       }

    }
    */
    DEBUG_LOG("%s,type=%04X\r\n",__FUNCTION__,pCmdBody->cmdId);
    g_uwRemoterCmdSave = pCmdBody->cmdId;

    switch ( pCmdBody->cmdId )
    {
            #if 0

        case PRTCL_CMD_MULTI_FUNCTION:
        {
            gDimCctDirect  = DIM_CCT_DIRECT_BRI;
            //g_ulOppMode  =0; //????OPP??????
            HandleMultiFuncCmdMsg( pPrtclData, pSrcMac );
            break;
        }

        case PRTCL_CMD_ON_DEV:
        {
            HandleP01V01OnCmdMsg( pPrtclData, pSrcMac );
            break;
        }

        case PRTCL_CMD_OFF_DEV:
        {
            HandleP01V01OffCmdMsg( pPrtclData, pSrcMac );
            break;
        }

        case PRTCL_CMD_DIM_ADD_DEV:
        {
            gDimCctDirect  = DIM_CCT_DIRECT_BRI;
            g_ulOppMode  = 0; //????OPP??????
            HandleP01V01DimAddCmd( pPrtclData, pSrcMac );
            break;
        }

        case PRTCL_CMD_DIM_SUB_DEV:
        {
            gDimCctDirect  = DIM_CCT_DIRECT_BRI;
            g_ulOppMode  = 0; //????OPP??????
            HandleP01V01DimSubCmd( pPrtclData, pSrcMac );
            break;
        }

        case PRTCL_CMD_CCT_ADD_DEV:
        {
            gDimCctDirect  = DIM_CCT_DIRECT_CCT;
            HandleP01V01CCTAddCmd( pPrtclData, pSrcMac );
            break;
        }

        case PRTCL_CMD_CCT_SUB_DEV:
        {
            gDimCctDirect  = DIM_CCT_DIRECT_CCT;
            g_ulOppMode  = 0; //????OPP??????
            HandleP01V01CCTSubCmd( pPrtclData, pSrcMac );
            break;
        }

        case PRTCL_CMD_RESETFACTORYMODE:
        {
            HandleP01V01ResetCmdMsg( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_SPECIAL_ON_CMD :
        {
            HandleP01V01SpcOnCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_SPECIAL_OFF_CMD :
        {
            HandleP01V01SpcOffCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_SPECIAL_DIM_ADD_DEV:
        {
            gDimCctDirect  = DIM_CCT_DIRECT_BRI;
            g_ulOppMode  = 0; //????OPP??????
            HandleP01V01SpcDimAddCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_SPECIAL_DIM_SUB_DEV:
        {
            gDimCctDirect  = DIM_CCT_DIRECT_BRI;
            g_ulOppMode  = 0; //????OPP??????
            HandleP01V01SpcDimSubCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_SPECIAL_CCT_ADD_DEV:
        {
            gDimCctDirect  = DIM_CCT_DIRECT_CCT;
            g_ulOppMode  = 0; //????OPP??????
            HandleP01V01SpcCCTAddCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_SPECIAL_CCT_SUB_DEV:
        {
            gDimCctDirect  = DIM_CCT_DIRECT_CCT;
            g_ulOppMode  = 0; //????OPP??????
            HandleP01V01SpcCCTSubCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_SET_BRI_DEV:
        {
            HandleP01V01SetBriCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_SET_CCT_DEV:
        {
            HandleP01V01SetCCTCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_GET_LAMP_STATUS_DEV:
        {
            HandleP01V01GetLampStatusCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_SET_SCN_DEV:
        {
            HandleP01V01SetSCNCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_SET_BREATH_DEV:
        {
            HandleP01V01SetBreathCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_TOGGLE_DEV:
        {
            HandleP01V01ToggleCmd( pPrtclData, pSrcMac );
            break;
        }

        #endif
        case  PRTCL_CMD_MULTI_FUNCTION :
        {
            g_ucWifiCmd = 0;
            DEBUG_LOG("PRTCL_CMD_MULTI_FUNCTION=%d\r\n",g_ucCloseWifiInterface);
            if(g_ucCloseWifiInterface == 2)
            {
                g_ucWifiCmd = CLOSE_WIFI_CMD;
                break;
            }
            if(g_ucEnableBindAction == 1)
                HandleP01V01SpcOnCmd( pPrtclData, pSrcMac );
            break;
        }

        case  PRTCL_CMD_OFF_DEV :
        {
            g_ucWifiCmd = 0;
            DEBUG_LOG("PRTCL_CMD_OFF_DEV=%d\r\n",g_ucCloseWifiInterface);
            if(g_ucCloseWifiInterface == 2)
            {
                g_ucWifiCmd = CLOSE_WIFI_CMD;
                break;
            }
            if(g_ucEnableBindAction == 1)
                HandleP01V01SpcOffCmd( pPrtclData, pSrcMac );
            break;
        }
        case  PRTCL_CMD_SET_NETINFO_PINSN:
        {
            HandleP01V01SetNetInfoPINSNCmd( pPrtclData, pSrcMac );
            break;
        }

        default:
        {
            DEBUG_LOG( " --- unkown cmdi= %d  ----\r\n", pCmdBody->cmdId );
            break;
        }
    }
#endif
    return OPP_SUCCESS;
}

/******************************************************************************
 Function    : HandleP01V01Msg
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t  HandleP01V01Msg( uint8_t *pData, uint32_t totalLen, uint8_t  *pPrtclData, uint8_t *pSrcMac )
{
#if 0
    uint8_t  ret, i;
    uint8_t *pAppMac;
    WIFI_L_P01V01_HEAD    *pP01V01Head = ( WIFI_L_P01V01_HEAD * )pPrtclData;

    //TODO check password ,decode
    if( PWD_CHECK_MASK & pP01V01Head->pwdCrcValid )
    {
        //TODO check CRC
        DEBUG_LOG( "** need to decode data  **\r\n" );
    }

    //Check data len
    if( totalLen  != pP01V01Head->dataLen )
    {
        DEBUG_LOG( "*dataLen error total= %d  lenInMsg=%d*\r\n", totalLen, pP01V01Head->dataLen );
        return OPP_FAILURE;
    }

    if( CRC_CHECK_MASK & pP01V01Head->pwdCrcValid )
    {
        //TODO check CRC
        DEBUG_LOG( "** need to check crc **\r\n" );
    }

    pSrcMac[7] = pP01V01Head->grupNum;
    //DEBUG_LOG( "keyid %d,%d\r\n", pSrcMac[7], pP01V01Head->isPairCheck );

    //check pair or not
    if( CHECK_PAIR_YES == pP01V01Head->isPairCheck )
    {
        #if 1
        return OPP_FAILURE;
        #else
        //broadcast no check
        ret = IsDevCmdPermissionByMac( pSrcMac );

        //      ret = IsDevCmdPermissionByMac(pSrcMac);

        if( ret != OPP_SUCCESS )
        {
            OPP_Debug_Warning( " --- dev not pair ,mac not pair %x %x %x %x %x %x  ---\r\n", pSrcMac[0], pSrcMac[1], pSrcMac[2],
                               pSrcMac[3], pSrcMac[4], pSrcMac[5] );
            return OPP_FAILURE;
        }

        //check whether need to pass msg to app
        if( ( pP01V01Head->dMac[0] != 0xFF ) || ( pP01V01Head->dMac[1] != 0xFF ) || ( pP01V01Head->dMac[2] != 0xFF ) ||
            ( pP01V01Head->dMac[3] != 0xFF ) || ( pP01V01Head->dMac[4] != 0xFF ) || ( pP01V01Head->dMac[5] != 0xFF ) )
        {
            for( i = 0; i < 6; i++ )
            {
                if( pP01V01Head->dMac[i] != g_lMac[i] )
                {
                    OPP_Debug_Warning( " --self is  not target mac ---\r\n" );
                    OPP_Debug_Info( "\r\n t = %x-%x-%x-%x-%x-%x ", pP01V01Head->dMac[0], pP01V01Head->dMac[1], pP01V01Head->dMac[2],
                                    pP01V01Head->dMac[3], pP01V01Head->dMac[4], pP01V01Head->dMac[5] );
                    OPP_Debug_Info( "\r\n l = %x-%x-%x-%x-%x-%x ", g_lMac[0], g_lMac[1], g_lMac[2], g_lMac[3], g_lMac[4], g_lMac[5] );
                    return OPP_FAILURE;
                }
            }
        }
        else
        {
            //TODO check broadcast multiBroadcast
            OPP_Debug_Info( " - b mac -\r\n" );
        }

        #endif
    }

    HandleP01V01AppMsg( pData, totalLen, pP01V01Head->msgBody, pSrcMac );
#endif
    return OPP_SUCCESS;
}

/******************************************************************************
 Function    : wifi_set_custom_ie
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int wifi_set_custom_ie( uint8_t *oui, uint8_t subtype, void *data, uint16_t length )
{
    int i = 0;

    if ( length > 255 )
    {
        return -1;
    }

    if ( custom_ie != NULL )
    {
        free( custom_ie );
        custom_ie = NULL;
        custom_ie_len = 0;
    }

    custom_ie_len = 3 + length; // oui(3) + subtype(1) + data(length)
    custom_ie = ( uint8_t * ) malloc( custom_ie_len + 2 );

    if ( custom_ie == NULL )
    {
        custom_ie_len = 0;
        return -1;
    }

    custom_ie[i++] = 0xdd;
    custom_ie[i++] = custom_ie_len;
    memcpy( &custom_ie[i], oui, 3 );
    i += 3;
    //  custom_ie[i++] = subtype;
    memcpy( &custom_ie[i], data, length );
    return 0;
}

/******************************************************************************
 Function    : wifi_set_ie
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t* wifi_set_ie(
                uint8_t *pbuf,
                int index,
                uint32_t len,
                uint8_t *source,
                /*uint*/uint16_t *frlen)
{
    *pbuf = ( uint8_t )index;
    *( pbuf + 1 ) = ( uint8_t )len;

    if ( len > 0 )
    {
        memcpy( ( void * )( pbuf + 2 ), ( void * )source, len );
    }

    *frlen = *frlen + ( len + 2 );
    return ( pbuf + len + 2 );
}

/******************************************************************************
 Function    : Opp_CheckPaired
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t Opp_CheckPaired( uint8_t *mac_addr )
{
    return (uint8_t)(g_IsLampInPairState == OPP_YES);
}

/******************************************************************************
 Function    : Opp_FillUpLampStatus
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void Opp_FillUpLampStatus( uint8_t paired )
{
#if 0
    int i;
    uint16_t checksum = 0;
    uint8_t snLen;

    DEBUG_LOG( "FillUp\r\n" );
    PROBE_LIGHT_MSG_SEND light_msg_send;
    light_msg_send.msg_head.PACK_HEAD = 0xbebebebe;
    light_msg_send.msg_head.RESERVED[0] = g_IsLampInPairState ? LWPOWER_MSG_PAIRST_YES : LWPOWER_MSG_PAIRST_NO;
    light_msg_send.msg_head.RESERVED[1] = 0xFF;

    //light_msg_send.msg_head.RESERVED[2] = PRODUCTCLASS_OVERSEA;
    //light_msg_send.msg_head.RESERVED[3] = PRODUCTSKU_OVERSEA;
    //light_msg_send.msg_head.RESERVED[4] = g_Opp_Wifi_CurState;

    light_msg_send.msg_head.RESERVED[2] = 0;
    light_msg_send.msg_head.RESERVED[3] = 0;
    light_msg_send.msg_head.RESERVED[4] = 0;

	light_msg_send.msg_head.RESERVED[5] = 0;
	light_msg_send.msg_head.RESERVED[6] = 0;

    light_msg_send.msg_head.PROTOCOL_ID = 0x01;
	light_msg_send.msg_head.PROTOCOL_VERSION = 0x01;

    //light_msg_send.probe_light_data_send.LampVer = OPP_LAMP_CTRL_CFG_DATA_VER;

    light_msg_send.probe_light_data_send.LampVer = 0;

    // FIXME: or LAMP_SUPPORT_ONLY_CW??
    light_msg_send.probe_light_data_send.LampCap = ( uint8_t )( paired << 5 );
    #if 1
        //light_msg_send.probe_light_data_send.CurrentScn  = ( g_stThisLampInfo.stCurrStatus.ucSwitch << 7 ) | 0x01;
        light_msg_send.probe_light_data_send.CurrentScn  = ( 0 << 7 ) | 0x01;
        light_msg_send.probe_light_data_send.Bri = 102;
        light_msg_send.probe_light_data_send.CCT = 4000;
    #else
    if ( INVALID_SCN_NUM == gCallSenceNum )
    {
        light_msg_send.probe_light_data_send.CurrentScn  = ( g_stThisLampInfo.stCurrStatus.ucSwitch << 7 ) | 0x7F;
    }
    else if ( ( gCallSenceNum >= 0 ) && ( INVALID_SCN_NUM != gCallSenceNum ) )
    {
        light_msg_send.probe_light_data_send.CurrentScn  = ( g_stThisLampInfo.stCurrStatus.ucSwitch << 7 ) | gCallSenceNum;
    }

    //light_msg_send.probe_light_data_send.CurrentScn = (g_stThisLampInfo.stCurrStatus.ucSwitch<<7)|(gCallSenceNum&0x7F);
    light_msg_send.probe_light_data_send.Bri = g_stThisLampInfo.stCurrStatus.ucBri;
    light_msg_send.probe_light_data_send.CCT = g_stThisLampInfo.stCurrStatus.uwCCT;
    light_msg_send.probe_light_data_send.Red = g_stThisLampInfo.stCurrStatus.aucHue[0];
    light_msg_send.probe_light_data_send.Green = g_stThisLampInfo.stCurrStatus.aucHue[1];
    light_msg_send.probe_light_data_send.Blue = g_stThisLampInfo.stCurrStatus.aucHue[2];
    #endif
    if(PRTCL_CMD_SET_NETINFO_PINSN == g_uwRemoterCmdSave)
    {
        light_msg_send.probe_sub_cmd.Cmd = g_uwRemoterCmdSave;
        switch (g_ucWifiCmd&0x3f) {
            case  READ_PIN_SN_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = 0x06;
                light_msg_send.probe_sub_cmd.Len = 27;
                if(g_ucSNPINExistFlag == -1)
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 0;
                else
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 3;
                memset(g_aucSNPIN, 0, 24);
                oppDataReadSN(g_aucSNPIN, 16, &snLen);
                for(i=0;i<24;i++)
                    light_msg_send.probe_sub_cmd.RESERVE[i+1] = g_aucSNPIN[i];

                for(i=0;i<25;i++)
                    checksum += light_msg_send.probe_sub_cmd.RESERVE[i];
                light_msg_send.probe_sub_cmd.RESERVE[25] = (uint8_t)(checksum&0x00ff);
                light_msg_send.probe_sub_cmd.RESERVE[26] = (uint8_t)(checksum>>8);
                for(i=0;i<27;i++)
                    DEBUG_LOG( "%02X", light_msg_send.probe_sub_cmd.RESERVE[i] );
                DEBUG_LOG( "\r\n");
                break;
        }
#if 0
        switch (g_ucWifiCmd&0x3f)
        {
            case  SEARCH_CLASS_SKU_DEVICE:
                light_msg_send.probe_sub_cmd.SubCmd = 0x02;
                light_msg_send.probe_sub_cmd.Len = 0x01;
                if(g_ucTimeOver == 1)
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 0;
                else if(paired == 1)
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 2;
                else
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 1;
                break;
            case  BIND_CLASS_SKU_DEVICE:
                light_msg_send.probe_sub_cmd.SubCmd = 0x04;
                light_msg_send.probe_sub_cmd.Len = 0x01;
                if(g_ucWifiCmd&0x80)
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 2; //2：已绑定
                else if(paired == 1)
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 1; //绑定成功
                else
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 0;
                break;
            case  READ_PIN_SN_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = 0x06;
                light_msg_send.probe_sub_cmd.Len = 27;
                if(g_ucSNPINExistFlag == -1)
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 0;
                else
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 3;
                for(i=0;i<24;i++)
                    light_msg_send.probe_sub_cmd.RESERVE[i+1] = g_aucSNPIN[i];

                for(i=0;i<25;i++)
                    checksum += light_msg_send.probe_sub_cmd.RESERVE[i];
                light_msg_send.probe_sub_cmd.RESERVE[25] = (uint8_t)(checksum&0x00ff);
                light_msg_send.probe_sub_cmd.RESERVE[26] = (uint8_t)(checksum>>8);
                for(i=0;i<27;i++)
                    DEBUG_PRINTF( "%02X", light_msg_send.probe_sub_cmd.RESERVE[i] );
                DEBUG_PRINTF( "\r\n");
                break;
            case  CLOSE_WIFI_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = 0x08;
                light_msg_send.probe_sub_cmd.Len = 1;
                if(g_ucCloseWifiInterface == 2)
                {
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 3; //上电前已关闭
                    break;
                }
                TestReadSNPINFlagId( g_aucSNPIN, &g_ucCloseWifiInterface );
                if(g_ucCloseWifiInterface == 1)
                {
                    if(g_ucWifiCmd & 0x80)
                        light_msg_send.probe_sub_cmd.RESERVE[0] = 2; //已关闭，不需重复关闭
                    else
                        light_msg_send.probe_sub_cmd.RESERVE[0] = 1; //关闭空口功能成功
                }
                else
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 0;
                break;
            case OPEN_WIFI_RANDOM_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = 0x0A;
                light_msg_send.probe_sub_cmd.Len = 4;
                if(g_ucCloseWifiInterface == 0)                    //已开空口
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 2;
                else if(g_ucCloseWifiInterface == 1)               //不允许开空口
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 1;
                else
                    light_msg_send.probe_sub_cmd.RESERVE[0] = 0;   //允许开空口
                light_msg_send.probe_sub_cmd.RESERVE[1] = (uint8_t)(g_u16RandomData&0x00ff);
                light_msg_send.probe_sub_cmd.RESERVE[2] = (uint8_t)(g_u16RandomData>>8);
                light_msg_send.probe_sub_cmd.RESERVE[3] = 0;
                break;
            case SEND_DTLOCK_RANDOM_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = 0x0E;
                light_msg_send.probe_sub_cmd.Len = 1;
                light_msg_send.probe_sub_cmd.RESERVE[0] = 0;
                if(g_ucWifiCmd & 0x80)                             //random校验结果
                    light_msg_send.probe_sub_cmd.RESERVE[0] |= 0x02;
                if(g_ucWifiCmd & 0x40)                             // datalock校验结果
                    light_msg_send.probe_sub_cmd.RESERVE[0] |= 0x01;
                break;
            case SET_SSID_KEY_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = 0x12;
                light_msg_send.probe_sub_cmd.Len = 1;
                light_msg_send.probe_sub_cmd.RESERVE[0] = 0;
                break;
            case SET_IP_PORT_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = 0x14;
                light_msg_send.probe_sub_cmd.Len = 1;
                light_msg_send.probe_sub_cmd.RESERVE[0] = 0;
                break;
            case START_OTA_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = 0x16;
                light_msg_send.probe_sub_cmd.Len = 3;
                light_msg_send.probe_sub_cmd.RESERVE[0] = g_u8OTAProcess;
                light_msg_send.probe_sub_cmd.RESERVE[1] = OPP_LAMP_CTRL_CFG_DATA_VER;
                light_msg_send.probe_sub_cmd.RESERVE[2] = (OPP_LAMP_CTRL_CFG_DATA_VER>>8);
                break;
            case CHECK_PROCESS_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = 0x18;
                light_msg_send.probe_sub_cmd.Len = 1;
                light_msg_send.probe_sub_cmd.RESERVE[0] = g_u8OTAProcess;
                //light_msg_send.probe_sub_cmd.RESERVE[1] = OPP_LAMP_CTRL_CFG_DATA_VER;
                //light_msg_send.probe_sub_cmd.RESERVE[2] = (OPP_LAMP_CTRL_CFG_DATA_VER>>8);
                break;
            case WRITE_SNPIN_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = 0x1A;
                light_msg_send.probe_sub_cmd.Len = 1;
                light_msg_send.probe_sub_cmd.RESERVE[0] = 0;
                if(g_ucWifiCmd & 0x80)                             //写入PIN成功
                    light_msg_send.probe_sub_cmd.RESERVE[0] |= 0x02;
                if(g_ucWifiCmd & 0x40)                             // 写入SN成功
                    light_msg_send.probe_sub_cmd.RESERVE[0] |= 0x01;
                break;
            case WRITE_DTLOCKSALT_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = 0x1C;
                light_msg_send.probe_sub_cmd.Len = 1;
                light_msg_send.probe_sub_cmd.RESERVE[0] = 0;
                if(g_ucWifiCmd & 0x80)                             //写入盐值成功
                    light_msg_send.probe_sub_cmd.RESERVE[0] |= 0x02;
                if(g_ucWifiCmd & 0x40)                             // 写入datalock成功
                    light_msg_send.probe_sub_cmd.RESERVE[0] |= 0x01;
                break;
            case FLASH_LIGHT_CMD:
                light_msg_send.probe_sub_cmd.SubCmd = FLASH_LIGHT_CMD+1;
                light_msg_send.probe_sub_cmd.Len = 1;
                light_msg_send.probe_sub_cmd.RESERVE[0] = g_u8FlashLightResultCode;
                break;
        }
#endif

    }
    else
        light_msg_send.probe_sub_cmd.Cmd = g_uwRemoterCmdSave;
    //light_msg_send.probe_sub_cmd.SubCmd = g_uwRemoterCmdSave;


    light_msg_send.msg_tail.PACK_END = LWPOWER_MSG_TAIL;
    memcpy( g_probeRspMsgBody, ( uint8_t * )&light_msg_send, sizeof( light_msg_send ) );
    // printdata(g_probeRspMsgBody, sizeof(light_msg_send));
#endif
}

/******************************************************************************
 Function    : slProbeSendRequest
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slProbeSendRequest(const char* ifname, char* ssid, int ssid_len, uint8_t *pData, int data_len) 
{
    int ret = 0;
    int package_len = 0;
    pstProbeReqFrame = (stProbeReqFrame_t *)a_ucProbeBuf;
    uint8_t mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    static uint16_t sequence = 0;

    pstProbeReqFrame->header.frame_control = WLAN_FC_STYPE_PROBE_REQ;
    pstProbeReqFrame->header.duration = 0;
    memcpy(pstProbeReqFrame->header.addr1, mac, 6); //dest mac broadcast
    myHalWifiGetMacAddrByte(pstProbeReqFrame->header.addr2); //source mac
    memcpy(pstProbeReqFrame->header.addr3, mac, 6); // bssid broadcast
    pstProbeReqFrame->header.seq = sequence++;
    APP_SWAP_S(pstProbeReqFrame->header.seq);
    pstProbeReqFrame->ssid_id = 0;
    pstProbeReqFrame->ssid_len = ssid_len;
    package_len = sizeof(stProbeReqFrame_t);

    memcpy(&a_ucProbeBuf[package_len], ssid, ssid_len);
    package_len += ssid_len;

    memcpy(&a_ucProbeBuf[package_len], pData, data_len);
    package_len += data_len;

    my_hal_log_info("%u Probe Request Data(length %d):\r\n", xTaskGetTickCount(), package_len);
    for( int i = 0; i < package_len; i++ ) {
        my_hal_log_info("%02X ", a_ucProbeBuf[i] );
    }
    my_hal_log_info("\r\n");

    ret = wext_send_mgnt(ifname, (char *)a_ucProbeBuf, package_len, 0);
    my_hal_log_info("%s wext_send_mgnt ret = %d\r\n", ifname, ret);
}

/******************************************************************************
 Function    : slProbeSendRsponse
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slProbeSendRsponse(uint8_t dst_mac[6]) 
{

}

/******************************************************************************
 Function    : Opp_SendProbeRsp
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void Opp_SendProbeRsp( uint8_t *dst_mac )
{
#if 0
    int i;
    int ret = 0;
    uint8_t *pframe;
    //uint8_t *pframeRaw;
    ieee80211_header_t *hdr;
    uint8_t  my_addr[6] = {0};
    uint8_t  supported_rates[] = {0x82, 0x84, 0x8B, 0x96, 0x24, 0x30, 0x48, 0x6C};
    uint8_t *ssid_str = "OPPLE";
    uint8_t hdr_len = 24;
    uint16_t frame_len = hdr_len;
    uint16_t beacon_inteval = 65500; //as long as possible for our applications
    uint16_t cap_info = 0x0401;  // indicate this ap's capibility, don't care it
    uint8_t oui[] = {0xc8, 0x93, 0x46};
    uint8_t paired = 0;
    //pframe = malloc(frame_len + 255);
    pframe = g_RspData;

    if ( pframe == NULL )
    {
        return;
    }

    //pframeRaw = pframe;
    hilink_wifi_get_macaddr( my_addr );
    //memcpy(my_addr, g_lMac, 6);
    memset( pframe, 0, frame_len );
    hdr = ( ieee80211_header_t * ) pframe;
    hdr->duration = 60;
    hdr->frame_control = WLAN_FC_STYPE_PROBE_RSP;
    memcpy( hdr->addr1, dst_mac, 6 );
    memcpy( hdr->addr2, my_addr, 6 );
    memcpy( hdr->addr3, my_addr, 6 );
    pframe += hdr_len; // move pframe pointer to frame_start+wifi_hrdlen
    //timestamp will be inserted by hardware
    pframe += 8;
    frame_len += 8;
    // beacon interval: 2 bytes
    memcpy( pframe, &beacon_inteval, 2 );
    pframe += 2;
    frame_len += 2;
    // capability info: 2 bytes
    memcpy( pframe, &cap_info, 2 );
    pframe += 2;
    frame_len += 2;
    // ssid
    pframe = wifi_set_ie( pframe, _SSID_IE_, 5, ssid_str, &frame_len );
    // supported rates...
    pframe = wifi_set_ie( pframe, _SUPPORTEDRATES_IE_, 8, supported_rates,
                          &frame_len );
    paired = Opp_CheckPaired( dst_mac );
    //OPP_Debug_Data("paired %d\n", paired);
    Opp_FillUpLampStatus( paired );
    wifi_set_custom_ie( oui, 1, g_probeRspMsgBody, sizeof( g_probeRspMsgBody ) );
    memcpy( pframe, custom_ie, custom_ie_len + 2 );
    DEBUG_LOG( "frame_len =%d \r\n", frame_len );
    frame_len += custom_ie_len + 2;

    if( frame_len > 0 )
        for( i = 0; i < frame_len; i++ )
        {
            DEBUG_LOG( "%02X", g_RspData[i] );
        }

    ret = wext_send_mgnt( WLAN0_NAME, ( char * )g_RspData, frame_len,
                          0 ); //ret = dna_wlan_send_raw_packet((char *)hdr, frame_len);
    g_IsEverSndRsp = 1;
    DEBUG_LOG( "************** send  rsp = %d  ****len=%d***********\r\n", ret, frame_len );
    //printdata(hdr,frame_len);
    //free(pframeRaw);
#endif
}

/******************************************************************************
 Function    : wlu_parse_tlvs
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t *wlu_parse_tlvs( /*@returned@*/ uint8_t *tlv_buf, uint32_t buflen, uint32_t key )
{
    uint8_t *cp     = tlv_buf;
    int  totlen = ( int ) buflen;

    /* find tagged parameter */
    while ( totlen >= ( int ) 2 )
    {
        uint32_t tag;
        int len;
        tag = *cp;
        len = ( int ) * ( cp + 1 );

        /* validate remaining totlen */
        if ( ( tag == key ) && ( totlen >= ( len + 2 ) ) )
        {
            return ( cp );
        }

        cp += ( len + 2 );
        totlen -= ( len + 2 );
    }

    return NULL;
}

/******************************************************************************
 Function    : wlu_is_mxchip_ie
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int wlu_is_mxchip_ie( uint8_t **wpaie, uint8_t **tlvs, uint32_t *tlvs_len )
{
    uint8_t *ie = *wpaie;

    /* If the contents match the ALI_OUI */
    if ( ( ie[1] >= ( uint8_t ) 6 ) &&
         ( memcmp( &ie[2], ALI_OUI, ( size_t ) 3 ) == 0 ) )
    {
        return 1;//true;
    }

    /* point to the next ie */
    ie += ie[1] + 2;
    /* calculate the length of the rest of the buffer */
    *tlvs_len -= ( uint32_t ) ( ie - *tlvs );
    /* update the pointer to the start of the buffer */
    *tlvs = ie;
    return 0;//false;
}

/******************************************************************************
 Function    : probe_request_rx_cb
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int probe_request_rx_cb( uint8_t *header, uint16_t length, uint16_t frame_type )
{
    int i=0;
    //int i, j, k;
    //uint8_t *p = header + 24; // go to probe request TLV
    uint8_t *parse, *ie, *mxchip_ie;
    uint32_t parse_len;
    uint8_t *src_addr = header + 10;
    uint8_t src_info[7] = {0};
    //DEBUG_LOG( "--------probe_request_rx_cb:%d,%d,%d,%d \r\n", length, frame_type, sizeof( dna_wlan_mac_hdr_t ), LOS_Tick2MS(( UINT32 )LOS_TickCountGet()) );
    header -= sizeof( dna_wlan_mac_hdr_t );
    src_addr = header + 10;
    length += sizeof( dna_wlan_mac_hdr_t );
    //if(1 == g_ucCloseWifiInterface)
    //{
    //    DEBUG_LOG( "g_ucCloseWifiInterface = 1, return \r\n");
    //    return 0;
    //}

#if 1
    dna_wlan_mac_hdr_t *pHeader = (dna_wlan_mac_hdr_t *)header;

    if (pHeader->addr2[4] == 0x12 && pHeader->addr2[5] == 0xCE) {
        my_hal_log_info("%u prob rx cb type %d\r\n", xTaskGetTickCount(),  frame_type);
        if( length > 0 ) {
            for( i = 0; i < length; i++ ) {
                my_hal_log_info( "%02X ", *( header + i ) );
            }
        }
        my_hal_log_info("\r\n\r\n");
    }
    
#endif

#if 0
    parse = header + 24;
    parse_len = length - 24;

    ////wifi_send_mgnt(0,(char*)parse, parse_len);

    memcpy( src_info, src_addr, 6 );

    while ( ( ie = wlu_parse_tlvs( parse, parse_len, ( uint32_t ) DOT11_MNG_VS_ID ) ) != 0 )
    {
#if 0
        my_hal_log_info("prob rx cb type %d\r\n", frame_type);
        if( length > 0 ) {
            for( i = 0; i < length; i++ ) {
                my_hal_log_info( "%02X ", *( header + i ) );
            }
        }
        my_hal_log_info("\r\n\r\n");
#endif
        if ( wlu_is_mxchip_ie( &ie, &parse, &parse_len )  )
        {
            mxchip_ie = ie;
            parse_len = mxchip_ie[1];
            mxchip_ie[parse_len + 2] = 0;
            //DEBUG_LOG( "-rcv=%d -\r\n", length );
            RecvDataFromProble( &mxchip_ie[6], mxchip_ie[1] - 4, src_info );
#if 0
            if(g_ucWifiCmd == CLOSE_WIFI_CMD)
            {
                if(0 != TestWriteSNPINFlagId(1))
                    TestWriteSNPINFlagId(1);
            }
#endif
            if(g_ucWifiCmd >= CLOSE_WIFI_CMD)
                memcpy( g_aucDstMac, src_info, 6 );
            if(g_ucWifiCmd != 0)
                Opp_SendProbeRsp( src_info );
            g_ucWifiCmd = 0;
            //RecvDataFromProble(&mxchip_ie[6], mxchip_ie[1]-4, src_info);
            break;
        }
    }

#endif

    return 0;
}

/******************************************************************************
 Function    : startLowerPowerAp
 Description : (none)
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void startLowerPowerAp()
{
    extern int ( *p_wlan_mgmt_filter )( uint8_t * ie, uint16_t ie_len, uint16_t frame_type );
    uint32_t i;
    uint8_t snLen;
    //uint8_t buf[32] = {0,0,0,0,0,0,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    g_ucCloseWifiInterface = 0;

//  for(i = 0; i < 24; i++){
//      g_aucSNPIN[i] = 0;
//  }
//  oppDataReadSN(g_aucSNPIN, 16, &snLen);

    p_wlan_mgmt_filter = probe_request_rx_cb;
}

#if 0
void Opp_SendOTAResultStatus(void)
{
    g_ucWifiCmd = START_OTA_CMD;
    g_uwRemoterCmdSave = 0x7200;
    memcpy( g_aucPairedMacAddr, g_aucDstMac, 6 );
    DEBUG_LOG("\r\n");
    Opp_SendProbeRsp( g_aucDstMac );
    g_ucWifiCmd = 0;
}


void Opp_SendOTAProcessStatus(void)
{
    g_ucWifiCmd = CHECK_PROCESS_CMD;
    DEBUG_LOG("\r\n");
    Opp_SendProbeRsp( g_aucDstMac );
    g_ucWifiCmd = 0;
}

void RecvDataFromProble( uint8_t *pData, uint32_t dataLen, uint8_t *pSrcMac )
{
    //todo change to new process
    //int i = 0;
    uint8_t ret ;
    WIFI_LOW_PRTCL_MAIN_HEAD    *pPrtclMainHead = ( WIFI_LOW_PRTCL_MAIN_HEAD * )pData;
    //WIFI_LOW_PRTCL_MAIN_TAIL      *pPrtclMainTail =  ( WIFI_LOW_PRTCL_MAIN_TAIL * )( pData + dataLen - sizeof(
    //                                                                                                 WIFI_LOW_PRTCL_MAIN_TAIL ) );
    g_pLowerRecvdata = pData;
    g_pLowerRecvLen =   dataLen;
    ret = IsPrtclMainMsgValid( pData, dataLen, pSrcMac );
#if 0
    for ( i = 0; i < 6; i++ )
    {
        g_DevRandom[i] = Build_uint16( pPrtclMainHead->devState[2 * i + 1], pPrtclMainHead->devState[2 * i] );
    }
#endif

    if( ret != OPP_SUCCESS )
    {
        DEBUG_LOG( "\r\n --- MSG INVALID ---- \r\n" );
        return;
    }

    //dispatch msg
    if( ( pPrtclMainHead->prtclId >= PRTCL01ID ) )
    {
        if( pPrtclMainHead->prtclVersion >= PRTCL01V01 )
        {
            HandleP01V01Msg( pData, dataLen, pPrtclMainHead->prtclBody, pSrcMac );
            return ;
        }
    }

    DEBUG_LOG( "--- no hand prtclId=%d  ver=%d ---- \r\n", pPrtclMainHead->prtclId, pPrtclMainHead->prtclVersion );
    DEBUG_LOG( "--- END ---- \r\n" );
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */




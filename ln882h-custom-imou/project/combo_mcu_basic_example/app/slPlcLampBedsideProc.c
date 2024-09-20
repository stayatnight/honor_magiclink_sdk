/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slPlclLampBedsideProc.c                                       *
* Description :                                                               *
*               smart lamp plc lamp bedside process source file               *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-12-01                                                    *
* Mod History : (none)                                                        *
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "slPlcLampBedsideProc.h"
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)
#include "slData.h"
#include "slTaskPower.h"
#include "magiclink.h"
#include "sys.h"
#include "timers.h"
#include "atShell.h"
//#include "myRingBuffer.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
//#define SL_LAMP_SERIAL_UART                                MY_HAL_UART_0

#define LIGHT_BRIGHT_MAX                                   (65535)
#define LIGHT_BRIGHT_MIN                                   (uint16_t)(LIGHT_BRIGHT_MAX * 1.0f / 100.0f + 0.5f)
#define LIGHT_COLOR_CCT_MIN                                (3000)
#define LIGHT_COLOR_CCT_MAX                                (5700)

#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE) //bedside lamp
#undef LIGHT_COLOR_CCT_MIN
#define LIGHT_COLOR_CCT_MIN                                (2700)
#undef LIGHT_COLOR_CCT_MAX
#define LIGHT_COLOR_CCT_MAX                                (6500)
#define LIGHT_CCT_DEFAULT                                  (LIGHT_COLOR_CCT_MID)
#endif

#define LIGHT_COLOR_CCT_MID                                (LIGHT_COLOR_CCT_MIN+(LIGHT_COLOR_CCT_MAX-LIGHT_COLOR_CCT_MIN)/2)

/* bright: value <---> percent */
#define LIGHT_BRIGHT_TO_PERCENT(b)                         (uint32_t)((b) * 100.0f / 65535 + 0.5f)
#define LIGHT_PERCENT_TO_BRIGHT(p)                         (uint16_t)((p) * 65535.0f / 100 + 0.5f)

#define RL_LAMP_REPORT_INTERVAL                            (800) //ms

#define SL_LAMP_SERIAL_RING_BUFFER_SIZE                    (128)

#define SL_LAMP_SERIAL_SEND_REPEAT                         (3)
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
typedef struct
{
    uint32_t interval;
    uint32_t recvTick;
    uint8_t ucResetStep;
} slSerialInfo_t;
/******************************************************************************
*                             Declarations                                    *
******************************************************************************/

/******************************************************************************
*                                Globals                                      *
******************************************************************************/
//static uint8_t s_lampSerialRingBuffer_buf[SL_LAMP_SERIAL_RING_BUFFER_SIZE];
static uint8_t s_LampSerialRecieveBuf[32];
//static uint8_t s_LampSerialSendBuf[32];
static myLampParam_t s_stCurLampParam = {0};
static myLampParam_t s_stLstLampParam = {0};
static xTimerHandle s_resetWindowTimerHandle;
//static myRingBuffer_t s_lampSerialRingBuffer;
static slSerialInfo_t s_slSerialInfo;
/******************************************************************************
*                          Extern Declarations                                *
******************************************************************************/

/******************************************************************************
*                             Declarations                                    *
******************************************************************************/
static void _lampStateControlHook();
/******************************************************************************
*                            Implementations                                  *
******************************************************************************/
/******************************************************************************
 Function    : _lampSerialMsgSend
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static int _lampSerialMsgSend(uint8_t *pBuf, uint32_t ulLen, uint8_t repeatCnt) 
{
    slsMsgType_t *pMsg = (slsMsgType_t *)pBuf;

    pMsg->stHeader.ucCheckSum = 0;
    for (uint8_t i = 4; i < ulLen; i++) {
        pMsg->stHeader.ucCheckSum += pBuf[i];
    }
     return slSerialMsgCtrl(pBuf, ulLen, repeatCnt);
}

/******************************************************************************
 Function    : _lampSerialMsgProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampSerialMsgProc(uint8_t *pMsg, uint8_t msgLen, uint8_t ucCheckSum) 
{
    uint8_t newCheckSum = 0;
    uint8_t ucMsgType = pMsg[0];
    myLampParam_t *pLampParam = &s_stCurLampParam;

    for (uint8_t i = 0; i < msgLen; i++) {
        newCheckSum += pMsg[i];
    }
    if (newCheckSum != ucCheckSum) {
        my_hal_log_info("check sum error %02x-%02x\r\n", newCheckSum, ucCheckSum);
        return;
    }

#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)
    if (0 != rlFlagGet(RL_FLAG_POS_PICKUP)) {
        rlFlagSet(RL_FLAG_POS_PICKUP, 0);
        my_hal_log_info("lamp set pick up 0\r\n");
    }
#endif

    switch (ucMsgType) {
    case PLC_MSG_ID_STATUS_RSP:
        {            
            slsMsgStatusRsp_t *pRsp = (slsMsgStatusRsp_t *)pMsg;

//          my_hal_log_info("lamp serial get status rsp onoff\r\n");
            if (pRsp->ucReset == 0x01) {
                rlFlagSet(RL_FLAG_SYS_FACTORY_RESET, 1);
            }
            else {
                g_stRlData.liveData.ucBatteryValue = pRsp->ucBattery;
                if (0 == slSerialGetSendLen()) {
                    g_stRlData.liveData.ucLightMode = pRsp->ucLightMode;
                    pLampParam->ucSwitch = pRsp->ucOnoff;
                    pLampParam->uwBri = pRsp->uwBri;
                    pLampParam->uwCCT = pRsp->uwCct;
                }
                else {
                    my_hal_log_info("can not update lamp data\r\n");
                }
                _lampStateControlHook();  
            }
            s_slSerialInfo.recvTick = xTaskGetTickCount();
        }
        break;
    default:
        break;
    }
}

/******************************************************************************
 Function    : _lampSerialProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampSerialRecieveProc(void) 
{
    uint8_t ucTemp = 0;
    uint32_t len = slSerialGetRecvLen();
    static uint8_t ucRecieveState = 0;
    static uint8_t ucMsgLen = 0;
    static uint8_t ucCheckSum = 0;

    if (len == 0) {
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)
        if (xTaskGetTickCount() - s_slSerialInfo.recvTick >= 10000) {
            if (0 == rlFlagGet(RL_FLAG_POS_PICKUP)) {
                rlFlagSet(RL_FLAG_POS_PICKUP, 1);
                _lampStateControlHook();
                my_hal_log_info("lamp set pick up 1\r\n");
            }  
        }
#endif
        return;
    }

    switch (ucRecieveState) {
    case 0: //recieve header 
        slSerialRecieve(&ucTemp, 1);
//      my_hal_log_info("ucTemp %02x\r\n", ucTemp);
        if (ucTemp == 0xA5) {
            ucRecieveState = 1;
        }
        break;
    case 1:
        slSerialRecieve(&ucTemp, 1);
        ucRecieveState = (ucTemp == 0x5A) ? 2 : 0;
        break;
    case 2:
        slSerialRecieve(&ucMsgLen, 1);
        ucRecieveState = (ucMsgLen < 2 || ucMsgLen > 32) ? 0 : 3;
        break;
    case 3:
        if (len >= ucMsgLen) {
            slSerialRecieve(&ucCheckSum, 1);
            slSerialRecieve(s_LampSerialRecieveBuf, len-1);
            _lampSerialMsgProc(s_LampSerialRecieveBuf, len - 1, ucCheckSum);
            ucRecieveState = 0;
//          s_slSerialInfo.busyFlag = 0;
        }
        break;
    }
}

#if 0
/******************************************************************************
 Function    : _lampSerialSendProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampSerialSendProc(void) 
{
    uint32_t ulen = 0;

    if (s_slSerialInfo.busyFlag) {
        if (xTaskGetTickCount() - s_slSerialInfo.sendTick >= s_slSerialInfo.ackTimeout) {
            s_slSerialInfo.busyFlag = 0;
        }
        else {
            return;
        }
    }

    if (0 == myRingBufferGetDataSize(&s_lampSerialRingBuffer)) {
        return;
    }

    ulen = myRingBufferRead(&s_lampSerialRingBuffer, s_LampSerialSendBuf, 32);
    if (ulen > 0) {
        slsMsgType_t *pMsg = (slsMsgType_t *)s_LampSerialSendBuf;
        if (pMsg->ucMsgType == PLC_MSG_ID_STATUS_REQ) {
            s_slSerialInfo.busyFlag = 1;
        }
        pMsg->stHeader.ucCheckSum = 0;
        for (uint8_t i = 4; i < ulen; i++) {
            pMsg->stHeader.ucCheckSum += s_LampSerialSendBuf[i];
        }
        s_slSerialInfo.sendTick = xTaskGetTickCount();
        _lampSerialSend(s_LampSerialSendBuf, ulen);
//      my_hal_log_info("lamp serial send msg %d\r\n", ulen);
    }
}
#endif

/******************************************************************************
 Function    : slLampSerialGetStatusCmd
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slLampSerialGetStatusCmd() 
{
    uint8_t buf[16];
    slsMsgStatusReq_t *pReq = (slsMsgStatusReq_t *)buf;

    pReq->stHeader.uwHeader = PLC_MSG_HEADER_DEF;
    pReq->stHeader.ucLength = sizeof(slsMsgStatusReq_t)-3;
    pReq->stHeader.ucCheckSum = 0;
    pReq->ucMsgType = PLC_MSG_ID_STATUS_REQ;
    return _lampSerialMsgSend(buf, sizeof(slsMsgStatusReq_t), 1);
}

/******************************************************************************
 Function    : _lampSerialGetDevStatusProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampSerialGetDevStatusProc(void) 
{
    uint32_t cTick = xTaskGetTickCount();
    static uint32_t lTick = 0;

    if (cTick - lTick >= s_slSerialInfo.interval) {
        lTick = cTick;
        if (0 == slSerialGetSendLen()) {
            slLampSerialGetStatusCmd();
        } 
    }
}

#if 0
/******************************************************************************
 Function    : _lampSerialGetDevStatusProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampSerialGetDevStatusProc(void) 
{
    uint8_t buf[16];
    slsMsgStatusReq_t *pReq = (slsMsgStatusReq_t *)buf;
    uint32_t cTick = xTaskGetTickCount();
    static uint32_t lTick = 0;

    if (cTick - lTick >= s_slSerialInfo.interval) {
        lTick = cTick;
//      my_hal_log_info("lamp serial get status req\r\n");
        pReq->stHeader.uwHeader = PLC_MSG_HEADER_DEF;
        pReq->stHeader.ucLength = sizeof(slsMsgStatusReq_t)-3;
        pReq->stHeader.ucCheckSum = 0;
        pReq->ucMsgType = PLC_MSG_ID_STATUS_REQ;
        myRingBufferWrite(&s_lampSerialRingBuffer, buf, sizeof(slsMsgStatusReq_t));
    }
}
#endif

/******************************************************************************
 Function    : _lampStateControlHook
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampStateControlHook()
{
    int changed = 0;
    myLampParam_t *pCurLampParam = &s_stCurLampParam;
    myLampParam_t *pLstLampParam = &s_stLstLampParam;

    if (pLstLampParam->ucSwitch != pCurLampParam->ucSwitch) {
        pLstLampParam->ucSwitch = pCurLampParam->ucSwitch;
        changed = 1;
        my_hal_log_info("switch changed %d\r\n", pCurLampParam->ucSwitch);
    }

    if (pLstLampParam->uwBri != pCurLampParam->uwBri) {
        pLstLampParam->uwBri = pCurLampParam->uwBri;
        changed = 1;
        my_hal_log_info("brightness changed %d\r\n", pCurLampParam->uwBri);
    }

    if (pLstLampParam->uwCCT != pCurLampParam->uwCCT) {
        pLstLampParam->uwCCT = pCurLampParam->uwCCT;
        changed = 1;
        my_hal_log_info("cct changed %d\r\n", pCurLampParam->uwCCT);
    }

#if defined(LIGHT_STATE_SAVE_EN) && (LIGHT_STATE_SAVE_EN == 1)
    if (changed && pCurLampParam->ucSwitch != 0) {
        memcpy((void*)&g_stRlData.saveData.stLampSaveParam, (void *)pCurLampParam, sizeof(myLampParam_t));
        rlFlagSet(RL_FLAG_SYS_CFG_DATA_SAVE, 1);
    }
#endif

#if defined(APP_MAGIC_LINK_USE) && (APP_MAGIC_LINK_USE != 0)
        if (g_stRlData.fctData.fctMode == 0) {
            extern void MagicLinkDataRsync(void);
            MagicLinkDataRsync();
        }
#endif
}

/******************************************************************************
 Function    : _lampFactoryReset
 Description : reading lamp factory reset
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void _lampFactoryReset(void) 
{
    static uint32_t lTick = 0;

    if (s_slSerialInfo.ucResetStep == 0) {
        uint8_t buf[16];
        slsMsgResetReq_t *pReq = (slsMsgResetReq_t *)buf;

        pReq->stHeader.uwHeader = PLC_MSG_HEADER_DEF;
        pReq->stHeader.ucLength = sizeof(slsMsgResetReq_t)-3;
        pReq->stHeader.ucCheckSum = 0;

        pReq->ucMsgType = PLC_MSG_ID_RESET_REQ;
        _lampSerialMsgSend(buf, sizeof(slsMsgResetReq_t), 2);
        lTick = xTaskGetTickCount();
        s_slSerialInfo.ucResetStep = 1;
    }
    else {
        if (xTaskGetTickCount() - lTick >= 1000) {
            rlFlagSet(RL_FLAG_SYS_FACTORY_RESET, 0);
            slDataFactoryReset();
//#if defined(APP_MAGIC_LINK_USE) && (APP_MAGIC_LINK_USE != 0)
//            if (g_stRlData.fctData.fctMode == 0) {
//                extern int MagicLinkReset(void);
//                MagicLinkReset();
//            }
//#endif
//            rlDataRestoreFactory();
//            vTaskDelay(10);
//            sys_reset();
        }
    }
}

/******************************************************************************
 Function    : _lampFactoryResetWindowTimeoutHandle
 Description : reading lamp factory reset window timeout handle
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void _lampFactoryResetWindowTimeoutHandle(xTimerHandle pxTimer) 
{
    my_hal_log_info("close factory reset window\r\n");
    rlFlagSet(RL_FLAG_SYS_FACTORY_WINDOW, 0);
}

/******************************************************************************
 Function    : _lampNightLightModeCtrl
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t _lampNightLightModeCtrl(uint8_t lightMode, uint32_t ulPeroidMs) 
{
    uint8_t buf[16];
    myLampParam_t *pLampParam = &s_stCurLampParam;

    pLampParam->ucSceneNo = lightMode;
    pLampParam->ucSwitch = 1;
    switch (lightMode) {
    case NL_LAMP_LIGHT_MODE_READING:
        pLampParam->uwBri = 65535;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MID;
        break;
    case NL_LAMP_LIGHT_MODE_WRITEING:
        pLampParam->uwBri = 65535;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MAX;
        break;
    case NL_LAMP_LIGHT_MODE_RELAX:
        pLampParam->uwBri = 65535;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MIN;
        break;
    case NL_LAMP_LIGHT_MODE_MOON:
        pLampParam->uwBri = 32768;
        pLampParam->uwCCT = 4000;
        break;
    case NL_LAMP_LIGHT_MODE_HYPNOSIS:
        break;
    case NL_LAMP_LIGHT_MODE_NIGHTLIGHT:
        pLampParam->uwBri = 655;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MIN;
        break;
    default:
        pLampParam->uwBri = 65535;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MID;
        break;
    }

    if (lightMode == NL_LAMP_LIGHT_MODE_HYPNOSIS) {
        slsMsgLightModeReq_t *pReq = (slsMsgLightModeReq_t *)buf;
        
        pReq->stHeader.uwHeader = PLC_MSG_HEADER_DEF;
        pReq->stHeader.ucLength = sizeof(slsMsgLightModeReq_t)-3;
        pReq->stHeader.ucCheckSum = 0;

        pReq->ucMsgType = PLC_MSG_ID_DEV_MODE_REQ;
        pReq->ucLightMode = lightMode;
        pReq->uwPeroidMs = 0;

        _lampSerialMsgSend(buf, sizeof(slsMsgLightModeReq_t), SL_LAMP_SERIAL_SEND_REPEAT);
    }
    else {
        slsMsgDevStateReq_t *pReq = (slsMsgDevStateReq_t *)buf;

        pReq->stHeader.uwHeader = PLC_MSG_HEADER_DEF;
        pReq->stHeader.ucLength = sizeof(slsMsgDevStateReq_t)-3;
        pReq->stHeader.ucCheckSum = 0;

        pReq->ucMsgType = PLC_MSG_ID_DEV_STATE_REQ;
        pReq->ucOnoff = pLampParam->ucSwitch;
        pReq->uwBri = pLampParam->uwBri;
        pReq->uwCct = pLampParam->uwCCT;
        pReq->uwPeroid = (uint16_t)ulPeroidMs;

        _lampSerialMsgSend(buf, sizeof(slsMsgDevStateReq_t), SL_LAMP_SERIAL_SEND_REPEAT);

        _lampStateControlHook();
    }
   

    return 0;
}

/******************************************************************************
 Function    : rlLampDevStatusCtrl
 Description : reading lamp device status control
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampDevStatusCtrl(myLampParam_t *pLampParam, uint32_t ulPeroidMs)
{
    return 0;
}

/******************************************************************************
 Function    : rlLampDevPwmCtrl
 Description : reading lamp device pwm control
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampDevPwmCtrl(const uint16_t uwPwm[4], uint32_t ulPeroidMs, uint16_t uwBri)
{
    return 0;
}

/******************************************************************************
 Function    : rlLampSwitchCtrl
 Description : reading lamp switch ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampSwitchCtrl(uint8_t status, uint32_t ulPeroidMs)
{
    uint8_t buf[16];
    slsMsgOnoffReq_t *pReq = (slsMsgOnoffReq_t *)buf;
    myLampParam_t *pLampParam = &s_stCurLampParam;

    pLampParam->ucSwitch = status ? 1 : 0;

    pReq->stHeader.uwHeader = PLC_MSG_HEADER_DEF;
    pReq->stHeader.ucLength = sizeof(slsMsgOnoffReq_t)-3;
    pReq->stHeader.ucCheckSum = 0;

    pReq->ucMsgType = PLC_MSG_ID_ONOFF_REQ;
    pReq->ucOnoff = pLampParam->ucSwitch;
    pReq->uwPeroid = (uint16_t)ulPeroidMs;

    _lampSerialMsgSend(buf, sizeof(slsMsgOnoffReq_t), SL_LAMP_SERIAL_SEND_REPEAT);

    _lampStateControlHook();

    return 0;
}

/******************************************************************************
 Function    : rlLampSwitchRevert
 Description : reading lamp switch revert
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampSwitchRevert(uint32_t ulPeroidMs)
{
    return 0;
}

/******************************************************************************
 Function    : rlLampBriCtrl
 Description : reading lamp bright control
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampBriCtrl(uint16_t uwBri, uint32_t ulPeroidMs)
{
    uint8_t buf[16];
    slsMsgBrightReq_t *pReq = (slsMsgBrightReq_t *)buf;
    myLampParam_t *pLampParam = &s_stCurLampParam;

    pLampParam->uwBri = APP_RANGE(uwBri, LIGHT_BRIGHT_MIN, LIGHT_BRIGHT_MAX);

    pReq->stHeader.uwHeader = PLC_MSG_HEADER_DEF;
    pReq->stHeader.ucLength = sizeof(slsMsgBrightReq_t)-3;
    pReq->stHeader.ucCheckSum = 0;

    pReq->ucMsgType = PLC_MSG_ID_BRIGHT_REQ;
    pReq->uwBri = pLampParam->uwBri;
    pReq->uwPeroid = (uint16_t)ulPeroidMs;

    _lampSerialMsgSend(buf, sizeof(slsMsgBrightReq_t), SL_LAMP_SERIAL_SEND_REPEAT);

    _lampStateControlHook();
}

/******************************************************************************
 Function    : rlLampBriPercentCtrl
 Description : reading lamp bright percent control
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampBriPercentCtrl(uint8_t ucPercent, uint32_t ulPeroidMs)
{
    return rlLampBriCtrl(LIGHT_PERCENT_TO_BRIGHT(ucPercent), ulPeroidMs);
}

/******************************************************************************
 Function    : rlLampCctCtrl
 Description : 灯cct控制模块
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampCctCtrl(uint16_t uwCCT, uint32_t ulPeroidMs)
{
    uint8_t buf[16];
    slsMsgCctReq_t *pReq = (slsMsgCctReq_t *)buf;
    myLampParam_t *pLampParam = &s_stCurLampParam;

    pLampParam->uwCCT = APP_RANGE(uwCCT, LIGHT_COLOR_CCT_MIN, LIGHT_COLOR_CCT_MAX); 

    pReq->stHeader.uwHeader = PLC_MSG_HEADER_DEF;
    pReq->stHeader.ucLength = sizeof(slsMsgCctReq_t)-3;
    pReq->stHeader.ucCheckSum = 0;

    pReq->ucMsgType = PLC_MSG_ID_CCT_REQ;
    pReq->uwCct = pLampParam->uwCCT;
    pReq->uwPeroid = (uint16_t)ulPeroidMs;

    _lampSerialMsgSend(buf, sizeof(slsMsgCctReq_t), SL_LAMP_SERIAL_SEND_REPEAT);

    _lampStateControlHook();

    return 0;
}

/******************************************************************************
 Function    : rlLampLightModeCtrl
 Description : 灯lightmode控制模块
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampLightModeCtrl(uint8_t lightMode, uint32_t ulPeroidMs)
{
#if (APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_BEDSIDE)
    return _lampNightLightModeCtrl(lightMode, ulPeroidMs);
#endif
}

/******************************************************************************
 Function    : rlLampBlinkCtrl
 Description : reading lamp blink ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampBlinkCtrl(uint32_t blinkCnt, uint32_t ulPeroidMs, uint8_t lock, uint32_t arg) 
{
    return 0;
}

/******************************************************************************
 Function    : rlLampAdjustDurationCtrl
 Description : lamp adjust duration ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampAdjustDurationCtrl(uint16_t uwDuration) 
{
    uint8_t buf[16];
    slsMsgAdjustDurationReq_t *pReq = (slsMsgAdjustDurationReq_t *)buf;

    pReq->stHeader.uwHeader = PLC_MSG_HEADER_DEF;
    pReq->stHeader.ucLength = sizeof(slsMsgAdjustDurationReq_t)-3;
    pReq->stHeader.ucCheckSum = 0;

    pReq->ucMsgType = PLC_MSG_ID_ADJUST_DURATION_REQ;
    pReq->uwDuration = uwDuration;

    _lampSerialMsgSend(buf, sizeof(slsMsgAdjustDurationReq_t), SL_LAMP_SERIAL_SEND_REPEAT);

    return 0;
}

/******************************************************************************
 Function    : rlLampSleepDurationCtrl
 Description : lamp sleep duration ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampSleepDurationCtrl(uint16_t uwDuration) 
{
    uint8_t buf[16];
    slsMsgSleepDurationReq_t *pReq = (slsMsgSleepDurationReq_t *)buf;

    pReq->stHeader.uwHeader = PLC_MSG_HEADER_DEF;
    pReq->stHeader.ucLength = sizeof(slsMsgSleepDurationReq_t)-3;
    pReq->stHeader.ucCheckSum = 0;

    pReq->ucMsgType = PLC_MSG_ID_SLEEP_DURATION_REQ;
    pReq->uwDuration = uwDuration;

    _lampSerialMsgSend(buf, sizeof(slsMsgSleepDurationReq_t), SL_LAMP_SERIAL_SEND_REPEAT);

    return 0;
}

/******************************************************************************
 Function    : rlLampGetStatus
 Description : opple lamp get status
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampGetStatus(uint8_t *pOnoff, uint16_t *pBright, uint16_t *pCct) 
{
    myLampParam_t *pLampParam = &s_stCurLampParam;

    if (pOnoff) {
        *pOnoff = pLampParam->ucSwitch;
    }

    if (pBright) {
        *pBright = pLampParam->uwBri;
    }

    if (pCct) {
        *pCct = pLampParam->uwCCT;
    }

    return 0;
}

/******************************************************************************
 Function    : rlLampGetOnoff
 Description : opple lamp get onoff
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlLampGetOnoff(void) 
{
    return s_stCurLampParam.ucSwitch;
}

/******************************************************************************
 Function    : rlLampGetBrightness
 Description : opple lamp get brightness
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint16_t rlLampGetBrightness(void) 
{
    return s_stCurLampParam.uwBri;
}

/******************************************************************************
 Function    : rlLampGetCCT
 Description : opple lamp get cct
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint16_t rlLampGetCCT(void) 
{
    return s_stCurLampParam.uwCCT;
}

/******************************************************************************
 Function    : rlLampGetBriPercent
 Description : reading lamp get bright percent
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlLampGetBriPercent(void) 
{
    return (uint8_t)LIGHT_BRIGHT_TO_PERCENT(s_stCurLampParam.uwBri);
}

/******************************************************************************
 Function    : rlLampGetLightMode
 Description : reading lamp get light mode
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlLampGetLightMode(void) 
{
    return s_stCurLampParam.ucSceneNo;
}

/******************************************************************************
 Function    : slSerialLampBedsideInit
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slSerialLampBedsideInit(void)
{
    int ret = 0;
    myLampParam_t *pLampParam = &s_stCurLampParam;

    pLampParam->ucCtrlType = 0;
    pLampParam->ucSceneNo = 1;
    pLampParam->ucSwitch = 0;
    pLampParam->uwBri = 65535;
#if defined(APP_DEV_CCT_SUPPORT) && (APP_DEV_CCT_SUPPORT == 1)
    pLampParam->uwCCT = LIGHT_CCT_DEFAULT;
#endif  
    s_stLstLampParam = s_stCurLampParam;

    s_slSerialInfo.interval = 600;
    s_slSerialInfo.recvTick = 0;
    s_slSerialInfo.ucResetStep = 0;

    my_hal_log_info("serial lamp bedside init ok\r\n");
    return 0;
}

/******************************************************************************
 Function    : slSerialLampBedsideProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slSerialLampBedsideProc(void)
{
    _lampSerialRecieveProc();

    _lampSerialGetDevStatusProc();

    if (rlFlagGet(RL_FLAG_SYS_FACTORY_RESET)) {
        _lampFactoryReset();
    }
}

#endif //#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)

#ifdef __cplusplus
}
#endif


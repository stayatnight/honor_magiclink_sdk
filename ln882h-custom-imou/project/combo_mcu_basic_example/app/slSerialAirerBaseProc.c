/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slSerialAirerBaseProc.c                                       *
* Description :                                                               *
*               smart lamp serial airer base process source file              *
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
#include "slSerialAirerBaseProc.h"
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_AIRER_BASE)
#include "slData.h"
#include "slTaskSerial.h"
#include "magiclink.h"
#include "sys.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define SL_SERIAL_AIRER_BASE_SEND_REPEAT                         (1)
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/

/******************************************************************************
*                             Declarations                                    *
******************************************************************************/

/******************************************************************************
*                                Globals                                      *
******************************************************************************/
static uint8_t s_airerBaseSerialRecieveBuf[32];
/******************************************************************************
*                          Extern Declarations                                *
******************************************************************************/

/******************************************************************************
*                             Declarations                                    *
******************************************************************************/

/******************************************************************************
*                            Implementations                                  *
******************************************************************************/
/******************************************************************************
 Function    : _airerBaseSerialMsgSend
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static int _airerBaseSerialMsgSend(uint8_t *pBuf, uint32_t ulLen, uint8_t repeatCnt) 
{
    slSerialMsgHeader_t *pMsg = (slSerialMsgHeader_t *)pBuf;

    pMsg->uwCheckSum = 0xbeaf;
    for (uint8_t i = 6; i < ulLen; i++) {
        pMsg->uwCheckSum += pBuf[i];
    }
     return slSerialMsgCtrl(pBuf, ulLen, repeatCnt);
}

/******************************************************************************
 Function    : _airerBaseMsgResponse
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _airerBaseMsgResponse(uint16_t uwMsgType, uint8_t ucOpCode) 
{
    uint8_t buf[32];
    slSerialMsgCommonRsp_t *pRsp = (slSerialMsgCommonRsp_t *)buf;

    pRsp->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pRsp->stHeader.uwLength = sizeof(slSerialMsgCommonRsp_t)-4;
    pRsp->stHeader.uwCheckSum = 0;
    pRsp->stHeader.uwProtocolVer = 1;

    pRsp->uwMsgType = uwMsgType;
    pRsp->ucOpCode = ucOpCode;

    _airerBaseSerialMsgSend(buf, sizeof(slSerialMsgCommonRsp_t), 1);
}

/******************************************************************************
 Function    : _airerBaseMsgProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _airerBaseMsgProc(uint8_t *pMsg, uint8_t msgLen) 
{
    slSerialMsgType_t *pSerialMsgType = (slSerialMsgType_t*)pMsg;   

    switch (pSerialMsgType->uwMsgType) {
    case SERIAL_MSG_ID_S2M_FACTORY_RESET_RSP:
        break;
    case SERIAL_MSG_ID_S2M_STATUS_REPORT_REQ:
        {
            slSerialMsgAirerBaseStatusReportReq_t *pReq = (slSerialMsgAirerBaseStatusReportReq_t *)pMsg;
            stSlDevAirerData_t *pAirerBaseData = &g_stRlData.liveData.stSlDevAirerData;

            pAirerBaseData->ucLightOnoff = pReq->ucLightOnoff;
            pAirerBaseData->ucPosition = pReq->ucCurPosition;
            rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 1); 
            _airerBaseMsgResponse(SERIAL_MSG_ID_M2S_STATUS_REPORT_RSP, 0);
        }
        break;
    case SERIAL_MSG_ID_S2M_FACTORY_RESET_REPORT_REQ:
        _airerBaseMsgResponse(SERIAL_MSG_ID_M2S_FACTORY_RESET_REPORT_RSP, 0);
        rlFlagSet(RL_FLAG_SYS_FACTORY_RESET, 1);
        break;
    case SERIAL_MSG_ID_S2M_GET_STATUS_RSP:
        {            
            slSerialMsgAirerBaseGetStatusRsp_t *pRsp = (slSerialMsgAirerBaseGetStatusRsp_t *)pMsg;
            stSlDevAirerData_t *pAirerBaseData = &g_stRlData.liveData.stSlDevAirerData;

            pAirerBaseData->ucLightOnoff = pRsp->ucLightOnoff;
            pAirerBaseData->ucPosition = pRsp->ucCurPosition;
            rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 1); 
        }
        break;
    case SERIAL_MSG_ID_S2M_LAMP_ONOFF_CTRL_RSP:
        break;
    case SERIAL_MSG_ID_S2M_AIRER_MOTION_CTRL_RSP:
        break;
    default:
        break;
    }
}

/******************************************************************************
 Function    : _airerBaseSerialRecieveProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _airerBaseSerialRecieveProc(void) 
{
    uint8_t ucTemp = 0;
    uint16_t newCheckSum = 0xbeaf;
    uint32_t len = slSerialGetRecvLen();
    static uint8_t ucRecieveState = 0;
    static uint16_t uwMsgLen = 0;
    static uint16_t uwCheckSum = 0;
    static uint16_t uwProtocolVer = 0;

    if (len == 0) {
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
        if (len >= 2) {
            slSerialRecieve((uint8_t *)&uwMsgLen, 2);
            ucRecieveState = (uwMsgLen < 2 || uwMsgLen > 32) ? 0 : 3;
        }  
        break;
    case 3:
        if (len >= uwMsgLen) {
            slSerialRecieve((uint8_t *)&uwCheckSum, 2);
            // slSerialRecieve((uint8_t *)&uwProtocolVer, 2);
            slSerialRecieve(s_airerBaseSerialRecieveBuf, len-2);
            for (uint8_t i = 0; i < len-2; i++) {
                newCheckSum += s_airerBaseSerialRecieveBuf[i];
            }
            if (newCheckSum == uwCheckSum) {
                _airerBaseMsgProc(&s_airerBaseSerialRecieveBuf[2], len - 4);
            }
            else {
                my_hal_log_info("check sum error %02x-%02x\r\n", newCheckSum, uwCheckSum);
            }
            ucRecieveState = 0;
        }
        break;
    }
}

/******************************************************************************
 Function    : _airerBaseFactoryReset
 Description : airer base factory reset
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void _airerBaseFactoryReset(void) 
{
    uint8_t buf[32];
    slSerialMsgFactoryResetReq_t *pReq = (slSerialMsgFactoryResetReq_t *)buf;
    static uint32_t lTick = 0;
    static uint8_t s_ucResetStep = 0;

    if (s_ucResetStep == 0) {
        pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
        pReq->stHeader.uwLength = sizeof(slSerialMsgFactoryResetReq_t)-4;
        pReq->stHeader.uwCheckSum = 0;
        pReq->stHeader.uwProtocolVer = 1;

        pReq->uwMsgType = SERIAL_MSG_ID_M2S_FACTORY_RESET_REQ;
        _airerBaseSerialMsgSend(buf, sizeof(slSerialMsgFactoryResetReq_t), 1);
        lTick = xTaskGetTickCount();
        s_ucResetStep = 1;
    }
    else {
        if (xTaskGetTickCount() - lTick >= 1000) {
            rlFlagSet(RL_FLAG_SYS_FACTORY_RESET, 0);
            // slDataFactoryReset();
#if defined(APP_MAGIC_LINK_USE) && (APP_MAGIC_LINK_USE != 0)
           if (g_stRlData.fctData.fctMode == 0) {
               extern int MagicLinkReset(void);
               MagicLinkReset();
           }
#endif
           slDataFactoryReset();
           vTaskDelay(10);
           sys_reset();
        }
    }
}

/******************************************************************************
 Function    : rlAirerBaseLightOnoffCtrl
 Description : airer base light onoff ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlAirerBaseLightOnoffCtrl(uint8_t status, uint32_t ulPeroidMs)
{
    uint8_t buf[32];
    slSerialMsgLampOnoffCtrlReq_t *pReq = (slSerialMsgLampOnoffCtrlReq_t *)buf;
    stSlDevAirerData_t *pstSlDevAirerData = &g_stRlData.liveData.stSlDevAirerData;

    pstSlDevAirerData->ucLightOnoff = status ? 1 : 0;

    pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pReq->stHeader.uwLength = sizeof(slSerialMsgLampOnoffCtrlReq_t)-4;
    pReq->stHeader.uwCheckSum = 0;
    pReq->stHeader.uwProtocolVer = 1;

    pReq->uwMsgType = SERIAL_MSG_ID_M2S_LAMP_ONOFF_CTRL_REQ;
    pReq->ucOnoff = pstSlDevAirerData->ucLightOnoff;
    pReq->uwPeroidMs = (uint16_t)ulPeroidMs;

    _airerBaseSerialMsgSend(buf, sizeof(slSerialMsgLampOnoffCtrlReq_t), SL_SERIAL_AIRER_BASE_SEND_REPEAT);

    rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 1);

    return 0;
}

/******************************************************************************
 Function    : rlAirerBaseMotionfCtrl
 Description : airer base motion ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlAirerBaseMotionfCtrl(uint8_t ucMotion)
{
    uint8_t buf[32];
    slSerialMsgAirerMotionCtrlReq_t *pReq = (slSerialMsgAirerMotionCtrlReq_t *)buf;
    stSlDevAirerData_t *pstSlDevAirerData = &g_stRlData.liveData.stSlDevAirerData;

    pstSlDevAirerData->ucMotionCtrl = ucMotion;

    pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pReq->stHeader.uwLength = sizeof(slSerialMsgAirerMotionCtrlReq_t)-4;
    pReq->stHeader.uwCheckSum = 0;
    pReq->stHeader.uwProtocolVer = 1;

    pReq->uwMsgType = SERIAL_MSG_ID_M2S_AIRER_MOTION_CTRL_REQ;
    pReq->ucMotion = pstSlDevAirerData->ucMotionCtrl;

    _airerBaseSerialMsgSend(buf, sizeof(slSerialMsgAirerMotionCtrlReq_t), SL_SERIAL_AIRER_BASE_SEND_REPEAT);

    return 0;
}

/******************************************************************************
 Function    : rlAirerBaseGetLightOnoff
 Description : airer base get light onoff
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlAirerBaseGetLightOnoff(void) 
{
    return g_stRlData.liveData.stSlDevAirerData.ucLightOnoff;
}

/******************************************************************************
 Function    : rlAirerBaseGetPosition
 Description : airer base get position
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlAirerBaseGetPosition(void) 
{
    return g_stRlData.liveData.stSlDevAirerData.ucPosition;
}

/******************************************************************************
 Function    : slSerialAirerBaseInit
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slSerialAirerBaseInit(void)
{
    my_hal_log_info("serial airer base init ok\r\n");
    return 0;
}

/******************************************************************************
 Function    : slSerialAirerBaseProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slSerialAirerBaseProc(void)
{
    _airerBaseSerialRecieveProc();

    if (rlFlagGet(RL_FLAG_SYS_DATA_SYNC_NEED)) {
        rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 0);
        extern void slAirerBaseMagicLinkDataRsync(void);
        slAirerBaseMagicLinkDataRsync();
    }

    if (rlFlagGet(RL_FLAG_SYS_FACTORY_RESET)) {
        _airerBaseFactoryReset();
    }
}

#endif //#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)

#ifdef __cplusplus
}
#endif


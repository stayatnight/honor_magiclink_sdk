/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slSerialLampAuroraProc.c                                      *
* Description :                                                               *
*               smart lamp serial lamp aurora process source file             *
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
#include "slSerialLampAuroraProc.h"
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_AURORA)
#include "slData.h"
#include "slTaskSerial.h"
#include "magiclink.h"
#include "sys.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define SL_SERIAL_LAMP_AURORA_SEND_REPEAT                         (1)
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/

/******************************************************************************
*                             Declarations                                    *
******************************************************************************/

/******************************************************************************
*                                Globals                                      *
******************************************************************************/
static uint8_t s_lampAuroraSerialRecieveBuf[32];
/******************************************************************************
*                          Extern Declarations                                *
******************************************************************************/

/******************************************************************************
*                             Declarations                                    *
******************************************************************************/
static void _lampAuroraStateControlHook();
/******************************************************************************
*                            Implementations                                  *
******************************************************************************/
/******************************************************************************
 Function    : _lampAuroraSerialMsgSend
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static int _lampAuroraSerialMsgSend(uint8_t *pBuf, uint32_t ulLen, uint8_t repeatCnt) 
{
    slSerialMsgHeader_t *pMsg = (slSerialMsgHeader_t *)pBuf;

    pMsg->uwCheckSum = 0xbeaf;
    for (uint8_t i = 6; i < ulLen; i++) {
        pMsg->uwCheckSum += pBuf[i];
    }
     return slSerialMsgCtrl(pBuf, ulLen, repeatCnt);
}

/******************************************************************************
 Function    : _lampAuroraMsgResponse
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampAuroraMsgResponse(uint16_t uwMsgType, uint8_t ucOpCode) 
{
    uint8_t buf[32];
    slSerialMsgCommonRsp_t *pRsp = (slSerialMsgCommonRsp_t *)buf;

    pRsp->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pRsp->stHeader.uwLength = sizeof(slSerialMsgCommonRsp_t)-4;
    pRsp->stHeader.uwCheckSum = 0;
    pRsp->stHeader.uwProtocolVer = 1;

    pRsp->uwMsgType = uwMsgType;
    pRsp->ucOpCode = ucOpCode;

    _lampAuroraSerialMsgSend(buf, sizeof(slSerialMsgCommonRsp_t), 1);
}

/******************************************************************************
 Function    : _lampAuroraMsgProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampAuroraMsgProc(uint8_t *pMsg, uint8_t msgLen) 
{
    slSerialMsgType_t *pSerialMsgType = (slSerialMsgType_t*)pMsg;   

    switch (pSerialMsgType->uwMsgType) {
    case SERIAL_MSG_ID_S2M_FACTORY_RESET_RSP:
        break;
    case SERIAL_MSG_ID_S2M_STATUS_REPORT_REQ:
        {
            slSerialMsgLampAuroraStatusReportReq_t *pReq = (slSerialMsgLampAuroraStatusReportReq_t *)pMsg;
            stSlDevAirerData_t *pLampAuroraData = &g_stRlData.liveData.stSlDevAirerData;

            pLampAuroraData->ucLightOnoff = pReq->ucLightOnoff;
            pLampAuroraData->ucPosition = pReq->ucCurPosition;
            _lampAuroraStateControlHook();  
            _lampAuroraMsgResponse(SERIAL_MSG_ID_M2S_STATUS_REPORT_RSP, 0);
        }
        break;
    case SERIAL_MSG_ID_S2M_FACTORY_RESET_REPORT_REQ:
        _lampAuroraMsgResponse(SERIAL_MSG_ID_M2S_FACTORY_RESET_REPORT_RSP, 0);
        rlFlagSet(RL_FLAG_SYS_FACTORY_RESET, 1);
        break;
    case SERIAL_MSG_ID_S2M_GET_STATUS_RSP:
        {            
            slSerialMsgLampAuroraGetStatusRsp_t *pRsp = (slSerialMsgLampAuroraGetStatusRsp_t *)pMsg;
            stSlDevAirerData_t *pLampAuroraData = &g_stRlData.liveData.stSlDevAirerData;

            pLampAuroraData->ucLightOnoff = pRsp->ucLightOnoff;
            pLampAuroraData->ucPosition = pRsp->ucCurPosition;
            _lampAuroraStateControlHook();  
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
 Function    : _lampAuroraSerialRecieveProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampAuroraSerialRecieveProc(void) 
{
    uint8_t ucTemp = 0;
    uint16_t newCheckSum = 0;
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
            slSerialRecieve((uint8_t *)&uwProtocolVer, 2);
            slSerialRecieve(s_lampAuroraSerialRecieveBuf, len-4);
            for (uint8_t i = 0; i < len-1; i++) {
                newCheckSum += s_lampAuroraSerialRecieveBuf[i];
            }
            if (newCheckSum == uwCheckSum) {
                _lampAuroraMsgProc(s_lampAuroraSerialRecieveBuf, len - 4);
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
 Function    : _lampAuroraStateControlHook
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampAuroraStateControlHook(void)
{
    int changed = 0;
    stSlDevAirerData_t *pCurLampAuroraData = &g_stRlData.liveData.stSlDevAirerData;
    stSlDevAirerData_t *pLstLampAuroraData = &s_stLastDevAirerData;

    if (pLstLampAuroraData->ucLightOnoff != pCurLampAuroraData->ucLightOnoff) {
        pLstLampAuroraData->ucLightOnoff = pCurLampAuroraData->ucLightOnoff;
        changed = 1;
        my_hal_log_info("light onoff changed %d\r\n", pCurLampAuroraData->ucLightOnoff);
    }

    if (pLstLampAuroraData->ucPosition != pCurLampAuroraData->ucPosition) {
        pLstLampAuroraData->ucPosition = pCurLampAuroraData->ucPosition;
        changed = 1;
        my_hal_log_info("position changed %d\r\n", pCurLampAuroraData->ucPosition);
    }

#if defined(LIGHT_STATE_SAVE_EN) && (LIGHT_STATE_SAVE_EN == 1)
    if (changed) {
        memcpy((void *)&g_stRlData.saveData.stSlDevAirerData, (void *)pCurLampAuroraData, sizeof(stSlDevAirerData_t));
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
 Function    : _lampAuroraFactoryReset
 Description : lamp aurora factory reset
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void _lampAuroraFactoryReset(void) 
{
    uint8_t buf[32];
    slSerialMsgFactoryResetReq_t *pReq = (slSerialMsgFactoryResetReq_t *)buf;
    static uint32_t lTick = 0;
    static uint8_t s_ucResetStep = 0;

    if (s_ucResetStep == 0) {
        pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
        pReq->stHeader.uwLength = sizeof(slSerialMsgFactoryResetReq_t)-4;
        pReq->stHeader.uwCheckSum = 0;
        pReq->stHeader.uwProtocolVer = 0;

        pReq->uwMsgType = SERIAL_MSG_ID_M2S_FACTORY_RESET_REQ;
        _lampAuroraSerialMsgSend(buf, sizeof(slSerialMsgFactoryResetReq_t), 1);
        lTick = xTaskGetTickCount();
        s_ucResetStep = 1;
    }
    else {
        if (xTaskGetTickCount() - lTick >= 1000) {
            rlFlagSet(RL_FLAG_SYS_FACTORY_RESET, 0);
            slDataFactoryReset();
        }
    }
}

/******************************************************************************
 Function    : rlLampAuroraLightOnoffCtrl
 Description : lamp aurora light onoff ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampAuroraLightOnoffCtrl(uint8_t status, uint32_t ulPeroidMs)
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

    _lampAuroraSerialMsgSend(buf, sizeof(slSerialMsgLampOnoffCtrlReq_t), SL_SERIAL_LAMP_AURORA_SEND_REPEAT);

    _lampAuroraStateControlHook();

    return 0;
}

/******************************************************************************
 Function    : rlLampAuroraMotionfCtrl
 Description : lamp aurora motion ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampAuroraMotionfCtrl(uint8_t ucMotion)
{
    uint8_t buf[32];
    slSerialMsgAirerMotionCtrlReq_t *pReq = (slSerialMsgAirerMotionCtrlReq_t *)buf;
    stSlDevAirerData_t *pstSlDevAirerData = &g_stRlData.liveData.stSlDevAirerData;

    pstSlDevAirerData->ucMotionCtrl = ucMotion;

    pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pReq->stHeader.uwLength = sizeof(slSerialMsgAirerMotionCtrlReq_t)-4;
    pReq->stHeader.uwCheckSum = 0;
    pReq->stHeader.uwProtocolVer = 1;

    pReq->uwMsgType = SERIAL_MSG_ID_M2S_LAMP_ONOFF_CTRL_REQ;
    pReq->ucMotion = pstSlDevAirerData->ucMotionCtrl;

    _lampAuroraSerialMsgSend(buf, sizeof(slSerialMsgAirerMotionCtrlReq_t), SL_SERIAL_LAMP_AURORA_SEND_REPEAT);

    return 0;
}

/******************************************************************************
 Function    : rlLampAuroraGetLightOnoff
 Description : lamp aurora get light onoff
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlLampAuroraGetLightOnoff(void) 
{
    return g_stRlData.liveData.stSlDevAirerData.ucLightOnoff;
}

/******************************************************************************
 Function    : rlLampAuroraGetPosition
 Description : lamp aurora get position
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlLampAuroraGetPosition(void) 
{
    return g_stRlData.liveData.stSlDevAirerData.ucPosition;
}

/******************************************************************************
 Function    : slSerialLampAuroraInit
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slSerialLampAuroraInit(void)
{
    my_hal_log_info("serial lamp aurora init ok\r\n");
    return 0;
}

/******************************************************************************
 Function    : slSerialLampAuroraProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slSerialLampAuroraProc(void)
{
    _lampAuroraSerialRecieveProc();

    if (rlFlagGet(RL_FLAG_SYS_FACTORY_RESET)) {
        _lampAuroraFactoryReset();
    }
}

#endif //#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_AURORA)

#ifdef __cplusplus
}
#endif


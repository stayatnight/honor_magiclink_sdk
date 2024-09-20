/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slSerialBathBullyBaseProc.c                                   *
* Description :                                                               *
*               smart lamp serial bathbully base process source file          *
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
#include "slSerialBathBullyBaseProc.h"
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_BATHBULLY_BASE)
#include "slData.h"
#include "slTaskSerial.h"
#include "magiclink.h"
#include "sys.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define SL_SERIAL_BATHBULLY_BASE_SEND_REPEAT                         (1)
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/

/******************************************************************************
*                             Declarations                                    *
******************************************************************************/

/******************************************************************************
*                                Globals                                      *
******************************************************************************/
static uint8_t s_bathBullyBaseSerialRecieveBuf[32];
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
 Function    : _bathBullyBaseSerialMsgSend
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static int _bathBullyBaseSerialMsgSend(uint8_t *pBuf, uint32_t ulLen, uint8_t repeatCnt) 
{
    slSerialMsgHeader_t *pMsg = (slSerialMsgHeader_t *)pBuf;

    pMsg->uwCheckSum = 0xbeaf;
    for (uint8_t i = 6; i < ulLen; i++) {
        pMsg->uwCheckSum += pBuf[i];
    }
     return slSerialMsgCtrl(pBuf, ulLen, repeatCnt);
}

/******************************************************************************
 Function    : _bathBullyBaseMsgResponse
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _bathBullyBaseMsgResponse(uint16_t uwMsgType, uint8_t ucOpCode) 
{
    uint8_t buf[32];
    slSerialMsgCommonRsp_t *pRsp = (slSerialMsgCommonRsp_t *)buf;

    pRsp->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pRsp->stHeader.uwLength = sizeof(slSerialMsgCommonRsp_t)-4;
    pRsp->stHeader.uwCheckSum = 0;
    pRsp->stHeader.uwProtocolVer = 1;

    pRsp->uwMsgType = uwMsgType;
    pRsp->ucOpCode = ucOpCode;

    _bathBullyBaseSerialMsgSend(buf, sizeof(slSerialMsgCommonRsp_t), 1);
}

/******************************************************************************
 Function    : _bathBullyBaseMsgProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _bathBullyBaseMsgProc(uint8_t *pMsg, uint8_t msgLen) 
{
    slSerialMsgType_t *pSerialMsgType = (slSerialMsgType_t*)pMsg;   

    switch (pSerialMsgType->uwMsgType) {
    case SERIAL_MSG_ID_S2M_FACTORY_RESET_RSP:
        break;
    case SERIAL_MSG_ID_S2M_STATUS_REPORT_REQ:
        {
            slSerialMsgBathBullyBaseStatusReportReq_t *pReq = (slSerialMsgBathBullyBaseStatusReportReq_t *)pMsg;
            stSlDevBathBullyData_t *pBathBullyBaseData = &g_stRlData.liveData.stSlDevBathBullyData;

            pBathBullyBaseData->ucLightOnoff = pReq->ucLightOnoff;
            pBathBullyBaseData->ucDryOnoff = pReq->ucDryOnoff;
            pBathBullyBaseData->scTemperature = pReq->scTemperature;
            pBathBullyBaseData->ucLightMode = pReq->ucLightMode;
            pBathBullyBaseData->ucFanOnoff = pReq->ucFanOnoff;
            pBathBullyBaseData->ucVentilationOnoff = pReq->ucVentilationOnoff;
            pBathBullyBaseData->ucWarmFanGear = pReq->ucWarmFanGear;
            pBathBullyBaseData->ucWarmFanOnoff = pReq->ucWarmFanOnoff;
            // pBathBullyBaseData->ucPosition = pReq->ucCurPosition;
            rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 1);
            _bathBullyBaseMsgResponse(SERIAL_MSG_ID_M2S_STATUS_REPORT_RSP, 0);
        }
        break;
    case SERIAL_MSG_ID_S2M_FACTORY_RESET_REPORT_REQ:
        _bathBullyBaseMsgResponse(SERIAL_MSG_ID_M2S_FACTORY_RESET_REPORT_RSP, 0);
        rlFlagSet(RL_FLAG_SYS_FACTORY_RESET, 1);
        break;
    case SERIAL_MSG_ID_S2M_GET_STATUS_RSP:
        {            
            slSerialMsgBathBullyBaseGetStatusRsp_t *pRsp = (slSerialMsgBathBullyBaseGetStatusRsp_t *)pMsg;
            stSlDevBathBullyData_t *pBathBullyBaseData = &g_stRlData.liveData.stSlDevBathBullyData;

            pBathBullyBaseData->ucLightOnoff = pRsp->ucLightOnoff;
            // pBathBullyBaseData->ucPosition = pRsp->ucCurPosition;
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
 Function    : _bathBullyBaseSerialRecieveProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _bathBullyBaseSerialRecieveProc(void) 
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
            slSerialRecieve(s_bathBullyBaseSerialRecieveBuf, len-2);
            for (uint8_t i = 0; i < len-2; i++) {
                newCheckSum += s_bathBullyBaseSerialRecieveBuf[i];
            }
            if (newCheckSum == uwCheckSum) {
                _bathBullyBaseMsgProc(&s_bathBullyBaseSerialRecieveBuf[2], len - 4);
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
 Function    : _bathBullyBaseFactoryReset
 Description : bathBully base factory reset
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void _bathBullyBaseFactoryReset(void) 
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
        _bathBullyBaseSerialMsgSend(buf, sizeof(slSerialMsgFactoryResetReq_t), 1);
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
 Function    : rlBathBullyBaseLightOnoffCtrl
 Description : bathBully base light onoff ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlBathBullyBaseLightOnoffCtrl(uint8_t status, uint32_t ulPeroidMs)
{
    uint8_t buf[32];
    slSerialMsgLampOnoffCtrlReq_t *pReq = (slSerialMsgLampOnoffCtrlReq_t *)buf;
    stSlDevBathBullyData_t *pstSlDevBathBullyData = &g_stRlData.liveData.stSlDevBathBullyData;

    pstSlDevBathBullyData->ucLightOnoff = status ? 1 : 0;

    pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pReq->stHeader.uwLength = sizeof(slSerialMsgLampOnoffCtrlReq_t)-4;
    pReq->stHeader.uwCheckSum = 0;
    pReq->stHeader.uwProtocolVer = 1;

    pReq->uwMsgType = SERIAL_MSG_ID_M2S_LAMP_ONOFF_CTRL_REQ;
    pReq->ucOnoff = pstSlDevBathBullyData->ucLightOnoff;
    pReq->uwPeroidMs = (uint16_t)ulPeroidMs;

    _bathBullyBaseSerialMsgSend(buf, sizeof(slSerialMsgLampOnoffCtrlReq_t), SL_SERIAL_BATHBULLY_BASE_SEND_REPEAT);

    rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 1);

    return 0;
}

/******************************************************************************
 Function    : rlBathBullyBaseLightModeCtrl
 Description : bathBully base light mode ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlBathBullyBaseLightModeCtrl(uint8_t ucMode, uint32_t ulPeroidMs)
{
    uint8_t buf[32];
    slSerialMsgLampModeCtrlReq_t *pReq = (slSerialMsgLampModeCtrlReq_t *)buf;
    stSlDevBathBullyData_t *pstSlDevBathBullyData = &g_stRlData.liveData.stSlDevBathBullyData;

    pstSlDevBathBullyData->ucLightMode = ucMode;

    pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pReq->stHeader.uwLength = sizeof(slSerialMsgLampModeCtrlReq_t)-4;
    pReq->stHeader.uwCheckSum = 0;
    pReq->stHeader.uwProtocolVer = 1;

    pReq->uwMsgType = SERIAL_MSG_ID_M2S_LAMP_MODE_CTRL_REQ;
    pReq->ucLightMode = pstSlDevBathBullyData->ucLightMode;
    pReq->uwPeroidMs = (uint16_t)ulPeroidMs;

    _bathBullyBaseSerialMsgSend(buf, sizeof(slSerialMsgLampModeCtrlReq_t), SL_SERIAL_BATHBULLY_BASE_SEND_REPEAT);

    rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 1);

    return 0;
}

/******************************************************************************
 Function    : rlBathBullyBaseDryOnoffCtrl
 Description : bathBully base dry onoff ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlBathBullyBaseDryOnoffCtrl(uint8_t state)
{
    uint8_t buf[32];
    slSerialMsgBathBullyDryOnoffCtrlReq_t *pReq = (slSerialMsgBathBullyDryOnoffCtrlReq_t *)buf;
    stSlDevBathBullyData_t *pstSlDevBathBullyData = &g_stRlData.liveData.stSlDevBathBullyData;

    pstSlDevBathBullyData->ucDryOnoff = state;

    pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pReq->stHeader.uwLength = sizeof(slSerialMsgBathBullyFanOnoffCtrlReq_t)-4;
    pReq->stHeader.uwCheckSum = 0;
    pReq->stHeader.uwProtocolVer = 1;

    pReq->uwMsgType = SERIAL_MSG_ID_M2S_BATHBULLY_DRY_CTRL_REQ;
    pReq->ucOnoff = state;

    _bathBullyBaseSerialMsgSend(buf, sizeof(slSerialMsgBathBullyDryOnoffCtrlReq_t), SL_SERIAL_BATHBULLY_BASE_SEND_REPEAT);

    rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 1);

    return 0;
}

/******************************************************************************
 Function    : rlBathBullyBaseFanOnoffCtrl
 Description : bathBully base fan onoff ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlBathBullyBaseFanOnoffCtrl(uint8_t state)
{
    uint8_t buf[32];
    slSerialMsgBathBullyFanOnoffCtrlReq_t *pReq = (slSerialMsgBathBullyFanOnoffCtrlReq_t *)buf;
    stSlDevBathBullyData_t *pstSlDevBathBullyData = &g_stRlData.liveData.stSlDevBathBullyData;

    pstSlDevBathBullyData->ucFanOnoff = state;

    pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pReq->stHeader.uwLength = sizeof(slSerialMsgBathBullyFanOnoffCtrlReq_t)-4;
    pReq->stHeader.uwCheckSum = 0;
    pReq->stHeader.uwProtocolVer = 1;

    pReq->uwMsgType = SERIAL_MSG_ID_M2S_BATHBULLY_FAN_CTRL_REQ;
    pReq->ucOnoff = state;

    _bathBullyBaseSerialMsgSend(buf, sizeof(slSerialMsgBathBullyFanOnoffCtrlReq_t), SL_SERIAL_BATHBULLY_BASE_SEND_REPEAT);

    rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 1);

    return 0;
}

/******************************************************************************
 Function    : rlBathBullyBaseVentilationOnoffCtrl
 Description : bathBully base ventilation onoff ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlBathBullyBaseVentilationOnoffCtrl(uint8_t state)
{
    uint8_t buf[32];
    slSerialMsgBathBullyVentilationOnoffCtrlReq_t *pReq = (slSerialMsgBathBullyVentilationOnoffCtrlReq_t *)buf;
    stSlDevBathBullyData_t *pstSlDevBathBullyData = &g_stRlData.liveData.stSlDevBathBullyData;

    pstSlDevBathBullyData->ucVentilationOnoff = state;

    pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pReq->stHeader.uwLength = sizeof(slSerialMsgBathBullyVentilationOnoffCtrlReq_t)-4;
    pReq->stHeader.uwCheckSum = 0;
    pReq->stHeader.uwProtocolVer = 1;

    pReq->uwMsgType = SERIAL_MSG_ID_M2S_BATHBULLY_VENTI_CTRL_REQ;
    pReq->ucOnoff = state;

    _bathBullyBaseSerialMsgSend(buf, sizeof(slSerialMsgBathBullyVentilationOnoffCtrlReq_t), SL_SERIAL_BATHBULLY_BASE_SEND_REPEAT);

    rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 1);

    return 0;
}


/******************************************************************************
 Function    : rlBathBullyBaseWarmFanOnoffCtrl
 Description : bathBully base warmfan onoff ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlBathBullyBaseWarmFanOnoffCtrl(uint8_t state)
{
    uint8_t buf[32];
    slSerialMsgBathBullyWarmFanOnoffCtrlReq_t *pReq = (slSerialMsgBathBullyWarmFanOnoffCtrlReq_t *)buf;
    stSlDevBathBullyData_t *pstSlDevBathBullyData = &g_stRlData.liveData.stSlDevBathBullyData;

    pstSlDevBathBullyData->ucWarmFanOnoff = state;

    pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pReq->stHeader.uwLength = sizeof(slSerialMsgBathBullyWarmFanOnoffCtrlReq_t)-4;
    pReq->stHeader.uwCheckSum = 0;
    pReq->stHeader.uwProtocolVer = 1;

    pReq->uwMsgType = SERIAL_MSG_ID_M2S_BATHBULLY_WARM_FAN_CTRL_REQ;
    pReq->ucOnoff = state;

    _bathBullyBaseSerialMsgSend(buf, sizeof(slSerialMsgBathBullyWarmFanOnoffCtrlReq_t), SL_SERIAL_BATHBULLY_BASE_SEND_REPEAT);

    rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 1);

    return 0;
}


/******************************************************************************
 Function    : rlBathBullyBaseWarmFanGearCtrl
 Description : bathBully base warmfan gear ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlBathBullyBaseWarmFanGearCtrl(uint8_t state)
{
    uint8_t buf[32];
    slSerialMsgBathBullyWarmFanGearCtrlReq_t *pReq = (slSerialMsgBathBullyWarmFanGearCtrlReq_t *)buf;
    stSlDevBathBullyData_t *pstSlDevBathBullyData = &g_stRlData.liveData.stSlDevBathBullyData;

    pstSlDevBathBullyData->ucWarmFanGear = state;

    pReq->stHeader.uwHeader = SERIAL_MSG_HEADER_DEF;
    pReq->stHeader.uwLength = sizeof(slSerialMsgBathBullyWarmFanGearCtrlReq_t)-4;
    pReq->stHeader.uwCheckSum = 0;
    pReq->stHeader.uwProtocolVer = 1;

    pReq->uwMsgType = SERIAL_MSG_ID_M2S_BATHBULLY_WARM_GEAR_CTRL_REQ;
    pReq->ucWarmFanGear = state;
    // pReq->ucOnoff = state;

    _bathBullyBaseSerialMsgSend(buf, sizeof(slSerialMsgBathBullyWarmFanGearCtrlReq_t), SL_SERIAL_BATHBULLY_BASE_SEND_REPEAT);

    rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 1);

    return 0;
}

/******************************************************************************
 Function    : rlBathBullyBaseGetLightOnoff
 Description : bathBully base get light onoff
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlBathBullyBaseGetLightOnoff(void) 
{
    return g_stRlData.liveData.stSlDevBathBullyData.ucLightOnoff;
}

/******************************************************************************
 Function    : rlBathBullyBaseGetLightMode
 Description : bathBully base get light mode
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlBathBullyBaseGetLightMode(void) 
{
    return g_stRlData.liveData.stSlDevBathBullyData.ucLightMode;
}

/******************************************************************************
 Function    : rlBathBullyBaseGetDryOnoff
 Description : bathBully base get dry onoff
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlBathBullyBaseGetDryOnoff(void) 
{
    return g_stRlData.liveData.stSlDevBathBullyData.ucDryOnoff;
}

/******************************************************************************
 Function    : rlBathBullyBaseGetFanOnoff
 Description : bathBully base get fan onoff
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlBathBullyBaseGetFanOnoff(void) 
{
    return g_stRlData.liveData.stSlDevBathBullyData.ucFanOnoff;
}

/******************************************************************************
 Function    : rlBathBullyBaseGetVentilationOnoff
 Description : bathBully base get ventilation onoff
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlBathBullyBaseGetVentilationOnoff(void) 
{
    return g_stRlData.liveData.stSlDevBathBullyData.ucVentilationOnoff;
}

/******************************************************************************
 Function    : rlBathBullyBaseGetWarmFanOnoff
 Description : bathBully base get warm fan onoff
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlBathBullyBaseGetWarmFanOnoff(void) 
{
    return g_stRlData.liveData.stSlDevBathBullyData.ucWarmFanOnoff;
}

/******************************************************************************
 Function    : rlBathBullyBaseGetWarmFanGearOnoff
 Description : bathBully base get warm fan gear onoff
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlBathBullyBaseGetWarmFanGearOnoff(void) 
{
    return g_stRlData.liveData.stSlDevBathBullyData.ucWarmFanGear;
}

/******************************************************************************
 Function    : slSerialBathBullyBaseInit
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slSerialBathBullyBaseInit(void)
{
    my_hal_log_info("serial bathBully base init ok\r\n");
    return 0;
}

/******************************************************************************
 Function    : slSerialBathBullyBaseProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slSerialBathBullyBaseProc(void)
{
    _bathBullyBaseSerialRecieveProc();

    if (rlFlagGet(RL_FLAG_SYS_DATA_SYNC_NEED)) {
        rlFlagSet(RL_FLAG_SYS_DATA_SYNC_NEED, 0);
       extern void slBathBullyMagicLinkDataRsync(void);
        slBathBullyMagicLinkDataRsync();
    }

    if (rlFlagGet(RL_FLAG_SYS_FACTORY_RESET)) {
        _bathBullyBaseFactoryReset();
    }
}

#endif //#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_BATHBULLY_BASE)

#ifdef __cplusplus
}
#endif


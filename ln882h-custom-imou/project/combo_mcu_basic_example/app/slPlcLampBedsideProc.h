/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slPlcLampBedsideProc.h                                        *
* Description :                                                               *
*               smart lamp plc lamp bedside process header file               *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-12-01                                                    *
* Mod History : (none)                                                        *
******************************************************************************/
#ifndef __SL_PLC_LAMP_BEDSIDE_H__
#define __SL_PLC_LAMP_BEDSIDE_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "myHal.h"
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define PLC_MSG_HEADER_DEF                                 0x5AA5

#define PLC_MSG_ID_STATUS_REQ                              0x01
#define PLC_MSG_ID_STATUS_RSP                              0x02

#define PLC_MSG_ID_ONOFF_REQ                               0x03

#define PLC_MSG_ID_BRIGHT_REQ                              0x05

#define PLC_MSG_ID_BRIGHT_PERCENT_REQ                      0x07

#define PLC_MSG_ID_CCT_REQ                                 0x09

#define PLC_MSG_ID_CCT_PERCENT_REQ                         0x0B

#define PLC_MSG_ID_DEV_STATE_REQ                           0x0D

#define PLC_MSG_ID_DEV_MODE_REQ                            0x0F

#define PLC_MSG_ID_RESET_REQ                               0x11

#define PLC_MSG_ID_ADJUST_DURATION_REQ                     0x13

#define PLC_MSG_ID_SLEEP_DURATION_REQ                      0x15

//define blink arg type
#define RL_LAMP_BLINK_ARG_SYS_FACTORY_RESET                1
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
#pragma pack(1)
typedef struct {
    uint16_t uwHeader;
    uint8_t ucLength;
    uint8_t ucCheckSum;
} slsMsgHeader_t;

typedef struct {
    slsMsgHeader_t stHeader;
    uint8_t ucMsgType;
} slsMsgType_t;

typedef struct {
    slsMsgHeader_t stHeader;
    uint8_t ucMsgType;
} slsMsgStatusReq_t;

typedef struct {
    uint8_t ucMsgType;
    uint8_t ucBattery;
    uint8_t ucLightMode;
    uint8_t ucOnoff;
    uint16_t uwBri;
    uint16_t uwCct;
    uint8_t ucReset;
} slsMsgStatusRsp_t;

typedef struct {
    slsMsgHeader_t stHeader;
    uint8_t ucMsgType;
    uint8_t ucOnoff;
    uint16_t uwPeroid;
} slsMsgOnoffReq_t;

typedef struct {
    slsMsgHeader_t stHeader;
    uint8_t ucMsgType;
    uint16_t uwBri;
    uint16_t uwPeroid;
} slsMsgBrightReq_t;

typedef struct {
    slsMsgHeader_t stHeader;
    uint8_t ucMsgType;
    uint8_t ucBriPercent;
    uint16_t uwPeroid;
} slsMsgBrightPercentReq_t;

typedef struct {
    slsMsgHeader_t stHeader;
    uint8_t ucMsgType;
    uint16_t uwCct;
    uint16_t uwPeroid;
} slsMsgCctReq_t;

typedef struct {
    slsMsgHeader_t stHeader;
    uint8_t ucMsgType;
    uint8_t ucOnoff;
    uint16_t uwBri;
    uint16_t uwCct;
    uint16_t uwPeroid;
} slsMsgDevStateReq_t;

typedef struct {
    slsMsgHeader_t stHeader;
    uint8_t ucMsgType;
    uint8_t ucLightMode;
    uint16_t uwPeroidMs;
} slsMsgLightModeReq_t;

typedef struct {
    slsMsgHeader_t stHeader;
    uint8_t ucMsgType;
} slsMsgResetReq_t;

typedef struct {
    slsMsgHeader_t stHeader;
    uint8_t ucMsgType;
    uint16_t uwDuration;
} slsMsgAdjustDurationReq_t;

typedef struct {
    slsMsgHeader_t stHeader;
    uint8_t ucMsgType;
    uint16_t uwDuration;
} slsMsgSleepDurationReq_t;

typedef enum
{
    NL_LAMP_LIGHT_MODE_GUEST=0,
    NL_LAMP_LIGHT_MODE_RELAX=1,
    NL_LAMP_LIGHT_MODE_VIEWING=2,
    NL_LAMP_LIGHT_MODE_DINNER=3,
    NL_LAMP_LIGHT_MODE_WORKING=6,
    NL_LAMP_LIGHT_MODE_SLEEPING=7,
    NL_LAMP_LIGHT_MODE_READING=8,
    NL_LAMP_LIGHT_MODE_NIGHTLIGHT=100,
    NL_LAMP_LIGHT_MODE_BEDTIME=101,
    NL_LAMP_LIGHT_MODE_MOON = 101,
    NL_LAMP_LIGHT_MODE_HYPNOSIS = 102,
    NL_LAMP_LIGHT_MODE_WRITEING=200,
} nlLampLightMode_t;
#pragma pack()
/******************************************************************************
*                           Extern Declarations                               *
******************************************************************************/

/******************************************************************************
*                              Declarations                                   *
******************************************************************************/
// interface ctrl
int8_t rlLampSwitchCtrl(uint8_t status, uint32_t ulPeroidMs);
int8_t rlLampSwitchRevert(uint32_t ulPeroidMs);
int8_t rlLampBriCtrl(uint16_t uwBri, uint32_t ulPeroidMs);
int8_t rlLampBriPercentCtrl(uint8_t ucPercent, uint32_t ulPeroidMs);
int8_t rlLampBriCtrlNextClass(uint32_t ulPeroidMs);
int8_t rlLampCctCtrl(uint16_t uwCCT, uint32_t ulPeroidMs);
int8_t rlLampLightModeCtrl(uint8_t lightMode, uint32_t ulPeroidMs);
int8_t rlLampBlinkCtrl(uint32_t blinkCnt, uint32_t ulPeroidMs, uint8_t lock, uint32_t arg);
void slLampPowerOnHook(void);

//interface get
int8_t rlLampGetStatus(uint8_t *pOnoff, uint16_t *pBright, uint16_t *pCct);
uint8_t rlLampGetOnoff(void);
uint16_t rlLampGetBrightness(void);
uint16_t rlLampGetCCT(void);
uint8_t rlLampGetBriPercent(void);
uint8_t rlLampGetLightMode(void);

//interface task
int slSerialLampBedsideInit(void);
void slSerialLampBedsideProc(void);

#endif //#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)

#ifdef __cplusplus
}
#endif

#endif /*__SL_PLC_LAMP_BEDSIDE_H__*/

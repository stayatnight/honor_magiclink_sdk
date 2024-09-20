/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slSerialLampAuroraProc.h                                      *
* Description :                                                               *
*               smart lamp serial lamp aurora process header file             *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-12-01                                                    *
* Mod History : (none)                                                        *
******************************************************************************/
#ifndef __SL_SERIAL_LAMP_AURORA_H__
#define __SL_SERIAL_LAMP_AURORA_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "myHal.h"
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_AURORA)
#include "slSerialMsg.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/

/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
#pragma pack(1)
/*(0x0210)LAMP->GATEWAY:晾衣架状态上报请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucLightOnoff;
    uint8_t ucCurPosition;
} slSerialMsgLampAuroraStatusReportReq_t;

/*(0x0310)LAMP->GATEWAY:晾衣架状态查询响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucLightOnoff;
    uint8_t ucCurPosition;
} slSerialMsgLampAuroraGetStatusRsp_t;
#pragma pack()
/******************************************************************************
*                           Extern Declarations                               *
******************************************************************************/

/******************************************************************************
*                              Declarations                                   *
******************************************************************************/
// interface ctrl
int8_t rlLampAuroraLightOnoffCtrl(uint8_t status, uint32_t ulPeroidMs);
int8_t rlLampAuroraMotionfCtrl(uint8_t ucMotion);

//interface get
uint8_t rlLampAuroraGetLightOnoff(void);
uint8_t rlLampAuroraGetPosition(void);

//interface task
int slSerialLampAuroraInit(void);
void slSerialLampAuroraProc(void);

#endif //#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_AURORA)

#ifdef __cplusplus
}
#endif

#endif /*__SL_SERIAL_LAMP_AURORA_H__*/

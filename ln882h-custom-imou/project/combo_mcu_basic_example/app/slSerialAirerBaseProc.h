/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slSerialAirerBaseProc.h                                       *
* Description :                                                               *
*               smart lamp serial airer base process header file              *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-12-01                                                    *
* Mod History : (none)                                                        *
******************************************************************************/
#ifndef __SL_SERIAL_AIRER_BASE_H__
#define __SL_SERIAL_AIRER_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "myHal.h"
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_AIRER_BASE)
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
    uint16_t uwMsgType;
    uint8_t ucLightOnoff;
    uint8_t ucCurPosition;
} slSerialMsgAirerBaseStatusReportReq_t;

/*(0x0310)LAMP->GATEWAY:晾衣架状态查询响应消息结构定义*/
typedef struct {
    uint16_t uwMsgType;
    uint8_t ucLightOnoff;
    uint8_t ucCurPosition;
} slSerialMsgAirerBaseGetStatusRsp_t;
#pragma pack()
/******************************************************************************
*                           Extern Declarations                               *
******************************************************************************/

/******************************************************************************
*                              Declarations                                   *
******************************************************************************/
// interface ctrl
int8_t rlAirerBaseLightOnoffCtrl(uint8_t status, uint32_t ulPeroidMs);
int8_t rlAirerBaseMotionfCtrl(uint8_t ucMotion);

//interface get
uint8_t rlAirerBaseGetLightOnoff(void);
uint8_t rlAirerBaseGetPosition(void);

//interface task
int slSerialAirerBaseInit(void);
void slSerialAirerBaseProc(void);

#endif //#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_AIRER_BASE)

#ifdef __cplusplus
}
#endif

#endif /*__SL_SERIAL_AIRER_BASE_H__*/

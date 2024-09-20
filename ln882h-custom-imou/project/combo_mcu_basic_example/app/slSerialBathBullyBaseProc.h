/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slSerialBathBullyBaseProc.h                                   *
* Description :                                                               *
*               smart lamp serial bathbully base process header file          *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-12-08                                                    *
* Mod History : (none)                                                        *
******************************************************************************/
#ifndef __SL_SERIAL_BATHBULLY_BASE_H__
#define __SL_SERIAL_BATHBULLY_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "myHal.h"
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_BATHBULLY_BASE)
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
    uint8_t ucLightMode;
    int8_t scTemperature;
    uint8_t ucDryOnoff;
    uint8_t ucFanOnoff;
    uint8_t ucVentilationOnoff;
    uint8_t ucWarmFanOnoff;
    uint8_t ucWarmFanGear;
} slSerialMsgBathBullyBaseStatusReportReq_t;

/*(0x0310)LAMP->GATEWAY:晾衣架状态查询响应消息结构定义*/
typedef struct {
    uint16_t uwMsgType;
    uint8_t ucLightOnoff;
    uint8_t ucLightMode;
    int8_t scTemperature;
    uint8_t ucDryOnoff;
    uint8_t ucFanOnoff;
    uint8_t ucVentilationOnoff;
    uint8_t ucWarmFanOnoff;
    uint8_t ucWarmFanGear;
} slSerialMsgBathBullyBaseGetStatusRsp_t;
#pragma pack()
/******************************************************************************
*                           Extern Declarations                               *
******************************************************************************/

/******************************************************************************
*                              Declarations                                   *
******************************************************************************/
// interface ctrl
int8_t rlBathBullyBaseLightOnoffCtrl(uint8_t status, uint32_t ulPeroidMs);
int8_t rlBathBullyBaseLightModeCtrl(uint8_t ucMode, uint32_t ulPeroidMs);
int8_t rlBathBullyBaseDryOnoffCtrl(uint8_t state);
int8_t rlBathBullyBaseFanOnoffCtrl(uint8_t state);
int8_t rlBathBullyBaseVentilationOnoffCtrl(uint8_t state);
int8_t rlBathBullyBaseWarmFanOnoffCtrl(uint8_t state);
int8_t rlBathBullyBaseWarmFanGearCtrl(uint8_t state);

//interface get
uint8_t rlBathBullyBaseGetLightOnoff(void);
uint8_t rlBathBullyBaseGetLightMode(void);
uint8_t rlBathBullyBaseGetDryOnoff(void);
uint8_t rlBathBullyBaseGetFanOnoff(void);
uint8_t rlBathBullyBaseGetVentilationOnoff(void);
uint8_t rlBathBullyBaseGetWarmFanOnoff(void);
uint8_t rlBathBullyBaseGetWarmFanGearOnoff(void);

//interface task
int slSerialBathBullyBaseInit(void);
void slSerialBathBullyBaseProc(void);

#endif //#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_BATHBULLY_BASE)

#ifdef __cplusplus
}
#endif

#endif /*__SL_SERIAL_BATHBULLY_BASE_H__*/

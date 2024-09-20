/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slTaskSerial.h                                                *
* Description :                                                               *
*               smart lamp task serial header file                            *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-11-07                                                    *
* Mod History : (none)                                                        *
******************************************************************************/
#ifndef __SL_TASK_SERIAL_H__
#define __SL_TASK_SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "myHal.h"
#if defined(APP_TASK_LAMP_SERIAL_USE) && (APP_TASK_LAMP_SERIAL_USE == 1)
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE) //床头灯
#define slSerialDeviceInit    slSerialLampBedsideInit
#define slSerialDeviceProc    slSerialLampBedsideProc
#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_AURORA) //氛围灯
#define slSerialDeviceInit    slSerialLampAuroraInit
#define slSerialDeviceProc    slSerialLampAuroraProc
#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_AIRER_BASE)  //晾衣架
#define slSerialDeviceInit    slSerialAirerBaseInit
#define slSerialDeviceProc    slSerialAirerBaseProc
#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_BATHBULLY_BASE)  //浴霸
#define slSerialDeviceInit    slSerialBathBullyBaseInit
#define slSerialDeviceProc    slSerialBathBullyBaseProc
#else
#define slSerialDeviceInit()
#define slSerialDeviceProc()
#endif
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
#pragma pack(1)
#pragma pack()
/******************************************************************************
*                           Extern Declarations                               *
******************************************************************************/

/******************************************************************************
*                              Declarations                                   *
******************************************************************************/
int slSerialMsgCtrl(uint8_t *pBuf, uint32_t ulLen, uint8_t repeatCnt);
int slSerialRecieve(uint8_t *pBuf, uint32_t ulLength);
uint32_t slSerialGetRecvLen(void);
uint32_t slSerialGetSendLen(void);
void slSerialClearSendBusy(void);

//interface task
void slTaskLampSerial(void *arg);

#endif //#if (APP_TASK_LAMP_SERIAL_USE)

#ifdef __cplusplus
}
#endif

#endif /*__SL_TASK_SERIAL_H__*/

/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slProfileProc.h                                               *
* Description :                                                               *
*               smart lamp  profile proc header file                          *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-11-28                                                    *
* Mod History : (none)                                                        *
******************************************************************************/
#ifndef __SL_PROFILE_PROC_H__
#define __SL_PROFILE_PROC_H__

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "myHal.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/

/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/

/******************************************************************************
*                           Extern Declarations                               *
******************************************************************************/

/******************************************************************************
*                              Declarations                                   *
******************************************************************************/
#if IS_LAMP_DEV_TYPE_USED()

int slpLampSetLightSwitchInt(const void *data, unsigned int len);
int slpLampGetLightSwitchInt(void **data, unsigned int *len);
int slpLampSetLightBrightnessInt(const void *data, unsigned int len);
int slpLampGetLightBrightnessInt(void **data, unsigned int *len);
int slpLampGetHumanSensorInt(void **data, unsigned int *len);
int slpLampSetAutoInt(void **data, unsigned int *len);
int slpLampGetAutoInt(void **data, unsigned int *len);
#if defined(APP_DEV_CCT_SUPPORT) && (APP_DEV_CCT_SUPPORT != 0)
int slpLampSetLightCctInt(const void *data, unsigned int len);
int slpLampGetLightCctInt(void **data, unsigned int *len);
#endif
int slpLampSetLightModeInt(const void *data, unsigned int len);
int slpLampGetLightModeInt(void **data, unsigned int *len);
int slpLampSetLightDurationInt(const void *data, unsigned int len);
int slpLampGetLightDurationInt(void **data, unsigned int *len);
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)
int slpLampSetLightPickupInt(const void *data, unsigned int len);
int slpLampGetLightPickupInt(void **data, unsigned int *len);
int slpLampSetLightSleepDurationInt(const void *data, unsigned int len);
int slpLampGetLightSleepDurationInt(void **data, unsigned int *len);
int slpLampGetLightBatteryInt(void **data, unsigned int *len);
#endif

#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_AIRER_BASE)

int slpAirerSetLightSwitchInt(const void *data, unsigned int len);
int slpAirerGetLightSwitchInt(void **data, unsigned int *len);
int slpAirerSetAirerOnoffInt(const void *data, unsigned int len);
int slpAirerGetAirerOnoffInt(void **data, unsigned int *len);
int slpAirerSetAirerMotionInt(const void *data, unsigned int len);
int slpAirerGetAirerMotionInt(void **data, unsigned int *len);
int slpAirerGetAirerPositionInt(void **data, unsigned int *len);

#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_BATHBULLY_BASE)

int slpBathBullySetLightSwitchInt(const void *data, unsigned int len);
int slpBathBullyGetLightSwitchInt(void **data, unsigned int *len);
int slpBathBullySetLightModeInt(const void *data, unsigned int len);
int slpBathBullyGetLightModeInt(void **data, unsigned int *len);
int slpBathBullySetDryOnoffInt(const void *data, unsigned int len);
int slpBathBullyGetDryOnoffInt(void **data, unsigned int *len);
int slpBathBullySetFanOnoffInt(const void *data, unsigned int len);
int slpBathBullyGetFanOnoffInt(void **data, unsigned int *len);
int slpBathBullySetVentilationOnoffInt(const void *data, unsigned int len);
int slpBathBullyGetVentilationOnoffInt(void **data, unsigned int *len);
int slpBathBullySetWarmFanOnoffInt(const void *data, unsigned int len);
int slpBathBullyGetWarmFanOnoffInt(void **data, unsigned int *len);
int slpBathBullySetWarmFanGearInt(const void *data, unsigned int len);
int slpBathBullyGetWarmFanGearInt(void **data, unsigned int *len);
int slpBathBullyGetTemperatureInt(void **data, unsigned int *len);

#endif

#ifdef __cplusplus
}
#endif

#endif /*__SL_PROFILE_PROC_H__*/




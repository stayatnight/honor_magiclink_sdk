/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slTaskLamp.h                                                  *
* Description :                                                               *
*               smart lamp task lamp header file                              *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-09-07                                                    *
* Mod History : (none)                                                        *
******************************************************************************/
#ifndef __SL_TASK_LAMP_H__
#define __SL_TASK_LAMP_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "myHal.h"

#if defined(APP_TASK_LAMP_USE) && (APP_TASK_LAMP_USE == 1)
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
//define blink arg type
#define RL_LAMP_BLINK_ARG_SYS_FACTORY_RESET                1
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
//#pragma pack(1)
typedef enum
{
    RL_LAMP_CTRL_TYPE_SUB,
    RL_LAMP_CTRL_TYPE_ADD,
    RL_LAMP_CTRL_TYPE_OUT,
    RL_LAMP_CTRL_TYPE_NUM
} rlLampCtrlType_t;

typedef enum
{
    RL_LAMP_BRI_CLASS_1,
    RL_LAMP_BRI_CLASS_2,
    RL_LAMP_BRI_CLASS_3,
    RL_LAMP_BRI_CLASS_4,
    RL_LAMP_BRI_CLASS_5,
    RL_LAMP_BRI_CLASS_6,
    RL_LAMP_BRI_CLASS_7,
    RL_LAMP_BRI_CLASS_8,
    RL_LAMP_BRI_CLASS_9,
    RL_LAMP_BRI_CLASS_10,
    RL_LAMP_BRI_CLASS_11,
    RL_LAMP_BRI_CLASS_12,
    RL_LAMP_BRI_CLASS_13,
    RL_LAMP_BRI_CLASS_14,
    RL_LAMP_BRI_CLASS_15,
    RL_LAMP_BRI_CLASS_16,
    RL_LAMP_BRI_CLASS_17,
    RL_LAMP_BRI_CLASS_18,
    RL_LAMP_BRI_CLASS_19,
    RL_LAMP_BRI_CLASS_20,
    RL_LAMP_BRI_CLASS_NUM,
} rlLampBriClass_t;

typedef enum
{
    RL_LAMP_LIGHT_MODE_VISITOR=0,
    RL_LAMP_LIGHT_MODE_LEISURE=1,
    RL_LAMP_LIGHT_MODE_MOVIE=2,
    RL_LAMP_LIGHT_MODE_DINNER=3,
    RL_LAMP_LIGHT_MODE_WORK=6,
    RL_LAMP_LIGHT_MODE_SLEEP=7,
    RL_LAMP_LIGHT_MODE_READ=8,
    RL_LAMP_LIGHT_MODE_NIGHT=100,
    RL_LAMP_LIGHT_MODE_BEFORE_SLEEP=101,
    RL_LAMP_LIGHT_MODE_WRITE=200,
} rlLampLightMode_t;

typedef enum
{
    CL_LAMP_LIGHT_MODE_GUEST=0,
    CL_LAMP_LIGHT_MODE_RELAX=1,
    CL_LAMP_LIGHT_MODE_VIEWING=2,
    CL_LAMP_LIGHT_MODE_DINNER=3,
    CL_LAMP_LIGHT_MODE_WORKING=6,
    CL_LAMP_LIGHT_MODE_SLEEPING=7,
    CL_LAMP_LIGHT_MODE_READING=8,
    CL_LAMP_LIGHT_MODE_NIGHTLIGHT=100,
    CL_LAMP_LIGHT_MODE_BEDTIME=101,
    CL_LAMP_LIGHT_MODE_BOTHSIDE = 102,
    CL_LAMP_LIGHT_MODE_WRITEING=200,
} clLampLightMode_t;

typedef enum {
    RL_LAMP_WALL_SWITCH_MODE_1,
    RL_LAMP_WALL_SWITCH_MODE_2,
    RL_LAMP_WALL_SWITCH_MODE_3,
    RL_LAMP_WALL_SWITCH_MODE_NUM
} slLampWallSwitchMode_t;
//#pragma pack()
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
void rlTaskLamp(void *arg);

#endif //APP_TASK_LAMP_USE

#ifdef __cplusplus
}
#endif

#endif /*__SL_TASK_LAMP_H__*/

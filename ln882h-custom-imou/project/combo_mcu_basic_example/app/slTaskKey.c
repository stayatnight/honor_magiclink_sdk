/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slTaskKey.c                                                   *
* Description :                                                               *
*               smart lamp task Key source file                               *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-09-07                                                    *
* Mod History : (none)                                                        *
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "slTaskKey.h"
#if defined(APP_TASK_KEY_USE) && (APP_TASK_KEY_USE == 1)
#include "myKeyboard.h"
#include "slData.h"
#include "slTaskLamp.h"
#include "atShell.h"
#include "timers.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/

/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/

/******************************************************************************
*                                Globals                                      *
******************************************************************************/
const halIO_t s_a_normalKeyHalIo[] = APP_LIGHT_NORMAL_KEY_IO;
static uint32_t s_a_normalKeyVal[] = APP_LIGHT_NORMAL_KEY_VAL;
extern uint8_t g_ke_press;

static xTimerHandle factory_resetHandle = NULL;
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
 Function    : _normalKeyShortPressCb
 Description : KEY short press process
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _normalKeyShortPressCb(uint32_t keyVal, uint32_t flag)
{
    switch (keyVal) {
    case LIGHT_KEY_VAL_SWITCH:
        my_hal_log_debug("key switch sp %d\r\n", flag);
        if (g_stRlData.fctData.fctMode != 0) {
            Printf("press key switch\r\n");
        }
        break;
    default:
        break;
    }
}

/******************************************************************************
 Function    : _normalKeyShortReleaseCb
 Description : KEY short release process
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _normalKeyShortReleaseCb(uint32_t keyVal, uint32_t flag)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;
     
    switch (keyVal) {
    case LIGHT_KEY_VAL_SWITCH:
        printf("key switch sr %d\r\n", flag);
        if (g_stRlData.fctData.fctMode != 0) {
            printf("release key switch\r\n");
        }
        #if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_NIGHT_PTJX)
        rlLampSwitchRevert(0);
        g_ke_press = 1;
        sample_mutex_set(1);
        #else
        rlLampSwitchRevert(pLiveData->uwAdjDuration);
        #endif
        break;
    default:
        break;
    }
}

/******************************************************************************
 Function    : _normalKeyLongPressCb
 Description : KEY long press process
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _normalKeyLongPressCb(uint32_t keyVal, uint32_t flag)
{
    switch (keyVal) {
    case LIGHT_KEY_VAL_SWITCH:
        printf("key switch lp %d tick %lu\r\n", flag, xTaskGetTickCount());
        #if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT  || APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_NIGHT_PTJX)
        if (flag == 12) {
           rlLampBlinkCtrl(3, 1000, 1, RL_LAMP_BLINK_ARG_SYS_FACTORY_RESET); 
        }
        #else
        if (0 != rlLampGetOnoff()) {
            if (flag == 0) {
                rlFlagRevert(RL_FLAG_LAMP_BRI_DIRECTION);
            }
            rlLampBriCtrlNextClass(200);
        }
        else if (flag == 25 && rlFlagGet(RL_FLAG_SYS_FACTORY_WINDOW)){
            rlFlagSet(RL_FLAG_SYS_FACTORY_RESET, 1);
            my_hal_log_info("manual factory reset\r\n");
        }
        #endif
        break;
    default:
        break;
    }
}

/******************************************************************************
 Function    : _normalKeyLongReleaseCb
 Description : KEY long press process
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _normalKeyLongReleaseCb(uint32_t keyVal, uint32_t flag)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    switch (keyVal) {
    case LIGHT_KEY_VAL_SWITCH:
        my_hal_log_debug("key switch lr %d\r\n" );
        if (g_stRlData.fctData.fctMode != 0) {
            Printf("release key switch\r\n");
        }
        if (0 == rlLampGetOnoff() && flag < 25) {
            rlLampSwitchRevert(pLiveData->uwAdjDuration);
        }
        break;
    default:
        break;
    }
}

/******************************************************************************
 Function    : _normalKeyboardKeyStatusGet
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static uint32_t _normalKeyboardKeyStatusGet(uint32_t keyValue)
{
    halIOStatus_t status = HAL_IO_STATUS_LOW;

    for (int i = 0; i < sizeof(s_a_normalKeyHalIo) / sizeof(halIO_t); i++) {
        if (keyValue == s_a_normalKeyVal[i]) {
            halIOGetStatus(s_a_normalKeyHalIo[i], &status);
            break;
        }
    }

    return (uint32_t)!status;
}

/******************************************************************************
 Function    : _rlKeyGetTickMs
 Description : rl KEY get tick ms
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint32_t _rlKeyGetTickMs() 
{
    return xTaskGetTickCount();
}

/******************************************************************************
 Function    : _rlTaskKeyInit
 Description : rl KEY Proc模块初始化
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static int _rlTaskKeyInit(void) 
{
    for (int i = 0; i < sizeof(s_a_normalKeyHalIo) / sizeof(halIO_t); i++) {
        #if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_NIGHT_PTJX)
        halIOInit(s_a_normalKeyHalIo[i], HAL_IO_MODE_IN_PULLUP, HAL_IO_IRQ_NULL, NULL);
        #else
        halIOInit(s_a_normalKeyHalIo[i], HAL_IO_MODE_IN_PULLDOWN, HAL_IO_IRQ_NULL, NULL);
        #endif
    }

    myKeyboardInit(10, 3, 1000, 200, 100, _rlKeyGetTickMs, _normalKeyboardKeyStatusGet);
    myKeyboardRigisterCallback(_normalKeyShortPressCb,
                                _normalKeyShortReleaseCb,
                                _normalKeyLongPressCb,
                                _normalKeyLongReleaseCb);

    for (int i = 0; i < sizeof(s_a_normalKeyVal) / sizeof(uint32_t); i++) {
        myKeyboardRegisterKey(s_a_normalKeyVal[i]);
    }
    my_hal_log_info("task 'key' init ok\r\n");
    return 0;
}

/******************************************************************************
 Function    : rlTaskKey
 Description : reading lamp task key
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void rlTaskKey(void* arg)
{
    // rlFlagSet(RL_FLAG_TASK_KEY_RUNNING, 1);
    // if(0 > _rlTaskKeyInit()) {
    //     rlFlagSet(RL_FLAG_TASK_KEY_RUNNING, 0);
    // }

//     while(1) {
//    //     vTaskDelay(10);
//      //   myKeyboardLoop();
//     }

//  //   my_hal_log_info("task 'key' delete\r\n");
//  //   vTaskDelete(NULL);
}

#endif //#if (APP_TASK_KEY_USE)

#ifdef __cplusplus
}
#endif


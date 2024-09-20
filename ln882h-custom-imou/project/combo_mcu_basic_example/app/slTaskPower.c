/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slTaskPower.c                                                 *
* Description :                                                               *
*               smart lamp task power source file                             *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-10-08                                                    *
* Mod History : (none)                                                        *
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "slTaskPower.h"
#include "slData.h"
#include "slTaskLamp.h"
#include "timers.h"
#include "semphr.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define APP_LIGHT_POWER_IO                                  HAL_IO_PA17
#define SL_POWER_START_DELAY                                0
#define SL_POWER_DOWN_FILTER_DURATION                       100 //ms

#define SL_POWER_LEVEL_OFF_LEVEL                            HAL_IO_STATUS_LOW
#define SL_POWER_LEVEL_FILTER_CNT                           3
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
typedef struct
{
    uint8_t powerState;
    uint8_t powerIrqFlag;
    uint32_t startDelayMs;
    uint32_t pulseSilentDuration;
    xTimerHandle wallSwitchCntClearTimerHandle;
    xSemaphoreHandle powerStateCallbackMutex;
} slPowerInfo_t;
/******************************************************************************
*                                Globals                                      *
******************************************************************************/
static slPowerInfo_t s_slPowerInfo;
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
 Function    : _powerIrqFlagSet
 Description : sl power change callback
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _powerIrqFlagSet(uint8_t state) 
{
    halIOIrqDisable(APP_LIGHT_POWER_IO);
    s_slPowerInfo.powerIrqFlag = state;
    halIOIrqEnable(APP_LIGHT_POWER_IO);
}
/******************************************************************************
 Function    : _powerStateChangeCallback
 Description : power change callback
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _powerStateChangeCallback(uint8_t status) 
{  
    if (s_slPowerInfo.powerState == status) {
        return;
    }
    s_slPowerInfo.powerState = status;
    my_hal_log_info("power state change callback %d\r\n", status); 
    if (status == 0) {
        rlLampSwitchCtrl(0, 0);
        xTimerStop(s_slPowerInfo.wallSwitchCntClearTimerHandle, 0);
//      rlDataWriteConfigData();
    }
    else {
        xTimerStart(s_slPowerInfo.wallSwitchCntClearTimerHandle, 0);
        slLampPowerOnHook();
        if (SL_POWER_FACTORY_RESET_WALL_SW_OPER_CNT <= ++g_stRlData.saveData.ucWallSwitchOperCnt) {
            rlFlagSet(RL_FLAG_SYS_FACTORY_RESET, 1);
            my_hal_log_info("power wall switch set factory reset\r\n");
            return;
        }
        my_hal_log_info("power wall switch operate count is %d\r\n", g_stRlData.saveData.ucWallSwitchOperCnt);
        rlFlagSet(RL_FLAG_SYS_CFG_DATA_SAVE, 1);
    }
}

/******************************************************************************
 Function    : _slPowerLevelCheck
 Description : sl level power down check
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _slPowerLevelCheck(uint32_t interval) 
{
    halIOStatus_t status = HAL_IO_STATUS_LOW;
    static halIOStatus_t lState = HAL_IO_STATUS_LOW;
    static int ucFilterCnt = SL_POWER_LEVEL_FILTER_CNT;

    s_slPowerInfo.startDelayMs = 
        (s_slPowerInfo.startDelayMs > interval) ? 
        s_slPowerInfo.startDelayMs - interval : 0;

    halIOGetStatus(APP_LIGHT_POWER_IO, &status);
    if (status != lState) {
        lState = status;
        ucFilterCnt = SL_POWER_LEVEL_FILTER_CNT;
    }
    else if (ucFilterCnt > 0){
        if (--ucFilterCnt <= 0) {
            uint8_t newState = (status == SL_POWER_LEVEL_OFF_LEVEL ? 0 : 1);
            
            if (s_slPowerInfo.powerState != newState) {
                s_slPowerInfo.powerState = newState;
                my_hal_log_debug("power state change to %d\r\n", s_slPowerInfo.powerState);
                if (s_slPowerInfo.startDelayMs == 0) {
                    _powerStateChangeCallback(s_slPowerInfo.powerState);
                }
            }            
        }
    }
}

/******************************************************************************
 Function    : _slPowerPulseCheck
 Description : pulse power down check
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _slPowerPulseCheck(uint32_t interval) 
{
    static uint32_t lTick = 0;

    if (s_slPowerInfo.powerIrqFlag) {
        _powerIrqFlagSet(0);
        s_slPowerInfo.pulseSilentDuration = 0;
        lTick = xTaskGetTickCount();
        if (s_slPowerInfo.powerState == 0) {
            _powerStateChangeCallback(1);
        }
    }
    else {
        uint32_t cTick = xTaskGetTickCount();

        s_slPowerInfo.pulseSilentDuration += cTick - lTick;
        lTick = cTick;
        if (s_slPowerInfo.pulseSilentDuration >= SL_POWER_DOWN_FILTER_DURATION) {
            s_slPowerInfo.pulseSilentDuration = 0;
            if (s_slPowerInfo.powerState != 0) {
                _powerStateChangeCallback(0);
            }
        }
    }
}

/******************************************************************************
 Function    : _powerIoIrqHandle
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _powerIoIrqHandle(uint32_t id, halIOIrqEvent_t event) 
{
    s_slPowerInfo.powerIrqFlag = 1;
}

/******************************************************************************
 Function    : _powerWallSwitchCntClearTimeoutHandle
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _powerWallSwitchCntClearTimeoutHandle(xTimerHandle pxTimer) 
{
    my_hal_log_info("clear wall switch operate count\r\n");
    g_stRlData.saveData.ucWallSwitchOperCnt = 0;
    rlFlagSet(RL_FLAG_SYS_CFG_DATA_SAVE, 1);
}

/******************************************************************************
 Function    : _slTaskPowerInit
 Description : sl power Proc模块初始化
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t slPowerGetStatus(void) 
{
    return s_slPowerInfo.powerState;
}

/******************************************************************************
 Function    : slPowerPulseContrlSim
 Description : sl power pulse control simulation
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slPowerPulseContrlSim(uint8_t en) 
{
#if defined(APP_DEV_PWR_TYPE_USED) && (APP_DEV_PWR_TYPE_USED & APP_DEV_PWR_TYPE_IO_PULSE != 0)
    if (en) {
        halIOIrqEnable(APP_LIGHT_POWER_IO);
    }
    else {
        halIOIrqDisable(APP_LIGHT_POWER_IO);
        s_slPowerInfo.powerIrqFlag = 0;
    }
#endif
}

/******************************************************************************
 Function    : _slTaskPowerInit
 Description : sl power Proc模块初始化
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static int _slTaskPowerInit(void) 
{
    vTaskDelay(50);

#if defined(APP_DEV_PWR_TYPE_USED) && (APP_DEV_PWR_TYPE_USED & APP_DEV_PWR_TYPE_IO_PULSE)
    halIOInit(APP_LIGHT_POWER_IO, HAL_IO_MODE_IN_NOPULL, HAL_IO_IRQ_FALLING, _powerIoIrqHandle);
#endif
    s_slPowerInfo.powerState = 0;
    s_slPowerInfo.startDelayMs = SL_POWER_START_DELAY;
    s_slPowerInfo.wallSwitchCntClearTimerHandle = 
        xTimerCreate((const char *)"power", 
                     (SL_POWER_WALL_SWITCH_CNT_CLEAR_TIMEOUT / portTICK_RATE_MS), 
                     0, 
                     NULL, 
                     _powerWallSwitchCntClearTimeoutHandle);
    my_hal_log_info("task 'power' init ok\r\n");
    return 0;
}

/******************************************************************************
 Function    : slTaskPower
 Description : smart lamp task power
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slTaskPower(void* arg)
{
    rlFlagSet(RL_FLAG_TASK_PWR_RUNNING, 1);
    if(0 > _slTaskPowerInit()) {
        rlFlagSet(RL_FLAG_TASK_PWR_RUNNING, 0);
    }

#if defined(APP_DEV_PWR_TYPE_USED) && (APP_DEV_PWR_TYPE_USED & APP_DEV_PWR_TYPE_ONOFF != 0)
    _powerStateChangeCallback(1);
#endif

    while(rlFlagGet(RL_FLAG_TASK_PWR_RUNNING)) {
        vTaskDelay(10);
#if defined(APP_DEV_PWR_TYPE_USED) && (APP_DEV_PWR_TYPE_USED & APP_DEV_PWR_TYPE_IO_PULSE != 0)
        _slPowerPulseCheck(10);
#elif defined(APP_DEV_PWR_TYPE_USED) && (APP_DEV_PWR_TYPE_USED & APP_DEV_PWR_TYPE_IO_LEVEL != 0)
        _slPowerLevelCheck(10);
#else
        rlFlagSet(RL_FLAG_TASK_PWR_RUNNING, 0);
#endif
    }

    my_hal_log_info("task 'power' delete\r\n");
    vTaskDelete(NULL);
}

#ifdef __cplusplus
}
#endif


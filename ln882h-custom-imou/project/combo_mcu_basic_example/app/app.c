/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : app.c                                                         *
* Description :                                                               *
*               app应用主处理任务源文件                                         *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-09-06                                                    *
* Mod History : (none)                                                        *
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "app.h"
#include "slData.h"
#include "slTaskLamp.h"
#include "slTaskSerial.h"
#include "slTaskKey.h"
#if defined(APP_DEV_PWR_TYPE_USED)
#include "slTaskPower.h"
#endif
#include "slTaskFCT.h"
#include "magiclink.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define RL_TASK_INIT_STACK_SIZE   1024
#define RL_TASK_INIT_PRIORITY     tskIDLE_PRIORITY + 4 + PRIORITIE_OFFSET

#if defined(APP_TASK_LAMP_USE) && (APP_TASK_LAMP_USE == 1)
#define RL_TASK_LAMP_STACK_SIZE   2048
#define RL_TASK_LAMP_PRIORITY     tskIDLE_PRIORITY + 4 + PRIORITIE_OFFSET
#endif

#if defined(APP_TASK_LAMP_SERIAL_USE) && (APP_TASK_LAMP_SERIAL_USE == 1)
#define RL_TASK_LAMP_SERIAL_STACK_SIZE   2048
#define RL_TASK_LAMP_SERIAL_PRIORITY     tskIDLE_PRIORITY + 4 + PRIORITIE_OFFSET
#endif

#if defined(APP_TASK_KEY_USE) && (APP_TASK_KEY_USE == 1)
#define RL_TASK_KEY_STACK_SIZE    1024
#define RL_TASK_KEY_PRIORITY      tskIDLE_PRIORITY + 6 + PRIORITIE_OFFSET
#endif

#if defined(APP_TASK_FCT_USE) && (APP_TASK_FCT_USE == 1)
#define RL_TASK_FCT_STACK_SIZE    1024
#define RL_TASK_FCT_PRIORITY      tskIDLE_PRIORITY + 2 + PRIORITIE_OFFSET
#endif

#if defined(APP_DEV_PWR_TYPE_USED)
#define RL_TASK_PWR_STACK_SIZE    1024
#define RL_TASK_PWR_PRIORITY      tskIDLE_PRIORITY + 4 + PRIORITIE_OFFSET
#endif

#define MY_SSID "zhiyi"
#define MY_KEY  "zhiyi2024"
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/

/******************************************************************************
*                             Declarations                                    *
******************************************************************************/

/******************************************************************************
*                                Globals                                      *
******************************************************************************/

/******************************************************************************
*                          Extern Declarations                                *
******************************************************************************/

/******************************************************************************
*                            Implementations                                  *
******************************************************************************/
/******************************************************************************
 Function    : appstartTask
 Description : app task start
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void appstartTask(void *arg) 
{
    my_hal_log_info("\r\napp task entry\r\n");

#if defined(APP_TASK_LAMP_USE) && (APP_TASK_LAMP_USE == 1)
    if(xTaskCreate(rlTaskLamp, ((const char*)"lamp"), RL_TASK_LAMP_STACK_SIZE, NULL,
        RL_TASK_LAMP_PRIORITY, NULL) != pdPASS) {
        my_hal_log_error("create lamp task fail\r\n");
    }
#endif

#if defined(APP_TASK_LAMP_SERIAL_USE) && (APP_TASK_LAMP_SERIAL_USE == 1)
    if(xTaskCreate(slTaskLampSerial, ((const char*)"serial"), RL_TASK_LAMP_SERIAL_STACK_SIZE, NULL,
        RL_TASK_LAMP_SERIAL_PRIORITY, NULL) != pdPASS) {
        my_hal_log_error("create lamp serial task fail\r\n");
    }
#endif

#if defined(APP_TASK_KEY_USE) && (APP_TASK_KEY_USE == 1)
    if(xTaskCreate(rlTaskKey, ((const char*)"key"), RL_TASK_KEY_STACK_SIZE, NULL,
        RL_TASK_KEY_PRIORITY, NULL) != pdPASS) {
        my_hal_log_error("create key task fail\r\n");
    }
#endif

//#if defined(APP_TASK_FCT_USE) && (APP_TASK_FCT_USE == 1)
#if (0)
    if(xTaskCreate(rlTaskFct, ((const char *)"fct"), RL_TASK_FCT_STACK_SIZE, NULL,
        RL_TASK_FCT_PRIORITY, NULL) != pdPASS) {
        my_hal_log_error("create fct task fail\r\n");
    }
#endif

#if defined(APP_DEV_PWR_TYPE_USED)
    if(xTaskCreate(slTaskPower, ((const char *)"pwr"), RL_TASK_PWR_STACK_SIZE, NULL,
        RL_TASK_PWR_PRIORITY, NULL) != pdPASS) {
        my_hal_log_error("create pwr task fail\r\n");
    }
#endif

#if defined(APP_MAGIC_LINK_USE) && (APP_MAGIC_LINK_USE != 0)
    if (g_stRlData.fctData.fctMode == 0) {
        MagicLinkTaskEntry();
     }
#endif

    vTaskDelete(NULL);
}
/******************************************************************************
 Function    : appTaskEntry
 Description : app task entry
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void appTaskEntry(void)
{
    if(xTaskCreate(appstartTask, ((const char*)"init"), RL_TASK_INIT_STACK_SIZE, NULL,
        RL_TASK_INIT_PRIORITY, NULL) != pdPASS) {
        my_hal_log_error("create init task fail\r\n");
    }
}

#ifdef __cplusplus
}
#endif


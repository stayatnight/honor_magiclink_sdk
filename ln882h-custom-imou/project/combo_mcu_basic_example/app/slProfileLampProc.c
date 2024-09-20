/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slProfileProc.c                                               *
* Description :                                                               *
*               smart lamp profile proc source file                           *
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
#include "slProfileProc.h"
#if IS_LAMP_DEV_TYPE_USED()
#include "slData.h"
#include "slTaskLamp.h"
#include "slTaskSerial.h"
#include "atShell.h"
#include "timers.h"
#include "magiclink.h"
#include "semphr.h"

/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define LIGHT_SENSE_IO                                            HAL_IO_PA19
#define HUMAN_SENSE_IO                                            HAL_IO_PA17
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/

/******************************************************************************
*                                Globals                                      *
******************************************************************************/
static struct MagicLinkTestLight {
    int on;
    int lightMode;
    int brightness;
#if defined(APP_DEV_CCT_SUPPORT) && (APP_DEV_CCT_SUPPORT != 0)
    int colorTemperature;
#endif
    int duration;
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)
    int position;
    int sleepDuration;
    int batteryPercent;
#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_NIGHT_PTJX)    
    int hs_state;
    int lamp_auto;
#endif
} g_light = {
    .on = 0,
    #if APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_NIGHT_PTJX
    .lightMode = CL_LAMP_LIGHT_MODE_BOTHSIDE,
    #else
    .lightMode = 1,
    #endif
    .brightness = 65535,
#if defined(APP_DEV_CCT_SUPPORT) && (APP_DEV_CCT_SUPPORT != 0)
    .colorTemperature = 4350,
#endif
    .duration = 1,
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)
    .position = 0,
    .sleepDuration = 0,
    .batteryPercent = 0,
#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_NIGHT_PTJX)
    .hs_state = 0,
    .lamp_auto = 1,
#endif
};
/******************************************************************************
*                          Extern Declarations                                *
******************************************************************************/
uint8_t g_ke_press;
/******************************************************************************
*                             Declarations                                    *
******************************************************************************/
// xTimerHandle s_LampCtrlHandle = NULL;
static int lampctrl_cnt;
static volatile int sample_mutex, mutex_delay;
extern SemaphoreHandle_t nlSetMutex;
/******************************************************************************
*                            Implementations                                  *
******************************************************************************/
static int myMagicEvnetEmit(uint8_t status)
{
    int ret = 0;
    struct MagicLinkDataVal eventData[1] = {{0, sizeof(int), NULL}};

    eventData[0].data = (unsigned char *)&g_light.hs_state;
    if ((ret = MagicLinkReportServiceEvent("IRF", "humanPresence", 1, eventData)) != 0) {
        printf("magic event emit status: %d\r\n", ret);
    }
    printf("event emit: %d [%d, %s]\r\n",ret, __LINE__, __FUNCTION__);
    return ret;
}

void lampctrl_cnt_set(int val)
{
    lampctrl_cnt = val;
}

#if 1
void sample_mutex_set(int val)
{
    xSemaphoreTake(nlSetMutex, portMAX_DELAY);
    sample_mutex = val;
    mutex_delay = 0;
    xSemaphoreGive(nlSetMutex);
    printf("mutex: %d [%d, %s]\r\n", sample_mutex, __LINE__, __FUNCTION__);
}
#endif

void _lampctrlfunc(xTimerHandle pxTimer) 
{
    if ((lampctrl_cnt --) <= 0 && g_light.lamp_auto) {
        rlLampSwitchCtrl(0, 0);
        lampctrl_cnt_set(3200);
        // printf("turn off the lamp time out\r\n");
    }
}

/******************************************************************************
 Function    : slpLampSetLightSwitchInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampSetLightSwitchInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_light.on = *(int *)data > 0 ? 1 : 0;
    #if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_NIGHT_PTJX)
    g_ke_press = 1;
    rlLampSwitchCtrl((uint8_t)g_light.on, 0);
    #else
    rlLampSwitchCtrl((uint8_t)g_light.on, pLiveData->uwAdjDuration);
    #endif
    printf("set light switch %d\r\n", g_light.on);
    return 0;
}

/******************************************************************************
 Function    : slpLampGetLightSwitchInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampGetLightSwitchInt(void **data, unsigned int *len)
{
    *len = sizeof(g_light.on);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_light.on, *len);

    printf("get light switch %d \r\n", g_light.on);
    return 0;
}

int slpLampSetAutoInt(void **data, unsigned int *len)
{
    g_light.lamp_auto = *(int *)data;
    printf("set lamp auto %d \r\n", g_light.lamp_auto);
    return 0;
}

int slpLampGetAutoInt(void **data, unsigned int *len)
{
    *len = sizeof(g_light.lamp_auto);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;       
    }
    (void)memset(*data, 0, *len);

    (void)memcpy(*data, &g_light.lamp_auto, *len);

    printf("get lamp auto %d \r\n", g_light.lamp_auto);
    return 0;
}

int slpLampGetAuto(void)
{
    return g_light.lamp_auto;
}

/******************************************************************************
 Function    : slpLampSetLightBrightnessInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampSetLightBrightnessInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_light.brightness = *(int *)data;

    printf("set light brightness %d\r\n", g_light.brightness);
    rlLampBriPercentCtrl((uint16_t)g_light.brightness, pLiveData->uwAdjDuration);
    
    return 0;
}

/******************************************************************************
 Function    : slpLampGetLightBrightnessInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampGetLightBrightnessInt(void **data, unsigned int *len)
{
    *len = sizeof(g_light.brightness);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }
    (void)memset(*data, 0, *len);

    (void)memcpy(*data, &g_light.brightness, *len);

    printf("get light brightness %d\r\n", g_light.brightness);
    return 0;
}

/******************************************************************************
 Function    : slpLampGetHumanSensorInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampGetHumanSensorInt(void **data, unsigned int *len)
{
    *len = sizeof(g_light.hs_state);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;       
    }
    (void)memset(*data, 0, *len);

    (void)memcpy(*data, &g_light.hs_state, *len);

    return 0;
}

void slpLampSetHumanSensorInt(const void *data, unsigned int len)
{
    g_light.hs_state = *(int *)data;
    rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
}

#if defined(APP_DEV_CCT_SUPPORT) && (APP_DEV_CCT_SUPPORT != 0)
/******************************************************************************
 Function    : slpLampSetLightCctInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampSetLightCctInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_light.colorTemperature = *(int *)data;

    printf("set light cct %d\r\n", g_light.colorTemperature);
    rlLampCctCtrl((uint16_t)g_light.colorTemperature, pLiveData->uwAdjDuration);   

    return 0;
}

/******************************************************************************
 Function    : slpLampGetLightCctInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampGetLightCctInt(void **data, unsigned int *len)
{
    *len = sizeof(g_light.colorTemperature);

   *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err\r\n");
        return -1;
    }
    (void)memset(*data, 0, *len);

    (void)memcpy(*data, &g_light.colorTemperature, *len);

    printf("get light cct %d\r\n", g_light.colorTemperature);
    return 0;
}
#endif //#if defined(APP_DEV_CCT_SUPPORT) && (APP_DEV_CCT_SUPPORT != 0)

/******************************************************************************
 Function    : slpLampSetLightModeInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampSetLightModeInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_light.lightMode = *(int *)data;
    printf("set light mode %d\r\n", g_light.lightMode);
    #if APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_NIGHT_PTJX
    g_ke_press = 1;
    rlLampLightModeCtrl((uint8_t)g_light.lightMode, 0);
    #else
    rlLampLightModeCtrl((uint8_t)g_light.lightMode, pLiveData->uwAdjDuration);
    #endif

    return 0;
}

/******************************************************************************
 Function    : slpLampGetLightModeInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampGetLightModeInt(void **data, unsigned int *len)
{
    *len = sizeof(g_light.lightMode);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }
    (void)memset(*data, 0, *len);

    (void)memcpy(*data, &g_light.lightMode, *len);

    printf("get light mode %d\r\n", g_light.lightMode);
    return 0;
}

int slLampGetLightMode(void)
{
    return g_light.lightMode;
}

/******************************************************************************
 Function    : slpLampSetLightDurationInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampSetLightDurationInt(const void *data, unsigned int len)
{
    g_light.duration = *(int *)data;

    rlDataSetAdjDuration((eRlAdjDuation_t)g_light.duration);
    printf("set light duration %d\r\n", g_light.duration);

    return 0;
}

/******************************************************************************
 Function    : slpLampGetLightDurationInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampGetLightDurationInt(void **data, unsigned int *len)
{
    g_light.duration = (int)rlDataGetAdjDuration();
    *len = sizeof(g_light.duration);
    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err\r\n");
        return -1;
    }
    (void)memset(*data, 0, *len);

    (void)memcpy(*data, &g_light.duration, *len);

    printf("get light duration %d\r\n", g_light.duration);
    return 0;
}

#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)
/******************************************************************************
 Function    : slpLampSetLightPickupInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampSetLightPickupInt(const void *data, unsigned int len)
{
    printf("set light position\r\n");
    return 0;
}

/******************************************************************************
 Function    : slpLampGetLightPickupInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampGetLightPickupInt(void **data, unsigned int *len)
{
    *len = sizeof(g_light.position);
    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err\r\n");
        return -1;
    }
    (void)memset(*data, 0, *len);

    (void)memcpy(*data, &g_light.position, *len);

    printf("get light position %d\r\n", g_light.position);
    return 0;
}

/******************************************************************************
 Function    : slpLampSetLightSleepDurationInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampSetLightSleepDurationInt(const void *data, unsigned int len)
{
    g_light.sleepDuration = *(int *)data;

    rlDataSetSleepDuration((eSlSleepDuration_t)g_light.sleepDuration);
    printf("set light sleep duration %d\r\n", g_light.sleepDuration);

    return 0;
}

/******************************************************************************
 Function    : slpLampGetLightSleepDurationInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampGetLightSleepDurationInt(void **data, unsigned int *len)
{
    g_light.sleepDuration = (int)rlDataGetSleepDuration();
    *len = sizeof(g_light.sleepDuration);
    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err\r\n");
        return -1;
    }
    (void)memset(*data, 0, *len);

    (void)memcpy(*data, &g_light.sleepDuration, *len);

    printf("get light sleep duration %d\r\n", g_light.sleepDuration);
    return 0;
}

/******************************************************************************
 Function    : slpLampGetLightBatteryInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpLampGetLightBatteryInt(void **data, unsigned int *len)
{
    *len = sizeof(g_light.batteryPercent);
    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err\r\n");
        return -1;
    }
    (void)memset(*data, 0, *len);

    (void)memcpy(*data, &g_light.batteryPercent, *len);

    printf("get light battery %d\r\n", g_light.batteryPercent);
    return 0;
}
#endif //#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)

void MagicLinkDataRsync(void) 
{
    int tmp = 0;

    tmp = (int)rlLampGetOnoff();
    if (tmp != g_light.on) {
        g_light.on = tmp;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
        rlFlagSet(RL_FLAG_MAGIC_REPORT_LIGHT_NEED, 1);
    }

    tmp = (int)rlLampGetBriPercent();
    tmp = APP_MAX_VAL(1, tmp);
    if (tmp != g_light.brightness) {
        g_light.brightness = tmp;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
        rlFlagSet(RL_FLAG_MAGIC_REPORT_LIGHT_NEED, 1);
    }

#if defined(APP_DEV_CCT_SUPPORT) && (APP_DEV_CCT_SUPPORT != 0)
    tmp = (int)rlLampGetCCT();
    if (tmp != g_light.colorTemperature) {
        g_light.colorTemperature = tmp;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
        rlFlagSet(RL_FLAG_MAGIC_REPORT_LIGHT_NEED, 1);
    }
#endif

    tmp = (int)rlLampGetLightMode();
    if (tmp != g_light.lightMode) {
        g_light.lightMode = tmp;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
        rlFlagSet(RL_FLAG_MAGIC_REPORT_LIGHT_NEED, 1);
    }

#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)
    if (g_light.batteryPercent != (int)g_stRlData.liveData.ucBatteryValue) {
        g_light.batteryPercent = (int)g_stRlData.liveData.ucBatteryValue;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
        rlFlagSet(RL_FLAG_MAGIC_REPORT_BAT_NEED, 1);
    }
    tmp = (int)rlFlagGet(RL_FLAG_POS_PICKUP);
    if (g_light.position != tmp) {
        g_light.position = tmp;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
        rlFlagSet(RL_FLAG_MAGIC_REPORT_POS_NEED, 1);
    }
#endif
}

void MagicLinkDataReport(void) 
{
    if (rlFlagGet(RL_FLAG_MAGIC_REPORT_LIGHT_NEED)) {
        rlFlagSet(RL_FLAG_MAGIC_REPORT_LIGHT_NEED, 0);
#if defined(APP_DEV_CCT_SUPPORT) && (APP_DEV_CCT_SUPPORT != 0)
        printf("%d magic report onoff %d bri %d cct %d\r\n", xTaskGetTickCount(), g_light.on, g_light.brightness, g_light.colorTemperature);
#else
        printf("%d magic report onoff %d bri %d\r\n", xTaskGetTickCount(), g_light.on, g_light.brightness);
#endif
        MagicLinkReportServiceStatus("light");
    }

#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)
    if (rlFlagGet(RL_FLAG_MAGIC_REPORT_BAT_NEED)) {
        rlFlagSet(RL_FLAG_MAGIC_REPORT_BAT_NEED, 0);
        printf("%d magic report battery %d\r\n", xTaskGetTickCount(), g_light.batteryPercent);
        MagicLinkReportServiceStatus("battery");
    }

    if (rlFlagGet(RL_FLAG_MAGIC_REPORT_POS_NEED)) {
        rlFlagSet(RL_FLAG_MAGIC_REPORT_POS_NEED, 0);
        printf("%d magic report position %d\r\n", xTaskGetTickCount(), g_light.position);
        MagicLinkReportServiceStatus("position");
    }
#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_NIGHT_PTJX)
    MagicLinkReportServiceStatus("nightLight");
#endif
}

static uint8_t inter_lum[100];
void night_lamp_human_state_check(void)
{
    int onoff = 1, lum_ava = 0; 
    static int last_lum_ava = 2, old_lum_ava = 1;
    int human_motive = 1;
    static int last_human_motive = -1;
    int status = HAL_IO_STATUS_LOW;
    static uint32_t startup = 0;
    static uint32_t inter_no_hum_timer = 0, inter_hum_timer = 0;
    static uint8_t cur_inter_lum = 0;
    uint8_t bypass = 0;

    do { //delay to startup and mutex while just did the lum
        if (startup ++ < 500) {
            bypass = 1;
            break;
        }
        startup = 500;

        if (sample_mutex) {
            if (mutex_delay ++ >= 100) {
                mutex_delay = 0;
                sample_mutex = 0;
                bypass = 0;
            } else {
                bypass = 1;
            }
            break;
        }
    } while (0);

    if (bypass) {
        return;
    }
    
    halIOGetStatus(LIGHT_SENSE_IO, &status);
    status = !status;
    // printf("light gpio: %d\r\n", status);
    inter_lum[cur_inter_lum] = (uint8_t)status;
    cur_inter_lum = ((cur_inter_lum + 1) >= sizeof(inter_lum)) ? 0 : (++ cur_inter_lum);

    status = HAL_IO_STATUS_LOW;
    for (int i = 0; i < sizeof(inter_lum); i ++) {
        status |= inter_lum[i];
    }
    lum_ava = !status;
    if (slpLampGetLightSwitch() == 0) {
        last_lum_ava = lum_ava;
    } 

    halIOGetStatus(HUMAN_SENSE_IO, &status);
    // printf("human gpio: %d\r\n", status);
    if (status) {
        if (last_human_motive != status
            && inter_hum_timer ++ > 10) {
            last_human_motive = status;
            // printf("people coming notify!!!\r\n");
            slpLampSetHumanSensorInt(&status, sizeof(int));
            myMagicEvnetEmit(status);
            inter_hum_timer = 0;            
        }
        inter_no_hum_timer = 0;
    } else {
        if (last_human_motive != status
            && inter_no_hum_timer ++ > 500) {
            last_human_motive = status;
            // printf("no people notify!!\r\n");
            slpLampSetHumanSensorInt(&status, sizeof(int));
            myMagicEvnetEmit(status);
            inter_no_hum_timer = 0;
        } 
        inter_hum_timer = 0;      
    }

    if (g_light.lamp_auto) {
        if (lum_ava == 1) {
            if (status == HAL_IO_STATUS_HIGH) {
                if (slpLampGetLightSwitch() == 1) {
                    // printf("[%d, %s]\r\n", __LINE__, __FUNCTION__);
                    // lampctrl_cnt_set(30);
                    lampctrl_cnt_set(g_stRlData.liveData.uwAdjDuration);
                } else {
                    if (!sample_mutex) {
                        rlLampSwitchCtrl(1, 0);
                        printf("mutex: %d, duration: %d, [%d, %s]\r\n",sample_mutex, g_stRlData.liveData.uwAdjDuration, __LINE__, __FUNCTION__);
                        // lampctrl_cnt_set(30);
                        lampctrl_cnt_set(g_stRlData.liveData.uwAdjDuration);
                    }
                   
                }
            } else {
                if ((last_human_motive == 1 && slpLampGetLightSwitch() == 1)
                    ||(g_ke_press == 1 && slpLampGetLightSwitch() == 1)) {
                    // printf("[%d, %s]\r\n", __LINE__, __FUNCTION__);
                    // lampctrl_cnt_set(30);
                    lampctrl_cnt_set(g_stRlData.liveData.uwAdjDuration);
                    g_ke_press = 0;
                }
            }
        } else {
            if (old_lum_ava != lum_ava) {
                if (last_lum_ava == 1 && slpLampGetLightSwitch() == 1) {
                    // printf("[%d, %s]\r\n", __LINE__, __FUNCTION__);
                    // lampctrl_cnt_set(30);
                    lampctrl_cnt_set(g_stRlData.liveData.uwAdjDuration);
                } else if (last_lum_ava == 0 && slpLampGetLightSwitch() == 1) {
                    // printf("[%d, %s]\r\n", __LINE__, __FUNCTION__);
                    lampctrl_cnt_set(3);
                }
                old_lum_ava = lum_ava;
            }
            if (g_ke_press && slpLampGetLightSwitch() == 1) {
                g_ke_press = 0;
                // printf("%d, [%d, %s]\r\n", last_lum_ava, __LINE__, __FUNCTION__);
                if (last_lum_ava == 0) {
                    lampctrl_cnt_set(3);
                } else {
                    // lampctrl_cnt_set(30);
                    lampctrl_cnt_set(g_stRlData.liveData.uwAdjDuration);
                }
            }
        } 
        // if (status == HAL_IO_STATUS_HIGH) {
        //     if (slpLampGetLightSwitch() == 1) {
        //         lampctrl_cnt_set(30);
        //     } else {
        //         if (lum_ava) {
        //             slpLampSetLightSwitchInt(&onoff, sizeof(uint8_t));
        //         } else {;} //day time or the light is off (lum_ava == 0 && slpLampGetLightSwitch() == 0)
        //     }
        // }
    }

    // vTaskDelay(10 / portTICK_RATE_MS);
}

#endif

#ifdef __cplusplus
}
#endif


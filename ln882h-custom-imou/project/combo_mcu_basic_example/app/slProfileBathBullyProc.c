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
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_BATHBULLY_BASE)
#include "slData.h"
#include "slSerialBathBullyBaseProc.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/

/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/

/******************************************************************************
*                                Globals                                      *
******************************************************************************/
static struct MagicLinkTestBathBully {
    int lightOnoff;
    int lightMode;
    int dryOnoff;
    int fanOnoff;
    int ventilationOnoff;
    int warmFanOnoff;
    int warmFanGear;
    int temperature;
} g_bathBully = {
    .lightOnoff = 0,
    .lightMode = 0,
    .dryOnoff = 0,
    .fanOnoff = 0,
    .ventilationOnoff = 0,
    .warmFanOnoff = 0,
    .warmFanGear = 0,
    .temperature = 0,
};
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
 Function    : slpBathBullySetLightSwitchInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullySetLightSwitchInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_bathBully.lightOnoff = *(int *)data > 0 ? 1 : 0;
//  rlLampSwitchCtrl((uint8_t)g_bathBully.lightOnoff, pLiveData->uwAdjDuration);
    rlBathBullyBaseLightOnoffCtrl((uint8_t)g_bathBully.lightOnoff, pLiveData->uwAdjDuration);
    printf("set light switch %d\r\n", g_bathBully.lightOnoff);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullyGetLightSwitchInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullyGetLightSwitchInt(void **data, unsigned int *len)
{
    *len = sizeof(g_bathBully.lightOnoff);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_bathBully.lightOnoff, *len);

    printf("get light switch %d \r\n", g_bathBully.lightOnoff);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullySetLightModeInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullySetLightModeInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_bathBully.lightMode = *(int *)data;
    printf("set light mode %d\r\n", g_bathBully.lightMode);

//  rlLampLightModeCtrl((uint8_t)g_bathBully.lightMode, pLiveData->uwAdjDuration);
    rlBathBullyBaseLightModeCtrl((uint8_t)g_bathBully.lightMode, pLiveData->uwAdjDuration);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullyGetLightModeInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullyGetLightModeInt(void **data, unsigned int *len)
{
    *len = sizeof(g_bathBully.lightMode);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }
    (void)memset(*data, 0, *len);

    (void)memcpy(*data, &g_bathBully.lightMode, *len);

    printf("get light mode %d\r\n", g_bathBully.lightMode);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullySetDryOnoffInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullySetDryOnoffInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_bathBully.dryOnoff = *(int *)data > 0 ? 1 : 0;
    rlBathBullyBaseDryOnoffCtrl((uint8_t)g_bathBully.dryOnoff);
//  rlLampSwitchCtrl((uint8_t)g_bathBully.dryOnoff, pLiveData->uwAdjDuration);
    printf("set bathBully dry onoff %d\r\n", g_bathBully.dryOnoff);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullyGetDryOnoffInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullyGetDryOnoffInt(void **data, unsigned int *len)
{
    *len = sizeof(g_bathBully.dryOnoff);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_bathBully.dryOnoff, *len);

    printf("get bathBully dry onoff %d\r\n", g_bathBully.dryOnoff);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullySetFanOnoffInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullySetFanOnoffInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_bathBully.fanOnoff = *(int *)data > 0 ? 1 : 0;
    rlBathBullyBaseFanOnoffCtrl((uint8_t)g_bathBully.fanOnoff);
//  rlLampSwitchCtrl((uint8_t)g_light.on, pLiveData->uwAdjDuration);
    printf("set bathBully fan onoff %d\r\n", g_bathBully.fanOnoff);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullyGetFanOnoffInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullyGetFanOnoffInt(void **data, unsigned int *len)
{
    *len = sizeof(g_bathBully.fanOnoff);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_bathBully.fanOnoff, *len);

    printf("get bathBully fan onoff %d\r\n", g_bathBully.fanOnoff);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullySetVentilationOnoffInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullySetVentilationOnoffInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_bathBully.ventilationOnoff = *(int *)data > 0 ? 1 : 0;
    rlBathBullyBaseVentilationOnoffCtrl((uint8_t)g_bathBully.ventilationOnoff);
//  rlLampSwitchCtrl((uint8_t)g_light.on, pLiveData->uwAdjDuration);
    printf("set bathBully ventilation onoff %d\r\n", g_bathBully.ventilationOnoff);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullyGetVentilationOnoffInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullyGetVentilationOnoffInt(void **data, unsigned int *len)
{
    *len = sizeof(g_bathBully.ventilationOnoff);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_bathBully.ventilationOnoff, *len);

    printf("get bathBully ventilation onoff %d\r\n", g_bathBully.ventilationOnoff);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullySetWarmFanOnoffInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullySetWarmFanOnoffInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_bathBully.warmFanOnoff = *(int *)data > 0 ? 1 : 0;
    rlBathBullyBaseWarmFanOnoffCtrl((uint8_t)g_bathBully.warmFanOnoff);
//  rlLampSwitchCtrl((uint8_t)g_light.on, pLiveData->uwAdjDuration);
    printf("set bathBully warmFan onoff %d\r\n", g_bathBully.warmFanOnoff);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullyGetWarmFanOnoffInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullyGetWarmFanOnoffInt(void **data, unsigned int *len)
{
    *len = sizeof(g_bathBully.warmFanOnoff);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_bathBully.warmFanOnoff, *len);

    printf("get bathBully warmFan onoff %d\r\n", g_bathBully.warmFanOnoff);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullySetWarmFanGearInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullySetWarmFanGearInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_bathBully.warmFanGear = *(int *)data > 0 ? 1 : 0;
    rlBathBullyBaseWarmFanGearCtrl((uint8_t)g_bathBully.warmFanGear);
//  rlLampSwitchCtrl((uint8_t)g_light.on, pLiveData->uwAdjDuration);
    printf("set bathBully warmFan gear %d\r\n", g_bathBully.warmFanGear);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullyGetWarmFanGearInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullyGetWarmFanGearInt(void **data, unsigned int *len)
{
    *len = sizeof(g_bathBully.warmFanGear);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_bathBully.warmFanGear, *len);

    printf("get bathBully warmFan gear %d\r\n", g_bathBully.warmFanGear);
    return 0;
}

/******************************************************************************
 Function    : slpBathBullyGetTemperatureInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpBathBullyGetTemperatureInt(void **data, unsigned int *len)
{
    *len = sizeof(g_bathBully.temperature);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_bathBully.temperature, *len);

    printf("get bathBully temperature %d\r\n", g_bathBully.temperature);
    return 0;
}


/******************************************************************************
 Function    : slBathBullyMagicLinkDataRsync
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slBathBullyMagicLinkDataRsync(void) 
{
#if defined(APP_MAGIC_LINK_USE) && (APP_MAGIC_LINK_USE != 0)
    int tmp = 0;
    uint8_t changed = 0;
    stSlDevBathBullyData_t *pCurBathBullyBaseData = &g_stRlData.liveData.stSlDevBathBullyData;
    
    if (g_stRlData.fctData.fctMode != 0) return;

    tmp = (int)pCurBathBullyBaseData->ucLightOnoff;
    if (tmp != g_bathBully.lightOnoff) {
        g_bathBully.lightOnoff = tmp;
        changed = 1;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
    }

    tmp = (int)pCurBathBullyBaseData->ucLightMode;
    if (tmp != g_bathBully.lightMode) {
        g_bathBully.lightMode = tmp;
        changed = 1;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
    }

    tmp = (int)pCurBathBullyBaseData->ucDryOnoff;
    if (tmp != g_bathBully.dryOnoff) {
        g_bathBully.dryOnoff = tmp;
        changed = 1;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
    }

    tmp = (int)pCurBathBullyBaseData->ucFanOnoff;
    if (tmp != g_bathBully.fanOnoff) {
        g_bathBully.fanOnoff = tmp;
        changed = 1;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
    }

    tmp = (int)pCurBathBullyBaseData->ucVentilationOnoff;
    if (tmp != g_bathBully.ventilationOnoff) {
        g_bathBully.ventilationOnoff = tmp;
        changed = 1;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
    }

    tmp = (int)pCurBathBullyBaseData->ucWarmFanOnoff;
    if (tmp != g_bathBully.warmFanOnoff) {
        g_bathBully.warmFanOnoff = tmp;
        changed = 1;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
    }

    tmp = (int)pCurBathBullyBaseData->ucWarmFanGear;
    if (tmp != g_bathBully.warmFanGear) {
        g_bathBully.warmFanGear = tmp;
        changed = 1;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
    }

    tmp = (int)pCurBathBullyBaseData->scTemperature;
    if (tmp != g_bathBully.temperature) {
        g_bathBully.temperature = tmp;
        changed = 1;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
    }

#if defined(LIGHT_STATE_SAVE_EN) && (LIGHT_STATE_SAVE_EN == 1)
    if (changed) {
        memcpy((void *)&g_stRlData.saveData.stSlDevBathBullyData, (void *)pCurBathBullyBaseData, sizeof(stSlDevBathBullyData_t));
        rlFlagSet(RL_FLAG_SYS_CFG_DATA_SAVE, 1);
    }
#endif
#endif // #if defined(APP_MAGIC_LINK_USE) && (APP_MAGIC_LINK_USE != 0)
}

/******************************************************************************
 Function    : MagicLinkDataReport
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void MagicLinkDataReport(void) 
{
#if 1
// #if defined(APP_DEV_CCT_SUPPORT) && (APP_DEV_CCT_SUPPORT != 0)
//     printf("%d magic report onoff %d bri %d cct %d\r\n", xTaskGetTickCount(), g_light.on, g_light.brightness, g_light.colorTemperature);
// #else
//     printf("%d magic report onoff %d bri %d\r\n", xTaskGetTickCount(), g_bathBully.on, g_bathBully.brightness);
// #endif
//     MagicLinkReportServiceStatus("light");
    if (1) {
        // rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 0);
        printf("%d magic report temperature %d\r\n", xTaskGetTickCount(), g_bathBully.temperature);
        MagicLinkReportServiceStatus("bathBully");
        MagicLinkReportServiceStatus("light");
        MagicLinkReportServiceStatus("environment");
    }
#endif
}

#endif //#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_BATHBULLY_BASE)

#ifdef __cplusplus
}
#endif


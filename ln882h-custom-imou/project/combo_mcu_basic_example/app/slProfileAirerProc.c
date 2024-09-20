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
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_AIRER_BASE)
#include "slData.h"
#include "slSerialAirerBaseProc.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/

/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/

/******************************************************************************
*                                Globals                                      *
******************************************************************************/
static struct MagicLinkTestAirer {
    int lightOnoff;
    int airerOnoff;
    int airerMotionCtrl;
    int airerPosition;
} g_airerBase = {
    .lightOnoff = 0,
    .airerOnoff = 1,
    .airerMotionCtrl = 0,
    .airerPosition = 0,
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
 Function    : slpAirerSetLightSwitchInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpAirerSetLightSwitchInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_airerBase.lightOnoff = *(int *)data > 0 ? 1 : 0;
    rlAirerBaseLightOnoffCtrl((uint8_t)g_airerBase.lightOnoff, pLiveData->uwAdjDuration);
    printf("set light switch succ\r\n");
    return 0;
}

/******************************************************************************
 Function    : slpAirerGetLightSwitchInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpAirerGetLightSwitchInt(void **data, unsigned int *len)
{
    *len = sizeof(g_airerBase.lightOnoff);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_airerBase.lightOnoff, *len);

    printf("get light switch succ \r\n");
    return 0;
}

/******************************************************************************
 Function    : slpAirerSetAirerOnoffInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpAirerSetAirerOnoffInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_airerBase.airerOnoff = *(int *)data > 0 ? 1 : 0;
//  rlLampSwitchCtrl((uint8_t)g_light.on, pLiveData->uwAdjDuration);
    printf("set airer switch %d\r\n", g_airerBase.airerOnoff);

    return 0;
}

/******************************************************************************
 Function    : slpAirerGetAirerOnoffInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpAirerGetAirerOnoffInt(void **data, unsigned int *len)
{
    *len = sizeof(g_airerBase.airerOnoff);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_airerBase.airerOnoff, *len);

    printf("get airer switch %d\r\n", g_airerBase.airerOnoff);
    return 0;
}

/******************************************************************************
 Function    : slpAirerSetAirerMotionInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpAirerSetAirerMotionInt(const void *data, unsigned int len)
{
    stRlLiveData_t *pLiveData = &g_stRlData.liveData;

    g_airerBase.airerMotionCtrl = *(int *)data;
    rlAirerBaseMotionfCtrl((uint8_t)g_airerBase.airerMotionCtrl);
    printf("set airer motion ctrl %d\r\n", g_airerBase.airerMotionCtrl);
    return 0;
}

/******************************************************************************
 Function    : slpAirerGetAirerMotionInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpAirerGetAirerMotionInt(void **data, unsigned int *len)
{
    *len = sizeof(g_airerBase.airerMotionCtrl);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_airerBase.airerMotionCtrl, *len);

    printf("get airer motion ctrl %d\r\n", g_airerBase.airerMotionCtrl);
    return 0;
}

/******************************************************************************
 Function    : slpAirerGetAirerPositionInt
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slpAirerGetAirerPositionInt(void **data, unsigned int *len)
{
    *len = sizeof(g_airerBase.airerPosition);

    *data = malloc(*len);
    if (*data == NULL) {
        printf("malloc err \r\n");
        return -1;
    }

    (void)memset(*data, 0, *len);
    (void)memcpy(*data, &g_airerBase.airerPosition, *len);

    printf("get airer position\r\n", g_airerBase.airerPosition);
    return 0;
}

/******************************************************************************
 Function    : slAirerBaseMagicLinkDataRsync
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slAirerBaseMagicLinkDataRsync(void) 
{
#if defined(APP_MAGIC_LINK_USE) && (APP_MAGIC_LINK_USE != 0)
    int tmp = 0;
    uint8_t changed = 0;
    stSlDevAirerData_t *pCurAirerBaseData = &g_stRlData.liveData.stSlDevAirerData;
    
    if (g_stRlData.fctData.fctMode != 0) return;

    tmp = (int)pCurAirerBaseData->ucLightOnoff;
    if (tmp != g_airerBase.lightOnoff) {
        g_airerBase.lightOnoff = tmp;
        changed = 1;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
    }

    tmp = (int)pCurAirerBaseData->ucPosition;
    if (tmp != g_airerBase.airerPosition) {
        g_airerBase.airerPosition = tmp;
        changed = 1;
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 1);
    }

#if defined(LIGHT_STATE_SAVE_EN) && (LIGHT_STATE_SAVE_EN == 1)
    if (changed) {
        memcpy((void *)&g_stRlData.saveData.stSlDevAirerData, (void *)pCurAirerBaseData, sizeof(stSlDevAirerData_t));
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
#if 0
#if defined(APP_DEV_CCT_SUPPORT) && (APP_DEV_CCT_SUPPORT != 0)
    printf("%d magic report onoff %d bri %d cct %d\r\n", xTaskGetTickCount(), g_light.on, g_light.brightness, g_light.colorTemperature);
#else
    printf("%d magic report onoff %d bri %d\r\n", xTaskGetTickCount(), g_light.on, g_light.brightness);
#endif
    MagicLinkReportServiceStatus("light");
#endif
    MagicLinkReportServiceStatus("airer");
    MagicLinkReportServiceStatus("light");
}

#endif //#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_AIRER_BASE)

#ifdef __cplusplus
}
#endif


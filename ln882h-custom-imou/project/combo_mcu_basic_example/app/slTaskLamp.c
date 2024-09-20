/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slTaskLamp.c                                                  *
* Description :                                                               *
*               smart lamp task lamp source file                              *
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
#include "slTaskLamp.h"

#if defined(APP_TASK_LAMP_USE) && (APP_TASK_LAMP_USE == 1)
#include "dimmingLib.h"
#include "slData.h"
#include "slTaskPower.h"
#include "magiclink.h"
#include "sys.h"
#include "timers.h"
#include "atShell.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define RL_PWM_W                                           MY_HAL_PWM_0
#define RL_PWM_C                                           MY_HAL_PWM_4
#define RL_PWM_MAX                                         (9999)
#define LIGHT_BRIGHT_MAX                                   (65535)
#define LIGHT_BRIGHT_MIN                                   (uint16_t)(LIGHT_BRIGHT_MAX * 1.0f / 100.0f + 0.5f)
#define LIGHT_COLOR_CCT_MIN                                (3000)
#define LIGHT_COLOR_CCT_MAX                                (5700)

#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_READING) //reading
#define LIGHT_CCT_ENABLE                                   (1)
#define LIGHT_PWM_FREQ                                     (4000)
#define LIGHT_PWM_OUTPUT_MAX_POWER_RATIO                   (1.00)  //最大100%功率输出
#define LIGHT_PWM_BRIGHT_MIN_PERCENT                       (0.01f) //percent: 0.00f-1.00f : 0%-100%
#define LIGHT_PWM_BRIGHT_DEF_PERCENT                       (1.0f)  //percent: 0.00f-1.00f : 0%-100%
#define LIGHT_CCT_DEFAULT                                  (LIGHT_COLOR_CCT_MID)
#define LIGHT_PWM_MAX                                      RL_PWM_MAX
#define LIGHT_PWM_MIN                                      (uint32_t)(RL_PWM_MAX * 0.0f / 100)
#define LIGHT_LAMP_SOFT_CAPACITY_VAL                       (LIGHT_PWM_MIN * 30)
#define LIGHT_PWM_CURVE                                    MY_DIMMING_CURVE_CIE1976
#define LIGHT_SCENE_MODE_DEFAULT                           RL_LAMP_LIGHT_MODE_READ
#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_CEILING) //celling
#define LIGHT_PWM_FREQ                                     (500)
#define LIGHT_CCT_ENABLE                                   (1)
#define LIGHT_PWM_OUTPUT_MAX_POWER_RATIO                   (1.00)  //最大100%功率输出
#define LIGHT_PWM_BRIGHT_MIN_PERCENT                       (0.01f) //percent: 0.00f-1.00f : 0%-100%
#define LIGHT_PWM_BRIGHT_DEF_PERCENT                       (1.0f)  //percent: 0.00f-1.00f : 0%-100%
#define LIGHT_CCT_DEFAULT                                  (LIGHT_COLOR_CCT_MID)
#define LIGHT_PWM_MAX                                      RL_PWM_MAX
#define LIGHT_PWM_MIN                                      (uint32_t)(RL_PWM_MAX * 8.0f / 100)
#define LIGHT_LAMP_SOFT_CAPACITY_VAL                       (LIGHT_PWM_MIN * 30)
#define LIGHT_PWM_CURVE                                    MY_DIMMING_CURVE_CIE1976
#define LIGHT_SCENE_MODE_DEFAULT                           CL_LAMP_LIGHT_MODE_GUEST
#define LIGHT_STATE_SAVE_EN                                1
#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT_PTJX) //night lamp
#define LIGHT_CCT_ENABLE                                   (1)
#define LIGHT_PWM_FREQ                                     (4000)
#define LIGHT_PWM_OUTPUT_MAX_POWER_RATIO                   (1.00)  //最大100%功率输出
#define LIGHT_PWM_BRIGHT_MIN_PERCENT                       (0.00f) //percent: 0.00f-1.00f : 0%-100%
#define LIGHT_PWM_BRIGHT_DEF_PERCENT                       (1.0f)  //percent: 0.00f-1.00f : 0%-100%
#define LIGHT_CCT_DEFAULT                                  (LIGHT_COLOR_CCT_MID)
#define LIGHT_PWM_MAX                                      RL_PWM_MAX
#define LIGHT_PWM_MIN                                      (uint32_t)(RL_PWM_MAX * 0.0f / 100)
#define LIGHT_LAMP_SOFT_CAPACITY_VAL                       (LIGHT_PWM_MIN * 30)
#define LIGHT_PWM_CURVE                                    MY_DIMMING_CURVE_CIE1976
#define LIGHT_SCENE_MODE_DEFAULT                           RL_LAMP_LIGHT_MODE_READ
#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_READING_L) //resding_L
#define LIGHT_CCT_ENABLE                                   (1)
#define LIGHT_PWM_FREQ                                     (4000)
#define LIGHT_PWM_OUTPUT_MAX_POWER_RATIO                   (1.00)  //最大100%功率输出
#define LIGHT_PWM_BRIGHT_MIN_PERCENT                       (0.01f) //percent: 0.00f-1.00f : 0%-100%
#define LIGHT_PWM_BRIGHT_DEF_PERCENT                       (1.0f)  //percent: 0.00f-1.00f : 0%-100%
#define LIGHT_CCT_DEFAULT                                  (LIGHT_COLOR_CCT_MID)
#define LIGHT_PWM_MAX                                      RL_PWM_MAX
#define LIGHT_PWM_MIN                                      (uint32_t)(RL_PWM_MAX * 0.0f / 100)
#define LIGHT_LAMP_SOFT_CAPACITY_VAL                       (LIGHT_PWM_MIN * 30)
#define LIGHT_PWM_CURVE                                    MY_DIMMING_CURVE_CIE1976
#define LIGHT_SCENE_MODE_DEFAULT                           RL_LAMP_LIGHT_MODE_READ
#else
#endif

#define LIGHT_COLOR_CCT_MID                                (LIGHT_COLOR_CCT_MIN+(LIGHT_COLOR_CCT_MAX-LIGHT_COLOR_CCT_MIN)/2)

#ifndef LIGHT_CCT_DEFAULT
#define LIGHT_CCT_DEFAULT                                  LIGHT_COLOR_CCT_MID
#endif

#define LIGHT_PWM_BRIGHT_MIN                               (uint16_t)(LIGHT_BRIGHT_MAX * LIGHT_PWM_BRIGHT_MIN_PERCENT + 0.5f)
#define LIGHT_BRIGHT_DEFAULT                               (uint16_t)(LIGHT_BRIGHT_MAX * LIGHT_PWM_BRIGHT_DEF_PERCENT + 0.5f)
#define LIGHT_BRIGHT_LOCAL_TO_PWM(b)                       (uint16_t)(LIGHT_PWM_BRIGHT_MIN + (float)((b)-LIGHT_BRIGHT_MIN) * (LIGHT_BRIGHT_MAX - LIGHT_PWM_BRIGHT_MIN) / (LIGHT_BRIGHT_MAX-LIGHT_BRIGHT_MIN) + 0.5f)

/* bright: value <---> percent */
#define LIGHT_BRIGHT_TO_PERCENT(b)                         (uint32_t)((b) * 100.0f / 65535 + 0.5f)
#define LIGHT_PERCENT_TO_BRIGHT(p)                         (uint16_t)((p) * 65535.0f / 100 + 0.5f)

#define RL_LAMP_REPORT_INTERVAL                            (800) //ms
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
//typedef struct
//{
//    slLampWallSwitchMode_t mode;
//    uint16_t uwBri;
//    uint16_t uwCct;
//} stSlLampWallSwitchMode_t;

typedef struct
{
    uint8_t ucLightMode;
    uint8_t ucOnoff;
    uint16_t uwBri;
    uint16_t uwCct;
} stSlLampLightMode_t;
/******************************************************************************
*                             Declarations                                    *
******************************************************************************/
SemaphoreHandle_t nlSetMutex;

/******************************************************************************
*                                Globals                                      *
******************************************************************************/
const uint32_t k_a_uwBriPercent[] = 
{
    0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100
};

const stSlLampLightMode_t k_stSlLampWallSwitchMode[RL_LAMP_WALL_SWITCH_MODE_NUM] =
{
    {RL_LAMP_WALL_SWITCH_MODE_1, 1, 65535, LIGHT_COLOR_CCT_MID},
    {RL_LAMP_WALL_SWITCH_MODE_2, 1, 65535, LIGHT_COLOR_CCT_MAX},
    {RL_LAMP_WALL_SWITCH_MODE_3, 1, 65535, LIGHT_COLOR_CCT_MIN},
};

#if APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_READING || APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_READING_L
const stSlLampLightMode_t k_stSlLampLightMode[] =
{
    {RL_LAMP_LIGHT_MODE_WRITE, 1, 65535, LIGHT_COLOR_CCT_MID},
    {RL_LAMP_LIGHT_MODE_READ,  1, 65535, LIGHT_COLOR_CCT_MID},
    {RL_LAMP_LIGHT_MODE_NIGHT, 1, 65535, LIGHT_COLOR_CCT_MID},
};
#elif APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_CEILING
const stSlLampLightMode_t k_stSlLampLightMode[] =
{
    {CL_LAMP_LIGHT_MODE_GUEST,      1, 65535, LIGHT_COLOR_CCT_MID},
    {CL_LAMP_LIGHT_MODE_RELAX,      1, 65535, LIGHT_COLOR_CCT_MIN},
    {CL_LAMP_LIGHT_MODE_WORKING,    1, 65535, LIGHT_COLOR_CCT_MAX},
    {CL_LAMP_LIGHT_MODE_VIEWING,    1, 32768, LIGHT_COLOR_CCT_MIN},
    {CL_LAMP_LIGHT_MODE_BEDTIME,    1, 13107, LIGHT_COLOR_CCT_MIN},
    {CL_LAMP_LIGHT_MODE_NIGHTLIGHT, 1,   655, LIGHT_COLOR_CCT_MIN},
};
#elif APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT_PTJX
const stSlLampLightMode_t k_stSlLampLightMode[] =
{
    {RL_LAMP_LIGHT_MODE_WRITE, 1, 65535, LIGHT_COLOR_CCT_MID},
    {RL_LAMP_LIGHT_MODE_READ,  1, 65535, LIGHT_COLOR_CCT_MID},
    {RL_LAMP_LIGHT_MODE_NIGHT, 1, 65535, LIGHT_COLOR_CCT_MID},
};
#else
const stSlLampLightMode_t k_stSlLampLightMode[] =
{
    
};
#endif

static uint8_t gucLampId;
static myLampParam_t s_stCurLampParam = {0};
static myLampParam_t s_stLstLampParam = {0};
static xTimerHandle s_resetWindowTimerHandle, s_lampctrlTimer;
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
 Function    : _lampCubeRoot
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static float _lampCubeRoot(float a)
{
    float root = 1.0f;

    while (fabs(root*root*root-a)>= 0.0001) {
        root = root-(root*root*root-a)/(3*root*root);
    }
    return root;
}

/******************************************************************************
 Function    : _lampPwmBri2SightBri
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static uint16_t _lampPwmBri2SightBri(uint16_t pwmBri)
{
    float Y = (pwmBri-LIGHT_BRIGHT_MIN)*1.0f/(LIGHT_BRIGHT_MAX-LIGHT_BRIGHT_MIN);
    float sightBri = (Y <= 0.008856f ? 903.3*Y : 116*_lampCubeRoot(Y)-16);

    return (uint16_t)(LIGHT_BRIGHT_MIN + sightBri * (LIGHT_BRIGHT_MAX-LIGHT_BRIGHT_MIN) / 100.0f + 0.5f);
}

/******************************************************************************
 Function    : _lampSightBri2PwmBri
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static uint16_t _lampSightBri2PwmBri(uint16_t sightBri)
{
    float pwmBri = 0.0f;
    float L = (sightBri-LIGHT_BRIGHT_MIN) * 100.0f / (LIGHT_BRIGHT_MAX-LIGHT_BRIGHT_MIN);

    if (L <= 7.999624) {
        pwmBri = L / 903.3;
    } else {
        pwmBri = (L+16)/116;
        pwmBri = pwmBri * pwmBri * pwmBri;
    }

    pwmBri = pwmBri > 1.0f ? 1.0f : pwmBri;

    return (uint16_t)(LIGHT_BRIGHT_MIN + pwmBri * (LIGHT_BRIGHT_MAX-LIGHT_BRIGHT_MIN) + 0.5f);
}

/******************************************************************************
 Function    : _lampColor2Pwm
 Description : lamp color to pwm
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/                                        
static int8_t _lampColor2Pwm(uint32_t ulPwm[4], myLampParam_t *pLampParam)
{ 
    float afPWM[3] = {1.0f, 1.0f, 1.0f};
    uint32_t tmpPwm = 0;
    uint16_t pwmBri = _lampSightBri2PwmBri(pLampParam->uwBri);

#if LIGHT_CCT_ENABLE
    uint16_t halfCCT = (LIGHT_COLOR_CCT_MAX-LIGHT_COLOR_CCT_MIN) / 2;
    uint16_t cctPoint2 = LIGHT_COLOR_CCT_MIN + (uint16_t)(halfCCT / LIGHT_PWM_OUTPUT_MAX_POWER_RATIO);
    uint16_t cctPoint1 = LIGHT_COLOR_CCT_MIN + (LIGHT_COLOR_CCT_MAX-cctPoint2); 
    uint16_t uwCCT = APP_RANGE(pLampParam->uwCCT, LIGHT_COLOR_CCT_MIN, LIGHT_COLOR_CCT_MAX);
    
    if (uwCCT <= cctPoint1) {
        afPWM[0] = (float)1.0f;
        afPWM[1] = (float)(uwCCT - LIGHT_COLOR_CCT_MIN) / (cctPoint2 - LIGHT_COLOR_CCT_MIN);
     } else if (uwCCT < cctPoint2) {
        afPWM[0] = (float)(LIGHT_COLOR_CCT_MAX - uwCCT) / (cctPoint2 - LIGHT_COLOR_CCT_MIN);
        afPWM[1] = (float)(uwCCT - LIGHT_COLOR_CCT_MIN) / (cctPoint2 - LIGHT_COLOR_CCT_MIN);
     } else {
        afPWM[0] = (float)(LIGHT_COLOR_CCT_MAX - uwCCT) / (cctPoint2 - LIGHT_COLOR_CCT_MIN);
        afPWM[1] = (float)1.0f;
     }
#endif //LIGHT_CCT_ENABLE

#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_NIGHT_PTJX)
    ulPwm[0] = (uwCCT == LIGHT_COLOR_CCT_MIN) ? LIGHT_PWM_MAX : ((uwCCT == LIGHT_COLOR_CCT_MAX) ? LIGHT_PWM_MIN : LIGHT_PWM_MAX);
    ulPwm[1] = (uwCCT == LIGHT_COLOR_CCT_MIN) ? LIGHT_PWM_MIN : ((uwCCT == LIGHT_COLOR_CCT_MAX) ? LIGHT_PWM_MAX : LIGHT_PWM_MAX);
#else
     tmpPwm = LIGHT_PWM_MIN + 
        (uint32_t)((LIGHT_PWM_MAX-LIGHT_PWM_MIN) * pwmBri * afPWM[0] / LIGHT_BRIGHT_MAX);
    ulPwm[0] = APP_RANGE(tmpPwm, LIGHT_PWM_MIN, LIGHT_PWM_MAX);
    tmpPwm = LIGHT_PWM_MIN + 
        (uint32_t)((LIGHT_PWM_MAX-LIGHT_PWM_MIN) * pwmBri * afPWM[1] / LIGHT_BRIGHT_MAX);
    ulPwm[1] = APP_RANGE(tmpPwm, LIGHT_PWM_MIN, LIGHT_PWM_MAX);
#endif    
    ulPwm[2] = 0;
    ulPwm[3] = 0;
    my_hal_log_debug("bri %d pwmbri %d cct %d to pwm %d %d\r\n", pLampParam->uwBri, pwmBri, uwCCT, ulPwm[0], ulPwm[1]);

    return 0;
}

/******************************************************************************
 Function    : _lampPwmOutput
 Description : lamp pwm output
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampPwmOutput(uint32_t ulPwm1, uint32_t ulPwm2, uint32_t ulPwm3, uint32_t ulPwm4)
{ 
    myLampParam_t lampParam;
    static uint32_t lastPwm1 = 0;
    static uint32_t lastPwm2 = 0;

    myLampStatusGet(gucLampId, &lampParam);
    ulPwm1 = APP_RANGE(ulPwm1, LIGHT_PWM_MIN, LIGHT_PWM_MAX);
    ulPwm2 = APP_RANGE(ulPwm2, LIGHT_PWM_MIN, LIGHT_PWM_MAX);

    if (lampParam.ucSwitch == 0) {
#ifdef LIGHT_PWM_OFF_LEVEL
        
        if ((ulPwm1 + ulPwm2) < LIGHT_PWM_OFF_LEVEL) {
#else
        if ((ulPwm1 + ulPwm2) <= LIGHT_PWM_MIN*2) {
#endif
            ulPwm1 = ulPwm2 = 0;
        }
    }

    if (ulPwm1 != lastPwm1) {
        lastPwm1 = ulPwm1;
//      my_hal_log_debug("pwm1 out %d %d\r\n", ulPwm1, ulPwm2);
        myHalPwmOutput(RL_PWM_W, ulPwm1);
    }
    if (ulPwm2 != lastPwm2) {
        lastPwm2 = ulPwm2;
//      my_hal_log_debug("pwm2 out %d %d\r\n", ulPwm1, ulPwm2);
        myHalPwmOutput(RL_PWM_C, ulPwm2);
    }
}

/******************************************************************************
 Function    : _lampStateControlHook
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampStateControlHook(uint8_t lampId, const myLampParam_t *pLampParam)
{
    int changed = 0;
    myLampParam_t *pCurLampParam = &s_stCurLampParam;
    myLampParam_t *pLstLampParam = &s_stLstLampParam;

    if (pLstLampParam->ucSwitch != pCurLampParam->ucSwitch) {
        pLstLampParam->ucSwitch = pCurLampParam->ucSwitch;
        changed = 1;
        my_hal_log_info("switch changed %d\r\n", pCurLampParam->ucSwitch);
    }

    if (pLstLampParam->uwBri != pCurLampParam->uwBri) {
        pLstLampParam->uwBri = pCurLampParam->uwBri;
        changed = 1;
        my_hal_log_info("brightness changed %d\r\n", pCurLampParam->uwBri);
    }

    if (pLstLampParam->uwCCT != pCurLampParam->uwCCT) {
        pLstLampParam->uwCCT = pCurLampParam->uwCCT;
        changed = 1;
        my_hal_log_info("cct changed %d\r\n", pCurLampParam->uwCCT);
    }

#if defined(LIGHT_STATE_SAVE_EN) && (LIGHT_STATE_SAVE_EN == 1)
    if (changed && pCurLampParam->ucSwitch != 0) {
        memcpy((void*)&g_stRlData.saveData.stLampSaveParam, (void *)pCurLampParam, sizeof(myLampParam_t));
        rlFlagSet(RL_FLAG_SYS_CFG_DATA_SAVE, 1);
    }
#endif

#if defined(APP_MAGIC_LINK_USE) && (APP_MAGIC_LINK_USE != 0)
        if (g_stRlData.fctData.fctMode == 0) {
            extern void MagicLinkDataRsync(void);
            MagicLinkDataRsync();
        }
#endif
}

/******************************************************************************
 Function    : _lampBlinkEndHook
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampBlinkEndHook(uint32_t arg) 
{
    my_hal_log_info("blink end %d\r\n", arg);
    if (arg == RL_LAMP_BLINK_ARG_SYS_FACTORY_RESET) {
//#if defined(APP_MAGIC_LINK_USE) && (APP_MAGIC_LINK_USE != 0)
//        if (g_stRlData.fctData.fctMode == 0) {
//            extern int MagicLinkReset(void);
//            MagicLinkReset();
//        }
//#endif
//        rlDataRestoreFactory();
//        vTaskDelay(10);
//        sys_reset();
        slDataFactoryReset();
    }
}

/******************************************************************************
 Function    : _lampReportProc
 Description : reading lamp report proc
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void _lampReportProc(void) 
{
    static uint32_t free_delay = 0;

    free_delay = (free_delay > 10) ? (free_delay - 10) : 0;
    if (free_delay > 0) {
        return;
    }

    if ( 0 != rlFlagGet(RL_FLAG_MAGIC_REPORT_NEED)) {
        rlFlagSet(RL_FLAG_MAGIC_REPORT_NEED, 0);
        free_delay = RL_LAMP_REPORT_INTERVAL;
#if defined(APP_MAGIC_LINK_USE) && (APP_MAGIC_LINK_USE != 0)
        if (g_stRlData.fctData.fctMode == 0) {
            if (0 != rlFlagGet(RL_FLAG_SYS_DEV_ONLINE)) {
                extern void MagicLinkDataReport(void);
                MagicLinkDataReport();
            } 
        }
#endif
    }
}

/******************************************************************************
 Function    : _lampFactoryReset
 Description : reading lamp factory reset
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void _lampFactoryReset(void) 
{
    rlLampBlinkCtrl(3, 1000, 1, RL_LAMP_BLINK_ARG_SYS_FACTORY_RESET);
}

/******************************************************************************
 Function    : _lampFactoryResetWindowTimeoutHandle
 Description : reading lamp factory reset window timeout handle
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void _lampFactoryResetWindowTimeoutHandle(xTimerHandle pxTimer) 
{
    my_hal_log_info("close factory reset window\r\n");
    rlFlagSet(RL_FLAG_SYS_FACTORY_WINDOW, 0);
}

/******************************************************************************
 Function    : _lampReadingLightModeCtrl
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t _lampReadingLightModeCtrl(uint8_t lightMode, uint32_t ulPeroidMs) 
{
    myLampParam_t *pLampParam = &s_stCurLampParam;

    pLampParam->ucSceneNo = lightMode;
    pLampParam->ucSwitch = 1;
    switch (lightMode) {
    case RL_LAMP_LIGHT_MODE_WRITE:
        pLampParam->uwBri = 65535; //100%
        break;
    case RL_LAMP_LIGHT_MODE_READ:
        pLampParam->uwBri = 52428; //80%
        break;
    case RL_LAMP_LIGHT_MODE_NIGHT:
        pLampParam->uwBri = 655;
        break;
    default:
        pLampParam->uwBri = 65535;
        break;
    }

    return myLampCallSceneCtrl(gucLampId, ulPeroidMs, pLampParam);
}

/******************************************************************************
 Function    : _lampCeilingLightModeCtrl
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t _lampCeilingLightModeCtrl(uint8_t lightMode, uint32_t ulPeroidMs) 
{
    myLampParam_t *pLampParam = &s_stCurLampParam;

    pLampParam->ucSceneNo = lightMode;
    pLampParam->ucSwitch = 1;
    switch (lightMode) {
    case CL_LAMP_LIGHT_MODE_GUEST:
        pLampParam->uwBri = 65535;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MID;
        break;
    case CL_LAMP_LIGHT_MODE_RELAX:
        pLampParam->uwBri = 65535;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MIN;
        break;
    case CL_LAMP_LIGHT_MODE_WORKING:
        pLampParam->uwBri = 65535;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MAX;
        break;
    case CL_LAMP_LIGHT_MODE_VIEWING:
        pLampParam->uwBri = 32768;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MIN;
        break;
    case CL_LAMP_LIGHT_MODE_BEDTIME:
        pLampParam->uwBri = 13107;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MIN;
        break;
    case CL_LAMP_LIGHT_MODE_NIGHTLIGHT:
        pLampParam->uwBri = 655;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MIN;
        break;
    default:
        pLampParam->uwBri = 65535;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MID;
        break;
    }

    return myLampCallSceneCtrl(gucLampId, ulPeroidMs, pLampParam);
}

// extern xTimerHandle s_LampCtrlHandle;
// extern void _lampctrlfunc(xTimerHandle pxTimer);

int8_t _lampNightLightModeCtrl(uint8_t lightMode, uint32_t ulPeroidMs)
{
    myLampParam_t *pLampParam = &s_stCurLampParam;

    pLampParam->ucSceneNo = lightMode;
    pLampParam->ucSwitch = 1;   
    switch (lightMode) {
    case CL_LAMP_LIGHT_MODE_NIGHTLIGHT:
        pLampParam->uwBri = 65535;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MIN;
        break;
    case CL_LAMP_LIGHT_MODE_BEDTIME:
        pLampParam->uwBri = 65535;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MAX;
        break;
    case CL_LAMP_LIGHT_MODE_BOTHSIDE:
        pLampParam->uwBri = 65535;
        pLampParam->uwCCT = LIGHT_COLOR_CCT_MID;
        break;
    default:
        break;
    } 

    return myLampCallSceneCtrl(gucLampId, ulPeroidMs, pLampParam);
}


int slpLampGetLightSwitch(void)
{
    return s_stCurLampParam.ucSwitch;
}

/******************************************************************************
 Function    : rlLampGetNextClassVal
 Description : reading lamp get next class value
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static uint32_t rlLampGetNextClassVal(uint8_t ctrlType, uint32_t curVal,  const uint32_t *pTable, uint32_t maxClass)
{
    uint32_t newVal = curVal;

    if(ctrlType == 0) {
        if(newVal > pTable[0]) {
            uint8_t i=maxClass-1;
            while(i > 0) { if(newVal > pTable[i]) { newVal= pTable[i]; break; } else i--; }
            if(i==0) newVal=pTable[0];
        }
    } else {
        if(newVal < pTable[maxClass-1]) {
            uint8_t i=0;
            while(i < maxClass-1) { if(newVal < pTable[i]) {newVal = pTable[i]; break;} else i++; }
            if( i == maxClass-1 ) newVal=pTable[maxClass-1];
        }
    }
    return newVal;
}

/******************************************************************************
 Function    : rlLampDevStatusCtrl
 Description : reading lamp device status control
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampDevStatusCtrl(myLampParam_t *pLampParam, uint32_t ulPeroidMs)
{
    return myLampCallSceneCtrl(gucLampId, ulPeroidMs, pLampParam);
}

/******************************************************************************
 Function    : rlLampDevPwmCtrl
 Description : reading lamp device pwm control
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampDevPwmCtrl(const uint16_t uwPwm[4], uint32_t ulPeroidMs, uint16_t uwBri)
{
    uint32_t dstPwms[4];

    dstPwms[0] = uwPwm[0];
    dstPwms[1] = uwPwm[1];
    dstPwms[2] = uwPwm[2];
    dstPwms[3] = uwPwm[3];
    return myLampPwmsCtrl(gucLampId, ulPeroidMs, dstPwms, uwBri);
}

/******************************************************************************
 Function    : rlLampSwitchCtrl
 Description : reading lamp switch ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampSwitchCtrl(uint8_t status, uint32_t ulPeroidMs)
{
    myLampParam_t *pLampParam = &s_stCurLampParam;

    my_hal_log_info("rl lamp switch ctrl %d\r\n", status);
    pLampParam->ucSwitch = status ? 1 : 0;
    return myLampSwitchCtrl(gucLampId, ulPeroidMs, pLampParam->ucSwitch);
}

/******************************************************************************
 Function    : rlLampSwitchRevert
 Description : reading lamp switch revert
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampSwitchRevert(uint32_t ulPeroidMs)
{
    myLampParam_t *pLampParam = &s_stCurLampParam;

    my_hal_log_info("rl lamp switch revert\r\n");
    pLampParam->ucSwitch = !pLampParam->ucSwitch;
    return myLampSwitchCtrl(gucLampId, ulPeroidMs, pLampParam->ucSwitch);
}

/******************************************************************************
 Function    : rlLampSceneCall
 Description : reading lamp scene call
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampSceneCall(uint32_t sceneNo, uint32_t ulPeroidMs)
{
//  uint8_t bri = oppData.oppLiveData.wlLampStatus.ucBri;
//
//  if((sceneNo >= OPP_WL_LAMP_RGB_SCENE_NUM) && (oppData.oppLiveData.ucLampSceneMode == 0)) return -1;
//  if((sceneNo >= OPP_WL_LAMP_CCT_SCENE_NUM) && (oppData.oppLiveData.ucLampSceneMode != 0)) return -1;
//  if ( !oppData.oppLiveData.wlLampStatus.ucSwitch ) return -2;
//
//  oppTaskLampLog("oppLamp call scene proc: %d\r\n", (int)sceneNo);
//  if ( oppData.oppLiveData.ucLampSceneMode == 0 ) {
//      oppData.oppLiveData.ucLampCctSceneNo = sceneNo;
//      oppData.oppLiveData.wlLampStatus = oppData.oppCfgData.wlLampCctScene[oppData.oppLiveData.ucLampCctSceneNo];
//      oppData.oppLiveData.wlLampStatus.ucBri = bri;
//      oppLampCallSceneCtrl(oppLampInfo.lampId, ulStepNum, oppData.oppLiveData.wlLampStatus);
//  } else if ( oppData.oppLiveData.ucLampSceneMode == 1 ){
//      oppData.oppLiveData.ucLampRgbSceneNo = sceneNo;
//      oppData.oppLiveData.wlLampStatus = oppData.oppCfgData.wlLampRgbScene[oppData.oppLiveData.ucLampRgbSceneNo];
//      oppData.oppLiveData.wlLampStatus.ucBri = bri;
//      myLampCallSceneCtrl(oppLampInfo.lampId, ulStepNum, oppData.oppLiveData.wlLampStatus);
//  } else {
//      oppLampInfo.lampScene2.mode = (sceneNo >= 1 ? 0 : sceneNo + 1);
//  }
    return 0;
}

/******************************************************************************
 Function    : oppLampSceneCall
 Description : opple lamp scene call
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
//int8_t oppWLLampNextScene()
//{
//    uint8_t bri = oppData.oppLiveData.wlLampStatus.ucBri;
//
//    if ( !oppData.oppLiveData.wlLampStatus.ucSwitch ) return -1;
//    if ( oppData.oppLiveData.ucLampSceneMode == 0 ) {
//        oppData.oppLiveData.ucLampCctSceneNo = (oppData.oppLiveData.ucLampCctSceneNo + 1) % OPP_WL_LAMP_CCT_SCENE_NUM;
//        oppData.oppLiveData.wlLampStatus = oppData.oppCfgData.wlLampCctScene[oppData.oppLiveData.ucLampCctSceneNo];
//        oppData.oppLiveData.wlLampStatus.ucBri = bri;
//        oppLampCallSceneCtrl(oppLampInfo.lampId, 20, oppData.oppLiveData.wlLampStatus);
//    } else if ( oppData.oppLiveData.ucLampSceneMode == 1 ){
//        oppData.oppLiveData.ucLampRgbSceneNo = (oppData.oppLiveData.ucLampRgbSceneNo + 1) % OPP_WL_LAMP_RGB_SCENE_NUM;
//        oppData.oppLiveData.wlLampStatus = oppData.oppCfgData.wlLampRgbScene[oppData.oppLiveData.ucLampRgbSceneNo];
//        oppData.oppLiveData.wlLampStatus.ucBri = bri;
//        oppLampCallSceneCtrl(oppLampInfo.lampId, 20, oppData.oppLiveData.wlLampStatus);
//    } else {
//        oppLampInfo.lampScene2.mode = (oppLampInfo.lampScene2.mode >= 1 ? 0 : oppLampInfo.lampScene2.mode + 1);
//    }
//    return 0;
//}

/******************************************************************************
 Function    : rlLampBriCtrl
 Description : reading lamp bright control
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampBriCtrl(uint16_t uwBri, uint32_t ulPeroidMs)
{
    myLampParam_t *pLampParam = &s_stCurLampParam;

    my_hal_log_info("rl lamp bright ctrl %d\r\n", uwBri);
    pLampParam->uwBri = APP_RANGE(uwBri, LIGHT_BRIGHT_MIN, LIGHT_BRIGHT_MAX);
    return myLampBriCtrl(gucLampId, ulPeroidMs, pLampParam->uwBri);
}

/******************************************************************************
 Function    : rlLampBriPercentCtrl
 Description : reading lamp bright percent control
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampBriPercentCtrl(uint8_t ucPercent, uint32_t ulPeroidMs)
{
    myLampParam_t *pLampParam = &s_stCurLampParam;

    
    ucPercent = APP_MIN_VAL(ucPercent, 100);
    pLampParam->uwBri = LIGHT_PERCENT_TO_BRIGHT(ucPercent);
    pLampParam->uwBri = APP_RANGE(pLampParam->uwBri, LIGHT_BRIGHT_MIN, LIGHT_BRIGHT_MAX);
    my_hal_log_info("rl lamp bright ctrl percent %d value %d\r\n", ucPercent, pLampParam->uwBri);
    return myLampBriCtrl(gucLampId, ulPeroidMs, pLampParam->uwBri);
}

/******************************************************************************
 Function    : oppWLLampBriClassCtrl
 Description : opple lamp bright class control
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
//int8_t oppWLLampBriClassCtrl(oppWLLampBriClass_t briClass, uint32_t ulStepNum)
//{
//    uint8_t ucBri = gcucBri[briClass];
//
//    if ( !oppData.oppLiveData.wlLampStatus.ucSwitch )  return -1;
//    oppTaskLampLog("oppLamp bright class ctrl %d\r\n", briClass);
//    oppLampBriCtrl(oppLampInfo.lampId, ulStepNum, ucBri);
//    oppLampStatusGet(oppLampInfo.lampId, &oppData.oppLiveData.wlLampStatus);
//    return 0;
//}

/******************************************************************************
 Function    : rlLampBriCtrlNextClass
 Description : 灯获取下一等级亮度值
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampBriCtrlNextClass(uint32_t ulPeroidMs)
{
    int8_t ret = 0;
    uint16_t uwNewBri = 0;
    uint32_t percent = 0;
    myLampParam_t *pLampParam = &s_stCurLampParam;
    uint8_t ucBriDirection = rlFlagGet(RL_FLAG_LAMP_BRI_DIRECTION);

    percent = LIGHT_BRIGHT_TO_PERCENT(pLampParam->uwBri);
    percent = rlLampGetNextClassVal(ucBriDirection, percent,  k_a_uwBriPercent, sizeof(k_a_uwBriPercent) / sizeof(uint32_t));
    uwNewBri = LIGHT_PERCENT_TO_BRIGHT(percent);
    uwNewBri = APP_RANGE(uwNewBri, LIGHT_BRIGHT_MIN, LIGHT_BRIGHT_MAX);
    if (uwNewBri != pLampParam->uwBri) {
        pLampParam->uwBri = uwNewBri;
        my_hal_log_info("rl lamp bright ctrl next %d\r\n", pLampParam->uwBri);
        ret = myLampBriCtrl(gucLampId, ulPeroidMs, pLampParam->uwBri);
    }
    return ret;
}

/******************************************************************************
 Function    : oppLampGetBriClassValue
 Description : opple lamp get bri class value
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
//uint32_t oppLampGetBriClassValue(oppWLLampBriClass_t briClass)
//{
//    if ( briClass >= OPP_WL_LAMP_BRI_CLASS_NUM ) {
//        return 0;
//    }
//    return gcucBri[briClass];
//}

/******************************************************************************
 Function    : rlLampCctCtrl
 Description : 灯cct控制模块
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampCctCtrl(uint16_t uwCCT, uint32_t ulPeroidMs)
{
#if defined(APP_DEV_CCT_SUPPORT) && (APP_DEV_CCT_SUPPORT != 0)
    myLampParam_t *pLampParam = &s_stCurLampParam;

    if ( !pLampParam->ucSwitch )  return -1;
    my_hal_log_info("sl lamp cct ctrl %d\r\n", uwCCT);
    pLampParam->uwCCT = APP_RANGE(uwCCT, LIGHT_COLOR_CCT_MIN, LIGHT_COLOR_CCT_MAX); 
    myLampCCTCtrl(gucLampId, ulPeroidMs, pLampParam->uwCCT, pLampParam->uwBri);
#endif
    return 0;
}

/******************************************************************************
 Function    : rlLampLightModeCtrl
 Description : 灯lightmode控制模块
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampLightModeCtrl(uint8_t lightMode, uint32_t ulPeroidMs)
{
#if (APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_READING || APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_READING_L )
    _lampReadingLightModeCtrl(lightMode, ulPeroidMs);
#elif (APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_CEILING)
    _lampCeilingLightModeCtrl(lightMode, ulPeroidMs);
#elif (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT_PTJX)
    _lampNightLightModeCtrl(lightMode, ulPeroidMs);
#endif
}


/******************************************************************************
 Function    : rlLampBlinkCtrl
 Description : reading lamp blink ctrl
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampBlinkCtrl(uint32_t blinkCnt, uint32_t ulPeroidMs, uint8_t lock, uint32_t arg) 
{
    return myLampFlashCtrl(gucLampId, ulPeroidMs/2, ulPeroidMs, blinkCnt, lock, arg);
}

/******************************************************************************
 Function    : slLampPowerOnHook
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slLampPowerOnHook(void) 
{
    myLampParam_t *pCurLampParam = &s_stCurLampParam;
    myLampParam_t *pSaveLampParam = &g_stRlData.saveData.stLampSaveParam;

#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_CEILING) //celling
//  memcpy((void *)&lampParam, (void*)&g_stRlData.saveData.stLampSaveParam, sizeof(myLampParam_t)); 
    pCurLampParam->ucSwitch = 1;
    pCurLampParam->uwBri = pSaveLampParam->uwBri;
    pCurLampParam->uwCCT = pSaveLampParam->uwCCT;

    if (pCurLampParam->uwBri == 0) {
        pCurLampParam->uwBri = 65535;
    }
    if (pCurLampParam->uwCCT < LIGHT_COLOR_CCT_MIN || pCurLampParam->uwCCT > LIGHT_COLOR_CCT_MAX) {
        pCurLampParam->uwCCT = LIGHT_COLOR_CCT_MID;
    }
    if (g_stRlData.saveData.ucWallSwitchOperCnt > 0) {
        g_stRlData.saveData.ucLampWallSwitchMode = 
            (++g_stRlData.saveData.ucLampWallSwitchMode >= RL_LAMP_WALL_SWITCH_MODE_NUM) ? 
            RL_LAMP_WALL_SWITCH_MODE_1 : g_stRlData.saveData.ucLampWallSwitchMode;
        pCurLampParam->uwBri = k_stSlLampWallSwitchMode[g_stRlData.saveData.ucLampWallSwitchMode].uwBri;
        pCurLampParam->uwCCT = k_stSlLampWallSwitchMode[g_stRlData.saveData.ucLampWallSwitchMode].uwCct;
        my_hal_log_info("lamp start with wall switch mode %d\r\n", g_stRlData.saveData.ucLampWallSwitchMode);
    }
    else {
        my_hal_log_info("lamp start with last status\r\n");
    }
    rlLampDevStatusCtrl(pCurLampParam, g_stRlData.liveData.uwAdjDuration);
#endif
}

/******************************************************************************
 Function    : rlLampGetStatus
 Description : opple lamp get status
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int8_t rlLampGetStatus(uint8_t *pOnoff, uint16_t *pBright, uint16_t *pCct) 
{
    myLampParam_t *pLampParam = &s_stCurLampParam;

    if (pOnoff) {
        *pOnoff = pLampParam->ucSwitch;
    }

    if (pBright) {
        *pBright = pLampParam->uwBri;
    }

    if (pCct) {
        *pCct = pLampParam->uwCCT;
    }

    return 0;
}

/******************************************************************************
 Function    : rlLampGetOnoff
 Description : opple lamp get onoff
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlLampGetOnoff(void) 
{
    return s_stCurLampParam.ucSwitch;
}

/******************************************************************************
 Function    : rlLampGetBrightness
 Description : opple lamp get brightness
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint16_t rlLampGetBrightness(void) 
{
    return s_stCurLampParam.uwBri;
}

/******************************************************************************
 Function    : rlLampGetCCT
 Description : opple lamp get cct
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint16_t rlLampGetCCT(void) 
{
    return s_stCurLampParam.uwCCT;
}

/******************************************************************************
 Function    : rlLampGetBriPercent
 Description : reading lamp get bright percent
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlLampGetBriPercent(void) 
{
    return (uint8_t)LIGHT_BRIGHT_TO_PERCENT(s_stCurLampParam.uwBri);
}

/******************************************************************************
 Function    : rlLampGetLightMode
 Description : reading lamp get light mode
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint8_t rlLampGetLightMode(void) 
{
    return s_stCurLampParam.ucSceneNo;
}

static int night_gpio_init(void)
{
    halIOInit(HAL_IO_PA17, HAL_IO_MODE_IN_PULLDOWN, HAL_IO_IRQ_NULL, NULL);
    halIOInit(HAL_IO_PA19, HAL_IO_MODE_IN_PULLDOWN, HAL_IO_IRQ_NULL, NULL);

    return 0;
}

/******************************************************************************
 Function    : rlTaskLampInit
 Description : reading lamp thread lamp init
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static int rlTaskLampInit(void* arg)
{
    myLampParam_t *pLampParam = &s_stCurLampParam;

    pLampParam->minPwm = LIGHT_PWM_MIN;
    pLampParam->maxPwm = LIGHT_PWM_MAX;
    pLampParam->ucCtrlType = 0;
    pLampParam->ucSwitch = 0;
    pLampParam->uwBri = 65535;
    pLampParam->uwCCT = LIGHT_CCT_DEFAULT;
    pLampParam->ucSceneNo = g_stRlData.saveData.stLampSaveParam.ucSceneNo;
    
    myHalPwmInit(RL_PWM_W, LIGHT_PWM_FREQ, LIGHT_PWM_MAX);
    myHalPwmInit(RL_PWM_C, LIGHT_PWM_FREQ, LIGHT_PWM_MAX);
    myHalPwmOutput(RL_PWM_W, 0);
    myHalPwmOutput(RL_PWM_C, 0);
    
    myLampInit(10, NULL, NULL, NULL);
    myLampCreateLamp(&gucLampId, 
                     pLampParam, 
                     _lampPwmOutput, 
                     _lampColor2Pwm, 
                     getMyDimmingCurve(LIGHT_PWM_CURVE), 
                     NULL);
    my_hal_log_info("create lamp switch %d max pwm is %d\r\n", 
                 pLampParam->ucSwitch, LIGHT_PWM_MAX);
    myLampRegisterDimmingStatusCtrlHook(gucLampId, _lampStateControlHook);
    myLampRegisterFlashEndHook(gucLampId, _lampBlinkEndHook);

    #if (APP_DEV_TYPE_USED != APP_DEV_TYPE_LAMP_NIGHT && APP_DEV_TYPE_USED  != APP_DEV_TYPE_LAMP_NIGHT_PTJX)
    s_resetWindowTimerHandle = xTimerCreate((const char*)"reset", (30000 / portTICK_RATE_MS), 0, NULL, _lampFactoryResetWindowTimeoutHandle);
    xTimerStart(s_resetWindowTimerHandle, 0);
    #else
    extern void _lampctrlfunc(xTimerHandle pxTimer);
    night_gpio_init();
    s_lampctrlTimer = xTimerCreate((const char*)"lampctrl", (1000 / portTICK_RATE_MS), True, NULL, _lampctrlfunc);
    xTimerStart(s_lampctrlTimer, 0);
    uint8_t lightMode = CL_LAMP_LIGHT_MODE_BOTHSIDE;
    rlLampLightModeCtrl((uint8_t)lightMode, 0);
    lampctrl_cnt_set(3);
    uint8_t duration = rlDataGetAdjDuration();
    rlDataSetAdjDuration(duration);
    nlSetMutex = xSemaphoreCreateMutex();

    if (nlSetMutex == NULL) {
        my_hal_log_error("nlSetMutex create failed\r\n");
        while (1) {
            ;
        }
    }
    #endif
    my_hal_log_info("task 'lamp' init ok\r\n");
    return 0;
}

/******************************************************************************
 Function    : oppWLTaskLamp
 Description : opple thread lamp
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void rlTaskLamp(void *arg)
{
    rlFlagSet(RL_FLAG_TASK_LAMP_RUNNING, 1);     
    if(0 > rlTaskLampInit(arg)) {
        rlFlagSet(RL_FLAG_TASK_LAMP_RUNNING, 0);
    }

#if defined(APP_TASK_FCT_USE) && (APP_TASK_FCT_USE == 1)
    if(0 > rlTaskFctInit(arg)) {
        rlFlagSet(RL_FLAG_TASK_FCT_RUNNING, 0);
    }
#endif

    while (rlFlagGet(RL_FLAG_TASK_LAMP_RUNNING)) {      
        vTaskDelay(10);

        myLampLoop();

#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_NIGHT || APP_DEV_TYPE_USED  == APP_DEV_TYPE_LAMP_NIGHT_PTJX)
        extern void night_lamp_human_state_check(void);
        night_lamp_human_state_check();
#endif

#if defined(APP_TASK_FCT_USE) && (APP_TASK_FCT_USE == 1)
        LocalShellProc();
#endif

        _lampReportProc();

        if (rlFlagGet(RL_FLAG_SYS_FACTORY_RESET)) {
            _lampFactoryReset();
        }

        if (rlFlagGet(RL_FLAG_SYS_CFG_DATA_SAVE)) {
            rlFlagSet(RL_FLAG_SYS_CFG_DATA_SAVE, 0);
            rlDataWriteConfigData();
        }
    }

    my_hal_log_info("task 'lamp' delete\r\n");
    vTaskDelete(NULL);
}

#endif //APP_TASK_LAMP_USE

#ifdef __cplusplus
}
#endif


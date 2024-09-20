/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slTaskFCT.c                                                   *
* Description :                                                               *
*               smart lamp task fct source file                               *
* Note        : (none)                                                        *
* Author      : Kris Li                                                       *
* Date:       : 2022-09-27                                                    *
* Mod History : (none)                                                        *
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "slTaskFCT.h"
#if defined(APP_TASK_FCT_USE) && (APP_TASK_FCT_USE == 1)
#include "slData.h"
#include "atShell.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define FCT_SHELL_UART                                     MY_HAL_UART_0
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/

/******************************************************************************
*                             Declarations                                    *
******************************************************************************/
static void _fct_at_echo_Callback(int argc, char ** argv);
static void _fct_at_ga_Callback(int argc, char ** argv);
static void _fct_at_mcuInit_Callback(int argc, char ** argv);
static void _fct_at_version_Callback(int argc, char ** argv);
static void _fct_at_sdkversion_Callback(int argc, char ** argv);
static void _fct_at_mac_Callback(int argc, char ** argv);
static void _fct_at_FctInfoWrite_Callback(int argc, char ** argv);
static void _fct_at_pwm_Callback(int argc, char ** argv);
static void _fct_at_WIFI_Callback(int argc, char ** argv);
static void _fct_at_SNWrite_Callback(int argc, char ** argv);
static void _fct_at_SNRead_Callback(int argc, char ** argv);
static void _fct_at_exit_Callback(int argc, char ** argv);
static void _fct_at_enterFCT_Callback(int argc, char ** argv);
/******************************************************************************
*                                Globals                                      *
******************************************************************************/
const stSubCommandTable_t k_stMllCommandTable[] = 
{
    {"echo", _fct_at_echo_Callback},
    {"ga", _fct_at_ga_Callback},
    {"mcuInit", _fct_at_mcuInit_Callback},
    {"enterFCT", _fct_at_enterFCT_Callback},
    {"version", _fct_at_version_Callback},
    {"sdkversion", _fct_at_sdkversion_Callback},
    {"mac", _fct_at_mac_Callback},
    {"FCTinfoWrite", _fct_at_FctInfoWrite_Callback},
    {"pwm", _fct_at_pwm_Callback},
    {"WIFI", _fct_at_WIFI_Callback},
    {"SNWrite", _fct_at_SNWrite_Callback},
    {"SNRead", _fct_at_SNRead_Callback},
    {"exit", _fct_at_exit_Callback},
};
/******************************************************************************
*                          Extern Declarations                                *
******************************************************************************/

/******************************************************************************
*                            Implementations                                  *
******************************************************************************/
static int _fct_send(myHalUart_t uart, uint8_t* pucData, uint32_t ulDataLen) 
{
    int ret = 0;

    if (uart >= MY_HAL_UART_NUM || !pucData || ulDataLen == 0) {
        return 0;
    }

    while (0 == ret) {
        vTaskDelay(2);
        ret = myHalUartSend(uart, pucData, ulDataLen);
    }
    return ret;
}

/******************************************************************************
 Function    : _fct_at_echo_Callback
 Description : sub command "echo" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_echo_Callback(int argc, char **argv) 
{
    if (argc == 2) {
        int enable = atoi(argv[1]);
        Printf("echo enable set %d\r\n", enable);
        localShellSetEcho(enable);
    }  
}

/******************************************************************************
 Function    : _fct_at_ga_Callback
 Description : sub command "ga" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_ga_Callback(int argc, char ** argv)
{
    Printf("ga param count %d\r\n", argc);
    for (int i=0; i<argc; i++) {
        Printf("ga param%d %s\r\n", i, argv[i]);
    }
}

/******************************************************************************
 Function    : _fct_at_mcuInit_Callback
 Description : sub command "mcuInit" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_mcuInit_Callback(int argc, char **argv)
{
    Printf("Test mcuInit OK\r\n");
}

/******************************************************************************
 Function    : _fct_at_enterFCT_Callback
 Description : sub command "enterFCT" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_enterFCT_Callback(int argc, char **argv) 
{
    Printf("Test enterFCT OK\r\n");
    g_stRlData.fctData.fctMode = 1;
    rlDataWriteFctData();
    vTaskDelay(10);
    sys_reset();
}

/******************************************************************************
 Function    : _fct_at_version_Callback
 Description : sub command "version" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_version_Callback(int argc, char ** argv)
{
    if (g_stRlData.fctData.fctMode != 0) {
        Printf("Test version PID:B524,VERSION:%s %s\r\n", RL_FIRMWARE_VER);
    }  
}

/******************************************************************************
 Function    : _fct_at_sdkversion_Callback
 Description : sub command "sdkversion" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_sdkversion_Callback(int argc, char ** argv)
{
    char sdkversion[32] = {0};

    if (g_stRlData.fctData.fctMode != 0) {
        MagicLinkGetSDKVersion(sdkversion, 32);
        Printf("Test sdkversion %s\r\n", sdkversion);
    }
}

/******************************************************************************
 Function    : _fct_at_mac_Callback
 Description : sub command "mac" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_mac_Callback(int argc, char ** argv)
{
    char mac[32] = { 0 };

    if (g_stRlData.fctData.fctMode != 0) {
        myHalWifiGetMacAddr(mac, 32);
        Printf("Test mac %s\r\n", mac);
    }
}

/******************************************************************************
 Function    : _fct_at_FctInfoWrite_Callback
 Description : sub command "FCTinfoWrite" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_FctInfoWrite_Callback(int argc, char ** argv)
{
    if (g_stRlData.fctData.fctMode != 0) {
        if (argc == 2) {
            g_stRlData.fctData.fctOk = atoi(argv[1]);
            rlDataWriteFctData();
            Printf("Test FCTinfoWrite %s OK\r\n", argv[1]);
        } 
    }
}

/******************************************************************************
 Function    : _fct_at_pwm_Callback
 Description : sub command "pwm" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_pwm_Callback(int argc, char ** argv)
{
    if (g_stRlData.fctData.fctMode != 0) {
        if (argc == 2) {
            int pwm = atoi(argv[1]);
            if (pwm <= 1000 && pwm >= 0) {
                pwm *= 10;
                myHalPwmOutput(MY_HAL_PWM_0, pwm);
                myHalPwmOutput(MY_HAL_PWM_4, pwm);
                Printf("Test pwm %s OK\r\n", argv[1]);
            }
        }
        else if (argc == 3) {
            int pwm1 = atoi(argv[1]);
            int pwm2 = atoi(argv[2]);
            if ((pwm1 <= 1000 && pwm1 >= 0) && (pwm2 <= 1000 && pwm2 >= 0)) {
                pwm1 *= 10;
                pwm2 *= 10;
                myHalPwmOutput(MY_HAL_PWM_0, pwm1);
                myHalPwmOutput(MY_HAL_PWM_4, pwm2);
                Printf("Test pwm %s %s OK\r\n", argv[1], argv[2]);
            }
        }
    }
}

/******************************************************************************
 Function    : _fct_at_WIFI_Callback
 Description : sub command "WIFI" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_WIFI_Callback(int argc, char ** argv)
{
    uint8_t timeout = 0;

    if (g_stRlData.fctData.fctMode != 0) {
        if (argc == 3 && strlen(argv[2]) >= 8) {
            myHalWifiStationConnect(argv[1], strlen(argv[1]), argv[2], strlen(argv[2]));
            while (timeout++ < 19 && 0 == myHalWifiGetConnectStatus()) {
                vTaskDelay(1000);
            }
            if (timeout < 20) {
                Printf("Test WIFI %s %s OK\r\n", argv[1], argv[2]);
            }
            else {
                Printf("Test WIFI %s %s Fail\r\n", argv[1], argv[2]);
            }
        }  
    }
}

/******************************************************************************
 Function    : _fct_at_SNWrite_Callback
 Description : sub command "SNWrite" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_SNWrite_Callback(int argc, char ** argv)
{
    if (g_stRlData.fctData.fctMode != 0) {
        if (argc == 2) {
            if (strlen(argv[1]) < 32) {
                strcpy(g_stRlData.fctData.sn, argv[1]);
                rlDataWriteFctData();
                Printf("Test SNWrite %s OK\r\n", argv[1]);
            }
        }
    }
}

/******************************************************************************
 Function    : _fct_at_SNRead_Callback
 Description : sub command "SNRead" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_SNRead_Callback(int argc, char ** argv)
{
    char sn[32] = { 0 };

    if (g_stRlData.fctData.fctMode != 0) {
//      myHalWifiGetMacAddr(sn, 32);
        Printf("Test SNRead %s\r\n", g_stRlData.fctData.sn);
    }
}

/******************************************************************************
 Function    : _fct_at_exit_Callback
 Description : sub command "exit" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _fct_at_exit_Callback(int argc, char **argv) 
{
    if (g_stRlData.fctData.fctMode != 0) {
        Printf("Test exit OK\r\n");
        g_stRlData.fctData.fctMode = 0;
        rlDataWriteFctData();
    }
}

/******************************************************************************
 Function    : mllShellInit
 Description : local shell init
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int rlTaskFctInit(void* arg)
{
    localShellRegisterCmd("Test", k_stMllCommandTable, APP_ARRAY_SIZE(k_stMllCommandTable));
    myHalUartInit(FCT_SHELL_UART, 115200, NULL, MY_HAL_UART_MODE_DMA);
    LocalShellInit(FCT_SHELL_UART, _fct_send, myHalUartRecv, 0);
    my_hal_log_info("task 'fct' init ok\r\n");
    return 0;
}

/******************************************************************************
 Function    : rlTaskFct
 Description : reading lamp fct task
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void rlTaskFct(void* arg)
{
    rlFlagSet(RL_FLAG_TASK_FCT_RUNNING, 1);     
    if(0 > rlTaskFctInit(arg)) {
        rlFlagSet(RL_FLAG_TASK_FCT_RUNNING, 0);
    }

    while (rlFlagGet(RL_FLAG_TASK_FCT_RUNNING)) {      
        vTaskDelay(10);

        LocalShellProc();
    }

    my_hal_log_info("task 'fct' delete\r\n");
    vTaskDelete(NULL);
}

#endif //#if (APP_TASK_FCT_USE)

#ifdef __cplusplus
}
#endif


/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : atcmd_user.c                                                  *
* Description :                                                               *
*               atcmd user source file                                        *
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
#include "atcmd_user.h"
#include "magiclink_log.h"
#include "wifi_conf.h"
#include "wifi_util.h"
#include "netif.h"
#include "slProbe.h"
#include "slTaskPower.h"
#include "slTaskSerial.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/

/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
typedef struct
{
    const char *pchCmd;
    void (*pfnSubCmdCallback)(int argc, char **argv);
} stSubCommandTable_t;
/******************************************************************************
*                             Declarations                                    *
******************************************************************************/
static void fATUU(void *arg);
static void _at_sub_ga_Callback(int argc, char **argv);
static void _at_sub_sa_Callback(int argc, char **argv);
static void _at_sub_firm_Callback(int argc, char **argv);
static void _at_sub_blk_Callback(int argc, char **argv);
static void _at_sub_log_Callback(int argc, char **argv);
static void _at_sub_pwr_Callback(int argc, char **argv);
static void _at_sub_sta_Callback(int argc, char **argv);

#if 0
static void _at_sub_sta_Callback(int argc, char **argv);
static void _at_sub_ap_Callback(int argc, char **argv);
static void _at_sub_preg_Callback(int argc, char **argv);
static void _at_sub_preq_Callback(int argc, char **argv);
static void _at_sub_prsp_Callback(int argc, char **argv);
#endif
/******************************************************************************
*                                Globals                                      *
******************************************************************************/
log_item_t at_user_items[] = {
    {"ATUU", fATUU, {NULL,NULL}},
};

const stSubCommandTable_t k_stSubCommandTable[] = 
{
    "ga", _at_sub_ga_Callback,
    "sa", _at_sub_sa_Callback,
    "firm", _at_sub_firm_Callback,
    "blk", _at_sub_blk_Callback,
    "log", _at_sub_log_Callback,
    "pwr", _at_sub_pwr_Callback,
    "sta", _at_sub_sta_Callback,
#if 0
    "ap", _at_sub_ap_Callback,
    "sta", _at_sub_sta_Callback,   
    "preg", _at_sub_preg_Callback,
    "preq", _at_sub_preq_Callback,
    "prsp", _at_sub_prsp_Callback,
#endif
};
/******************************************************************************
*                          Extern Declarations                                *
******************************************************************************/

/******************************************************************************
*                            Implementations                                  *
******************************************************************************/
/******************************************************************************
 Function    : _at_sub_ga_Callback
 Description : sub command "ga" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_ga_Callback(int argc, char **argv)
{
    char sn[32] = { 0 };

    myHalWifiGetMacAddr(sn, 32);

    printf("device mac: %s\r\n", sn);
    printf("wifi sta status:%d\r\n", myHalWifiGetConnectStatus());

    if (0 != myHalWifiGetConnectStatus()) {
        memset(sn, 0, 32);
        wext_get_ssid(WLAN0_NAME, (uint8_t*)sn);
        printf("ssid:%s\r\n", sn);

        extern struct netif xnetif[NET_IF_NUM];
        uint8_t *pIp = LwIP_GetIP(&xnetif[0]);
        memset(sn, 0, 32);
        sprintf(sn, "%d.%d.%d.%d", pIp[0], pIp[1], pIp[2], pIp[3]);
        printf("ip:%s\r\n", sn);
    }
}

/******************************************************************************
 Function    : _at_sub_sa_Callback
 Description : sub command "ga" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_sa_Callback(int argc, char **argv)
{
    if (argc > 0) {
        printf("set mac:%s\r\n", argv[0]);
        wifi_set_mac_address(argv[0]);
    }
    else {
        printf("set mac: EC0BAEEC1B82\r\n");
        wifi_set_mac_address("EC0BAEEC1B82");
    }
}

/******************************************************************************
 Function    : _at_sub_firm_Callback
 Description : sub command "firm" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_firm_Callback(int argc, char **argv)
{
    int index = 0;

    if (argc > 0) {
        index = atoi(argv[0]);
        printf("cmd ota image %d\r\n", index);
        extern void cmd_ota_image(bool cmd);
        cmd_ota_image((bool)index);
    }
}

/******************************************************************************
 Function    : _at_sub_blk_Callback
 Description : sub command "blk" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_blk_Callback(int argc, char **argv) 
{
#if defined(APP_TASK_LAMP_USE) && (APP_TASK_LAMP_USE == 1)
    int8_t ret = 0;

    switch (argc) {
    case 0:
        ret = rlLampBlinkCtrl(3, 1000, 1, 0);
        break;
    case 1:
        ret = rlLampBlinkCtrl((uint32_t)atoi(argv[0]), 1000, 1, 0);
        break;
    case 2:
        ret = rlLampBlinkCtrl((uint32_t)atoi(argv[0]), 1000, 1, (uint32_t)atoi(argv[1]));
        break;
    case 3:
        ret = rlLampBlinkCtrl((uint32_t)atoi(argv[0]), 1000, (uint8_t)atoi(argv[2]), (uint32_t)atoi(argv[1]));
        break;
    default:
        ret = rlLampBlinkCtrl((uint32_t)atoi(argv[0]), (uint32_t)atoi(argv[3]), (uint8_t)atoi(argv[2]), (uint32_t)atoi(argv[1]));
        break;
    }
    printf("rlLampBlinkCtrl ret=%d\r\n", ret);
#endif
}

/******************************************************************************
 Function    : _at_sub_log_Callback
 Description : sub command "log" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_log_Callback(int argc, char **argv) 
{
    if (argc == 0) {
        printf("current magic log level:%d\r\n", MagicLinkGetLogLevel());
        printf("current app   log level:%d\r\n", myHalLogGetLevel());
    }
    else if(argc == 2){
        if (atoi(argv[0]) == 0) {
            printf("set magic log level:%d\r\n", atoi(argv[1]));
            MagicLinkSetLogLevel((enum MagicLinkLogLevel)atoi(argv[1]));
        }
        else {
            printf("set app log level:%d\r\n", atoi(argv[1]));
            myHalLogSetLevel((uint8_t)atoi(argv[1]));
        }
    }
    else {
        printf("[ATUU=pwr]Usage: ATUU=log,1,1\n\r");
    }
}

/******************************************************************************
 Function    : _at_sub_pwr_Callback
 Description : sub command "pwr" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_pwr_Callback(int argc, char **argv) 
{
    if (argc == 0) {
        printf("[ATUU=pwr]Usage: ATUU=pwr,1\n\r");
    }
    else {
        int powerFlag = atoi(argv[0]);

        printf("set power flag:%d\r\n", powerFlag);
        slPowerPulseContrlSim(powerFlag);
    }
}

/******************************************************************************
 Function    : _at_sub_sta_Callback
 Description : sub command "pwr" 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_sta_Callback(int argc, char **argv) 
{
#if (APP_DEV_TYPE_USED == APP_DEV_TYPE_LAMP_BEDSIDE)
    extern int slLampSerialGetStatusCmd(void);
    slLampSerialGetStatusCmd();
#endif
}

#if 0
/******************************************************************************
 Function    : _at_sub_ap_Callback
 Description : sub command "ap 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_ap_Callback(int argc, char **argv) 
{
    int ret = 0;

    switch (argc) {
    case 1:
        if (1 == atoi(argv[0])) {
            ret = myHalWifiStartSoftAP("ap_test", "12345678");
        }
        else if (2 == atoi(argv[0])) {
            myHalWifiStartStaAp("ap_test", "12345678");
        }
        break;
    case 2:
        if (1 == atoi(argv[0])) {
            ret = myHalWifiStartSoftAP(argv[1], NULL);
        }
        else if (2 == atoi(argv[0])) {
            myHalWifiStartStaAp(argv[1], NULL);
        }
        break;
    case 3:
        if (1 == atoi(argv[0])) {
            ret = myHalWifiStartSoftAP(argv[1], argv[2]);
        }
        else if (2 == atoi(argv[0])) {
            myHalWifiStartStaAp(argv[1], argv[2]);
        }
        break;
    default:
        ret = -100;
        break;
    }
    printf("_at_sub_ap_Callback ret=%d\r\n", ret);
}

/******************************************************************************
 Function    : _at_sub_sta_Callback
 Description : sub command "ap 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_sta_Callback(int argc, char **argv) 
{
    int ret = -1;

    if (argc == 1) {
        if (0 != atoi(argv[0])) {
            ret = myHalWifiStationConnect("Xiaomi", strlen("Xiaomi"), "12345678", strlen("12345678"));
        }
        else {
            ret = wifi_disconnect();
        }
    }
    else if (argc == 2) {
        ret = myHalWifiStationConnect(argv[0], strlen(argv[0]), argv[1], strlen(argv[1]));
    }
    printf("_at_sub_sta_Callback ret=%d\r\n", ret);
}
//TODO 重连
/******************************************************************************
 Function    : _at_sub_preg_Callback
 Description : sub command "preg 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_preg_Callback(int argc, char **argv) 
{
    startLowerPowerAp();
}

/******************************************************************************
 Function    : _at_sub_preq_Callback
 Description : sub command "preq 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_preq_Callback(int argc, char **argv) 
{
    static uint8_t buf[1024] = { 0 };

    if (argc == 0) {
        slProbeSendRequest(WLAN0_NAME, "ap_test", strlen("ap_test"), buf, 0);
    }
    else if (argc == 1) {
        slProbeSendRequest(argv[0], "ap_test", strlen("ap_test"), buf, 0);
    }
    else if (argc == 2) {
        int len = atoi(argv[1]);

        memset(buf, 0x11, len);
        slProbeSendRequest(argv[0], "ap_test", strlen("ap_test"), buf, len);
    }
    else if (argc == 3) {
        int len = atoi(argv[1]);
        int interval = atoi(argv[2]);

        for (int i = 0; i < 10; i++) {
            memset(buf, i+1, len);
            slProbeSendRequest(argv[0], "ap_test", strlen("ap_test"), buf, len);
            vTaskDelay(interval);
        }
    }
}

/******************************************************************************
 Function    : _at_sub_prsp_Callback
 Description : sub command "prsp 处理函数
 Note        : (none)
 Input Para  : (none)
               (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _at_sub_prsp_Callback(int argc, char **argv) 
{

}
#endif

/******************************************************************************
 Function    : appTaskEntry
 Description : app task entry
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void user_cmd_app(int argc, char **argv) 
{
    char *subCmdStr = argv[1];

    if (argc < 2) {
        printf("please input sub cmd\r\n");
        return;
    }

    // sub command callback
    for (int i = 0; i < sizeof(k_stSubCommandTable)/sizeof(stSubCommandTable_t); i++) {
        if (0 == strncmp(subCmdStr, k_stSubCommandTable[i].pchCmd, strlen(subCmdStr))) {
            k_stSubCommandTable[i].pfnSubCmdCallback(argc-2, &argv[2]);
            return;
        }
    }
    printf("unknown sub command\r\n");
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
static void fATUU(void *arg)
{
    int argc;
    char *argv[MAX_ARGC] = {0};
    char buf[256] = {0};

	if(!arg){
		printf("[ATUU]Usage: ATU1=USER CMD\n\r");
		goto exit;
	}
	if(strlen((char*)arg) > 32){
		printf("[ATUU]Error: USER CMD length can't exceed 32\n\r");
		goto exit;
	}
//  atcmduserLog("[ATUU]: USER CMD [%s]\n\r", (char*)arg);
    strncpy(buf, arg, sizeof(buf));
    argc = parse_param(buf, argv);
    user_cmd_app(argc, argv);	
exit:
	return;
}

/******************************************************************************
 Function    : at_user_init
 Description : at user init
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void at_user_init(void) 
{
    log_service_add_table(at_user_items, sizeof(at_user_items)/sizeof(at_user_items[0]));
}

#ifdef __cplusplus
}
#endif


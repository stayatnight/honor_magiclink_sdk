#include "osal/osal.h"
#include "utils/debug/log.h"
#include "utils/debug/ln_assert.h"
#include "utils/system_parameter.h"
#include "utils/ln_psk_calc.h"
#include "utils/power_mgmt/ln_pm.h"
#include "utils/sysparam_factory_setting.h"
#include "wifi.h"
#include "wifi_port.h"
#include "netif/ethernetif.h"
#include "wifi_manager.h"
#include "lwip/tcpip.h"
#include "drv_adc_measure.h"
#include "hal/hal_adc.h"
#include "hal/hal_flash.h"
#include "ln_nvds.h"
#include "ln_wifi_err.h"
#include "ln_misc.h"
#include "ln882h.h"

#include "rwip_config.h"
#include "llm_int.h"

#include "ln_ble_gap.h"
#include "ln_ble_gatt.h"
#include "usr_app.h"
#include "usr_ble_app.h"
#include "ln_ble_app_kv.h"

#include "magiclink.h"
#include "demo.h"

static OS_Thread_t g_usr_app_thread;
static OS_Thread_t g_usr_test_thread;
#define USR_APP_TASK_STACK_SIZE   4800 //Byte

#define WIFI_TEMP_CALIBRATE             1

#if WIFI_TEMP_CALIBRATE
static OS_Thread_t g_temp_cal_thread;
#define TEMP_APP_TASK_STACK_SIZE   4*256 //Byte
#define TEST_TASK_SIZE 4*256
#endif

/* declaration */
static void wifi_init_ap(void);
static void wifi_init_sta(void);
static void usr_app_task_entry(void *params);
static void temp_cal_app_task_entry(void *params);

static uint8_t mac_addr[6]        = {0};
static uint8_t psk_value[40]      = {0x0};
static uint8_t ssid_value[33]      = {0x0};
static uint8_t target_sta_bssid[6] = {0x0};
static uint8_t target_sta_pwd[65] = { 0x0 };

wifi_sta_connect_t connect = {
    .ssid    = ssid_value,
    .pwd     = target_sta_pwd,
    .bssid   = target_sta_bssid,
    .psk_value = psk_value,
};

wifi_scan_cfg_t scan_cfg = {
        .channel   = 0,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time = 20,
};

wifi_softap_cfg_t ap_cfg = {
    .ssid            = "LN_AP_8899",
    .pwd             = "12345678",
    .bssid           = mac_addr,
    .ext_cfg = {
        .channel         = 6,
        .authmode        = WIFI_AUTH_WPA_WPA2_PSK,//WIFI_AUTH_OPEN,
        .ssid_hidden     = 0,
        .beacon_interval = 100,
        .psk_value = NULL,
    }
};

static uint8_t g_chip_uid[16]      = {0};
static uint8_t g_chip_wifi_mac[6]  = {0x00, 0x50, 0x00, 0x00, 0x00, 0x00};

static uint32_t djb_hash_hexdata(const char *input, uint32_t len)
{
    uint32_t hash = 5381;
    int c = *input;

    for (uint32_t i = 0; i < len; i++)
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        input++;
        c = *input;
    }
    return hash;
}

static void chip_mac_gen(void)
{
    hal_flash_read_unique_id(g_chip_uid);
    uint32_t hash = djb_hash_hexdata((const char *)g_chip_uid, sizeof(g_chip_uid));
    g_chip_wifi_mac[0] = 0x00;
    g_chip_wifi_mac[1] = 0x50;
    g_chip_wifi_mac[2] = (uint8_t)((hash) & 0xFF);
    g_chip_wifi_mac[3] = (uint8_t)((hash >> 8) & 0xFF);
    g_chip_wifi_mac[4] = (uint8_t)((hash >> 16) & 0xFF);
    g_chip_wifi_mac[5] = (uint8_t)((hash >> 24) & 0xFF);
}

static void wifi_scan_complete_cb(void * arg)
{
    LN_UNUSED(arg);

    ln_list_t *list;
    uint8_t node_count = 0;
    ap_info_node_t *pnode;

    wifi_manager_ap_list_update_enable(LN_FALSE);

    // 1.get ap info list.
    wifi_manager_get_ap_list(&list, &node_count);

    // 2.print all ap info in the list.
    LN_LIST_FOR_EACH_ENTRY(pnode, ap_info_node_t, list,list)
    {
        uint8_t * mac = (uint8_t*)pnode->info.bssid;
        ap_info_t *ap_info = &pnode->info;

        LOG(LOG_LVL_INFO, "\tCH=%2d,RSSI= %3d,", ap_info->channel, ap_info->rssi);
        LOG(LOG_LVL_INFO, "BSSID:[%02X:%02X:%02X:%02X:%02X:%02X],SSID:\"%s\"\r\n", \
                           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ap_info->ssid);
    }

    wifi_manager_ap_list_update_enable(LN_TRUE);
}

static void netdev_get_ip_callback(struct netif *nif)
{

}

static void wifi_init_sta(void)
{
    memcpy(mac_addr, g_chip_wifi_mac, sizeof(mac_addr));

    //1. net device(lwip)
    netdev_set_mac_addr(NETIF_IDX_STA, mac_addr);
    netdev_set_active(NETIF_IDX_STA);
    netdev_get_ip_cb_set(netdev_get_ip_callback);
#if LWIP_IPV6
    struct netif * netif1 = netdev_get_netif(NETIF_IDX_STA);
    netif_create_ip6_linklocal_address(netif1, 1);
    LOG(LOG_LVL_INFO, "IPv6 link-local address: %s\r\n", ipaddr_ntoa(netif_ip_addr6(netif1, 0)));
    netif_set_ip6_autoconfig_enabled(netif1, 1);
#endif

    //2. wifi start
    wifi_manager_reg_event_callback(WIFI_MGR_EVENT_STA_SCAN_COMPLETE, &wifi_scan_complete_cb);

    if(WIFI_ERR_NONE != wifi_sta_start(mac_addr, WIFI_NO_POWERSAVE)){
        LOG(LOG_LVL_ERROR, "[%s]wifi sta start filed!!!\r\n", __func__);
    }

    connect.psk_value = NULL;
    if (strlen(connect.pwd) != 0) {
        if (0 == ln_psk_calc(connect.ssid, connect.pwd, psk_value, sizeof (psk_value))) {
            connect.psk_value = psk_value;
            hexdump(LOG_LVL_INFO, "psk value ", psk_value, sizeof(psk_value));
        }
    }

    wifi_sta_connect(&connect, &scan_cfg);
}

static void ap_startup_cb(void * arg)
{
    netdev_set_state(NETIF_IDX_AP, NETDEV_UP);
}

static void wifi_init_ap(void)
{
    tcpip_ip_info_t  ip_info;
    server_config_t  server_config;

    ip_info.ip.addr      = ipaddr_addr((const char *)"192.168.4.1");
    ip_info.gw.addr      = ipaddr_addr((const char *)"192.168.4.1");
    ip_info.netmask.addr = ipaddr_addr((const char *)"255.255.255.0");

    server_config.server.addr   = ip_info.ip.addr;
    server_config.port          = 67;
    server_config.lease         = 2880;
    server_config.renew         = 2880;
    server_config.ip_start.addr = ipaddr_addr((const char *)"192.168.4.100");
    server_config.ip_end.addr   = ipaddr_addr((const char *)"192.168.4.150");
    server_config.client_max    = 3;
    dhcpd_curr_config_set(&server_config);

    memcpy(mac_addr, g_chip_wifi_mac, sizeof(mac_addr));
    mac_addr[5] += 1;

    //1. net device(lwip).
    netdev_set_mac_addr(NETIF_IDX_AP, mac_addr);
    netdev_set_ip_info(NETIF_IDX_AP, &ip_info);
    netdev_set_active(NETIF_IDX_AP);
    wifi_manager_reg_event_callback(WIFI_MGR_EVENT_SOFTAP_STARTUP, &ap_startup_cb);

    ap_cfg.ext_cfg.psk_value = NULL;
    if ((strlen(ap_cfg.pwd) != 0) &&
        (ap_cfg.ext_cfg.authmode != WIFI_AUTH_OPEN) &&
        (ap_cfg.ext_cfg.authmode != WIFI_AUTH_WEP)) {
        memset(psk_value, 0, sizeof(psk_value));
        if (0 == ln_psk_calc(ap_cfg.ssid, ap_cfg.pwd, psk_value, sizeof (psk_value))) {
            ap_cfg.ext_cfg.psk_value = psk_value;
            hexdump(LOG_LVL_INFO, "psk value ", psk_value, sizeof(psk_value));
        }
    }

    //2. wifi
    if(WIFI_ERR_NONE !=  wifi_softap_start(&ap_cfg)){
        LOG(LOG_LVL_ERROR, "[%s, %d]wifi_start() fail.\r\n", __func__, __LINE__);
    }
}

static int GetStaInfo()
{
    if (sysparam_sta_conn_cfg_get(&connect) != SYSPARAM_ERR_NONE) {
        printf("get conn cfg fail\r\n");
        return -1;
    }

    if (strlen(connect.ssid) == 0 || strlen(connect.pwd) == 0) {
        printf("no conn info\r\n");
        return -1;
    }
    return 0;
}

static void WifiConn()
{
    //1. sta mac get
     if (SYSPARAM_ERR_NONE != sysparam_sta_mac_get(mac_addr)) {
        LOG(LOG_LVL_ERROR, "sta mac get failed!!!\r\n");
        return;
    }

    //1. net device(lwip)
    netdev_set_mac_addr(NETIF_IDX_STA, mac_addr);
    netdev_set_active(NETIF_IDX_STA);
    sysparam_sta_mac_update((const uint8_t *)mac_addr);

    //2. wifi start
    if(WIFI_ERR_NONE != wifi_sta_start(mac_addr, WIFI_NO_POWERSAVE)){
        LOG(LOG_LVL_ERROR, "[%s]wifi sta start filed!!!\r\n", __func__);
    }
    if (strlen(connect.pwd) != 0) {
        if (0 == ln_psk_calc(connect.ssid, connect.pwd, connect.psk_value, sizeof (psk_value))) {
            // hexdump(LOG_LVL_INFO, "psk value ", psk_value, sizeof(psk_value));
        }
    }

    wifi_sta_connect(&connect, &scan_cfg);
    return;
}

static void WifiReconn()
{
    /* 获取wifi信息 */
    if (GetStaInfo() != 0) {
        printf("no sta info\r\n");
        return;
    }

    WifiConn();
    return;
}

static void usr_app_task_entry(void *params)
{
    LN_UNUSED(params);

    wifi_manager_init();

    chip_mac_gen();
    sysparam_sta_mac_update((const uint8_t *)g_chip_wifi_mac);
    sysparam_softap_mac_update((const uint8_t *)g_chip_wifi_mac);
    ln_kv_ble_app_init();

    WifiReconn();
    MagicLinkSDKRun();
    OS_ThreadDelete(NULL);
}


static void test_app_entry(void*params)
{
 LN_UNUSED(params);
for(;;)
{

     OS_MsDelay(1000);
     LOG(LOG_LVL_ERROR,"hello word!");
}

}
static void temp_cal_app_task_entry(void *params)
{
    LN_UNUSED(params);

    int8_t cap_comp = 0;
    uint16_t adc_val = 0;
    int16_t curr_adc = 0;
    uint8_t cnt = 0;

    if (NVDS_ERR_OK == ln_nvds_get_xtal_comp_val((uint8_t *)&cap_comp)) {
        if ((uint8_t)cap_comp == 0xFF) {
            cap_comp = 0;
        }
    }

    drv_adc_init();

    wifi_temp_cal_init(drv_adc_read(ADC_CH0), cap_comp);

    while (1)
    {
        OS_MsDelay(1000);

        adc_val = drv_adc_read(ADC_CH0);
        wifi_do_temp_cal_period(adc_val);

        curr_adc = (adc_val & 0xFFF);

        cnt++;
        if ((cnt % 60) == 0) {
            LOG(LOG_LVL_INFO, "adc raw: %4d, temp_IC: %4d\r\n", curr_adc, (int16_t)(25 + (curr_adc - 770) / 2.54f));
            LOG(LOG_LVL_INFO, "Total:%d; Free:%ld;\r\n", OS_HeapSizeGet(), OS_GetFreeHeapSize());
        }
    }
}

void creat_usr_app_task(void)
{
    {
        ln_pm_sleep_mode_set(ACTIVE);
        ln_pm_always_clk_disable_select(CLK_G_I2S | CLK_G_WS2811 | CLK_G_SDIO | CLK_G_AES);
        ln_pm_lightsleep_clk_disable_select(CLK_G_SPI0 | CLK_G_SPI1 | CLK_G_I2C0 | CLK_G_TRNG);
    }

    if(OS_OK != OS_ThreadCreate(&g_usr_app_thread, "WifiUsrAPP", usr_app_task_entry, NULL, OS_PRIORITY_BELOW_NORMAL, USR_APP_TASK_STACK_SIZE)) {
        LN_ASSERT(1);
    }

    // ble_creat_usr_app_task();

#if  WIFI_TEMP_CALIBRATE
    if(OS_OK != OS_ThreadCreate(&g_temp_cal_thread, "TempAPP", temp_cal_app_task_entry, NULL, OS_PRIORITY_BELOW_NORMAL, TEMP_APP_TASK_STACK_SIZE)) {
        LN_ASSERT(1);
    }
#endif

    /* print sdk version */
    {
        LOG(LOG_LVL_INFO, "LN882H SDK Ver: %s [build time:%s][0x%08x]\r\n",
                LN882H_SDK_VERSION_STRING, LN882H_SDK_BUILD_DATE_TIME, LN882H_SDK_VERSION);
    }

    if(OS_OK!=OS_ThreadCreate(&g_usr_test_thread,"test",test_app_entry,NULL,OS_PRIORITY_BELOW_NORMAL,TEST_TASK_SIZE)){
        LN_ASSERT(1);
    }

}

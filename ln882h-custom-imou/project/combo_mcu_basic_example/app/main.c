#include "proj_config.h"
#include "osal/osal.h"
#include "wifi.h"
#include "wifi_port.h"
#include "netif/ethernetif.h"
#include "usr_app.h"
#include "utils/system_parameter.h"
#include "utils/debug/CmBacktrace/cm_backtrace.h"
#include "utils/reboot_trace/reboot_trace.h"
#include "utils/runtime/runtime.h"
#include "hal/hal_interrupt.h"
#include "ln_utils.h"
#include "ln_misc.h"
#include "ln_nvds.h"
#include "ln_kv_api.h"
#include "ln_at.h"
#include "flash_partition_table.h"

#include "usr_app.h"


int main (int argc, char* argv[])
{
    LN_UNUSED(argc);
    LN_UNUSED(argv);

    //0. check reboot cause
    ln_chip_get_reboot_cause();

    //1.sys clock,interrupt
    SetSysClock();
    set_interrupt_priority();
    switch_global_interrupt(HAL_ENABLE);
    ln_runtime_measure_init();

    //2. register os heap mem
    OS_DefineHeapRegions();

    //3.init log&AT
    log_init();
    extern int ate_init(void);
    ate_init();

    //4.cm backtrace
    cm_backtrace_init("combo_mcu_basic_example", "hw", "sw");
    LOG(LOG_LVL_INFO, "------  combo_mcu_basic_example  ------\r\n");

    //5. init NVDS&KV
    if (NVDS_ERR_OK != ln_nvds_init(NVDS_SPACE_OFFSET)) {
        LOG(LOG_LVL_ERROR, "NVDS init filed!\r\n");
    }

    //7.rf preprocess,img cal
    wifi_rf_calibration();

    //8.Init wifi stack.
    wifi_init();
//    wlib_pvtcmd_output_cb_set(ln_at_vprintf);

    //9.Init lwip stack.
    lwip_tcpip_init();

    //10.Creat usr app task.
    creat_usr_app_task();

    OS_ThreadStartScheduler();

    return 0;
}

// ----------------------------------------------------------------------------

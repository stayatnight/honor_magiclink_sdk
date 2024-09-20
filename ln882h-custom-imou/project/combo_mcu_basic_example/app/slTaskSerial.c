/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slTaskSerial.c                                                *
* Description :                                                               *
*               smart lamp task serial source file                            *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-11-07                                                    *
* Mod History : (none)                                                        *
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "slTaskSerial.h"
#if defined(APP_TASK_LAMP_SERIAL_USE) && (APP_TASK_LAMP_SERIAL_USE == 1)
#include "slData.h"
#include "slTaskPower.h"
#include "magiclink.h"
#include "sys.h"
#include "timers.h"
#include "atShell.h"
#include "myRingBuffer.h"
#include "slPlcLampBedsideProc.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define SL_LAMP_SERIAL_UART                                MY_HAL_UART_0

#define RL_LAMP_REPORT_INTERVAL                            (800) //ms

#define SL_LAMP_SERIAL_RING_BUFFER_SIZE                    (128)
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
typedef struct
{
//  uint32_t interval;
    uint32_t sendTick;
//  uint32_t recvTick;
    uint32_t ackTimeout;
    uint8_t busyFlag;
//  uint8_t ucResetStep;
} slSerialInfo_t;
/******************************************************************************
*                             Declarations                                    *
******************************************************************************/

/******************************************************************************
*                                Globals                                      *
******************************************************************************/
static uint8_t s_lampSerialRingBuffer_buf[SL_LAMP_SERIAL_RING_BUFFER_SIZE];
static myRingBuffer_t s_lampSerialRingBuffer;
static uint8_t s_LampSerialSendBuf[32];
static slSerialInfo_t s_slSerialInfo;
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
 Function    : _lampSerialSend
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static int _lampSerialSend(uint8_t *pBuf, uint32_t ulLength) 
{
    return myHalUartSend(SL_LAMP_SERIAL_UART, pBuf, ulLength);
}

/******************************************************************************
 Function    : _lampSerialSendProc
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void _lampSerialSendProc(void) 
{
    uint32_t ulen = 0;

    if (s_slSerialInfo.busyFlag) {
        if (xTaskGetTickCount() - s_slSerialInfo.sendTick >= s_slSerialInfo.ackTimeout) {
            s_slSerialInfo.busyFlag = 0;
        }
        else {
            return;
        }
    }

    if (0 == myRingBufferGetDataSize(&s_lampSerialRingBuffer)) {
        return;
    }

    ulen = myRingBufferRead(&s_lampSerialRingBuffer, s_LampSerialSendBuf, 32);
    if (ulen > 0) {
        s_slSerialInfo.busyFlag = 1;
        s_slSerialInfo.sendTick = xTaskGetTickCount();
        _lampSerialSend(s_LampSerialSendBuf, ulen);
//      my_hal_log_info("lamp serial send msg %d\r\n", ulen);
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
 Function    : slSerialMsgCtrl
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slSerialMsgCtrl(uint8_t *pBuf, uint32_t ulLen, uint8_t repeatCnt) 
{
    int ret = 0;

    repeatCnt = repeatCnt > 5 ? 5 : repeatCnt;
    for (uint8_t i = 0; i < repeatCnt; i++) {
        ret = myRingBufferWrite(&s_lampSerialRingBuffer, pBuf, ulLen);
        if (ret != 0) {
            break;
        }
    }
    return ret;
}

/******************************************************************************
 Function    : slSerialRecieve
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
int slSerialRecieve(uint8_t *pBuf, uint32_t ulLength) 
{
    return myHalUartRecv(SL_LAMP_SERIAL_UART, pBuf, ulLength);
}

/******************************************************************************
 Function    : slSerialGetRecvLen
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint32_t slSerialGetRecvLen(void) 
{
    return myHalUartGetRecvLen(SL_LAMP_SERIAL_UART);
}

/******************************************************************************
 Function    : slSerialGetSendLen
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint32_t slSerialGetSendLen(void) 
{
    return myRingBufferGetDataSize(&s_lampSerialRingBuffer);
}

/******************************************************************************
 Function    : slSerialClearSendBusy
 Description : (none)
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slSerialClearSendBusy(void) 
{
    s_slSerialInfo.busyFlag = 0;
}

/******************************************************************************
 Function    : rlTaskLampSerialInit
 Description : reading lamp thread lamp init
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static int rlTaskLampSerialInit(void* arg)
{
    int ret = 0;

    s_slSerialInfo.ackTimeout = 300;
    s_slSerialInfo.busyFlag = 0;

    ret = myRingBufferNew(&s_lampSerialRingBuffer, SL_LAMP_SERIAL_RING_BUFFER_SIZE, s_lampSerialRingBuffer_buf);
    if (ret != 0) {
        my_hal_log_info("s_lampSerialRingBuffer init failed %d\r\n", ret);
    }
    myHalUartInit(SL_LAMP_SERIAL_UART, 115200, NULL, MY_HAL_UART_MODE_DMA);

    slSerialDeviceInit();

    my_hal_log_info("task 'lamp serial' init ok\r\n");
    return 0;
}

/******************************************************************************
 Function    : slTaskLampSerial
 Description : smart light thread lamp serial
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void slTaskLampSerial(void *arg)
{
    rlFlagSet(RL_FLAG_TASK_LAMP_RUNNING, 1);     
    if(0 > rlTaskLampSerialInit(arg)) {
        rlFlagSet(RL_FLAG_TASK_LAMP_RUNNING, 0);
    }

    while (rlFlagGet(RL_FLAG_TASK_LAMP_RUNNING)) {      
        vTaskDelay(10);

        slSerialDeviceProc();

        _lampSerialSendProc();   

        _lampReportProc();

        if (rlFlagGet(RL_FLAG_SYS_CFG_DATA_SAVE)) {
            rlFlagSet(RL_FLAG_SYS_CFG_DATA_SAVE, 0);
            rlDataWriteConfigData();
        }
    }

    my_hal_log_info("task 'lamp' delete\r\n");
    vTaskDelete(NULL);
}

#endif //#if (APP_TASK_LAMP_SERIAL_USE)

#ifdef __cplusplus
}
#endif


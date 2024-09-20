/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slTaskShell.c                                                 *
* Description :                                                               *
*               smart lamp task shell source file                             *
* Note        : (none)                                                        *
* Author      : 智能控制研发部                                                 *
* Date:       : 2017-05-31                                                    *
* Mod History : (none)                                                        *
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "oppWLConfig.h"
#include "oppWLData.h"
#include "oppWLTaskShell.h"
#include "oppWLElantra.h"
#include "tm1620.h"
#include "sk6812.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#if OPP_PLAT_SYS_LOG_ENABLE
#define oppShellLog                                        printf
#else
#define oppShellLog(format, ...)
#endif

#define OPP_SHELL_BUF_SIZE                                 OPP_HAL_UART_BUF_SIZE
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
typedef struct
{
    uint8_t u8_USART_RX_Buf[20];
    uint16_t u16_USART_Counter;
} UsartDbgBuffer_t;
/******************************************************************************
*                                Globals                                      *
******************************************************************************/
static char *shellBuf;
static UsartDbgBuffer_t UsartDbgBuffer;

uint32_t gulShellCmdVal = 0;
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
 Function    : oppAtoi
 Description : OPPLE 字符串转数字
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : 串口调试命令
******************************************************************************/
static int oppAtoi(const char *pStr)
{
    int8_t flag = 1;
    uint8_t len = 0;
    int val = 0;

    if(*pStr == '-')
    {
        flag = -1;
        pStr++;
    }
    len = (uint8_t)strlen((const char*)pStr);
    len--;
    if(len > 10)
    {
        len = 10;
    } 
    if(*pStr <0x30 || *pStr > 0x39)
    {
        return 0;
    }
    val = (*pStr-0x30);
    pStr++;
    len--;
    while(len > 0)
    {
        if(*pStr <0x30 || *pStr > 0x39)
        {
            break;
        }
        val = val * 10 + (*pStr-0x30);
        pStr++;
        len--;
    }
    return val * flag;
}

/******************************************************************************
 Function    : OppDebugShellIncludeSubStr
 Description : OPPLE串口调试模块判断是否包含指定子串的函数
 Note        : (none)
 Input Para  : pucCmdStr - 母串命令串的地址
               pucSubStr - 子串命令串的地址
 Output Para : (none)
 Return      : OPP_SUCCESS - 母串包含子串,且串首位置一样
               其它        - 母串不包含子串
 Other       : (none)
******************************************************************************/
static OPP_ERROR_CODE oppDebugShellIncludeSubStr(const uint8_t* pucCmdStr, const char* pucSubStr)
{
    uint8_t index = 0;

    if (0 == strcmp((const char*)pucCmdStr, (const char*)pucSubStr))
    {
        return OPP_SUCCESS;
    }
    else if(0 == strncmp((const char*)pucCmdStr, (const char*)pucSubStr, 2))
    {
        uint8_t len = (uint8_t)strlen((const char*)pucCmdStr);
        if(pucCmdStr[2] != ' ')
        {
            return OPP_FAILURE;
        }
        for(index = 3; index < len-2; index++)
        {
            if(pucCmdStr[index] <0x30 || pucCmdStr[index] > 0x39)
            {
                return OPP_FAILURE;
            }
        }
        gulShellCmdVal = (uint32_t)oppAtoi((const char*)&pucCmdStr[3]);
        return OPP_SUCCESS;
    }
    return OPP_FAILURE;
}

/******************************************************************************
 Function    : oppDbgUartSendStr
 Description : debug UART发送字符串
 Note        : (none)
 Input Para  : str--字符串地址
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
static void oppDbgUartSendStr(u8* str)
{
    oppHalUartSend(OPP_DEBUG_UART, str, strlen((const char*)str));
}

/******************************************************************************
 Function    : DebugUARTPrintf
 Description : Debug UART打印
 Note        : (none)
 Input Para  : fmt -- 格式
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
uint32_t oppDbgUartPrintf(char* fmt, ...)
{
    va_list ap;

    if ( !shellBuf ) {
        return 0;
    }
    va_start(ap, fmt);
    vsprintf(shellBuf, fmt, ap);
    oppDbgUartSendStr((u8*)shellBuf);
    va_end(ap);

    return 0;
}

/******************************************************************************
 Function    : oppShellShowHelpMenu
 Description : OPPLE串口调试模块显示串口命令帮助信息的函数
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : 串口调试命令
******************************************************************************/
static void oppShellShowHelpMenu(void)
{
    Printf("--------------------------------------------------\r\n");
    Printf("Master Command         Description\r\n");
    Printf("--------------------------------------------------\r\n");
    Printf("sa:    设置设备MAC地址.\r\n");
    Printf("ga:    获取设备当前地址.\r\n");
    Printf("sn:    主设备设置网络地址.\r\n");
    Printf("gn:    获取主设备网络地址.\r\n");
    Printf("cd:    清除主设备配网列表.\r\n");
    Printf("gd:    获取主设备配网列表.\r\n");
    Printf("cq:    清除主设备查询列表.\r\n");
    Printf("gq:    获取主设备查询列表.\r\n");
    Printf("sc:    设置从设备状态.\r\n");
    Printf("gc:    获取从设备状态.\r\n");
    Printf("sm:    设置设备模式.\r\n");
    Printf("gm:    获取设备模式.\r\n");
    Printf("t:     配网命令.\r\n");
    Printf("tt:    查询命令.\r\n");
    Printf("r:     主设备复位从设备命令.\r\n");
    Printf("f:     本设备恢复出厂设置.\r\n");
    Printf("h:     打印测试命令.\r\n"); 
    Printf("--------------------------------------------------\r\n");
}

/******************************************************************************
 Function    : oppShellProcCommandStr
 Description : OPPLE串口调试模块接收处理函数
 Note        : (none)
 Input Para  : pucStr   - 接收串口调试命令串的地址
               ulStrLen - 接收串口调试命令串的长度(单位:Byte)
 Output Para : (none)
 Return      : OPP_SUCCESS - 成功
               其它        - 失败码
 Other       : 串口调试模块提供给调试串口链路层调用
******************************************************************************/
static void oppShellProcCommandStr(const uint8_t* pucStr, uint32_t ulStrLen)
{
    Printf("\r\n");

    if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "ga\r\n")) {
        uint32_t ip = oppHalWifiGetIPAddress();
        Printf("IP addr: %d:%d:%d:%d\r\n", ip&0xFF, (ip>>8)&0xFF, (ip>>16)&0xFF, (ip>>24)&0xFF);
    }
    else if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "gl\r\n")) {
        Printf("link status: %d\r\n", oppHalWifiGetLinkStatus());
    } 
    else if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "sa\r\n")) {
        OPP_HOST_SERVER_PORT = gulShellCmdVal;
        oppDataWriteConfigData();
    } 
    else if ( OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "em\r\n") ) {
        oppwlElantraSetMode(gulShellCmdVal);
    } 
    else if ( OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "sp\r\n") ) {
        oppwlElantraSetSpeed(gulShellCmdVal);
    } 
    else if ( OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "gt\r\n") ) {
        oppTm_t opptm;
        uint32_t utc = oppHalRtcGetTime();

        oppDateTimeUTC2TM(utc, &opptm);
        //Printf("utc: %u\r\n", utc);
        Printf("date: %d-%d-%d\r\n", opptm.tm_year, opptm.tm_mon, opptm.tm_mday);
        Printf("time: %d-%d-%d\r\n", opptm.tm_hour, opptm.tm_min, opptm.tm_sec);
        Printf("week: %d-%d-%d\r\n", opptm.tm_wday, opptm.tm_yday, opptm.tm_isdst);
    } 
    else if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "st\r\n")) {
        oppHalRtcSetTime(gulShellCmdVal);
    } 
    else if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "sd\r\n")) {
        uint8_t rgb[3] = {0,0,0};

        rgb[gulShellCmdVal] = 255;
        for (uint32_t i=0; i<36; i++) {
            oppMdSK6812ColorSet(rgb, i);
        }
        oppMdSK6812Update();
    } else if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "sw\r\n")) { 
        oppWLLampSwitchCtrl(gulShellCmdVal, 500); 
    } else if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "br\r\n")) { 
        oppWLLampBriCtrl(gulShellCmdVal, 500);  
    } else if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "sm\r\n")) { 
        oppWLLampSceneModeCtrl(gulShellCmdVal, 500);  
    } else if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "sc\r\n")) { 
        oppWLLampSceneCall(gulShellCmdVal, 500);  
    } else if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "co\r\n")) { 
        uint8_t ucHue[3] = {0};
        
        switch (gulShellCmdVal) {
        case 0:
            ucHue[0] = 255;
            break;
        case 1:
            ucHue[1] = 255;
            break;
        case 2:
            ucHue[2] = 255;
            break;
        case 3:
            ucHue[0] = 255;
            ucHue[1] = 255;
            break;
        case 4:
            ucHue[0] = 255;
            ucHue[2] = 255;
            break;
        case 5:
            ucHue[2] = 255;
            ucHue[1] = 255;
            break;
        case 6:
            ucHue[0] = 255;
            ucHue[1] = 255;
            ucHue[2] = 255;
            break;
        case 7:
            break;
        default:
            break;
        }
        oppWLLampColorCtrl(ucHue, 500);  
    } else if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "h\r\n")) {
        oppShellShowHelpMenu();
    } else if(OPP_SUCCESS == oppDebugShellIncludeSubStr(pucStr, "f\r\n")) {
        //wifi_sta_connect(WIFI_CLEAR, NULL, 0, NULL, 0);
        oppDataRestoreFactory();
    }
    else {
         Printf("Unknown Command.\r\n");
    }
    return;
}

/******************************************************************************
 Function    : oppWLTaskShellInit
 Description : OPP shell init
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void oppWLTaskShellInit(uint32_t arg)
{
    shellBuf = malloc(OPP_SHELL_BUF_SIZE);
    oppHalUartInit(OPP_DEBUG_UART, 115200);
}

/******************************************************************************
 Function    : oppShellProc
 Description : Debug串口处理
 Note        : (none)
 Input Para  : (none)
 Output Para : (none)
 Return      : (none)
 Other       : (none)
******************************************************************************/
void* oppWLTaskShell(uint32_t arg)
{
    uint8_t rxData = 0;

    while(True)
    {
        LOS_TaskDelay(10);       
        if(0 == oppHalUartRecvData(OPP_DEBUG_UART, &rxData, 1)) {
            continue;
        }

        if(rxData == 0x08) { //回退
            if(UsartDbgBuffer.u16_USART_Counter > 0) {
                uint8_t str[3] = {0x08, ' ', 0x08};
                oppHalUartSend(OPP_DEBUG_UART, &str[0], 3);
                UsartDbgBuffer.u16_USART_Counter--;
            } 
            continue;
        }
        oppHalUartSend(OPP_DEBUG_UART, &rxData, 1);

        if (sizeof(UsartDbgBuffer.u8_USART_RX_Buf)-2 > UsartDbgBuffer.u16_USART_Counter) {
            UsartDbgBuffer.u8_USART_RX_Buf[UsartDbgBuffer.u16_USART_Counter++] = rxData;
        }
        if (rxData == 0x0D) {
            UsartDbgBuffer.u8_USART_RX_Buf[UsartDbgBuffer.u16_USART_Counter++] = 0x0A; 
            UsartDbgBuffer.u8_USART_RX_Buf[UsartDbgBuffer.u16_USART_Counter++] = 0;
            oppShellProcCommandStr((uint8_t*)UsartDbgBuffer.u8_USART_RX_Buf, UsartDbgBuffer.u16_USART_Counter); 
            UsartDbgBuffer.u16_USART_Counter = 0;
        }
    } 

    LOS_CurTaskDelete();
    return NULL;
}

#ifdef __cplusplus
}
#endif


/******************************************************************************
* Version     : OPP_PLATFORM V100R001C01B001D001                              *
* File        : oppLightShell.h                                                    *
* Description :                                                               *
*               OPPLE Light Shell定义头文件                                 *
* Note        : (none)                                                        *
* Author      : 智能控制研发部                                                *
* Date:       : 2015-09-01                                                    *
* Mod History : (none)                                                        *
******************************************************************************/
#ifndef __OPP_LIGHT_SHELL_H__
#define __OPP_LIGHT_SHELL_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include "../oppCommon/oppCommon.h"
#include "oppWLConfig.h"
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
#define OPP_DEBUG_UART                                     OPP_HAL_UART_1
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/

/******************************************************************************
*                           Extern Declarations                               *
******************************************************************************/

/******************************************************************************
*                              Declarations                                   *
******************************************************************************/
void oppWLTaskShellInit(uint32_t arg);
void* oppWLTaskShell(uint32_t arg);
uint32_t oppDbgUartPrintf(char* fmt, ...);

#ifdef __cplusplus
}
#endif



#endif /*__OPP_LIGHT_SHELL_H__*/




/******************************************************************************
* Version     : V100R001C01B001D001                                           *
* File        : slSerialMsg.h                                                 *
* Description :                                                               *
*               smart lamp serial msg header file                             *
* Note        : (none)                                                        *
* Author      : kris li                                                       *
* Date:       : 2022-12-08                                                    *
* Mod History : (none)                                                        *
******************************************************************************/
#ifndef __SL_SERIAL_MSG_H__
#define __SL_SERIAL_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*                                Includes                                     *
******************************************************************************/
#include <stdint.h>
/******************************************************************************
*                                Defines                                      *
******************************************************************************/
/* serial msg header define */
#define SERIAL_MSG_HEADER_DEF                              0x5AA5

/*GATEWAY->LAMP:恢复出厂设置请求*/
#define SERIAL_MSG_ID_M2S_FACTORY_RESET_REQ                0x0207
/*LAMP->GATEWAY:恢复出厂设置响应*/
#define SERIAL_MSG_ID_S2M_FACTORY_RESET_RSP                0x0208

/*LAMP->GATEWAY:设备状态上报请求消息*/
#define SERIAL_MSG_ID_S2M_STATUS_REPORT_REQ                0x0210
/*GATEWAY->LAMP:设备状态上报响应消息*/
#define SERIAL_MSG_ID_M2S_STATUS_REPORT_RSP                0x0211

/*LAMP->GATEWAY:恢复出厂设置上报请求消息*/
#define SERIAL_MSG_ID_S2M_FACTORY_RESET_REPORT_REQ         0x0212
/*GATEWAY->LAMP:恢复出厂设置上报响应消息*/
#define SERIAL_MSG_ID_M2S_FACTORY_RESET_REPORT_RSP         0x0213

/*GATEWAY->LAMP:查询智能设备状态请求消息*/
#define SERIAL_MSG_ID_M2S_GET_STATUS_REQ                   0x030F
/*LAMP->GATEWAY:查询智能设备状态响应消息*/
#define SERIAL_MSG_ID_S2M_GET_STATUS_RSP                   0x0310

/*GATEWAY->LAMP:开关请求消息*/
#define SERIAL_MSG_ID_M2S_LAMP_MODE_CTRL_REQ               0x030D
/*LAMP->GATEWAY:开关响应消息*/
#define SERIAL_MSG_ID_S2M_LAMP_MODE_CTRL_RSP               0x030E

/*GATEWAY->LAMP:开关请求消息*/
#define SERIAL_MSG_ID_M2S_LAMP_ONOFF_CTRL_REQ              0x0311
/*LAMP->GATEWAY:开关响应消息*/
#define SERIAL_MSG_ID_S2M_LAMP_ONOFF_CTRL_RSP              0x0312

/*GATEWAY->LAMP:晾衣架移动请求消息*/
#define SERIAL_MSG_ID_M2S_AIRER_MOTION_CTRL_REQ            0x0323
/*LAMP->GATEWAY:晾衣架移动响应消息*/
#define SERIAL_MSG_ID_S2M_AIRER_MOTION_CTRL_RSP            0x0324

/*GATEWAY->LAMP:浴霸干燥开关请求消息*/
#define SERIAL_MSG_ID_M2S_BATHBULLY_DRY_CTRL_REQ           0x0331
/*LAMP->GATEWAY:浴霸干燥开关响应消息*/
#define SERIAL_MSG_ID_S2M_BATHBULLY_DRY_CTRL_RSP           0x0332

/*GATEWAY->LAMP:浴霸吹风开关请求消息*/
#define SERIAL_MSG_ID_M2S_BATHBULLY_FAN_CTRL_REQ           0x0333
/*LAMP->GATEWAY:浴霸吹风开关响应消息*/
#define SERIAL_MSG_ID_S2M_BATHBULLY_FAN_CTRL_RSP           0x0334

/*GATEWAY->LAMP:浴霸唤气开关请求消息*/
#define SERIAL_MSG_ID_M2S_BATHBULLY_VENTI_CTRL_REQ         0x0335
/*LAMP->GATEWAY:浴霸唤气开关响应消息*/
#define SERIAL_MSG_ID_S2M_BATHBULLY_VENTI_CTRL_RSP         0x0336

/*GATEWAY->LAMP:浴霸暖风开关请求消息*/
#define SERIAL_MSG_ID_M2S_BATHBULLY_WARM_FAN_CTRL_REQ      0x0337
/*LAMP->GATEWAY:浴霸暖风开关响应消息*/
#define SERIAL_MSG_ID_S2M_BATHBULLY_WARM_FAN_CTRL_RSP      0x0338

/*GATEWAY->LAMP:浴霸暖风档位开关请求消息*/
#define SERIAL_MSG_ID_M2S_BATHBULLY_WARM_GEAR_CTRL_REQ     0x0339
/*LAMP->GATEWAY:浴霸暖风档位开关响应消息*/
#define SERIAL_MSG_ID_S2M_BATHBULLY_WARM_GEAR_CTRL_RSP     0x033A
/******************************************************************************
*                                Typedefs                                     *
******************************************************************************/
#pragma pack(1)
/*消息头结构定义*/
typedef struct {
    uint16_t uwHeader;
    uint16_t uwLength;
    uint16_t uwCheckSum;
    uint16_t uwProtocolVer;
} slSerialMsgHeader_t;

/*消息类型结构定义*/
typedef struct {
    uint16_t uwMsgType;
} slSerialMsgType_t;

/*通用响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgCommonRsp_t;

/*(0x0207)设备恢复出厂设置请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
} slSerialMsgFactoryResetReq_t;
/*(0x0208)设备恢复出厂设置响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgFactoryResetRsp_t;

/*(0x0212)LAMP->GATEWAY:设备恢复出厂设置上报请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
} slSerialMsgFactoryResetReportReq_t;
/*(0x0213)GATEWAY->LAMP:设备恢复出厂设置上报响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgFactoryResetReportRsp_t;

/*(0x0210)LAMP->GATEWAY:设备状态上报请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
} slSerialMsgStatusReportReq_t;
/*(0x0211)GATEWAY->LAMP:设备状态上报响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgStatusReportRsp_t;

/*(0x030F)GATEWAY->LAMP:设备状态查询请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
} slSerialMsgStatusQueryReq_t;
/*(0x0310)LAMP->GATEWAY:设备状态查询响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
} slSerialMsgStatusQueryRsp_t;

/*(0x030D)GATEWAY->LAMP:设备灯具模式请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucLightMode;
    uint16_t uwPeroidMs;
} slSerialMsgLampModeCtrlReq_t;
/*(0x030E)LAMP->GATEWAY:设备灯具模式响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgLampModeCtrlRsp_t;

/*(0x0311)GATEWAY->LAMP:设备灯具开关请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOnoff;
    uint16_t uwPeroidMs;
} slSerialMsgLampOnoffCtrlReq_t;
/*(0x0312)LAMP->GATEWAY:设备灯具开关响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgLampOnoffCtrlRsp_t;

/*(0x0321)GATEWAY->LAMP:晾衣架移动请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucMotion;
} slSerialMsgAirerMotionCtrlReq_t;
/*(0x0322)LAMP->GATEWAY:晾衣架移动响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgAirerMotionCtrlRsp_t;

/*(0x0331)GATEWAY->LAMP:设备浴霸干燥开关请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOnoff;
} slSerialMsgBathBullyDryOnoffCtrlReq_t;
/*(0x0332)LAMP->GATEWAY:设备浴霸干燥开关响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgBathBullyDryOnoffCtrlRsp_t;

/*(0x0333)GATEWAY->LAMP:设备浴霸吹风开关请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOnoff;
} slSerialMsgBathBullyFanOnoffCtrlReq_t;
/*(0x0334)LAMP->GATEWAY:设备浴霸吹风开关响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgBathBullyFanOnoffCtrlRsp_t;

/*(0x0335)GATEWAY->LAMP:设备浴霸唤气开关请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOnoff;
} slSerialMsgBathBullyVentilationOnoffCtrlReq_t;
/*(0x0336)LAMP->GATEWAY:设备浴霸唤气开关响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgBathBullyVentilationOnoffCtrlRsp_t;

/*(0x0337)GATEWAY->LAMP:设备浴霸暖风开关请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOnoff;
} slSerialMsgBathBullyWarmFanOnoffCtrlReq_t;
/*(0x0338)LAMP->GATEWAY:设备浴霸暖风开关响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgBathBullyWarmFanOnoffCtrlRsp_t;

/*(0x0339)GATEWAY->LAMP:设备浴霸暖风档位请求消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucWarmFanGear;
} slSerialMsgBathBullyWarmFanGearCtrlReq_t;
/*(0x033A)LAMP->GATEWAY:设备浴霸暖风档位响应消息结构定义*/
typedef struct {
    slSerialMsgHeader_t stHeader;
    uint16_t uwMsgType;
    uint8_t ucOpCode;
} slSerialMsgBathBullyWarmFanGearCtrlRsp_t;

#pragma pack()
/******************************************************************************
*                           Extern Declarations                               *
******************************************************************************/

/******************************************************************************
*                              Declarations                                   *
******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /*__SL_SERIAL_MSG_H__*/

/*
 * Copyright (c) Honor Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: magiclink sdk demo profile head file
 */

#ifndef __DEMO_PROFILE_H__
#define __DEMO_PROFILE_H__

#ifdef __cplusplus
extern "c"
{
#endif

#define MY_PRDKEY "5e82832afa854e3e8e165b3e65d67ec1"
#define MY_PRDSECRET "60a197ff445bf16f52234eb4da3c243b"

static const char *g_profile = "{\
\"prdID\": \"VGGR\",\
\"title\": \"阳光荣星浴霸Y681\",\
\"description\": \"阳光荣星浴霸Y681\",\
\"serviceList\": [{\
\"index\": 0,\
\"id\": \"baseInfo\",\
\"title\": \"产品基础信息\",\
\"description\": \"产品基础信息\",\
\"propertyList\": [{\
\"index\": 0,\
\"id\": \"prdID\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"产品的唯一标识\",\
\"value\": \"VGGR\",\
\"defaultValue\": \"VGGR\"\
},\
{\
\"index\": 1,\
\"id\": \"prdName\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"产品名称\",\
\"value\": \"@prdName\",\
\"defaultValue\": \"@prdName\"\
},\
{\
\"index\": 2,\
\"id\": \"manufactureID\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"产品厂商唯一标识\",\
\"value\": \"A024\",\
\"defaultValue\": \"A024\"\
},\
{\
\"index\": 3,\
\"id\": \"manufactureName\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"产品厂商名称\",\
\"value\": \"阳光照明\",\
\"defaultValue\": \"阳光照明\"\
},\
{\
\"index\": 4,\
\"id\": \"prdTypeId\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"产品类别唯一表示\",\
\"value\": \"M000\",\
\"defaultValue\": \"M000\"\
},\
{\
\"index\": 5,\
\"id\": \"prdTypeName\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"产品类别名称\",\
\"value\": \"卫浴\",\
\"defaultValue\": \"卫浴\"\
},\
{\
\"index\": 6,\
\"id\": \"prdIcon\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"产品图标\",\
\"value\": \"/icon\",\
\"defaultValue\": \"/icon\"\
},\
{\
\"index\": 7,\
\"id\": \"uiType\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"UI类型\",\
\"value\": \"H5\",\
\"defaultValue\": \"H5\"\
},\
{\
\"index\": 8,\
\"id\": \"protocolType\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"协议类型\",\
\"value\": \"WiFi\",\
\"defaultValue\": \"WiFi\"\
},\
{\
\"index\": 9,\
\"id\": \"supportNetCfg\",\
\"access\": 0,\
\"type\": \"int\",\
\"description\": \"是否需要配网\",\
\"value\": 1,\
\"defaultValue\": 1\
},\
{\
\"index\": 10,\
\"id\": \"supportRegister\",\
\"access\": 0,\
\"type\": \"int\",\
\"description\": \"是否支持注册到设备云\",\
\"value\": 1,\
\"defaultValue\": 1\
},\
{\
\"index\": 11,\
\"id\": \"pinMode\",\
\"access\": 0,\
\"type\": \"int\",\
\"description\": \"pin码方式：0,默认pin码; 1,输入pin码; 2,扫码\",\
\"value\": 0,\
\"defaultValue\": 0\
},\
{\
\"index\": 12,\
\"id\": \"authMode\",\
\"access\": 0,\
\"type\": \"int\",\
\"description\": \"设备云认证设备的方式：0,PKI; 1,license; 2,固定密钥; 3,账号认证\",\
\"value\": 2,\
\"defaultValue\": 2\
},\
{\
\"index\": 13,\
\"id\": \"supportCloudCtrl\",\
\"access\": 0,\
\"type\": \"int\",\
\"description\": \"是否支持云端控制：0,不支持; 1,支持\",\
\"value\": 1,\
\"defaultValue\": 1\
},\
{\
\"index\": 14,\
\"id\": \"netCfgWlan\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"softAp配网使用的wlan名称\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 15,\
\"id\": \"pluginName\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"插件名\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 16,\
\"id\": \"modelId\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"Model ID\",\
\"value\": \"0\",\
\"defaultValue\": \"0\"\
},\
{\
\"index\": 17,\
\"id\": \"offeringCode\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"offering code\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 18,\
\"id\": \"prdSubTypeId\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"产品子类别标识\",\
\"value\": \"12k\",\
\"defaultValue\": \"12k\"\
},\
{\
\"index\": 19,\
\"id\": \"prdSubTypeName\",\
\"access\": 0,\
\"type\": \"string\",\
\"description\": \"产品子类别名称\",\
\"value\": \"浴霸\",\
\"defaultValue\": \"浴霸\"\
}\
]\
},\
{\
\"index\": 1,\
\"id\": \"deviceInfo\",\
\"title\": \"设备基础信息\",\
\"description\": \"设备基础信息\",\
\"propertyList\": [{\
\"index\": 0,\
\"id\": \"sn\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"设备序列号\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 1,\
\"id\": \"udid\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"设备udid\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 2,\
\"id\": \"mac\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"设备mac\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 3,\
\"id\": \"devName\",\
\"access\": 7,\
\"type\": \"string\",\
\"description\": \"设备名称\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 4,\
\"id\": \"devModel\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"设备型号\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 5,\
\"id\": \"subDevType\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"子设备型号\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 6,\
\"id\": \"firmwareVersion\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"固件版本号\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 7,\
\"id\": \"sdkVersion\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"SDK版本号\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 8,\
\"id\": \"hardwareVersion\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"硬件版本号\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 9,\
\"id\": \"MCUVersion\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"MCU版本号\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
}\
]\
},\
{\
\"index\": 2,\
\"id\": \"netInfo\",\
\"title\": \"网络信息\",\
\"description\": \"网络信息\",\
\"propertyList\": [{\
\"index\": 0,\
\"id\": \"ssid\",\
\"access\": 1,\
\"type\": \"string\",\
\"description\": \"WLAN名称\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 1,\
\"id\": \"rssi\",\
\"access\": 1,\
\"type\": \"int\",\
\"description\": \"信号强度\",\
\"value\": 0,\
\"defaultValue\": 0\
},\
{\
\"index\": 2,\
\"id\": \"ip\",\
\"access\": 1,\
\"type\": \"string\",\
\"description\": \"ip地址\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 3,\
\"id\": \"bssid\",\
\"access\": 1,\
\"type\": \"string\",\
\"description\": \"bssid\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
}\
]\
},\
{\
\"index\": 3,\
\"id\": \"ota\",\
\"title\": \"升级服务\",\
\"description\": \"升级服务\",\
\"propertyList\": [{\
\"index\": 0,\
\"id\": \"otaStatus\",\
\"access\": 5,\
\"type\": \"int\",\
\"description\": \"0: 默认值, 100: 正在检测新版本, 200: 检测新版本完成, 300: 升级中, 400: 下载完成等待重启, 500: 超时, 600: 升级成功, -100: 升级失败\",\
\"value\": 0,\
\"defaultValue\": 0\
},\
{\
\"index\": 1,\
\"id\": \"checkResult\",\
\"access\": 5,\
\"type\": \"int\",\
\"description\": \"检测新版本结果，检测成功0，其他为失败\",\
\"value\": 0,\
\"defaultValue\": 0\
},\
{\
\"index\": 2,\
\"id\": \"downloadTimeout\",\
\"access\": 5,\
\"type\": \"int\",\
\"description\": \"预估升级时间(s)\",\
\"value\": 0,\
\"defaultValue\": 0\
},\
{\
\"index\": 3,\
\"id\": \"newVersion\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"新版本号\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 4,\
\"id\": \"MCUNewVersion\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"MCU新版本号\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 5,\
\"id\": \"introduction\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"新版本介绍\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 6,\
\"id\": \"MCUIntroduction\",\
\"access\": 5,\
\"type\": \"string\",\
\"description\": \"MCU新版本介绍\",\
\"value\": \"\",\
\"defaultValue\": \"\"\
},\
{\
\"index\": 7,\
\"id\": \"autoOtaSwitch\",\
\"access\": 7,\
\"type\": \"int\",\
\"description\": \"自动升级开关\",\
\"value\": 0,\
\"defaultValue\": 0\
},\
{\
\"index\": 8,\
\"id\": \"otaProgress\",\
\"access\": 5,\
\"type\": \"int\",\
\"description\": \"升级进度\",\
\"value\": 0,\
\"defaultValue\": 0\
}\
],\
\"actionList\": [{\
\"index\": 0,\
\"id\": \"checkVersion\",\
\"description\": \"检查版本更新\"\
},\
{\
\"index\": 1,\
\"id\": \"otaAction\",\
\"description\": \"触发版本更新\"\
}\
]\
},\
{\
\"index\": 4,\
\"id\": \"bathBully\",\
\"title\": \"浴霸服务\",\
\"description\": \"浴霸服务\",\
\"propertyList\": [{\
\"index\": 0,\
\"id\": \"on\",\
\"access\": 7,\
\"type\": \"int\",\
\"description\": \"干燥开关\",\
\"value\": 0,\
\"defaultValue\": 0,\
\"valueList\": [{\
\"value\": 0,\
\"description\": \"关机\"\
},\
{\
\"value\": 1,\
\"description\": \"开机\"\
}\
]\
},\
{\
\"index\": 1,\
\"id\": \"fanOn\",\
\"access\": 7,\
\"type\": \"int\",\
\"description\": \"吹风开关\",\
\"value\": 0,\
\"defaultValue\": 0,\
\"valueList\": [{\
\"value\": 0,\
\"description\": \"关\"\
},\
{\
\"value\": 1,\
\"description\": \"开\"\
}\
]\
},\
{\
\"index\": 2,\
\"id\": \"ventilationOn\",\
\"access\": 7,\
\"type\": \"int\",\
\"description\": \"换气开关\",\
\"value\": 0,\
\"defaultValue\": 0,\
\"valueList\": [{\
\"value\": 0,\
\"description\": \"关\"\
},\
{\
\"value\": 1,\
\"description\": \"开\"\
}\
]\
},\
{\
\"index\": 3,\
\"id\": \"warmFanOn\",\
\"access\": 7,\
\"type\": \"int\",\
\"description\": \"暖风开关\",\
\"value\": 0,\
\"defaultValue\": 0,\
\"valueList\": [{\
\"value\": 0,\
\"description\": \"关\"\
},\
{\
\"value\": 1,\
\"description\": \"开\"\
}\
]\
},\
{\
\"index\": 4,\
\"id\": \"warmFanGear\",\
\"access\": 7,\
\"type\": \"int\",\
\"description\": \"暖风档位\",\
\"value\": 0,\
\"defaultValue\": 0,\
\"valueList\": [{\
\"value\": 0,\
\"description\": \"弱暖风\"\
},\
{\
\"value\": 1,\
\"description\": \"强暖风\"\
}\
]\
}]\
},\
{\
\"index\": 5,\
\"id\": \"light\",\
\"title\": \"灯\",\
\"description\": \"灯\",\
\"propertyList\": [{\
\"index\": 0,\
\"id\": \"on\",\
\"access\": 7,\
\"type\": \"int\",\
\"description\": \"灯具开关状态；变化就上报\",\
\"value\": 0,\
\"defaultValue\": 0,\
\"valueList\": [{\
\"value\": 0,\
\"description\": \"关\"\
},\
{\
\"value\": 1,\
\"description\": \"开\"\
}\
]\
},\
{\
\"index\": 1,\
\"id\": \"lightMode\",\
\"access\": 7,\
\"type\": \"int\",\
\"description\": \"灯光模式\",\
\"value\": 0,\
\"defaultValue\": 0,\
\"valueList\": [{\
\"value\": 6,\
\"description\": \"工作模式\"\
},\
{\
\"value\": 100,\
\"description\": \"夜灯模式\"\
}\
]\
}]\
},\
{\
\"index\": 6,\
\"id\": \"environment\",\
\"title\": \"环境服务\",\
\"description\": \"环境服务\",\
\"propertyList\": [{\
\"index\": 0,\
\"id\": \"temp\",\
\"access\": 5,\
\"type\": \"int\",\
\"description\": \"温度，单位：摄氏度\",\
\"value\": 0,\
\"defaultValue\": 0,\
\"step\": 1,\
\"minValue\": -50,\
\"maxValue\": 50\
}]\
}\
],\
\"quickmenu\": [{\
\"switchInfo\": {\
\"path\": \"switch.on\",\
\"off\": 0,\
\"on\": 1\
},\
\"content\": [{\
\"text\": \"亮度:{brightness.brightness%1}%\"\
}]\
}]\
}";

#ifdef __cplusplus
}
#endif

#endif

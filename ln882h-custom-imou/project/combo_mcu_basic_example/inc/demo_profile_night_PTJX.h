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

#define MY_PRDKEY "955371580c4a4f53bd959839913fa4b5"
#define MY_PRDSECRET "408b3647afbd19f61f47733ef8b6eff6"

static const char *g_profile = "{\
	\"prdID\": \"PTJX\",\
	\"serviceList\": [{\
		\"propertyList\": [{\
			\"access\": 0,\
			\"defaultValue\": \"PTJX\",\
			\"description\": \"产品的唯一标识\",\
			\"index\": 0,\
			\"id\": \"prdID\",\
			\"type\": \"string\",\
			\"value\": \"PTJX\",\
			\"maxLength\": 4\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": \"阳光荣旭夜灯MTSL2015\",\
			\"description\": \"产品名称\",\
			\"index\": 1,\
			\"id\": \"prdName\",\
			\"type\": \"string\",\
			\"value\": \"阳光荣旭夜灯MTSL2015\"\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": \"A021\",\
			\"description\": \"产品厂商唯一标识\",\
			\"index\": 2,\
			\"id\": \"manufactureID\",\
			\"type\": \"string\",\
			\"value\": \"A021\",\
			\"maxLength\": 4\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": \"生态系统平台\",\
			\"description\": \"产品厂商名称\",\
			\"index\": 3,\
			\"id\": \"manufactureName\",\
			\"type\": \"string\",\
			\"value\": \"生态系统平台\"\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": \"B000\",\
			\"description\": \"产品类别唯一表示\",\
			\"index\": 4,\
			\"id\": \"prdTypeId\",\
			\"type\": \"string\",\
			\"value\": \"B000\",\
			\"maxLength\": 4\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": \"照明\",\
			\"description\": \"产品类别名称\",\
			\"index\": 5,\
			\"id\": \"prdTypeName\",\
			\"type\": \"string\",\
			\"value\": \"照明\"\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": \"WiFi\",\
			\"description\": \"协议类型\",\
			\"index\": 6,\
			\"id\": \"protocolType\",\
			\"type\": \"string\",\
			\"value\": \"WiFi\",\
			\"maxLength\": 4\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": 1,\
			\"description\": \"是否需要配网：0,否;1,是\",\
			\"index\": 7,\
			\"id\": \"supportNetCfg\",\
			\"type\": \"int\",\
			\"value\": 1\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": 1,\
			\"description\": \"是否支持注册到设备云：0,否;1,是\",\
			\"index\": 8,\
			\"id\": \"supportRegister\",\
			\"type\": \"int\",\
			\"value\": 1\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": 0,\
			\"description\": \"pin码方式：0,默认pin码; 1,输入pin码; 2,扫码\",\
			\"index\": 9,\
			\"id\": \"pinMode\",\
			\"type\": \"int\",\
			\"value\": 0\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": 2,\
			\"description\": \"设备云认证设备的方式：0,PKI; 1,license; 2,固定密钥; 3,账号认证\",\
			\"index\": 10,\
			\"id\": \"authMode\",\
			\"type\": \"int\",\
			\"value\": 2\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": 1,\
			\"description\": \"是否支持云端控制：0,不支持; 1,支持\",\
			\"index\": 11,\
			\"id\": \"supportCloudCtrl\",\
			\"type\": \"int\",\
			\"value\": 1\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": \"\",\
			\"description\": \"softAp配网使用的wlan名称\",\
			\"index\": 12,\
			\"id\": \"netCfgWlan\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": \"0\",\
			\"description\": \"Model ID\",\
			\"index\": 13,\
			\"id\": \"modelId\",\
			\"type\": \"string\",\
			\"value\": \"0\",\
			\"maxLength\": 6\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": \"\",\
			\"description\": \"offering code\",\
			\"index\": 14,\
			\"id\": \"offeringCode\",\
			\"type\": \"string\",\
			\"value\": \"\",\
			\"maxLength\": 16\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": \"13Y\",\
			\"description\": \"产品子类别标识\",\
			\"index\": 15,\
			\"id\": \"prdSubTypeId\",\
			\"type\": \"string\",\
			\"value\": \"13Y\",\
			\"maxLength\": 3\
		},\
		{\
			\"access\": 0,\
			\"defaultValue\": \"夜灯\",\
			\"description\": \"产品子类别名称\",\
			\"index\": 16,\
			\"id\": \"prdSubTypeName\",\
			\"type\": \"string\",\
			\"value\": \"夜灯\"\
		}],\
		\"description\": \"产品基础信息\",\
		\"index\": 0,\
		\"id\": \"baseInfo\",\
		\"title\": \"产品基础信息\"\
	},\
	{\
		\"propertyList\": [{\
			\"access\": 5,\
			\"maxlength\": 32,\
			\"defaultValue\": \"\",\
			\"index\": 0,\
			\"description\": \"设备序列号\",\
			\"id\": \"sn\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"maxlength\": 64,\
			\"defaultValue\": \"\",\
			\"index\": 1,\
			\"description\": \"设备udid\",\
			\"id\": \"udid\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"maxlength\": 17,\
			\"defaultValue\": \"\",\
			\"index\": 2,\
			\"description\": \"设备mac\",\
			\"id\": \"mac\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 7,\
			\"maxlength\": 63,\
			\"defaultValue\": \"\",\
			\"index\": 3,\
			\"description\": \"设备名称\",\
			\"id\": \"devName\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"maxlength\": 15,\
			\"defaultValue\": \"\",\
			\"index\": 4,\
			\"description\": \"设备型号\",\
			\"id\": \"devModel\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"maxlength\": 1,\
			\"defaultValue\": \"\",\
			\"index\": 5,\
			\"description\": \"子设备型号\",\
			\"id\": \"subDevType\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"maxlength\": 32,\
			\"defaultValue\": \"\",\
			\"index\": 6,\
			\"description\": \"固件版本号\",\
			\"id\": \"firmwareVersion\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"maxlength\": 32,\
			\"defaultValue\": \"\",\
			\"index\": 7,\
			\"description\": \"SDK版本号\",\
			\"id\": \"sdkVersion\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"maxlength\": 32,\
			\"defaultValue\": \"\",\
			\"index\": 8,\
			\"description\": \"硬件版本号\",\
			\"id\": \"hardwareVersion\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"maxlength\": 32,\
			\"defaultValue\": \"\",\
			\"index\": 9,\
			\"description\": \"MCU版本号\",\
			\"id\": \"MCUVersion\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		}],\
		\"index\": 1,\
		\"description\": \"设备基础信息\",\
		\"id\": \"deviceInfo\",\
		\"title\": \"设备基础信息\"\
	},\
	{\
		\"propertyList\": [{\
			\"access\": 5,\
			\"maxlength\": 32,\
			\"defaultValue\": \"\",\
			\"index\": 0,\
			\"description\": \"WLAN名称\",\
			\"id\": \"ssid\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"defaultValue\": 0,\
			\"index\": 1,\
			\"description\": \"信号强度\",\
			\"id\": \"rssi\",\
			\"type\": \"int\",\
			\"value\": 0\
		},\
		{\
			\"access\": 5,\
			\"maxlength\": 15,\
			\"defaultValue\": \"\",\
			\"index\": 2,\
			\"description\": \"ip地址\",\
			\"id\": \"ip\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"maxlength\": 17,\
			\"defaultValue\": \"\",\
			\"index\": 3,\
			\"description\": \"bssid\",\
			\"id\": \"bssid\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		}],\
		\"index\": 2,\
		\"description\": \"网络信息\",\
		\"id\": \"netInfo\",\
		\"title\": \"网络信息\"\
	},\
	{\
		\"propertyList\": [{\
			\"access\": 5,\
			\"defaultValue\": 0,\
			\"index\": 0,\
			\"description\": \"0: 默认值, 100: 正在检测新版本, 200: 检测新版本完成, 300: 升级中, 400: 下载完成等待重启, 500: 超时, 600: 升级成功, -100: 升级失败\",\
			\"id\": \"otaStatus\",\
			\"type\": \"int\",\
			\"value\": 0\
		},\
		{\
			\"access\": 5,\
			\"defaultValue\": 0,\
			\"index\": 1,\
			\"description\": \"检测新版本结果，检测成功0，其他为失败\",\
			\"id\": \"checkResult\",\
			\"type\": \"int\",\
			\"value\": 0\
		},\
		{\
			\"access\": 5,\
			\"defaultValue\": 0,\
			\"index\": 2,\
			\"description\": \"预估升级时间(s)\",\
			\"id\": \"downloadTimeout\",\
			\"type\": \"int\",\
			\"value\": 0\
		},\
		{\
			\"access\": 5,\
			\"defaultValue\": \"\",\
			\"index\": 3,\
			\"description\": \"新版本号\",\
			\"id\": \"newVersion\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"defaultValue\": \"\",\
			\"index\": 4,\
			\"description\": \"MCU新版本号\",\
			\"id\": \"MCUNewVersion\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"defaultValue\": \"\",\
			\"index\": 5,\
			\"description\": \"新版本介绍\",\
			\"id\": \"introduction\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 5,\
			\"defaultValue\": \"\",\
			\"index\": 6,\
			\"description\": \"MCU新版本介绍\",\
			\"id\": \"MCUIntroduction\",\
			\"type\": \"string\",\
			\"value\": \"\"\
		},\
		{\
			\"access\": 7,\
			\"defaultValue\": 0,\
			\"index\": 7,\
			\"description\": \"自动升级开关\",\
			\"id\": \"autoOtaSwitch\",\
			\"type\": \"int\",\
			\"value\": 0\
		},\
		{\
			\"access\": 5,\
			\"defaultValue\": 0,\
			\"index\": 8,\
			\"description\": \"升级进度\",\
			\"id\": \"otaProgress\",\
			\"type\": \"int\",\
			\"value\": 0\
		}],\
		\"index\": 3,\
		\"description\": \"升级服务\",\
		\"actionList\": [{\
			\"index\": 0,\
			\"description\": \"检查版本更新\",\
			\"id\": \"checkVersion\"\
		},\
		{\
			\"index\": 1,\
			\"description\": \"触发版本更新\",\
			\"id\": \"otaAction\"\
		}],\
		\"id\": \"ota\",\
		\"title\": \"升级服务\"\
	},\
	{\
		\"propertyList\": [{\
			\"access\": 7,\
			\"defaultValue\": 0,\
			\"valueList\": [{\
				\"description\": \"关\",\
				\"value\": 0\
			},\
			{\
				\"description\": \"开\",\
				\"value\": 1\
			}],\
			\"description\": \"\",\
			\"index\": 0,\
			\"id\": \"On\",\
			\"type\": \"int\",\
			\"value\": 0\
		},\
		{\
			\"access\": 7,\
			\"defaultValue\": 100,\
			\"valueList\": [{\
				\"description\": \"正面发光\",\
				\"value\": 100\
			},\
			{\
				\"description\": \"反面发光\",\
				\"value\": 101\
			},\
			{\
				\"description\": \"两面发光\",\
				\"value\": 102\
			}],\
			\"description\": \"\",\
			\"index\": 1,\
			\"id\": \"lightMode\",\
			\"type\": \"int\",\
			\"value\": 100\
		},\
		{\
			\"access\": 5,\
			\"defaultValue\": 0,\
			\"valueList\": [{\
				\"description\": \"无人\",\
				\"value\": 0\
			},\
			{\
				\"description\": \"有人\",\
				\"value\": 1\
			}],\
			\"description\": \"\",\
			\"index\": 2,\
			\"id\": \"hsStatus\",\
			\"type\": \"int\",\
			\"value\": 0\
		},\
		{\
			\"access\": 7,\
			\"defaultValue\": 0,\
			\"valueList\": [{\
				\"description\": \"关\",\
				\"value\": 0\
			},\
			{\
				\"description\": \"开\",\
				\"value\": 1\
			}],\
			\"description\": \"设备本身有一个自动化判定逻辑，检测到人经过打开灯，可以关闭此自动逻辑\n\",\
			\"index\": 3,\
			\"id\": \"autoSwitch\",\
			\"type\": \"int\",\
			\"value\": 0\
		},\
		{\
			\"access\": 3,\
			\"defaultValue\": 1,\
			\"valueList\": [{\
				\"description\": \"0.5min\",\
				\"value\": 1\
			},\
			{\
				\"description\": \"1min\",\
				\"value\": 2\
			},\
			{\
				\"description\": \"3min\",\
				\"value\": 3\
			},\
			{\
				\"description\": \"10min\",\
				\"value\": 4\
			},\
			{\
				\"description\": \"20min\",\
				\"value\": 5\
			}],\
			\"description\": \"检测到无人后多久关灯，设定了6个档位\",\
			\"index\": 4,\
			\"id\": \"offLightGea\",\
			\"type\": \"int\",\
			\"value\": 1\
		}],\
		\"description\": \"夜灯服务\",\
		\"index\": 4,\
		\"id\": \"nightLight\",\
		\"title\": \"夜灯服务\"\
	},\
	{\
		\"eventList\": [{\
			\"outDataList\": [0],\
			\"description\": \"有人无人\",\
			\"index\": 0,\
			\"eventType\": 0,\
			\"id\": \"humanPresence\"\
		}],\
		\"propertyList\": [{\
			\"access\": 5,\
			\"defaultValue\": 0,\
			\"valueList\": [{\
				\"description\": \"无人\",\
				\"value\": 0\
			},\
			{\
				\"description\": \"有人\",\
				\"value\": 1\
			}],\
			\"index\": 0,\
			\"id\": \"hsStatus\",\
			\"type\": \"int\",\
			\"value\": 0\
		}],\
		\"description\": \"人感\",\
		\"index\": 5,\
		\"id\": \"IRF\",\
		\"title\": \"人感\"\
	}],\
	\"description\": \"阳光荣旭夜灯MTSL2015\",\
	\"title\": \"阳光荣旭夜灯MTSL2015\"\
}\
}";

#ifdef __cplusplus
}
#endif

#endif

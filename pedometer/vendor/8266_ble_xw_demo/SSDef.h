#ifndef SSDEF_H
#define SSDEF_H

#include "../../proj/tl_common.h"

#define CMD_FW_UPDATE               		0x01
#define CMD_GET								0x02
#define CMD_SET								0x03
#define CMD_BIND							0x04
#define CMD_REMIND							0x05
#define CMD_APP_CTRL						0x06
#define CMD_BLE_DEV_CTRL					0x07
#define CMD_HEALTH_DATA						0x08
#define CMD_DUMP_STACK						0x20
#define CMD_LOG								0x21
#define CMD_FACTORY							0xAA
#define CMD_RESTART							0xF0

// get device supported func list.
#define FUNC_PEDOMETER						0x01
#define FUNC_SLEEP_DETEC					0x02
#define FUNC_SINGLE_MOVEMENT				0x04
#define FUNC_REAL_TIME_DATA					0x08
#define FUNC_DEVICE_UPDATE					0x10
#define FUNC_HEART_RATE						0x20
#define FUNC_ANCS							0x40
#define FUNC_USER_DEFINED					0x80

#define ALARM_TYPE_GET_UP				0x01
#define ALARM_TYPE_SLEEP				0x02
#define ALARM_TYPE_EXERCISE				0x04
#define ALARM_TYPE_MEDICINE				0x08
#define ALARM_TYPE_DATE					0x10
#define ALARM_TYPE_PARTY				0x20
#define ALARM_TYPE_MEETING				0x40
#define ALARM_TYPE_USER_DEFINED			0x80

#define FUNC_CTRL_PHOTO						0x01
#define FUNC_CTRL_MUSIC						0x02

#define FUNC_CALL_INCOMING					0x01
#define FUNC_CALL_NAME						0x02
#define FUNC_CALL_NUMBER					0x04

#define FUNC_MSG_NOTIFY_MESSAGE				0x01
#define FUNC_MSG_NOTIFY_EMAIL				0x02
#define FUNC_MSG_NOTIFY_QQ					0x04
#define FUNC_MSG_NOTIFY_WEIXIN				0x08
#define FUNC_MSG_NOTIFY_WEIBO				0x10
#define FUNC_MSG_NOTIFY_FACEBOOK			0x20
#define FUNC_MSG_NOTIFY_TWITTER				0x40
#define FUNC_MSG_NOTIFY_OTHERS				0x80

#define FUNC_MSG_NOTIFY_WHATSAPP			0x01
#define FUNC_MSG_NOTIFY_MESSENGER			0x02
#define FUNC_MSG_NOTIFY_INSTAGRAM			0x04
#define FUNC_MSG_NOTIFY_LINKED				0x08



typedef struct{
	u8 bCmdId;
	u8 bKey;
	u8 abData[18];
}SS_Packet_t;

typedef struct{
	u8 bCmdId;
	u8 bKey;
	u8 abArgument[12];
	u8 abRsv[6];
}SS_REQ_Update_Fw_t;

typedef struct{
	u8 bCmdId;
	u8 bKey;
	u8 bStateCode;
	u8 abRsv[17];
}SS_Rsp_Update_Fw_t;

typedef struct{
	u8 bCmdId;
	u8 bKey;
	u8 abRsv[18];
}SS_REQ_GET_t;

typedef struct{
	u8 bCmdId;
	u8 bKey;
	u8 abData[18];
}SS_RSP_GET_t;

int ss_rsp_fw_update(u8 *pCmd, u8 *pRsp, u8 *pRspLen );
int ss_rsp_get_cmd(u8 *pCmd, u8 *pRsp, u8 *pRspLen );


#endif

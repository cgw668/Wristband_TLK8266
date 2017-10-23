/*
 * ancs.h
 *
 *  Created on: 2016-5-18
 *      Author: Telink
 */

#ifndef ANCS_H_
#define ANCS_H_
///type

#define MAX_NOTIFY_DATA_LIST_NUM	40

#define DATA_MAX_LEN				0x20
#define ATT_ID_NUM					0x03
#define MAX_DATA_SRC_NUM			((DATA_MAX_LEN + 3) * ATT_ID_NUM)

typedef enum{
	ANCS_NO_STATE = 0,					//init state , not allow to send packet about ancs, expect adv packet.
	ANCS_GET_STATE_PREPARE	= 1,		//prepare get ancs infomation after bond
	ANCS_GET_START_HANDLE = 2,			//have send packet about get ancs starting handle
	ANCS_GET_START_HANDLE_FINISH  = 3,		//finish get start handle
	ANCS_WRITE_NOTISRC_CCC = 4,				// write CCC finish , and wait provider send information.
	ANCS_CONNECTION_ESTABLISHED = 5,
}ancs_sevice_state_t;

typedef enum{
	CategroyConnErr = -1,
	CategroyIDOther = 0,
	CategroyIDIncomingCall = 1,
	CategroyIDMissCall = 2,
	CategroyIDVoiceMail = 3,
	CategroyIDSocial	= 4,
	CategroyIDSchedule,
	CategroyIDEmail,
	CategroyIDNews,
	CategroyIDHealthyAndFitness,
	CategroyIDBunissAndFinance,
	CategroyIDLocation,
	CategroyIDEntertainment,
}ancs_categroyID_t;

typedef enum{
	EventIDNotificationAdded,
	EventIDNotificationModified,
	EventIDNotificationRemoved,
}ancs_eventID_t;

typedef enum{
	EventFlagSilent = BIT(0),
	EventFlagImportat = BIT(1),
}ancs_event_flags_t;

typedef enum{
	ATT_ID_APP_IDENTIFIER,
	ATT_ID_TITLE,
	ATT_ID_SUB_TITLE,
	ATT_ID_MESSAGE,
	ATT_ID_MESSAGE_SIZE,
	ATT_ID_DATE,
}ancs_notifyAttIdValues;

typedef enum{
	CMD_ID_GET_NOTIFY_ATTS,
	CMD_ID_GET_APP_ATTS,
}ancs_cmdIdValues;

typedef enum{
	CMD_RSP_PKT_START = BIT(0),
	CMD_RSP_PKT_END,
}ancs_cmdSequence_flag;

typedef struct{
	ancs_eventID_t 		EventID;
	ancs_event_flags_t 	EventFlags;
	ancs_categroyID_t 	CategroyID;
	u8 					CategroyCnt;
	u8 					NotifyUID[4];
}ancs_notifySrc_type_t;

typedef struct{
	u8 cmdId;
	u8 notifyUid[4];
	u8 attIds[7];///this array size can be varied
}ancs_getNotify_atts;

typedef struct{
	u8 cmdId;
	
}ancs_getApp_atts;

typedef struct{
	u8 	attId;
	u16 len;
	u8 	data[DATA_MAX_LEN];
}ancs_notifyAtts;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 	handle[2];
	u8 	data[1];
}rf_pkt_notifyRsp_t;



/////////////////////////ATT HANDLE RELEVANT STRUCTURE DEFINATION///////////////////////////////
#define ATT_DB_UUID16_NUM				20
#define	ATT_DB_UUID128_NUM				20
#define ATT_DB_FLAG_START_HANDLE		0xFF000000
#define	ATT_DB_FLAG_ENDING_HANDLE		0x00FF0000

#define ANCS_NEWS_GET_NOTIFY_ATT		0x01
#define ANCS_NEWS_GET_APP_ATT			0x02

#define ANCS_SEND_INIT_WRITE_REQ		0x80
#define ANCS_SEND_CMD_REQ				0x40

#define ANCS_REQ_GET_NOTIFY_ATT			0x20
#define ANCS_REQ_GET_APP_ATT			0x10


typedef struct {
	u8	num;
	u8	property;
	u16	startHandle;
	u16	endingHandle;
	u16	uuid;
	u16 ref;
} ancs_att_db_uuid16_t;			//8-byte

typedef struct {
	u8	num;///control point
	u8	property;///88
	u16	startHandle;///1f 00
	u16	endingHandle;///20 00	
	u8	uuid[16];///d9 d9 aa fd bd 9b 21 98 a8 49  e1 45 f3 d8 d1 69
} ancs_att_db_uuid128_t;			//20-byte

typedef struct {
	u16 startHandle;
	u16 endingHandle;
} att_db_handle_element;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust 
							//with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u16 handle;
	ancs_getNotify_atts attIdInfo;
}rf_pkt_write_req;

typedef struct{
	u32 dma_len;            //won't be a fixed number as previous, should adjust 
							//with the mouse package number
	u8	type;				//RFU(3)_MD(1)_SN(1)_NESN(1)-LLID(2)
	u8  rf_len;				//LEN(5)_RFU(3)
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u16 startHandle;
	u16 endingHandle;
}rf_pkt_custom_find_info_req;
/////////////////////////ATT HANDLE RELEVANT STRUCTURE DEFINATION///////////////////////////////




int ancsHaveNews ();
void ancsStackCallback(u8 *p);
u8 ancsFuncIsEn();
u8 ancsFuncEn();

#endif /* ANCS_H_ */

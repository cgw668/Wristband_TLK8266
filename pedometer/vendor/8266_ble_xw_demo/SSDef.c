#include "SSDef.h"


u16 wSSDeviceId = 0x1AA1;
u8	bSSFwVer	= 0x01;
u8	bSSMode		= 0x01;
u8	bSSBatStatus	= 0x00;
u8	bSSBatVal	= 99;
u8	bSSPairFlag	= 0x00;
u8	bSSRebootFlag	= 0x01;

u8  bSSFunc_main = FUNC_PEDOMETER | FUNC_ANCS;
u8	bSSAlarm_Cnt	 = 0x00;
u8	bSSAlarm_Type = ALARM_TYPE_GET_UP | ALARM_TYPE_EXERCISE ;
u8	bSSFunc_Ctrl = 0x00;
u8	bSSFunc_Call = FUNC_CALL_INCOMING | FUNC_CALL_NAME | FUNC_CALL_NUMBER;
u8	bSSFunc_Msg_Notify_1 =  FUNC_MSG_NOTIFY_MESSAGE	| FUNC_MSG_NOTIFY_QQ | FUNC_MSG_NOTIFY_WEIXIN;
u8	bSSFunc_other	= 0x00;
u8	bSSFunc_Msg_Cfg	= 0x07;
u8	bSSFunc_Msg_Notify_2 = 0x00;
u8	bSSFunc_other2	= 0x00;


const u8 abValidParam[] = {0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA, 0x55 ,0x55 ,0xAA, 0xAA};
int ss_rsp_fw_update(u8 *pCmd, u8 *pRsp, u8 *pRspLen )
{
	SS_REQ_Update_Fw_t *req = (SS_REQ_Update_Fw_t *)pCmd;
	SS_Rsp_Update_Fw_t *rsp = (SS_Rsp_Update_Fw_t *)pRsp;

	memset( rsp, 0, sizeof(SS_Rsp_Update_Fw_t));
	rsp->bCmdId = 0x01;
	rsp->bKey   = 0x01;
	if( req->bKey == 0x01 && !memcmp(req->abArgument, abValidParam, sizeof(abValidParam))) {
		rsp->bStateCode = 0x02; //not supported.
	}
	else {
		rsp->bStateCode = 0x03;	// error parameter.
	}
	*pRspLen = sizeof(SS_Rsp_Update_Fw_t);
	return 0;
}

int ss_rsp_get_cmd(u8 *pCmd, u8 *pRsp, u8 *pRspLen )
{
	SS_REQ_GET_t *req	= (SS_REQ_GET_t *)pCmd;
	SS_RSP_GET_t *rsp	= (SS_RSP_GET_t *)pRsp;

	memset( rsp, 0, sizeof(SS_Rsp_Update_Fw_t));
	rsp->bCmdId = 0x01;
	rsp->bKey   = req->bKey;

	switch( req->bKey )
	{
		case 0x01:
			{
				memcpy(&rsp->abData[0], wSSDeviceId, sizeof(wSSDeviceId));
				rsp->abData[2] = bSSFwVer;
				rsp->abData[3] = bSSMode;
				rsp->abData[4] = bSSBatStatus;
				rsp->abData[5] = bSSBatVal;
				rsp->abData[6] = bSSPairFlag;
				rsp->abData[7] = bSSRebootFlag;

				*pRspLen = sizeof(SS_RSP_GET_t);
			}
			break;
		case 0x02:
			{
				
			}
			break;
		case 0x03:
			{

			}
			break;
		case 0x04:
			{

			}
			break;
		case 0x05:
			{

			}
			break;
		case 0x06:
			{

			}
			break;
		case 0x10:
			{

			}
			break;
		default :
			*pRspLen = 0;
//			bChannel = 0;		//no rsp.
			break;
	}

	return 0;
}

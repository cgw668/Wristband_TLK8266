/*
 * ancs.c
 *
 *  Created on: 2016-5-18
 *      Author: Telink
 */

#include "../../proj/tl_common.h"

#include "../../proj_lib/ble/ll/ll.h"
#include "ancs.h"

//for old sdk , still blt_push_fifo(). peter.zhong.
#define blt_push_fifo(x) 	bls_ll_pushTxFifo(((u8 *)(x) + 4))
///////////////////////////////////////////////////////////////////////////
//					Externl Variables
//////////////////////////////////////////////////////////////////////////
extern u8		blt_state;


///////////////////////////////////////////////////////////////////////////
//					Local Variables
///////////////////////////////////////////////////////////////////////////
/*
 * Packet  Attribute command: FIND_BY_TYPE_VALUE_REQUEST
 * Used to obtain the handles of attributes that have 16 bit uuid type and attribute value.
 */

//////////////////////////////ANCS///////////////////////////////////
u8 pkt_att_find_by_type_req[33] = {
					sizeof(pkt_att_find_by_type_req) - 4, 0, 0 , 0,   //dma_length   4 bytes   value = sizeof(pkt_att_read_by_type_req) - 4
					0x02,   		// type :  ll data pdu   1 byte
					sizeof(pkt_att_find_by_type_req) - 6, 				// rf_length	1 byte  value = sizeof(pkt_att_read_by_type_req) - 6

					sizeof(pkt_att_find_by_type_req) - 10, 0, 			// l2cap_length		2 bytes   value = sizeof(pkt_att_read_by_type_req) - 10
					0x04, 0,			// chanID			2 byte
					ATT_OP_FIND_BY_TYPE_VALUE_REQ,  //opcode  1 byte
					0x01, 0x00, 			// starting  handle 2 bytes
					0xff, 0xff,  			// ending	handle  2 bytes
					0x00, 0x28,
					0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, // UUID   16 bytes
};
/**
 * The Write Request is used to request the server to write the value of an attribute and acknowledge that this has been achieved in a Write Response
 */
u8 init_writeReq[15] = {
		sizeof(init_writeReq) - 4, 0, 0 , 0,   //dma_length   4 bytes   value = sizeof(pkt_att_read_by_type_req) - 4
		0x02,   		// type :  ll data pdu   1 byte
		sizeof(init_writeReq) - 6, 				// rf_length	1 byte  value = sizeof(pkt_att_read_by_type_req) - 6

		sizeof(init_writeReq) - 10, 0, 			// l2cap_length		2 bytes   value = sizeof(pkt_att_read_by_type_req) - 10
		0x04, 0,			// chanID			2 byte
		ATT_OP_WRITE_REQ,  //opcode  1 byte
		0x24, 0x00, 			// handle
		0x01, 0x00,  			// value
};
///////////////////////////////////Service UUID////////////////////////////////////////
const u8 ancs_primary_service_uuid[16] =
	{0xD0, 0x00, 0x2D, 0x12, 0x1E, 0x4B, 0x0F, 0xA4, 0x99, 0x4E, 0xCE, 0xB5, 0x31, 0xF4, 0x05, 0x79};

const u8 ancs_control_point_uuid[16] = 
	{0xD9, 0xD9, 0xAA, 0xFD, 0xBD, 0x9B, 0x21, 0x98, 0xA8, 0x49, 0xE1, 0x45, 0xF3, 0xD8, 0xD1, 0x69};

const u8 ancs_notify_source_uuid[16] =
	{0xBD, 0x1D, 0xA2, 0x99, 0xE6, 0x25, 0x58, 0x8C, 0xD9, 0x42, 0x01, 0x63, 0x0D, 0x12, 0xBF, 0x9F};

const u8 ancs_data_source_uuid[16] =
	{0xFB, 0x7B, 0x7C, 0xCE, 0x6A, 0xB3, 0x44, 0xBE, 0xB5, 0x4B, 0xD6, 0x24, 0xE9, 0xC6, 0xEA, 0x22};

///////////////////////////////////////App identifier///////////////////////////////////
const u8 appId_call[] 		= 
	{'c','o','m','.','a','p','p','l','e','.','m','o','b','i','l','e','p','h','o','n','e'};
const u8 appId_weChat[] 		=
	{'c','o','m','.','t','e','n','c','e','n','t','.','x','i','n'};
const u8 appId_SMS[]			=
	{'c','o','m','.','a','p','p','l','e','.','M','o','b','i','l','e','S','M','S'};
const u8 appId_qq[]			=
	{'c','o','m','.','t','e','n','c','e','n','t','.','m','q','q'};
const u8 appId_365[]			=
	{'c','o','m','.','3','6','5','r','i','l','i','.','C','o','c','o'};
const u8 appId_anyDo[]		=
	{'c','o','m','.','a','n','y','d','o','.','A','n','y','D','O'};
const u8 appId_sysReminder[]	=
	{'c','o','m','.','a','p','p','l','e','.','r','e','m','i','n','d','e','r','s'};

u8 	ancs_func_valid 		= 0;
u8	ancs_dataSrcRcvNews		= 0;
u8	ancs_notifySrcRcvNews	= 0;
u16 ancs_notifySrcHandle 	= 0;

ancs_sevice_state_t  	ancs_system_state = ANCS_NO_STATE;
ancs_notifySrc_type_t 	*ancs_notifySrcData_ptr;

static u16 ancs_start_handle 		= 0;

////////////////////////////ATT HANDLE DATABASE////////////////////////////////
ancs_att_db_uuid16_t 					db16[ATT_DB_UUID16_NUM];
ancs_att_db_uuid128_t					db128[ATT_DB_UUID128_NUM];
rf_pkt_write_req 					getNotifyAtt_writeReq;
rf_packet_att_readByType_t			att_readByType_req;
att_db_handle_element				dbHandleList[ATT_DB_UUID16_NUM + ATT_DB_UUID128_NUM];

u8	hdNotifySource	= 0;
u8	hdControlPoint	= 0;
u8	hdDataSource	= 0;

u8	writeReqFlow	= 0;
u8	writeRspFlag	= 0;

int	cmdPktStartIdx	= -1;
u8	cmdPktIdx[ATT_ID_NUM] = {0};

ancs_notifyAtts curNotifyAttsIdTbl[ATT_ID_NUM];

u8 	obtainAttEnable = 0;
u16 curStartHandle 	= 1;
u16 curEndingHandle	= 2;

int i16				= 0;
int i128 			= 0;
int iListPtr 		= 0;

ancs_att_db_uuid16_t 	*p16 	= db16;
ancs_att_db_uuid128_t 	*p128	= db128;

u8	AttsTblIdx = 0;
u8	iCnt	   = 1;

ancs_notifySrc_type_t notifyTbl[MAX_NOTIFY_DATA_LIST_NUM];
ancs_notifySrc_type_t errorReturn;

u8 notifyTblEndPtr		= 0;
u8 notifyTblStartPtr	= 0;
u8 isNotifyTblFull 		= 0;

u8 findInfoFlow		= 0;

u8 ancs_dataSrcData[(MAX_DATA_SRC_NUM + 5) * 2];
u8 myBufLen = 0;

u8 op_getNotifyAtt = 1;

////////////START///////////////ANCS build ios service list /////////////START//////////
void att_storeHandleElement(u16 startHandle, u16 endingHandle){

	dbHandleList[iListPtr].startHandle 	= startHandle;
	dbHandleList[iListPtr].endingHandle	= endingHandle;

	iListPtr++;
}

u32 att_searchAttListByHandle(u16 startHandle, u16 endingHandle){

	u32 idxList	= 0;

	for(u8 i = 0; i < iListPtr; i++){
		
		if((startHandle == dbHandleList[i].startHandle)){
			idxList = ATT_DB_FLAG_START_HANDLE | i;
		}

		if(endingHandle == dbHandleList[i].endingHandle){
			idxList = ATT_DB_FLAG_ENDING_HANDLE | i;
		}
			
		if((idxList & ATT_DB_FLAG_ENDING_HANDLE) | (idxList & ATT_DB_FLAG_START_HANDLE)){
			break;
		}
	}

	return idxList;
}

u8 att_searchDb128ByUUID(u8 *p_uuid){

	for(u8 idx = 0; idx < i128; idx++){
		if(memcmp(db128[idx].uuid, p_uuid, 16) == 0){
			return idx;
		}
	}

	return 0;
}

void att_getAncsHandles(){///excuted once after searching all att handles from master
	
	db16[0].num 	= i16;
	db128[0].num	= i128; 
	
	hdControlPoint 	= att_searchDb128ByUUID(ancs_control_point_uuid);
	hdNotifySource	= att_searchDb128ByUUID(ancs_notify_source_uuid);
	hdDataSource	= att_searchDb128ByUUID(ancs_data_source_uuid);

	#if 0
	///as we get the ancs handles,we need to send write req to the NotifySrc Handle
	///if not, we couldn't get the detail infomation with AttUID
	init_writeReq[11] = db128[hdDataSource].endingHandle + 1;
	init_writeReq[12] = (db128[hdDataSource].endingHandle + 1) >> 8;
	
	if(blt_push_fifo(init_writeReq)){
		writeReqFlow |= ANCS_SEND_INIT_WRITE_REQ;
	}
	#endif
	///initial some fixed parameters of getNotifyAtt_writeReq
	getNotifyAtt_writeReq.dma_len	= sizeof(getNotifyAtt_writeReq) - 4;
	getNotifyAtt_writeReq.type		= 0x02;
	getNotifyAtt_writeReq.rf_len 	= sizeof(getNotifyAtt_writeReq) - 6;
	getNotifyAtt_writeReq.l2capLen	= sizeof(getNotifyAtt_writeReq) - 10;
	getNotifyAtt_writeReq.chanId 	= 0x04;
	getNotifyAtt_writeReq.opcode 	= ATT_OP_WRITE_REQ;	
}

void att_resetAttDb(){

	p16 			= db16;
	p128			= db128;

	i16				= 0;
	i128			= 0;
	iListPtr		= 0;
	curStartHandle 	= 1;
	curEndingHandle	= 2;
	obtainAttEnable = 0;

	writeRspFlag	= 0;
	writeReqFlow	= 0;

	hdControlPoint	= 0;
	hdNotifySource	= 0;
	hdDataSource	= 0;

	ancs_notifySrcHandle = 0;
	
	memset(db16, 0, ATT_DB_UUID16_NUM * sizeof(att_db_uuid16_t));
	memset(db128, 0, ATT_DB_UUID128_NUM * sizeof(att_db_uuid128_t));
	memset(dbHandleList, 0, (ATT_DB_UUID128_NUM + ATT_DB_UUID16_NUM) * sizeof(att_db_handle_element));
}

void att_sendReadByAtt_req(u16 startHandle, u16 endingHandle, u16 uuid, u8 pktOpcode){
	
	obtainAttEnable = 0;

	att_readByType_req.dma_len 			= sizeof(rf_packet_att_readByType_t);
	att_readByType_req.type				= 0x02;
	att_readByType_req.rf_len			= 0x0B;
	att_readByType_req.l2capLen			= 0x07;
	att_readByType_req.chanId			= 0x04;
	att_readByType_req.opcode			= pktOpcode;
	att_readByType_req.startingHandle 	= startHandle;
	att_readByType_req.startingHandle1 	= startHandle >> 8;
	att_readByType_req.endingHandle 	= endingHandle;
	att_readByType_req.endingHandle1 	= endingHandle >> 8;	
	att_readByType_req.attType[0]		= uuid;
	att_readByType_req.attType[1]		= uuid >> 8;

	blt_push_fifo(&att_readByType_req);
}

void att_procReadByGroupTypeRsp (u8 *p)
{
	// process response data
	u16 tmpEndingHandle;
	
	rf_packet_att_readByTypeRsp_t *p_rsp = (rf_packet_att_readByTypeRsp_t *) p;
	
	if (p_rsp->datalen == 0x14)		//uuid128
	{
		tmpEndingHandle = p_rsp->data[2] + ((p_rsp->data[3]) << 8);
		if (i128 < ATT_DB_UUID128_NUM)
		{
			p128->startHandle 	= p_rsp->data[0] + ((p_rsp->data[1]) << 8);			
			p128->endingHandle 	= tmpEndingHandle;
			memcpy (p128->uuid, p_rsp->data + 4, 16);
			att_storeHandleElement(p128->startHandle, p128->endingHandle);
			i128++;
			p128++;
			obtainAttEnable = 1;

		}
	}
	else if (p_rsp->datalen == 0x06) //uuid16
	{
		u8 *pd = p_rsp->data;
		while (p_rsp->l2capLen > 6)
		{
			tmpEndingHandle = pd[2] + (pd[3] << 8);
			if (i16 < ATT_DB_UUID16_NUM)
			{
				p16->startHandle 	= pd[0] + (pd[1] << 8);
				p16->endingHandle	= tmpEndingHandle;
				p16->uuid = pd[4] | (pd[5] << 8);
				att_storeHandleElement(p16->startHandle,p16->endingHandle);				
				i16 ++;
				p16++;
				
				if(obtainAttEnable == 0) obtainAttEnable = 1;

			}
			p_rsp->l2capLen -= 6;
			pd += 6;
		}
	}

	db16[0].num		= i16;
	db128[0].num	= i128;		

	curStartHandle 	= tmpEndingHandle + 1;
}

void att_procErrorRsp(u8 *p){
	rf_packet_att_errRsp_t *err_rsp = (rf_packet_att_errRsp_t *)p;

	if(err_rsp->errReason == (ATT_ERR_ATTR_NOT_FOUND - ATT_ERR_START)){///att not found
		if(err_rsp->errOpcode == ATT_OP_READ_BY_GROUP_TYPE_REQ){
			obtainAttEnable = 2;
		}else if(err_rsp->errOpcode == ATT_OP_READ_BY_TYPE_REQ){
			///jump to next dbHandleList element
			for(u8 i = 0; i < iListPtr; i++){
				if(curEndingHandle == dbHandleList[i].endingHandle){
					if(i == (iListPtr - 1)){
						///if err respone for last req happened in the last handle,then finish read by type operation.
						obtainAttEnable = 3;///finished
						att_getAncsHandles();
						break;
					}else{
						curStartHandle 	= dbHandleList[i + 1].startHandle;
						curEndingHandle	= dbHandleList[i + 1].endingHandle;
						att_sendReadByAtt_req(curStartHandle, curEndingHandle, GATT_UUID_CHARACTER, ATT_OP_READ_BY_TYPE_REQ);			
						break;
					}
				}
			}			
		}
	}
	else{
		///others error reason hanlde
	}

}

void att_procReadByTypeRsp(u8 *p){

	// process response data
	u32 idx = 0;
	u16 tmpStartHandle, tmpEndingHandle = 0;
	
	rf_packet_att_readByTypeRsp_t *p_rsp = (rf_packet_att_readByTypeRsp_t *) p;
	
	if (p_rsp->datalen == 21)		//uuid128
	{
		tmpEndingHandle	= p_rsp->data[3] + ((p_rsp->data[4]) << 8);
		tmpStartHandle	= p_rsp->data[0] + ((p_rsp->data[1]) << 8);
		if (i128 < ATT_DB_UUID128_NUM)
		{
			idx = att_searchAttListByHandle(tmpStartHandle, tmpEndingHandle);
			if(idx & (ATT_DB_FLAG_ENDING_HANDLE | ATT_DB_FLAG_START_HANDLE)){
				db128[idx].property	= p_rsp->data[2];

			}else{///in the store list
				p128->property 		= p_rsp->data[2];
				p128->startHandle	= tmpStartHandle;
				p128->endingHandle	= tmpEndingHandle;
				memcpy (p128->uuid, p_rsp->data + 5, 16);
				i128++;
				p128++;
			}

			obtainAttEnable = 2;
		}
	}
	else if (p_rsp->datalen == 7) //uuid16
	{
		u8 *pd = p_rsp->data;
		while (p_rsp->l2capLen > 7)
		{
			tmpEndingHandle = pd[3] + (pd[4] << 8);
			tmpStartHandle	= pd[0] + (pd[1] << 8);
			if (i16 < ATT_DB_UUID16_NUM)
			{
				idx = att_searchAttListByHandle(tmpStartHandle, tmpEndingHandle);

				if(idx & (ATT_DB_FLAG_ENDING_HANDLE | ATT_DB_FLAG_START_HANDLE)){
					db16[idx].property	= pd[2];
					db16[idx].ref		= 0;
				}else{
					p16->property 		= pd[2];
					p16->startHandle	= tmpStartHandle;
					p16->endingHandle 	= tmpEndingHandle;
					p16->uuid = pd[5] | (pd[6] << 8);
					p16->ref = 0;
					i16 ++;
					p16++;					
				}
				
				obtainAttEnable = 2;
			}
			p_rsp->l2capLen -= 7;
			pd += 7;
		}
	}
	
	curStartHandle = tmpEndingHandle + 1;

	///manage the start handle and ending handle for next event
	if((dbHandleList[iListPtr - 1].endingHandle < curStartHandle)){
		///judgement current ending handle is the last one
		obtainAttEnable = 3;///not set to 0 for avoid the conn_para_update send more
		att_getAncsHandles();
	}else{
		///in the list or not
		if(idx & (ATT_DB_FLAG_START_HANDLE | ATT_DB_FLAG_ENDING_HANDLE)){			
			curStartHandle 	= dbHandleList[(idx & 0xFFFF) + 1].startHandle;
			curEndingHandle	= dbHandleList[(idx & 0xFFFF) + 1].endingHandle;	
		}else if(idx & ATT_DB_FLAG_ENDING_HANDLE){
			///curEndingHandle = tmpEndingHandle;
		}
		///when the curStartHandle was equals to curEndingHandle,it is no logic in this.
		u32 tmpIdx = att_searchAttListByHandle(0, curStartHandle);

		if(tmpIdx & ATT_DB_FLAG_ENDING_HANDLE){
			u16 curHandleIdx = tmpIdx & 0xFFFF;

			if(curHandleIdx == (iListPtr - 1)){
				obtainAttEnable = 3;///finished the procedure of geting service detail
				att_getAncsHandles();
			}else{	
				curStartHandle 	= dbHandleList[curHandleIdx + 1].startHandle;
				curEndingHandle	= dbHandleList[curHandleIdx + 1].endingHandle;					
			}
		}
	}
}

void att_sendFindInfo_req(u16 curIdx){
	rf_pkt_custom_find_info_req myPkt;

	if(curIdx > 3){
		return;
	}

	myPkt.dma_len 		= sizeof(rf_pkt_custom_find_info_req) - 4;
	myPkt.type			= 0x02;
	myPkt.rf_len		= sizeof(rf_pkt_custom_find_info_req) - 5;
	myPkt.l2capLen		= myPkt.rf_len - 4;
	myPkt.chanId		= 0x0004;
	myPkt.opcode		= ATT_OP_FIND_INFO_REQ;
	myPkt.startHandle	= db128[hdControlPoint + curIdx].endingHandle + 1;
	myPkt.endingHandle	= db128[hdControlPoint + curIdx].endingHandle + 1;

	if(blt_push_fifo(&myPkt)){
		
	}
}

void ancs_buildServiceTable(){

	if(obtainAttEnable == 1){
		att_sendReadByAtt_req(curStartHandle, 0xFFFF, GATT_UUID_PRIMARY_SERVICE, ATT_OP_READ_BY_GROUP_TYPE_REQ);				
	}else if(obtainAttEnable == 2){
		if(curStartHandle > curEndingHandle){
			///exit from read by group type req
			curStartHandle	= dbHandleList[0].startHandle;
			curEndingHandle = dbHandleList[0].endingHandle;
		}
		
		att_sendReadByAtt_req(curStartHandle, curEndingHandle, GATT_UUID_CHARACTER, ATT_OP_READ_BY_TYPE_REQ);
	}else if(obtainAttEnable == 3){

		if((!hdControlPoint) || (!hdNotifySource) || (!hdDataSource)){
			return;
		}

		att_sendFindInfo_req(findInfoFlow);
	
	}else if(obtainAttEnable == 4){
		///as we get the ancs handles,we need to send write req to the NotifySrc Handle
		///if not, we couldn't get the detail infomation with AttUID
		init_writeReq[11] = db128[hdNotifySource].endingHandle + 1;
		init_writeReq[12] = (db128[hdNotifySource].endingHandle + 1) >> 8;
		
		if(blt_push_fifo(init_writeReq)){
			writeReqFlow |= ANCS_SEND_INIT_WRITE_REQ;
		}
		obtainAttEnable = 5;
	}
}
////////////END///////////////ANCS build ios service list /////////////END//////////

/*
 * 返回当前连接状态。
 * 返回值： 0  -- 未连接状态。
 * 		   others  -- 连接状态
 * */
u8 blt_get_state(){
	return blt_state;
}

/*
 * 最新消息信息。
 * 类型： ancs_notifySrc_type_t
 * 返回值： ancs_notifySrc_type_t 指针
 * 注意： 新的消息会覆盖旧的消息。
 * */
ancs_notifySrc_type_t* ancs_get_news(){
	return ancs_notifySrcData_ptr;
}

/*
 * Return Current connection state.
 * if return ANCS_CONNECTION_ESTABLISHED, indicate establish connection finished.
 * 返回 当前的连接状态。
 * 如果返回值为ANCS_CONNECTION_ESTABLISHED = 5时，表明连接成功。否则，连接过程正在进行。
 * */
ancs_sevice_state_t ancsConnState(){
	return ancs_system_state;
}


/*
 * 返回当前系统的ANCS功能是否有效
 * */
u8 ancsFuncIsEn(){
	return ancs_func_valid;
}

/*
 * 是能当前系统的ANCS功能，用于当前系统处理BLE协议栈的Response数据。
 * return 0。
 * */
u8 ancsFuncEn(){
	ancs_func_valid = 1;
	if(obtainAttEnable == 0){
		obtainAttEnable = 1;
	}
}

void ancs_resetNotifyTblParm(){
	
	notifyTblStartPtr 	= 0;
	notifyTblEndPtr		= 0;
	findInfoFlow		= 0;
	isNotifyTblFull		= 0;
	myBufLen			= 0;

	memset(&notifyTbl, 0x00, sizeof(notifyTbl));
	memset(&curNotifyAttsIdTbl, 0x00, sizeof(curNotifyAttsIdTbl));
}

void ancs_addNotifyDataTbl(u8* p){
	
	memcpy(&notifyTbl[notifyTblEndPtr++], p, sizeof(ancs_notifySrc_type_t));

	if((notifyTblEndPtr == MAX_NOTIFY_DATA_LIST_NUM) && (isNotifyTblFull == 0)){
		isNotifyTblFull = 1;
	}

	notifyTblEndPtr %= MAX_NOTIFY_DATA_LIST_NUM;

	if(isNotifyTblFull){
		notifyTblStartPtr 	= notifyTblEndPtr;
		isNotifyTblFull		= 0;
	}
}

void ancs_removeNotifyDataFromTbl(u8 curPtr){
	memset(&notifyTbl[curPtr], 0x00, sizeof(ancs_notifySrc_type_t));
}

ancs_notifySrc_type_t ancs_getCurNotify(u8 curPtr){

	if(curPtr > (MAX_NOTIFY_DATA_LIST_NUM - 1)){
		return errorReturn;
	}

	if(memcmp(&notifyTbl[curPtr], &errorReturn, sizeof(ancs_notifySrc_type_t)) == 0){
		return errorReturn;
	}
	
	return notifyTbl[curPtr];
}

void ancs_getCurNotifyAttsData(){
	
	if((writeReqFlow & ANCS_SEND_CMD_REQ) && (writeReqFlow & ANCS_REQ_GET_NOTIFY_ATT)){
		ancs_notifySrc_type_t tmp = ancs_getCurNotify(notifyTblEndPtr - 1);
		u8 *p  = &tmp;
		ancs_sendCmdReq(p, writeReqFlow);		
		#if 0
		ancs_removeNotifyDataFromTbl(notifyTblEndPtr - 1);
		#endif
	}
}

u8 tCnt[4] = {1,1,1,1};
u8 ancs_findCurNotifyIdentifier(){

	u8 ret = 0;

	if(curNotifyAttsIdTbl[0].len == 0){
		return 0;
	}

	if(memcmp(curNotifyAttsIdTbl[0].data, appId_weChat, sizeof(appId_weChat)) == 0){
		ret = 1;
		gpio_write(LED_WHITE, tCnt[ret - 1]++%2);
	}

	if(memcmp(curNotifyAttsIdTbl[0].data, appId_call, sizeof(appId_call)) == 0){
		ret = 2;
		gpio_write(LED_RED, tCnt[ret - 1]++%2);
	}

	if(memcmp(curNotifyAttsIdTbl[0].data, appId_SMS, sizeof(appId_SMS)) == 0){
		ret = 3;
		gpio_write(LED_BLUE, tCnt[ret - 1]++%2);
	}

	if(memcmp(curNotifyAttsIdTbl[0].data, appId_qq, sizeof(appId_qq)) == 0){
		ret = 4;
		gpio_write(LED_GREEN, tCnt[ret - 1]++%2);
	}

	if(memcmp(curNotifyAttsIdTbl[0].data, appId_365, sizeof(appId_365)) == 0){
		ret = 5;
	}

	if(memcmp(curNotifyAttsIdTbl[0].data, appId_anyDo, sizeof(appId_anyDo)) == 0){
		ret = 6;
	}

	if(memcmp(curNotifyAttsIdTbl[0].data, appId_sysReminder, sizeof(appId_sysReminder)) == 0){
		ret = 7;
	}

	if(ret){
		memset(&curNotifyAttsIdTbl, 0x00, sizeof(curNotifyAttsIdTbl));
	}

	return ret;

}

  
/*
 * 建立基于 ANCS 的连接，主要是BLE协议栈层的数据交互。
 * 返回当前连接的状态。
 * */
ancs_sevice_state_t ancsEstshConnection(){

	if(ancs_system_state == ANCS_CONNECTION_ESTABLISHED) // finish get ble information
		return ancs_system_state;

	if((ANCS_NO_STATE == ancs_system_state)/* && ancs_bond_start_tick && clock_time_exceed(ancs_bond_start_tick, 2*1000*1000)*/){

		ancs_system_state = ANCS_GET_STATE_PREPARE;

	}else if(ANCS_GET_STATE_PREPARE == ancs_system_state){

		memcpy( pkt_att_find_by_type_req + 17, ancs_primary_service_uuid, 16);
		if(blt_push_fifo(pkt_att_find_by_type_req)){
			ancs_system_state = ANCS_GET_START_HANDLE;
		}

	}else if(ANCS_GET_START_HANDLE_FINISH == ancs_system_state){

		u16 ancs_notifySrc_ccc_handle 	= ancs_start_handle + 6;///notify src
		ancs_notifySrcHandle 			= ancs_start_handle + 5;

		init_writeReq[11] = ancs_notifySrc_ccc_handle & 0xff;
		init_writeReq[12] = (ancs_notifySrc_ccc_handle >> 8) & 0xff;
		ancs_system_state = ANCS_WRITE_NOTISRC_CCC;

		///if(blt_push_fifo(init_writeReq)){
			ancs_system_state = ANCS_CONNECTION_ESTABLISHED;
		///}
	}
	return ancs_system_state;
}

/*
 * 获取当前新信息的状态。
 * 返回新信息的类型。
 * 返回值：
 * 		   非0 -- 新消息类型。
 * 		   0 -- 无新消息。
 * 		   -1 -- 连接未建立成功。
 * */
void ancs_sendCmdReq(u8 *p, u8 req){

	if(req & ANCS_REQ_GET_NOTIFY_ATT){
		
		ancs_notifySrc_type_t 	*p_notifyData = (ancs_notifySrc_type_t *) p;
		ancs_getNotify_atts 	getNotifyAtts;

		getNotifyAtts.cmdId		= 0x00;
		getNotifyAtts.attIds[0]	= ATT_ID_APP_IDENTIFIER;///not allow to set length for this attID
		getNotifyAtts.attIds[1]	= ATT_ID_TITLE;
		getNotifyAtts.attIds[2]	= 0x20;
		getNotifyAtts.attIds[3]	= 0x00;///length settle 16bits
		getNotifyAtts.attIds[4]	= ATT_ID_MESSAGE;
		getNotifyAtts.attIds[5]	= 0x20;
		getNotifyAtts.attIds[6]	= 0x00;
		///getNotifyAtts.attIds[7]	= ATT_ID_MESSAGE_SIZE;	
		
		memcpy(getNotifyAtts.notifyUid, p_notifyData->NotifyUID, 4);

		getNotifyAtt_writeReq.handle = db128[hdControlPoint].endingHandle;
		memcpy(&getNotifyAtt_writeReq.attIdInfo, &getNotifyAtts.cmdId, sizeof(ancs_getNotify_atts));

		blt_push_fifo(&getNotifyAtt_writeReq);

		writeRspFlag |= ANCS_NEWS_GET_NOTIFY_ATT;///mark	
		writeReqFlow &= (~ANCS_REQ_GET_NOTIFY_ATT);
	}else if(req & ANCS_REQ_GET_APP_ATT){
		///proc ANCS_REQ_GET_APP_ATT

		writeRspFlag |= ANCS_NEWS_GET_APP_ATT;
	}
}

///judge that notify from NP were all recieved or not
u8	searchAttCmd(){	
	for(u16 i = 0; i < myBufLen; i++){///find out the start index of ancs_dataSrcData[]
		if(ancs_dataSrcData[i] == getNotifyAtt_writeReq.attIdInfo.cmdId){
			if(memcmp(ancs_dataSrcData + i + 1, getNotifyAtt_writeReq.attIdInfo.notifyUid, 0x04) == 0){

				u8 	validAttIdNum 	= 0;
				u16 lastAttIdIdx	= 0;
				u16 lastAttIdLen 	= 0;
				
				for(u8 pos = i + 5; pos < (i + MAX_DATA_SRC_NUM + 5);){///try to search last att id

					if(((validAttIdNum == 0) && (ancs_dataSrcData[pos] == ATT_ID_APP_IDENTIFIER)) 
						||((ancs_dataSrcData[pos] > 0) && (ancs_dataSrcData[pos] < 9))){
						///20170323,the max number of att id is 9.
						validAttIdNum++;

						if(validAttIdNum == ATT_ID_NUM){
							lastAttIdIdx = pos;
							break;
						}else{
							pos+=3;
						}
					}else{
						pos++;
					}
				}

				if(lastAttIdIdx){///judge the data all valid
					cmdPktStartIdx = i;
					lastAttIdLen = (ancs_dataSrcData[lastAttIdIdx + 2] << 8) + ancs_dataSrcData[lastAttIdIdx + 1];
					///lastAttIdIdx+=3;
					///memcpy(T_temp2, &ancs_dataSrcData[lastAttIdIdx + 3], 0x20);
					for(u16 idx = 0; idx < lastAttIdLen; idx++){
						if(ancs_dataSrcData[lastAttIdIdx + 3 + idx] == 0){
							cmdPktStartIdx = -1;
							return 0;
						}
					}
					
					return 1;
				}
			}
		}
	}

	return 0;
}

int ancs_parseCmdRspPkt(){
	
	u8	cmdRspType;
	u16 totalLen;
	
	totalLen			= myBufLen;
	cmdRspType 			= ancs_dataSrcData[0];
	ancs_dataSrcRcvNews = 0;

	if(cmdPktStartIdx < 0){
		return -2;
	}

	if(cmdRspType == CMD_ID_GET_NOTIFY_ATTS){

		u8 *ptr = ancs_dataSrcData + cmdPktStartIdx + 5;
		
		do{
			if(AttsTblIdx){
				for(u8 i = 0; i < AttsTblIdx; i++){
					u8 len = *(ptr + 1) + (*(ptr + 2) << 8);
					
					if((*ptr == curNotifyAttsIdTbl[i].attId) && (len == curNotifyAttsIdTbl[i].len)){
						ptr = ptr + 1 + 2 + len;
					}
				}
			}
			
			u8 tmpLen	= *(ptr + 1) + (*(ptr + 2) << 8); 

			memcpy(&(curNotifyAttsIdTbl[AttsTblIdx].attId), ptr, tmpLen + 3);
			ptr += (tmpLen + 3);
			AttsTblIdx++;
			
			if(AttsTblIdx == ATT_ID_NUM){
				myBufLen 		= 0;
				AttsTblIdx 		= 0;
				cmdPktStartIdx 	= -1;
				
				memset(ancs_dataSrcData, 0x00, sizeof(ancs_dataSrcData));
				
				writeRspFlag &= (~ANCS_NEWS_GET_NOTIFY_ATT);
				break;
			}

			totalLen -= (tmpLen + 3);			
		}while(totalLen);

		#if 0
		if(curNotifyAttsIdTbl[0].attId == ATT_ID_APP_IDENTIFIER){
			if(memcmp(curNotifyAttsIdTbl[0].data, appId_weChat, curNotifyAttsIdTbl[0].len) == 0){
				gpio_write(LED_RED, iCnt++%2);
			}
		}

		#endif
	
	}else if(cmdRspType == CMD_ID_GET_APP_ATTS){

	}else{
	
	}

	return -1;
}

void ancs_sendInitWriteReq(){

	init_writeReq[11] = db128[hdDataSource].endingHandle + 1;
	init_writeReq[12] = (db128[hdDataSource].endingHandle + 1) >> 8;

	blt_push_fifo(init_writeReq);
	
	writeReqFlow &= (~ANCS_SEND_INIT_WRITE_REQ);
	writeReqFlow |= ANCS_SEND_CMD_REQ;
}
	
volatile u8 a_test_data[8] = {0};

ancs_categroyID_t ancsNewsType(){
	
	memcpy (a_test_data, ancs_notifySrcData_ptr, 8);
	ancs_notifySrcRcvNews = 0;
	
	return ancs_notifySrcData_ptr->CategroyID;
#if 0
		if(ancs_notifySrcData_ptr->EventID == EventIDNotificationAdded || ancs_notifySrcData_ptr->EventID == EventIDNotificationModified){
			if(ancs_notifySrcData_ptr->CategroyID == CategroyIDIncomingCall || ancs_notifySrcData_ptr->CategroyID == CategroyIDNews)
			{
//				buzzer_enter_mode(BUZZER_MODE_ALERT);
			}
		}else{
//			buzzer_enter_mode(BUZZER_MODE_OFF);
		}
#endif
}

int ancs_procNews(){
	if(ancs_notifySrcRcvNews){
		return ancsNewsType();
	}else if(ancs_dataSrcRcvNews){
		return ancs_parseCmdRspPkt();
	}
}


/*
 * 主要用于处理ANCS连接的过程。
 * 包括两个过程：
 * 1. 建立连接完成之前，建立连接的过程。
 * 2. 建立连接完成之后， 用于获取是否有新的消息，以及获取新的消息的类型。
 * 返回值：
 * 			非0 -- 新消息类型。
 * 		   	0 -- 无新消息。
 * 		   	-1 -- 连接未建立成功。
 * */

int ancsHaveNews ()///ancs send cmd out
{
	if(blt_get_state() == 0){
		return -1;  // no connection
	}

	if(ancs_func_valid == 0){
		return -1;
	}
	if(ancs_func_valid && (ANCS_CONNECTION_ESTABLISHED != ancs_system_state)){	// establishing connection
		ancsEstshConnection();
		return -1;
	}else{	//  to attain ancs state
		///try to read att
		if(obtainAttEnable && (ancsConnState() == ANCS_CONNECTION_ESTABLISHED)){
			ancs_buildServiceTable();
		}
		
		return ancs_procNews();
	}
}


/*
 * 用于处理协议栈回调函数。
 * 接收最新的状态信息。
 * */
void ancsStackCallback(u8 *p){///ancs recieve rsp data from ios and handle them

	rf_packet_l2cap_req_t * req = (rf_packet_l2cap_req_t *)p;
	
	switch(req->opcode){
	case ATT_OP_FIND_BY_TYPE_VALUE_RSP:
		if(ANCS_GET_START_HANDLE == ancs_system_state){
			ancs_start_handle = req->data[0] | (req->data[1] << 8);
			ancs_system_state =  ANCS_GET_START_HANDLE_FINISH;
		}
		break;
	case ATT_OP_READ_BY_GROUP_TYPE_RSP:	
		att_procReadByGroupTypeRsp(p);
		break;
			
	case ATT_OP_ERROR_RSP:	
		att_procErrorRsp(p);
		if(writeReqFlow & ANCS_SEND_INIT_WRITE_REQ){
			ancs_sendInitWriteReq();
		}
		break;		
	case ATT_OP_READ_BY_TYPE_RSP:	
		att_procReadByTypeRsp(p);
		break;
	case ATT_OP_FIND_INFO_RSP:///it maybe need a func to handle the rsp pkt
		findInfoFlow = BIT(findInfoFlow);
		if(findInfoFlow > 2){
			obtainAttEnable = 4;
		}
		break;
	case ATT_OP_WRITE_RSP:	
		if(writeReqFlow & ANCS_SEND_INIT_WRITE_REQ){
			ancs_sendInitWriteReq();
		}
		break;		
	case ATT_OP_HANDLE_VALUE_NOTI:{
			u16 tmpHandle = 0;
			tmpHandle = (req->data[0] | (req->data[1] << 8));
			if(ancs_notifySrcHandle == tmpHandle){			
				ancs_notifySrcData_ptr  = (ancs_notifySrc_type_t *)(req->data + 2);
				ancs_notifySrcRcvNews 	= 1;
				ancs_addNotifyDataTbl(ancs_notifySrcData_ptr);

				if(op_getNotifyAtt && (myBufLen == 0)){
					writeReqFlow |= ANCS_REQ_GET_NOTIFY_ATT;
				}
			}
			if(db128[hdDataSource].endingHandle == tmpHandle){
				
				if(writeRspFlag & (ANCS_NEWS_GET_APP_ATT | ANCS_NEWS_GET_NOTIFY_ATT)){
					
					rf_pkt_notifyRsp_t *T_p = (rf_pkt_notifyRsp_t *)p;

					memcpy(ancs_dataSrcData + myBufLen, T_p->data, T_p->l2capLen - 3);
					myBufLen += (T_p->l2capLen - 3);

					ancs_dataSrcRcvNews = searchAttCmd();
				}
			}		
			break;
		}
	}
}

/*
 * ANCS 初始化，用于初始化一些ANCS相关的变量。
 * 一般情况下，在程序上电或者断开连接的情况下会调用。
 * */
void ancsInit(){

	att_resetAttDb();
	ancs_resetNotifyTblParm();
	
	ancs_system_state 		= ANCS_NO_STATE;
	ancs_notifySrcRcvNews	= 0;
	ancs_dataSrcRcvNews		= 0;

	ancs_func_valid = 0;
}

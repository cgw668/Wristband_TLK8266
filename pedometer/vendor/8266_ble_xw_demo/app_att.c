#include "../../proj/tl_common.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj_lib/weixin/weixin.h"

#include "SSDef.h"

#if (__PROJECT_8261_BLE_WEIXIN__ ||__PROJECT_8266_BLE_XW_DEMO__)
//////////////////////////////////////////////////////////////////
///				GATT SERVICE LENGTH //////////////////////////////
#define WX_INDICATION_OFFSET_IN_GATT		13+5


typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  u16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  u16 timeout;
} gap_periConnectParams_t;

const u16 clientCharacterCfgUUID 		= GATT_UUID_CLIENT_CHAR_CFG;
const u16 extReportRefUUID 				= GATT_UUID_EXT_REPORT_REF;
const u16 reportRefUUID 				= GATT_UUID_REPORT_REF;
const u16 characterPresentFormatUUID 	= GATT_UUID_CHAR_PRESENT_FORMAT;
const u16 my_primaryServiceUUID 		= GATT_UUID_PRIMARY_SERVICE;
static const u16 my_characterUUID		= GATT_UUID_CHARACTER;

const u16 my_devServiceUUID 			= SERVICE_UUID_DEVICE_INFORMATION;
const u16 my_PnPUUID 					= CHARACTERISTIC_UUID_PNP_ID;
const u16 my_devNameUUID 				= GATT_UUID_DEVICE_NAME;

//device information
const u16 my_gapServiceUUID 			= SERVICE_UUID_GENERIC_ACCESS;
// Device Name Characteristic Properties
static u8 my_devNameCharacter 			= CHAR_PROP_READ | CHAR_PROP_NOTIFY;
// Appearance Characteristic Properties
const u16 my_appearanceUIID 			= 0x2a01;
const u16 my_periConnParamUUID 			= 0x2a04;
static u8 my_appearanceCharacter 		= CHAR_PROP_READ;
// Peripheral Preferred Connection Parameters Characteristic Properties
static u8 my_periConnParamChar 			= CHAR_PROP_READ;
u16 my_appearance 						= GAP_APPEARE_UNKNOWN;
gap_periConnectParams_t my_periConnParameters = {20, 40, 0, 1000};

//const u8	my_devName[] = {'t','R','e','m','o','t','e'};
const u8 my_devName[] = {'Y', 'D', 'Y', '_', 'P', '1', '0', '2'};

// Device Name Characteristic Properties
static u8 my_PnPCharacter = CHAR_PROP_READ;
const u8	my_PnPtrs [] = {0x02, 0x8a, 0x24, 0x66, 0x82, 0x01, 0x00};

//////////////////////// Battery /////////////////////////////////////////////////
const u16 my_batServiceUUID       			= SERVICE_UUID_BATTERY;
static u8 my_batProp 						= CHAR_PROP_READ | CHAR_PROP_NOTIFY;
const u16 my_batCharUUID       				= CHARACTERISTIC_UUID_BATTERY_LEVEL;
u8 		  my_batVal[1] 						= {99};

/////////////////////////////////////////////////////////
const u8 my_OtaUUID[16]		= TELINK_SPP_DATA_OTA;
const u8 my_OtaServiceUUID[16]		= TELINK_OTA_UUID_SERVICE;
const u16 userdesc_UUID		= GATT_UUID_CHAR_USER_DESC;


static u8 my_OtaProp		= CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP;
u8	 	my_OtaData 		= 0x00;
const u8  my_OtaName[] = {'O', 'T', 'A'};

/////////////////////////////////////////////////////////
const u16 my_wxServiceUUID       	= 0xfee7;
const u16 my_wxWriteUUID			= 0xfec7;
const u16 my_wxIndicateUUID			= 0xfec8;
const u16 my_wxReadUUID				= 0xfec9;


const u8  my_wxName[] = {'L', 'i', 'g', 'h', 't'};

static u8 my_wxWriteProp 	= CHAR_PROP_WRITE;
static u8 my_wxIndicateProp = CHAR_PROP_INDICATE;
static u8 my_wxReadProp 	= CHAR_PROP_READ;

u8 my_wxReadData[6] = {0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5};
////////////////////////////////////////////////////////////////////
//		20-byte packet
////////////////////////////////////////////////////////////////////


#if 0
#define			LogMsgUsb	LogMsg
#else
#define			LogMsgUsb
#endif

static	u8 wx_state = 0;
u32	wx_id[2];
u32 wx_challege_n_rsp = 0;
u8	wx_p_md5[16];
u8	*wx_p_mac;
u8	*wx_p_deviceID;
u8	wx_session_key[16], *wx_p_key_init, *wx_p_key;
static u8 my_wxIndicateCCC[2];
static u8 wx_buff_req[256];
static u8 wx_buff_rsp[256];
static int wx_len_rsp, wx_rsp_ready;
static int wx_len_req, wx_len_req_cur;
u8	wx_req_p_dat[256];
u8	wx_req_html;
int		wx_req_dat_len = 0;

int	wx_push_data_request (u8 *pd, int len, int html)
{
	if (wx_req_dat_len || len > 255 || wx_state != 4)
		return 0;

	wx_req_html = html;
	memcpy (wx_req_p_dat, pd, len);
	wx_req_dat_len = len;
	return len;
}

int wxWrite(void * p)
{
	static u32 wx_write;
	wx_write++;
	rf_packet_att_write_t *req = (rf_packet_att_write_t*)p;
	int n = req->l2capLen - 3;
	memcpy (wx_buff_rsp + wx_len_rsp, &req->value, n);
	wx_len_rsp += n;

	//response received from host
	if (wx_len_rsp && wx_len_rsp >= wx_buff_rsp[2] * 256 + wx_buff_rsp[3])
	{
		wx_rsp_ready = wx_len_rsp;
		wx_len_rsp = 0;
	}

	return 1;
}

void task_weixin (int init)
{
	int data_len;
	int data_type;
	int	view_op, view_id, backgroup_op;
	if (init)			//initialization on connect
	{
		wx_state = 0;
		wx_len_rsp = wx_rsp_ready = 0;
		wx_len_req = wx_len_req_cur = 0;
		my_wxIndicateCCC[0] = 0;
		my_wxIndicateCCC[1] = 0;
		wx_req_dat_len = 0;
		return;
	}

	///////////////////// get data from host //////////////
	if (wx_rsp_ready)	//process data from host
	{
		int rsp = wx_buff_rsp[4] * 256 + wx_buff_rsp[5];
		if (rsp == ECI_resp_auth && wx_rsp_ready > 8)
		{
			//session key
			if (wx_state == 1 && wx_process_auth_response(wx_buff_rsp, wx_rsp_ready, wx_p_key) == 16)
			{
				LogMsgUsb ("auth_response with session key: ", wx_p_key, 16);
				wx_state = 2;
			}
			else
			{
				wx_state = 0;
			}
		}
		else if (rsp == ECI_resp_init)
		{
			//check challenge response
			if (wx_state == 3 &&
				wx_process_init_response(wx_buff_rsp, wx_rsp_ready, wx_p_key, wx_id, wx_id + 1, wx_challege_n_rsp, 0))
			{
				LogMsgUsb ("init response with id: ", wx_id, 8);
				wx_state = 4;
			}
			else
			{
				wx_state = 0;
			}

		}
		else if (rsp == ECI_resp_sendData)
		{
			wx_req_dat_len = 0;
			if (wx_state != 4)
			{
				wx_state = 0;
			}
			else
			{
				u8 *pd = wx_process_send_data_response(wx_buff_rsp, wx_rsp_ready, wx_p_key, &data_len);
				if (pd)
				{
					//weixin rsp data handle here.
					LogMsgUsb ("send data response: ", pd, data_len);
				}
			}

		}
		else if (rsp == ECI_push_recvData)
		{
			if (wx_state != 4)
			{
				wx_state = 0;
			}
			else
			{
				u8 *pd = wx_process_recv_data_push(wx_buff_rsp, wx_rsp_ready, wx_p_key, &data_len, &data_type);
				if (pd)
				{
					if (data_type)
						LogMsgUsb ("receive push data from HTML: ", pd, data_len);
					else
						LogMsgUsb ("receive push data from server: ", pd, data_len);
				}
			}

		}
		else if (rsp == ECI_push_switchView)
		{
			if (wx_state != 4)
			{
				wx_state = 0;
			}
			else
			{
				data_len = wx_process_switch_view_push(wx_buff_rsp, wx_rsp_ready, wx_p_key, &view_op, &view_id);
				if (data_len)	//view_op=1: enter; view_op=2: exit; view_id=1: wechat; view_id=2: html
				{
					if (view_id == 1 && view_op == 1)
					{
						LogMsgUsb ("enter WeChat view: ", 0, 0);
					}
					else if (view_id == 1 && view_op == 2)
					{
						LogMsgUsb ("exit WeChat view: ", 0, 0);
					}
					else if (view_id == 2 && view_op == 1)
					{
						LogMsgUsb ("enter HTML view: ", 0, 0);
					}
					else if (view_id == 2 && view_op == 2)
					{
						LogMsgUsb ("exit HTML view: ", 0, 0);
					}
				}

			}
		}
		else if (rsp == ECI_push_switchBackgroud)
		{
			if (wx_state != 4)
			{
				wx_state = 0;
			}
			else
			{
				data_len = wx_process_switch_backgroud_push(wx_buff_rsp, wx_rsp_ready, wx_p_key, &backgroup_op);
				if (data_len)	//op=1: background; op=2: foreground; op=3: sleep
				{

				}
			}
		}
		else if (rsp == ECI_err_decode)
		{
			LogMsgUsb ("receive error code", rsp, 2);
			wx_state = 0;
			wx_len_rsp = wx_rsp_ready = 0;
			wx_len_req = wx_len_req_cur = 0;
		}
		wx_rsp_ready = 0;
	}

	//////////////////// prepare request ///////////////////
	if (!wx_len_req)
	{
		if (!wx_state)
		{
			wx_len_rsp = wx_rsp_ready = 0;
			wx_len_req = wx_len_req_cur = 0;
			wx_len_req = wx_gen_auth_request(wx_buff_req, wx_p_deviceID, wx_p_md5, wx_p_mac, wx_p_key_init);
			LogMsgUsb ("auth request:", wx_buff_req, wx_len_req);
			if (wx_p_key_init)
			{
				wx_p_key = wx_session_key;
				memcpy (wx_p_key, wx_p_key_init, 16);
			}
			else
			{
				wx_p_key = 0;
			}
			wx_state = 1;			//auth_req
		}
		else if (wx_state == 2)		//auth_rsp valid
		{
			wx_challege_n_rsp = clock_time ();
			wx_len_req = wx_gen_init_request(wx_buff_req, 0, (u8 *)&wx_challege_n_rsp, wx_p_key);
			wx_challege_n_rsp = weixin_crc32(0, (u8*)&wx_challege_n_rsp, 4);
			wx_state = 3;			//auth_init
			LogMsgUsb ("init request:", wx_buff_req, wx_len_req);
		}
		else if (wx_state == 4)		//session OK: send data
		{
			if (wx_req_dat_len > 0)
			{
				// send to wechat
				wx_len_req = wx_gen_data_request (wx_buff_req, wx_req_p_dat, wx_req_dat_len, wx_req_html, wx_p_key);
				wx_req_dat_len = -wx_req_dat_len;
				LogMsgUsb ("send data request:", wx_buff_req, wx_len_req);
			}
		}
		////////////////// padding /////////////////////////
		if (wx_len_req)				//padding with 0
		{
			int left = wx_len_req % 20;
			if (left)
			{
				memset (wx_buff_req + wx_len_req, 0, 20 - left);
				wx_len_req += 20 - left;
			}
		}
	}

	//////////////////// sending request /////////////////////
	if (wx_len_req && (my_wxIndicateCCC[0] & 2))
	{
		if (bls_att_pushIndicateData (WX_INDICATION_OFFSET_IN_GATT, wx_buff_req + wx_len_req_cur, 20) == BLE_SUCCESS)
		{
			wx_len_req_cur += 20;
			if (wx_len_req_cur >= wx_len_req)
			{
				wx_len_req = wx_len_req_cur = 0;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////
/*				seensum 	app		service	 1					  */
////////////////////////////////////////////////////////////////////

#define SENSSUM_UUID_SERVICE   	{0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xf0, 0x0a, 0x00, 0x00}		//SENSSUM


const u8 my_SSServiceUUID[16]		= SENSSUM_UUID_SERVICE;
const u8 my_SSChar1UUID[2] 			= {0xF6, 0x0A};
const u8 my_SSChar2UUID[2]			= {0xF4, 0x0A};
const u8 my_SSChar3UUID[2]			= {0xF1, 0x0A};
const u8 my_SSChar4UUID[2]			= {0xF2, 0x0A};

static u8 my_SSChar1Prop 			= CHAR_PROP_READ | CHAR_PROP_WRITE;
static u8 my_SSChar2Prop			= CHAR_PROP_READ | CHAR_PROP_NOTIFY;
static u8 my_SSChar3Prop			= CHAR_PROP_READ | CHAR_PROP_WRITE;
static u8 my_SSChar4Prop			= CHAR_PROP_READ | CHAR_PROP_NOTIFY;

u8 my_SSChar1_data[20] 			= {0x00};
u8 my_SSChar2_data[20]			= {0x00};
u8 my_SSChar2_CCC[2]			= {0x00,0x00};
u8 my_SSChar3_data[20] 			= {0x00};
u8 my_SSChar4_data[20]			= {0x00};
u8 my_SSChar4_CCC[2]			= {0x00,0x00};

////////////////////////////////////////////////////////////////////
/*				seensum 	app	  UART	service	 				  */
////////////////////////////////////////////////////////////////////


#define SENSSUM_UART_PROFILE_UUID	{0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e}		//uart profile uuid
#define SENSSUM_UART_WRITE_UUID		{0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e}		//uart write uuid
#define SENSSUM_UART_READ_UUID		{0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0, 0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e}		//uart read uuid

const u8 my_SSUartServiceUUID[16]	 	= SENSSUM_UART_PROFILE_UUID;
const u8 my_SSUartWriteUUID[16]			= SENSSUM_UART_WRITE_UUID;
const u8 my_SSUartReadUUID[16]			= SENSSUM_UART_READ_UUID;

u8 my_SSUartWriteProp 					= CHAR_PROP_WRITE;
u8 my_SSUartReadProp					= CHAR_PROP_READ | CHAR_PROP_NOTIFY;

u8 my_SSUartWriteBuf[20]			 	= {0x00};
u8 my_SSUartReadBuf[20]					= {0x00};
u8 my_SSUartReadCCC[2]					= {0x00, 0x00};

////////////////////////////////////////////////////////////////////////////////
/*						app interface handler								  */
////////////////////////////////////////////////////////////////////////////////
#define HANDLE_SS_CHAR2							30
#define HANDLE_SS_CHAR4							35
#define HANDLE_SS_UART							41

u8 ab_SSNotify_buff[20] = {0x00};
u8 b_SSNotify_len = 0;
const u8 abNotifyChannel[4] = {0, HANDLE_SS_CHAR2, HANDLE_SS_CHAR4, HANDLE_SS_UART};
u8 bChannel = 0;

void task_seensum()
{
	if( !bChannel && !b_SSNotify_len)
		return;

	bls_att_pushNotifyData(abNotifyChannel[bChannel],ab_SSNotify_buff, b_SSNotify_len );


	bChannel = 0;
	b_SSNotify_len = 0;
	return;
}

int SSPacketWrite(void *p)
{
	rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;

	if( (req->l2cap -3) != 20 )		//packet not 20, ignore packet.
	{
		bChannel = 0;				//no need to rsp? or should return error?
		return 1;
	}

	SS_Packet_t *packet = (SS_Packet_t *)(req->dat);

	switch( packet->bCmdId )
	{
		case CMD_FW_UPDATE:
			{
				ss_rsp_fw_update((u8 *)packet, ab_SSNotify_buff, &b_SSNotify_len );
			}
			break;
		case CMD_GET:
			{

			}
			break;
		case CMD_SET:
			{

			}
			break;
		case CMD_BIND:
			{

			}
			break;
		case CMD_REMIND:
			{

			}
			break;
		case CMD_APP_CTRL:
			{

			}
			break;
		case CMD_BLE_DEV_CTRL:
			{

			}
			break;
		case CMD_HEALTH_DATA:
			{

			}
			break;
		case CMD_DUMP_STACK:
			{

			}
			break;
		case CMD_LOG:
			{

			}
			break;
		case CMD_FACTORY:
			{

			}
			break;
		case CMD_RESTART:
			{

			}
			break;
		default:
			bChannel = 0;
			break;
	}
	return 0;
}

int SSChar1Write(void *p)
{
	bChannel = 1;
	return SSPacketWrite(p);
}

int SSChar3Write(void *p)
{
	bChannel = 2;
	return SSPacketWrite(p);
}

int SSUartWrite(void *p)
{
	bChannel = 3;
	return SSPacketWrite(p);
}


// TM : to modify
const attribute_t my_Attributes[] = {
	{42,0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_devName), (u8*)(&my_devNameUUID), (u8*)(my_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

	// 0008 - 000a  device Information Service
	{3,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_devServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_PnPCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_PnPtrs),(u8*)(&my_PnPUUID), (u8*)(my_PnPtrs), 0},

	////////////////////////////////////// Battery Service /////////////////////////////////////////////////////
	// 000b - 000d
	{3,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_batServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_batProp), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_batVal),(u8*)(&my_batCharUUID), 	(u8*)(my_batVal), 0},	//value

	////////////////////////////////////// Weixin  Service /////////////////////////////////////////////////////
	{8,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_wxServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_wxWriteProp), 0},			//write
	{0,ATT_PERMISSIONS_WRITE,2,1,(u8*)(&my_wxWriteUUID), 	0, &wxWrite},	//value
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_wxIndicateProp), 0},		//indicate
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_wxIndicateUUID), 	0, 0},//value
	{0,ATT_PERMISSIONS_READ|ATT_PERMISSIONS_WRITE,2,sizeof(my_wxIndicateCCC),(u8*)(&clientCharacterCfgUUID), 	(u8*)(my_wxIndicateCCC), 0},	//value
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_wxReadProp), 0},			//read: mac address
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_wxReadData),(u8*)(&my_wxReadUUID), 	(u8*)(my_wxReadData), 0},

	////////////////////////////////////// OTA  Service /////////////////////////////////////////////////////
	{4,ATT_PERMISSIONS_READ, 2,16,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_OtaServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ, 2, 1,(u8*)(&my_characterUUID), 		(u8*)(&my_OtaProp), 0},				//prop
	{0,ATT_PERMISSIONS_WRITE,16,1,(u8*)(&my_OtaUUID),	(&my_OtaData), &otaWrite, &otaRead},			//value
	{0,ATT_PERMISSIONS_READ, 2,sizeof (my_OtaName),(u8*)(&userdesc_UUID), (u8*)(my_OtaName), 0},

	////////////////////////////////////// app-interface  Service //////////////////////////////////////////////
	{11,ATT_PERMISSIONS_READ,2,sizeof(my_SSServiceUUID),(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_SSServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_SSChar1Prop), 0},			
	{0,ATT_PERMISSIONS_READ|ATT_PERMISSIONS_WRITE,sizeof(my_SSChar1UUID),sizeof(my_SSChar1_data),(u8*)(&my_SSChar1UUID), (u8*)(&my_SSChar1_data), &SSChar1Write},//, &wxWrite},	//value
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_SSChar2Prop), 0},			
	{0,ATT_PERMISSIONS_READ,sizeof(my_SSChar2UUID),sizeof(my_SSChar2_data),(u8*)(&my_SSChar2UUID), (u8*)(&my_SSChar2_data)},//	notify
	{0,ATT_PERMISSIONS_READ|ATT_PERMISSIONS_WRITE,2,sizeof(my_SSChar2_CCC),(u8*)(&clientCharacterCfgUUID), 	(u8*)(my_SSChar2_CCC), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_SSChar3Prop), 0},			
	{0,ATT_PERMISSIONS_READ|ATT_PERMISSIONS_WRITE,sizeof(my_SSChar3UUID),sizeof(my_SSChar3_data),(u8*)(&my_SSChar3UUID), (u8*)(&my_SSChar3_data), &SSChar3Write},//, &wxWrite},	//value
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_SSChar4Prop), 0},			
	{0,ATT_PERMISSIONS_READ,sizeof(my_SSChar4UUID),sizeof(my_SSChar4_data),(u8*)(&my_SSChar4UUID), (u8*)(&my_SSChar4_data)},//  notify
	{0,ATT_PERMISSIONS_READ|ATT_PERMISSIONS_WRITE,2,sizeof(my_SSChar4_CCC),(u8*)(&clientCharacterCfgUUID), 	(u8*)(my_SSChar4_CCC), 0},	
	
	////////////////////////////////////// app-uart  Service ///////////////////////////////////////////////////
	{6,ATT_PERMISSIONS_READ,2,sizeof(my_SSUartServiceUUID),(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_SSUartServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_SSUartWriteProp), 0},			
	{0,ATT_PERMISSIONS_READ|ATT_PERMISSIONS_WRITE,sizeof(my_SSUartWriteUUID),sizeof(my_SSUartWriteBuf),(u8*)(&my_SSUartWriteUUID), (u8*)(&my_SSUartWriteBuf), &SSUartWrite},//, &wxWrite},	//value
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_SSUartReadProp), 0},			
	{0,ATT_PERMISSIONS_READ,sizeof(my_SSUartReadUUID),sizeof(my_SSUartReadBuf),(u8*)(&my_SSUartReadUUID), (u8*)(&my_SSUartReadBuf)},//  notify
	{0,ATT_PERMISSIONS_READ|ATT_PERMISSIONS_WRITE,2,sizeof(my_SSUartReadCCC),(u8*)(&clientCharacterCfgUUID), 	(u8*)(my_SSUartReadCCC), 0},

};



void my_att_init (u8 *p_mac, u8 *p_device_type, u8 *p_device_id, u8 *p_key)
{
	bls_att_setAttributeTable ((u8 *)my_Attributes);
	ts_md5 ((char *)p_device_type, (char *)p_device_id, wx_p_md5);
	wx_p_mac = p_mac;
	wx_p_deviceID = p_device_id;
	wx_p_key_init = p_key;
}

#endif

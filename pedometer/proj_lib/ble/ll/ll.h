#ifndef LL__H_
#define LL__H_



#include "../../../proj/tl_common.h"


#include "../ble_common.h"
#include "../att.h"
#include "../uuid.h"
#include "../l2cap.h"
#include "../gap.h"
#include "../hci/hci_const.h"
#include "../hci/hci_event.h"
#include "../blt_config.h"

#include "ll_encrypt.h"
#include "ll_pm.h"
#include "ll_adv.h"
#include "ll_scan.h"
#include "ll_init.h"
#include "ll_whitelist.h"
#include "ll_slave.h"
#include "ll_master.h"

extern u8					blt_state;

#define         VENDOR_ID                       0x0211
#define			BLUETOOTH_VER_4_0				6
#define			BLUETOOTH_VER_4_1				7
#define			BLUETOOTH_VER_4_2				8
#define			BLUETOOTH_VER					BLUETOOTH_VER_4_2
#define			BLUETOOTH_VER_SUBVER			0x4103

void blt_set_bluetooth_version (u8 v);


/////////////////////////////////////////////////////////////////////////////
#define		CLOCK_SYS_CLOCK_1250US			(1250 * sys_tick_per_us)
#define		FLG_RF_CONN_DONE	(FLD_RF_IRQ_CMD_DONE | FLD_RF_IRQ_FSM_TIMEOUT | FLD_RF_IRQ_FIRST_TIMEOUT | FLD_RF_IRQ_RX_TIMEOUT)


/////////////////////////////////////////////////////////////////////////////
#define 				LL_ROLE_MASTER              0
#define 				LL_ROLE_SLAVE               1

#define					BLM_CONN_HANDLE				BIT(7)
#define					BLS_CONN_HANDLE				BIT(6)
/////////////////////////////////////////////////////////////////////////////
#define					LL_CONNECTION_UPDATE_REQ	0x00
#define					LL_CHANNEL_MAP_REQ			0x01
#define					LL_TERMINATE_IND			0x02

#define					LL_UNKNOWN_RSP				0x07

#define 				LL_FEATURE_REQ              0x08
#define 				LL_FEATURE_RSP              0x09

#define 				LL_VERSION_IND              0x0C
#define 				LL_REJECT_IND         		0x0D
#define 				LL_SLAVE_FEATURE_REQ        0x0E

#define 				LL_PING_REQ					0x12
#define					LL_PING_RSP					0x13
#define 				LL_LENGTH_REQ				0x14
#define					LL_LENGTH_RSP				0x15

#define					SLAVE_LL_ENC_OFF			0
#define					SLAVE_LL_ENC_REQ			1
#define					SLAVE_LL_ENC_RSP_T			2
#define					SLAVE_LL_ENC_START_REQ_T	3
#define					SLAVE_LL_ENC_START_RSP		4
#define					SLAVE_LL_ENC_START_RSP_T	5
#define					SLAVE_LL_ENC_PAUSE_REQ		6
#define					SLAVE_LL_ENC_PAUSE_RSP_T	7
#define					SLAVE_LL_ENC_PAUSE_RSP		8
#define					SLAVE_LL_REJECT_IND_T		9

#define					MASTER_LL_ENC_OFF				0
#define					MASTER_LL_ENC_REQ				1
#define					MASTER_LL_ENC_RSP_T				2
#define					MASTER_LL_ENC_START_REQ_T		3
#define					MASTER_LL_ENC_START_RSP			4
#define					MASTER_LL_ENC_START_RSP_T		5
#define					MASTER_LL_ENC_PAUSE_REQ			6
#define					MASTER_LL_ENC_PAUSE_RSP_T		7
#define					MASTER_LL_ENC_PAUSE_RSP			8
#define					MASTER_LL_REJECT_IND_T			9
#define					MASTER_LL_ENC_SMP_INFO_S		10
#define					MASTER_LL_ENC_SMP_INFO_E		11

#define					LL_ENC_REQ					0x03
#define					LL_ENC_RSP					0x04
#define					LL_START_ENC_REQ			0x05
#define					LL_START_ENC_RSP			0x06

#define					LL_PAUSE_ENC_REQ			0x0a
#define					LL_PAUSE_ENC_RSP			0x0b
#define					LL_REJECT_IND				0x0d



//ble link layer state
#define			BLS_LINK_STATE_IDLE				0
#define			BLS_LINK_STATE_ADV				BIT(0)
#define			BLS_LINK_STATE_SCAN				BIT(1)
#define			BLS_LINK_STATE_INIT				BIT(2)
#define 		BLS_LINK_STATE_CONN				BIT(3)

//#define 		BLS_LINK_STATE_CONN_SLAVE		BIT(3)
//#define 		BLS_LINK_STATE_CONN_MASTER		BIT(4)



#define				BLE_STATE_IDLE			0
#define				BLE_STATE_ADV			1
#define				BLE_STATE_SCAN			2
#define				BLE_STATE_INIT			3

#define				BLE_STATE_BTX_S			4
#define				BLE_STATE_BTX_E			5
#define				BLE_STATE_BRX_S			6
#define				BLE_STATE_BRX_E			7

#define				BLE_STATE_SLOT			8



#define			BLS_FLAG_ADV_IN_SLAVE_MODE				BIT(6)
#define			BLS_FLAG_SCAN_ENABLE					BIT(0)



#define			MAX_OCTETS_DATA_LEN_27					27
#define			MAX_OCTETS_DATA_LEN_EXTENSION			251


#define			LL_PACKET_OCTET_TIME(n)				((n) * 8 + 112)

#define 		DATA_LENGTH_REQ_PENDING				1
#define			DATA_LENGTH_REQ_DONE				2


my_fifo_t			blt_rxfifo;
u8					blt_rxfifo_b[];

my_fifo_t			blt_txfifo;
u8					blt_txfifo_b[];
//////////////////////////////////////


typedef struct {
	u8		macAddress_public[6];
	u8		macAddress_random[6];   //host may set this
}ll_mac_t;

ll_mac_t  bltMac;

typedef struct {
	u8		ll_recentAvgRSSI;
} st_ll_conn_t;

st_ll_conn_t  bltParam;


typedef struct {
	u16		connEffectiveMaxRxOctets;
	u16		connEffectiveMaxTxOctets;
	u16 	connMaxRxOctets;
	u16 	connMaxTxOctets;
	u16		connRemoteMaxRxOctets;
	u16 	connRemoteMaxTxOctets;
	u16		supportedMaxRxOctets;
	u16		supportedMaxTxOctets;

	u16 	connInitialMaxTxOctets;
	u8		connMaxTxRxOctets_req;
}ll_data_extension_t;

ll_data_extension_t  bltData;





////////////////// Telink defined Event Callback  ////////////////////////
typedef void (*blt_event_callback_t)(u8 e, u8 *p, int n);

#define			BLT_EV_FLAG_ADV						0
#define			BLT_EV_FLAG_ADV_DURATION_TIMEOUT	1
#define			BLT_EV_FLAG_SCAN_RSP				2
#define			BLT_EV_FLAG_CONNECT					3    //
#define			BLT_EV_FLAG_TERMINATE				4    //
#define			BLT_EV_FLAG_PAIRING_BEGIN			5
#define			BLT_EV_FLAG_PAIRING_FAIL			6
#define			BLT_EV_FLAG_ENCRYPTION_CONN_DONE    7
#define			BLT_EV_FLAG_DATA_LENGTH_EXCHANGE	8
#define			BLT_EV_FLAG_GPIO_EARLY_WAKEUP		9
#define			BLT_EV_FLAG_CHN_MAP_REQ				10
#define			BLT_EV_FLAG_CONN_PARA_REQ			11
#define			BLT_EV_FLAG_CHN_MAP_UPDATE			12
#define			BLT_EV_FLAG_CONN_PARA_UPDATE		13
#define			BLT_EV_FLAG_SUSPEND_ENTER			14
#define			BLT_EV_FLAG_SUSPEND_EXIT			15
#define			BLT_EV_FLAG_READ_P256_KEY			16
#define			BLT_EV_FLAG_GENERATE_DHKEY			17
#define			BLT_EV_FLAG_ADV_REPORT				18



#define 		EVENT_MASK_ADV_DURATION_TIMEOUT		BIT(BLT_EV_FLAG_ADV_DURATION_TIMEOUT)
#define			EVENT_MASK_SCAN_RSP					BIT(BLT_EV_FLAG_SCAN_RSP)
#define			EVENT_MASK_CONNECT					BIT(BLT_EV_FLAG_CONNECT)
#define			EVENT_MASK_TERMINATE				BIT(BLT_EV_FLAG_TERMINATE)
#define			EVENT_MASK_CHN_MAP_REQ				BIT(BLT_EV_FLAG_CHN_MAP_REQ)
#define			EVENT_MASK_CONN_PARA_REQ			BIT(BLT_EV_FLAG_CONN_PARA_REQ)
#define			EVENT_MASK_CHN_MAP_UPDATE			BIT(BLT_EV_FLAG_CHN_MAP_UPDATE)
#define			EVENT_MASK_CONN_PARA_UPDATE			BIT(BLT_EV_FLAG_CONN_PARA_UPDATE)
#define			EVENT_MASK_READ_P256_KEY			BIT(BLT_EV_FLAG_READ_P256_KEY)
#define			EVENT_MASK_GENERATE_DHKEY			BIT(BLT_EV_FLAG_GENERATE_DHKEY)



typedef void (*ll_irq_tx_callback_t)(void);

typedef int (*ll_irq_rx_data_callback_t)(u8 *, u32);
typedef int (*ll_irq_rx_post_callback_t)(void);

typedef void (*ll_irq_systemTick_conn_callback_t)(void);


typedef int (*blc_main_loop_data_callback_t)(u8 *);
typedef int (*blc_main_loop_post_callback_t)(void);





typedef int (*blt_LTK_req_callback_t)(u8* rand, u16 ediv);



extern my_fifo_t		hci_tx_fifo;



/******************************* User Interface  ************************************/
void		irq_blt_sdk_handler ();;
int 		blt_sdk_main_loop(void);

void 		blc_ll_initBasicMCU (u8 *public_adr);

ble_sts_t 	blc_ll_setRandomAddr(u8 *randomAddr);


ble_sts_t 	blc_ll_readBDAddr(u8 *addr);

u8			blc_ll_getCurrentState(void);

u8 			blc_ll_getLatestAvgRSSI(void);

u16   		blc_ll_setInitTxDataLength (u16 maxTxOct);   //core4.2 long data packet






// application
void		bls_app_registerEventCallback (u8 e, blt_event_callback_t p);







/************************* Stack Interface, user can not use!!! ***************************/
//encryption
ble_sts_t 		bls_ll_getLtkVsConnHandleFail (u16 connHandle);
ble_sts_t  		bls_ll_setLtk (u16 connHandle,  u8*ltk);

void 			blc_ll_setEncryptionBusy(u8 enc_busy);
bool 			blc_ll_isEncryptionBusy(void);
void 			blc_ll_registerLtkReqEvtCb(blt_LTK_req_callback_t* evtCbFunc);

void 			blc_ll_setIdleState(void);

ble_sts_t 		bls_hci_le_getLocalSupportedFeatures(u8 *features);
ble_sts_t 		bls_hci_le_getRemoteSupportedFeatures(u16 connHandle);


ble_sts_t 		blc_hci_le_readBufferSize_cmd(u8 *pData);


//core4.2 data extension
void 			blc_ll_initDataLengthExtension (void);
ble_sts_t   	blc_ll_exchangeDataLength (u8 opcode, u16 maxTxOct);
ble_sts_t 		blc_hci_setTxDataLength (u16 connHandle, u16 tx, u16 txtime);
ble_sts_t 		blc_hci_readSuggestedDefaultTxDataLength (u8 *tx, u8 *txtime);
ble_sts_t 		blc_hci_writeSuggestedDefaultTxDataLength (u16 tx, u16 txtime);




int blm_send_acl_to_btusb (u16 conn, u8 *p);






static inline u8  blc_ll_getTxFifoNumber (void)
{
	return  ((reg_dma_tx_wptr - reg_dma_tx_rptr) & 7 )  +  ( (blt_txfifo.wptr - blt_txfifo.rptr) & 15 ) ;
}



static inline void blc_ll_resetInfoRSSI(void)
{
	bltParam.ll_recentAvgRSSI = 0;
}

static inline void blc_ll_recordRSSI(u8 rssi)
{
	if(bltParam.ll_recentAvgRSSI == 0) {
		bltParam.ll_recentAvgRSSI = rssi;
	}
	else {
		bltParam.ll_recentAvgRSSI = (bltParam.ll_recentAvgRSSI + rssi) >> 1;
	}
}







#endif /* LL__H_ */

/*
 * ll_scan.h
 *
 *  Created on: 2017-3-7
 *      Author: Administrator
 */

#ifndef LL_SCAN_H_
#define LL_SCAN_H_



typedef struct {
	u8		scan_en;
	u8		scan_type;
	u8		scan_filterPolicy;
	u8		filter_dup;

	u8		scanDevice_num;
	u8		scanRspDevice_num;
	u8		advReport_en;
	s8		T_SCAN_REQ_INTVL;

	//u32		scan_interval;
}st_ll_scan_t;



extern 			rf_packet_scan_req_t	pkt_scan_req;


typedef int (*ll_procScanPkt_callback_t)(u8 *, u8 *, u32);
typedef int (*ll_procScanDat_callback_t)(u8 *);

extern ll_procScanDat_callback_t  blc_ll_procScanDatCb;
extern ll_procScanPkt_callback_t  blc_ll_procScanPktCb;





/******************************* User Interface  ************************************/
void 		blc_ll_initScanning_module(u8 *public_adr);


ble_sts_t   blc_ll_setScanParameter (u8 scan_type, u16 scan_interval, u16 scan_window, u8  ownAddrType, u8 filter_policy);
ble_sts_t 	blc_ll_setScanEnable (u8 scan_enable, u8 filter_duplicate);

int			blc_ll_filterAdvDevice (u8 type, u8 * mac);
int 		blc_ll_addScanRspDevice(u8 type, u8 *mac);
bool 		blc_ll_isScanRspReceived(u8 type, u8 *mac);
void 		blc_ll_clearScanRspDevice(void);






int  	blc_ll_procScanPkt(u8 *raw_pkt, u8 *new_pkt, u32 tick_now);
int  	blc_ll_procScanData(u8 *raw_pkt);


void 	blc_ll_registerProcScanModeCb(ll_procScanPkt_callback_t cb1, ll_procScanDat_callback_t cb2);


#endif /* LL_SCAN_H_ */

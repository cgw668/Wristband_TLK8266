#pragma once

//////////////////////////////////////////////////////////////////////////////
/**
 *  @brief  Definition for Device info
 */

#define  MAX_DEV_NAME_LEN 				18

#ifndef DEV_NAME
#define DEV_NAME                        "tModule"
#endif



/////////////////// Flash  Address Config ////////////////////////////
#if( __TL_LIB_8261__ || (MCU_CORE_TYPE == MCU_CORE_8261) )
	#ifndef		CFG_ADR_MAC
	#define		CFG_ADR_MAC						0x1F000
	#endif

	#ifndef		CUST_CAP_INFO_ADDR
	#define		CUST_CAP_INFO_ADDR				0x1E000
	#endif

	#ifndef		CUST_TP_INFO_ADDR
	#define		CUST_TP_INFO_ADDR				0x1E040
	#endif

	#ifndef		CUST_RC32K_CAP_INFO_ADDR
	#define		CUST_RC32K_CAP_INFO_ADDR		0x1E080
	#endif
#else  //8266 8267 8269
	#ifndef		CFG_ADR_MAC
	#define		CFG_ADR_MAC						0x76000
	#endif

	#ifndef		CUST_CAP_INFO_ADDR
	#define		CUST_CAP_INFO_ADDR				0x77000
	#endif

	#ifndef		CUST_TP_INFO_ADDR
	#define		CUST_TP_INFO_ADDR				0x77040
	#endif

	#ifndef		CUST_RC32K_CAP_INFO_ADDR
	#define		CUST_RC32K_CAP_INFO_ADDR		0x77080
	#endif
#endif


//master
#ifndef		CFG_ADR_PEER
#define		CFG_ADR_PEER					0x78000
#endif


/////////////////// Code Zise & Feature ////////////////////////////

#if ( __TL_LIB_8261__ || (MCU_CORE_TYPE == MCU_CORE_8261) )
	#define BLE_STACK_SIMPLIFY_4_SMALL_FLASH_ENABLE		1
	#define BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE		0
#endif


#ifndef BLE_STACK_SIMPLIFY_4_SMALL_FLASH_ENABLE
#define BLE_STACK_SIMPLIFY_4_SMALL_FLASH_ENABLE			0
#endif


#ifndef BLE_STACK_SIMPLIFY_4_RAMCODE_ENABLE
#define BLE_STACK_SIMPLIFY_4_RAMCODE_ENABLE				0
#endif



//for 8261 128k flash
#if (BLE_STACK_SIMPLIFY_4_SMALL_FLASH_ENABLE)
	#define		BLS_SEND_BT_STD_EVENT_ENABLE					0
	#define		BLS_ADV_INTERVAL_CHECK_ENABLE					0
#endif


//for 8269 32k ramcode
#if (BLE_STACK_SIMPLIFY_4_RAMCODE_ENABLE)
	#define		BLS_ADV_INTERVAL_CHECK_ENABLE					0
#endif










#ifndef BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE
#define BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE			1
#endif





//default ll_master_multi connection
#ifndef  LL_MASTER_SINGLE_CONNECTION
#define  LL_MASTER_SINGLE_CONNECTION					0
#endif

#if (LL_MASTER_SINGLE_CONNECTION )
	#define  LL_MASTER_MULTI_CONNECTION					0
#else
	#define  LL_MASTER_MULTI_CONNECTION					1
#endif











#if (BLE_MODULE_LIB_ENABLE || BLE_MODULE_APPLICATION_ENABLE)  //for ble module
	#define		BLS_DMA_DATA_LOSS_DETECT_AND_SOLVE_ENABLE		1
	#define		BLS_SEND_TLK_MODULE_EVENT_ENABLE				1
	#define		BLS_SEND_BT_STD_EVENT_ENABLE					0
	#define		BLS_ADV_INTERVAL_CHECK_ENABLE					0
#endif



//when rf dma & uart dma work together
#ifndef		BLS_DMA_DATA_LOSS_DETECT_AND_SOLVE_ENABLE
#define		BLS_DMA_DATA_LOSS_DETECT_AND_SOLVE_ENABLE		0
#endif

#ifndef		BLS_SEND_TLK_MODULE_EVENT_ENABLE
#define 	BLS_SEND_TLK_MODULE_EVENT_ENABLE				0
#endif

#ifndef		BLS_SEND_BT_STD_EVENT_ENABLE
#define 	BLS_SEND_BT_STD_EVENT_ENABLE					1
#endif


#ifndef		BLS_ADV_INTERVAL_CHECK_ENABLE
#define		BLS_ADV_INTERVAL_CHECK_ENABLE					1
#endif

#if LIB_TELINK_MESH_SCAN_MODE_ENABLE
#define		BLS_TELINK_MESH_SCAN_MODE_ENABLE				1
#endif

/////////////////  scan mode config  //////////////////////////
#ifndef		BLS_TELINK_MESH_SCAN_MODE_ENABLE
#define		BLS_TELINK_MESH_SCAN_MODE_ENABLE				0
#endif

#if(BLS_TELINK_MESH_SCAN_MODE_ENABLE)
	#define		BLS_BT_STD_SCAN_MODE_ENABLE					0
#else
	#ifndef		BLS_BT_STD_SCAN_MODE_ENABLE
	#define		BLS_BT_STD_SCAN_MODE_ENABLE					1
	#endif
#endif





/////////////////////HCI UART variables///////////////////////////////////////
#define UART_DATA_LEN    64      // data max 252
typedef struct{
    unsigned int len;        // data max 252
    unsigned char data[UART_DATA_LEN];
}uart_data_t;





#ifndef BLE_LL_IRQTASK_RUN_IN_RAM
#define BLE_LL_IRQTASK_RUN_IN_RAM				1
#endif




///////////////////////////////////////dbg channels///////////////////////////////////////////
#ifndef	DBG_CHN0_TOGGLE
#define DBG_CHN0_TOGGLE
#endif

#ifndef	DBG_CHN0_HIGH
#define DBG_CHN0_HIGH
#endif

#ifndef	DBG_CHN0_LOW
#define DBG_CHN0_LOW
#endif

#ifndef	DBG_CHN1_TOGGLE
#define DBG_CHN1_TOGGLE
#endif

#ifndef	DBG_CHN1_HIGH
#define DBG_CHN1_HIGH
#endif

#ifndef	DBG_CHN1_LOW
#define DBG_CHN1_LOW
#endif

#ifndef	DBG_CHN2_TOGGLE
#define DBG_CHN2_TOGGLE
#endif

#ifndef	DBG_CHN2_HIGH
#define DBG_CHN2_HIGH
#endif

#ifndef	DBG_CHN2_LOW
#define DBG_CHN2_LOW
#endif

#ifndef	DBG_CHN3_TOGGLE
#define DBG_CHN3_TOGGLE
#endif

#ifndef	DBG_CHN3_HIGH
#define DBG_CHN3_HIGH
#endif

#ifndef	DBG_CHN3_LOW
#define DBG_CHN3_LOW
#endif

#ifndef	DBG_CHN4_TOGGLE
#define DBG_CHN4_TOGGLE
#endif

#ifndef	DBG_CHN4_HIGH
#define DBG_CHN4_HIGH
#endif

#ifndef	DBG_CHN4_LOW
#define DBG_CHN4_LOW
#endif

#ifndef	DBG_CHN5_TOGGLE
#define DBG_CHN5_TOGGLE
#endif

#ifndef	DBG_CHN5_HIGH
#define DBG_CHN5_HIGH
#endif

#ifndef	DBG_CHN5_LOW
#define DBG_CHN5_LOW
#endif



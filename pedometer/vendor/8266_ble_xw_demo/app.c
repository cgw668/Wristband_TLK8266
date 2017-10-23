#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj/drivers/keyboard.h"
#include "../common/tl_audio.h"
#include "../common/blt_led.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/audio.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../common/blt_soft_timer.h"
#include "../../proj_lib/ble/ble_smp.h"

//ANCS func
#include "ancs.h"


//Pedometer func
#include "../8266_ble_xw_demo/pedometer/drv_KX022.h"
#include "../8266_ble_xw_demo/pedometer/hal_ped.h"

//Display
#include "../8266_ble_xw_demo/display/i2c_oled.h"

#if (__PROJECT_8267_BLE_REMOTE__ || __PROJECT_8261_BLE_REMOTE__ ||__PROJECT_8266_BLE_XW_DEMO__)


#define  USER_TEST_APP_TIMER_EARLY_WAKEUP		0  //test app timer wakeup
#define  USER_TEST_BLT_SOFT_TIMER				1 //test soft timer



MYFIFO_INIT(hci_tx_fifo, 72, 4);
MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);


////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////
//		handle 0x0e: consumper report
#define			HID_HANDLE_CONSUME_REPORT			21
#define			HID_HANDLE_KEYBOARD_REPORT			25
#define			AUDIO_HANDLE_MIC					43


//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] =  {0x11, 0x11, 0x11, 0x88, 0x88, 0x88};	// little endian
u8  tbl_mac_auth[] = {0x88, 0x88, 0x88, 0x11, 0x11, 0x11};// big endian

u8	tbl_advData[] = {
		 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
		 0x03, 0x02, 0xe7, 0xfe,		// incomplete list of service class UUIDs (0xfee7)
		 0x09, 0x09, 'Y', 'D', 'Y', '_', 'P', '1', '0', '2',		// name
		 0x09, 0xff, 0x11, 0x02, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1, 0xef,
};
u8	tbl_scanRsp [] = {
		 0x09, 0x09, 'Y', 'D', 'Y', '_', 'P', '1', '0', '2',
	};

/////////////////////////// led management /////////////////////
enum{
	LED_POWER_ON = 0,
	LED_AUDIO_ON,	//1
	LED_AUDIO_OFF,	//2
	LED_SHINE_SLOW, //3
	LED_SHINE_FAST, //4
	LED_SHINE_OTA, //5
};

const led_cfg_t led_cfg[] = {
	    {1000,    0,      1,      0x00,	 },    //power-on, 1s on
	    {100,	  0 ,	  0xff,	  0x02,  },    //audio on, long on
	    {0,	      100 ,   0xff,	  0x02,  },    //audio off, long off
	    {500,	  500 ,   3,	  0x04,	 },    //1Hz for 3 seconds
	    {250,	  250 ,   6,	  0x04,  },    //2Hz for 3 seconds
	    {125,	  125 ,   200,	  0x08,  },    //4Hz for 50 seconds
};


u32	advertise_begin_tick;


u8	ui_mic_enable = 0;

extern kb_data_t	kb_event;


u8 key_buf[8] = {0};
u8 key_type;
#define CONSUMER_KEY   0
#define KEYBOARD_KEY   1

#define LED_OFF			0x00
#define LED_TIME     	0x01
#define LED_HEART_RATE	0x02
#define LED_PEDOMETER	0x03

u8  bTouchIcVal 		= 0;
u8  bTouchIcValPre		= 0;

u8 bLed_State = 0;
u8 bVal = 1;
u32 display_pin = 0;

u32	ab_digital[4] = {0};

u32 tmp_data1;
u8 tmp_data2;

u8 sendTerminate_before_enterDeep = 0;

int key_not_released;
u32 latest_user_event_tick;
u8  user_task_flg;

static u8 key_voice_press = 0;
static u8 ota_is_working = 0;
static u32 key_voice_pressTick = 0;

#if (STUCK_KEY_PROCESS_ENABLE)
u32 stuckKey_keyPressTime;
#endif




#if (BLE_AUDIO_ENABLE)





int lowBattDet_enable = 0;
void		ui_enable_mic (u8 en)
{
	ui_mic_enable = en;

	gpio_set_output_en (GPIO_PC3, en);		//AMIC Bias output
	gpio_write (GPIO_PC3, en);

	device_led_setup(led_cfg[en ? LED_AUDIO_ON : LED_AUDIO_OFF]);


	if(en){  //audio on
		lowBattDet_enable = 1;
		battery2audio();////switch auto mode
	}
	else{  //audio off
		audio2battery();////switch manual mode
		lowBattDet_enable = 0;
	}
}


void	task_audio (void)
{
	static u32 audioProcTick = 0;
	if(clock_time_exceed(audioProcTick, 5000)){
		audioProcTick = clock_time();
	}
	else{
		return;
	}

	///////////////////////////////////////////////////////////////
	log_event(TR_T_audioTask);
	proc_mic_encoder ();		//about 1.2 ms @16Mhz clock

	//////////////////////////////////////////////////////////////////
	if (blc_ll_getTxFifoNumber() < 8)
	{
		int *p = mic_encoder_data_buffer ();
		if (p)					//around 3.2 ms @16MHz clock
		{
			log_event (TR_T_audioData);
			bls_att_pushNotifyData (AUDIO_HANDLE_MIC, (u8*)p, ADPCM_PACKET_LEN);
		}
	}
}
#endif


#if (USER_TEST_APP_TIMER_EARLY_WAKEUP)
void gpio_test_app_timer_wakeup(u8 e, u8 *p)
{
	static u32 Ctick = 0;

	if(clock_time_exceed(Ctick, 23000)){
		Ctick = clock_time();

		// GPIO toggle

		bls_pm_setUserTimerWakeup(23000 ,1);
	}
}


#endif


#if (USER_TEST_BLT_SOFT_TIMER)

int gpio_test1(void)
{
	//gpio 1 toggle to see the effect

	//20 ms & 40 ms repeat
	static u8 flg = 0;
	flg = !flg;
	if(flg){
		return 20000; //20ms
	}
	else{
		return 40000; // 40ms
	}
	//return 0;
}
#if 0
int gpio_test2(void)
{
	//gpio 2 toggle to see the effect

	//timer last for 5 second
	if(clock_time_exceed(0, 5000000)){
		return -1;
	}
	else{
		return 0;
	}
}
#else



void data2array(u32 data, u8 *array, u8 len)
{
	for(u8 i = 0; i < len; i++)
	{
		array[len-1-i] = data % 10;
		data /= 10;
	}
	return;
}

void init_display_button(u32 pin)
{
	gpio_set_func (pin, AS_GPIO);		//gpio
	gpio_set_output_en (pin, 0);		//output enable
	gpio_set_input_en (pin, 1);		//output disable
	gpio_setup_up_down_resistor(pin,PM_PIN_PULLUP_10K);

	bTouchIcValPre 	= gpio_read(pin);
	bTouchIcVal	= bTouchIcValPre;

	bLed_State  = LED_OFF;				//Initial LED TO BE OFF.

	display_pin = pin;
}

_attribute_ram_code_ void display_button_proc(void)
{
	bTouchIcVal = gpio_read(display_pin);
	if( bTouchIcVal != bTouchIcValPre )// TW301有信号变动，开始处理。这里演示功能每3次，会清一次屏幕(类似关闭屏幕效果)。
	{
		bLed_State ++;			//增加LED State处理。
		bLed_State %= 4; 		//增加LED State处理。

		bTouchIcValPre = bTouchIcVal;
	}
	return;
}

#endif

#if 0
void init_dbg_pin()
{
	gpio_set_func(GPIO_PD1, AS_GPIO);
	gpio_set_input_en(GPIO_PD1, 0);
	gpio_set_output_en(GPIO_PD1, 1);
}

int gpio_test3(void)
{
	//gpio 3 toggle to see the effect
	static u8 bLed = 0;
	init_dbg_pin();
	gpio_write(GPIO_PD1, bLed);
	bLed++;
	bLed%=2;
	return 0;
}
#else

void init_dbg_pin()
{
	gpio_set_func(GPIO_PD1, AS_GPIO);
	gpio_set_input_en(GPIO_PD1, 0);
	gpio_set_output_en(GPIO_PD1, 1);
}


u32 step_count;
u32 step_count_pre;


int gpio_test3(void)
{
	static u8 bLed = 0;
	init_dbg_pin();
	gpio_write(GPIO_PD1, bLed);
	bLed++;
	bLed%=2;
	//gpio 3 toggle to see the effect
	drv_KX022_ReadFIFO();
	step_count_pre = step_count;
	step_count = accGetSteps();
	return 0;
}

int pedo_meter_proc(void)
{
	drv_KX022_ReadFIFO();
	step_count_pre = step_count;
	step_count = accGetSteps();
	return 0;
}
#endif


#endif

void early_wakeup_output_signal()
{
	static u8 bLed = 0;
	gpio_write(GPIO_PD1, bLed);
	bLed++;
	bLed%=2;
}


extern u8 dfu_reboot;
void 	ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{
	if(*p == HCI_ERR_CONN_TIMEOUT){

	}
	else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}
	else{

	}

	 //user has push terminate pkt to ble TX buffer before deepsleep
	if(sendTerminate_before_enterDeep == 1){
		sendTerminate_before_enterDeep = 2;
	}

	ancsInit();

	advertise_begin_tick = clock_time();
}

extern u32 tick_test;
void	task_connect (u8 e, u8 *p, int n)
{
//	bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);   //10ms *(99+1) = 1000 ms
//	bls_l2cap_requestConnParamUpdate (304, 320 , 0, 600);
#if(TLK_WEIXIN_ENABLE)
	task_weixin(1);				//initial weixin func when connected.
#endif
	tick_test = clock_time();
}


void	update_done (u8 e, u8 *p, int n)
{



}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


//This function process ...
void deep_wakeup_proc(void)
{
#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
	//if deepsleep wakeup is wakeup by GPIO(key press), we must quickly scan this
	//press, hold this data to the cache, when connection established OK, send to master
	//deepsleep_wakeup_fast_keyscan
	if(analog_read(DEEP_ANA_REG0) == CONN_DEEP_FLG){
		if(kb_scan_key (KB_NUMLOCK_STATUS_POWERON,1) && kb_event.cnt){
			deepback_key_state = DEEPBACK_KEY_CACHE;
			key_not_released = 1;
			memcpy(&kb_event_cache,&kb_event,sizeof(kb_event));
		}
	}
#endif
}




void deepback_pre_proc(int *det_key)
{
#if 0
	// to handle deepback key cache
	extern u32 blt_conn_start_tick; //ble connect establish time
	if(!(*det_key) && deepback_key_state == DEEPBACK_KEY_CACHE && blt_state == BLT_LINK_STATE_CONN \
			&& clock_time_exceed(blt_conn_start_tick,25000)){

		memcpy(&kb_event,&kb_event_cache,sizeof(kb_event));
		*det_key = 1;

		if(key_not_released || kb_event_cache.keycode[0] == VK_M){  //no need manual release
			deepback_key_state = DEEPBACK_KEY_IDLE;
		}
		else{  //need manual release
			deepback_key_tick = clock_time();
			deepback_key_state = DEEPBACK_KEY_WAIT_RELEASE;
		}
	}
#endif
}

void deepback_post_proc(void)
{
	//manual key release
	if(deepback_key_state == DEEPBACK_KEY_WAIT_RELEASE && clock_time_exceed(deepback_key_tick,150000)){
		key_not_released = 0;

		key_buf[2] = 0;
		bls_att_pushNotifyData (HID_HANDLE_KEYBOARD_REPORT, key_buf, 8); //release
		deepback_key_state = DEEPBACK_KEY_IDLE;
	}
}



void key_change_proc(void)
{
#if 0
	latest_user_event_tick = clock_time();  //record latest key change time

	if(key_voice_press){  //clear voice key press flg
		key_voice_press = 0;
	}

	u8 key = kb_event.keycode[0];

	if ( (key & 0xf0) == 0xf0)			//key in consumer report
	{
		key_not_released = 1;
		key_type = CONSUMER_KEY;

		u16 media_key;
		if(key == CR_VOL_UP){
			media_key= 0x0001;  //vol+
		}
		else if(key == CR_VOL_DN){
			media_key = 0x0002; //vol-
		}
		else{
			media_key = 1 << (key & 0x0f);
		}

		bls_att_pushNotifyData (HID_HANDLE_CONSUME_REPORT, (u8 *)&media_key, 2);
	}
	else if (key)			// key in standard reprot
	{
		key_not_released = 1;
#if (BLE_AUDIO_ENABLE)
		if (key == VK_M)
		{
			if(ui_mic_enable){
				//adc_clk_powerdown();
				ui_enable_mic (0);
			}
			else{ //if voice not on, mark voice key press tick
				key_voice_press = 1;
				key_voice_pressTick = clock_time();
			}
		}
		else
#endif
		{
			key_type = KEYBOARD_KEY;
			key_buf[2] = key;
			bls_att_pushNotifyData (HID_HANDLE_KEYBOARD_REPORT, key_buf, 8);
		}
	}
	else {
		key_not_released = 0;
		if(key_type == CONSUMER_KEY){
			u16 media_key = 0;
			bls_att_pushNotifyData (HID_HANDLE_CONSUME_REPORT, (u8 *)&media_key, 2);  //release
		}
		else{
			key_buf[2] = 0;
			bls_att_pushNotifyData (HID_HANDLE_KEYBOARD_REPORT, key_buf, 8); //release
		}
	}
#endif
}



void proc_keyboard (u8 e, u8 *p, int n)
{

	static u32 keyScanTick = 0;
	if(e == BLT_EV_FLAG_GPIO_EARLY_WAKEUP || clock_time_exceed(keyScanTick, 10000)){
		keyScanTick = clock_time();
	}
	else{
		return;
	}


	kb_event.keycode[0] = 0;
	int det_key = kb_scan_key (0, 1);


	if (det_key){
		key_change_proc();
	}
	


#if (BLE_AUDIO_ENABLE)
	 //long press voice 1 second
	if(key_voice_press && !ui_mic_enable && clock_time_exceed(key_voice_pressTick,1000000)){
		key_voice_press = 0;
		ui_enable_mic (1);
	}
#endif
}



u16 ui_manual_latency_when_key_press(void)
{
	u16 cur_interval = bls_ll_getConnectionInterval();

	if(cur_interval > 40){  //50 ms = 40*1.25ms
		return 0;
	}
	else{
		return   (40/cur_interval) - 1;        //50ms
	}
}



extern u32	scan_pin_need;
void blt_pm_proc(void)
{
	return;
#if(BLE_REMOTE_PM_ENABLE)
#if 0
	if(ota_is_working || ui_mic_enable){
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
	}
	else{

		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);

		user_task_flg = scan_pin_need || key_not_released || DEVICE_LED_BUSY;

		if(user_task_flg){
#if (LONG_PRESS_KEY_POWER_OPTIMIZE)
			extern int key_matrix_same_as_last_cnt;
			if(key_matrix_same_as_last_cnt > 5){  //key matrix stable can optize
				bls_pm_setManualLatency( ui_manual_latency_when_key_press() );
			}
			else{
				bls_pm_setManualLatency(0);  //latency off: 0
			}
#else
			bls_pm_setManualLatency(0);
#endif
		}


#if 1 //deepsleep
		if(sendTerminate_before_enterDeep == 1){ //sending Terminate and wait for ack before enter deepsleep
			if(user_task_flg){  //detect key Press again,  can not enter deep now
				sendTerminate_before_enterDeep = 0;
			}
		}
		else if(sendTerminate_before_enterDeep == 2){  //Terminate OK
			bls_pm_setSuspendMask (DEEPSLEEP_ADV); //when terminate, link layer change back to adc state
			bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio PAD wakeup deesleep
			analog_write(DEEP_ANA_REG0, CONN_DEEP_FLG);
		}

		//adv 60s, deepsleep
		if( bls_ll_getCurrentState() == BLS_LINK_STATE_ADV && \
			clock_time_exceed(advertise_begin_tick , 60 * 1000000)){
			bls_pm_setSuspendMask (DEEPSLEEP_ADV); //set deepsleep
			bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio PAD wakeup deesleep
			analog_write(DEEP_ANA_REG0, ADV_DEEP_FLG);
		}
		//conn 60s no event(key/voice/led), enter deepsleep
		else if( bls_ll_getCurrentState() == BLS_LINK_STATE_CONN && !user_task_flg && \
				clock_time_exceed(latest_user_event_tick, 60 * 1000000) ){

			bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //push terminate cmd into ble TX buffer
			sendTerminate_before_enterDeep = 1;
		}
#endif
	}
#endif
#endif
}


_attribute_ram_code_ void  ble_remote_set_sleep_wakeup (u8 e, u8 *p, int n)
{
//	if( bls_ll_getCurrentState() == BLS_LINK_STATE_CONN && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 * CLOCK_SYS_CLOCK_1MS){  //suspend time > 30ms.add gpio wakeup
//		bls_pm_setWakeupSource(PM_WAKEUP_CORE);  //gpio CORE wakeup suspend
//	}
	gpio_write(GPIO_PD3, 1);
	if( gpio_read(GPIO_PC4) ) //GPIO_PC4 high.
	{
		gpio_write(GPIO_PC6, 1);
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);
	}
	gpio_write(GPIO_PD3, 0);
}

_attribute_ram_code_ void ble_suspend_exit(u8 e, u8 *p, int n)
{
	gpio_write(GPIO_PC6, 0);
}


void rf_customized_param_load(void)
{
	  //flash 0x77000 customize freq_offset adjust cap value, if not customized, default ana_81 is 0xd0
	 if( (*(unsigned char*) CUST_CAP_INFO_ADDR) != 0xff ){
		 //ana_81<4:0> is cap value(0x00 - 0x1f)
		 analog_write(0x81, (analog_read(0x81)&0xe0) | ((*(unsigned char*) CUST_CAP_INFO_ADDR)&0x1f) );
	 }

	 //flash 0x77040 customize TP0, flash 0x77041 customize TP1
	 if( ((*(unsigned char*) (CUST_TP_INFO_ADDR)) != 0xff) && ((*(unsigned char*) (CUST_TP_INFO_ADDR+1)) != 0xff) ){
		 rf_update_tp_value(*(unsigned char*) (CUST_TP_INFO_ADDR), *(unsigned char*) (CUST_TP_INFO_ADDR+1));
	 }

	 // customize 32k RC cap value, if not customized, default ana_32 is 0x80
	 if( (*(unsigned char*) CUST_RC32K_CAP_INFO_ADDR) != 0xff ){
		 //ana_81<4:0> is cap value(0x00 - 0x1f)
		 analog_write(0x32, *(unsigned char*) CUST_RC32K_CAP_INFO_ADDR );
	 }
}

//////////////////////////////////////weixin /////////////////////////
//addr need re-located

#define				CFG_ADR_AES_KEY			(CFG_ADR_MAC + 16)
#define				CFG_ADR_DEVICE_ID		(CFG_ADR_MAC + 32)
#define				CFG_ADR_DEVICE_TYPE		(CFG_ADR_MAC + 64)

#define				WX_ENABLE_AES			0

u8	wx_aes_key[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07, 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};

const	u8	ff_32_byte[32] = {
		0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

void init_weixin_button()
{
	gpio_set_func(GPIO_PD4, AS_GPIO);
	gpio_set_input_en(GPIO_PD4, 1);
	gpio_set_output_en(GPIO_PD4, 0);

	gpio_set_func(GPIO_PD5, AS_GPIO);
	gpio_set_input_en(GPIO_PD5, 1);
	gpio_set_output_en(GPIO_PD5, 0);

	gpio_set_func(GPIO_PC5, AS_GPIO);
	gpio_set_input_en(GPIO_PC5, 1);
	gpio_set_output_en(GPIO_PC5, 0);

	gpio_set_func(GPIO_PC6, AS_GPIO);
	gpio_set_input_en(GPIO_PC6, 1);
	gpio_set_output_en(GPIO_PC6, 0);

	gpio_write (GPIO_PD4, 1);			//pull up
	gpio_write (GPIO_PD5, 1);			//pull up

	gpio_write (GPIO_PC5, 1);			//pull up
	gpio_write (GPIO_PC6, 1);			//pull up

}

void app_attClientHandle (u8 conn_handle, u8* data)
{
	u32 time_stamp = *(u32*)data;
	rf_packet_l2cap_cust_t *p = (rf_packet_l2cap_cust_t *)data;

	if(	ancsFuncIsEn())
		ancsStackCallback(p);
	return;
}

void proc_ancs_en(u8 ancsEvId, u8 ancsCateId){
	if(CategroyIDIncomingCall == ancsCateId ){
		if(EventIDNotificationRemoved == ancsEvId){
			///gpio_write(LED_RED, 1);   // led off
		}else{  // modified  or added
			///gpio_write(LED_RED, 0);   // led on
		}
	}else if(CategroyIDSocial == ancsCateId){  // news
		if(EventIDNotificationRemoved == ancsEvId){
			///gpio_write(LED_BLUE, 1);   // led off
		}else{  // modified  or added
			///gpio_write(LED_BLUE, 0);   // led on
		}
	}
}

void	task_bond_done (u8 e, u8 *p, int n)
{
	ancsFuncEn();
}

u32 tick_test ;

void user_init()
{
	rf_customized_param_load();  //load customized freq_offset cap value and tp value

	//for USB debug
	//usb_log_init ();
	//usb_dp_pullup_en (1);  //open USB enum


	////////////////// BLE stack initialization ////////////////////////////////////
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff){
		memcpy (tbl_mac, pmac, 6);
	}
	else{

	}


	wx_set_adv_mac (tbl_advData, tbl_mac, sizeof(tbl_advData)-1);
///////////// BLE stack Initialization ////////////////
	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory
	//blc_ll_initScanning_module(tbl_mac);		//scan module: 		 optional
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
	blc_ll_initPowerManagement_module();        //pm module:      	 optional

//	bls_ll_init (tbl_mac);  	//link layer initialization
//	extern void my_att_init ();
//	my_att_init (); //gatt initialization

	if (memcmp (ff_32_byte, CFG_ADR_AES_KEY, 16) != 0)
	{
		memcpy (wx_aes_key, CFG_ADR_AES_KEY, 16);
	}

	for(int i=0; i<sizeof(tbl_mac); i++)	//reverse_mac.
	{
		tbl_mac_auth[i] = tbl_mac[sizeof(tbl_mac)-1-i];
	}
	extern void	my_att_init (u8 *p_mac, u8 *p_device_type, u8 *p_device_id, u8 *p_key);
	my_att_init (tbl_mac_auth,
			(u8 *) (memcmp (ff_32_byte, CFG_ADR_DEVICE_TYPE, 32) != 0 ? CFG_ADR_DEVICE_ID : "WeChatDev"),
			(u8 *) (memcmp (ff_32_byte, CFG_ADR_DEVICE_ID, 32) != 0 ? CFG_ADR_DEVICE_ID : "WeChatBluetoothDevice"),
			WX_ENABLE_AES ? wx_aes_key : 0
			);
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER ); 	//smp initialization to be disabled.
//	bls_smp_enableParing (SMP_PARING_DISABLE_TRRIGER );



///////////////////// USER application initialization ///////////////////
	bls_ll_setAdvData( tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData(tbl_scanRsp, sizeof(tbl_scanRsp));

	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_1_28_S, ADV_INTERVAL_1_28_S, \
									 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_37, ADV_FP_NONE);
	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}
	bls_ll_setAdvEnable(1);  //adv enable
	rf_set_power_level_index (RF_POWER_8dBm);

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);

#if BLE_REMOTE_SECURITY_ENABLE
	bls_app_registerEventCallback (BLT_EV_FLAG_ENCRYPTION_CONN_DONE, &task_bond_done);
#endif


	///////////////////// keyboard matrix initialization///////////////////
#if 0
	u32 pin[] = KB_DRIVE_PINS;
	for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
	{
		gpio_set_wakeup(pin[i],1,1);  	   //drive pin core(gpio) high wakeup suspend
		cpu_set_gpio_wakeup (pin[i],1,1);  //drive pin pad high wakeup deepsleep
	}
#endif
#if(KEYSCAN_IRQ_TRIGGER_MODE)
	reg_irq_src = FLD_IRQ_GPIO_EN;   //clear gpio interupt ststus
#endif

//	bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_keyboard);
	bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &early_wakeup_output_signal);


	///////////////////// Adc Battery Check Init ///////////////////////////
//	adc_BatteryCheckInit(2);///add by Q.W


	///////////////////// Power Management initialization ///////////////////
#if(BLE_REMOTE_PM_ENABLE)
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &ble_suspend_exit);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif


	////////////////LED initialization /////////////////////////
//	device_led_init(GPIO_LED, 1);


	//////////////// TEST  ////////////////////////////////////
#if (USER_TEST_APP_TIMER_EARLY_WAKEUP)
	bls_app_registerEventCallback (BLT_EV_FLAG_USER_TIMER_WAKEUP, &gpio_test_app_timer_wakeup);
#endif

	advertise_begin_tick = clock_time();

#if(TLK_WEIXIN_ENABLE)
	init_weixin_button();
#endif


#if(TLK_ANCS_ENABLE)
	ancsInit();
	blc_l2cap_reg_att_cli_hander (app_attClientHandle );
	tick_test = clock_time();
#endif


	//pedometer  i2c interface.
#if(TLK_PEDOMETER_ENABLE)
	i2c_init();
	drv_KX022_Init();
	HalPedInit();
#endif

#if(TLK_LED_ENABLE)
	////////////////// oled_porter_11.4 init begin////////////////////////
	i2c_init(); //打开硬件I2C。

	//使能屏幕控制引脚GPIO_PA5.
	gpio_set_func (GPIO_PA5, AS_GPIO);		//gpio
	gpio_set_output_en (GPIO_PA5, 1);		//output enable
	gpio_set_input_en (GPIO_PA5, 0);		//intput disable
	gpio_write (GPIO_PA5, 0);

	init_display_button(GPIO_PC1);
	//初始化屏幕,目前暂时别在main_loop()里面调用该函数。
	OLED_Init();
	extern u32 tick_expected;
	tick_expected = clock_time();
	////////////////// oled_porter_11.4 init end ////////////////////////
#endif

#if (USER_TEST_BLT_SOFT_TIMER)
	blt_soft_timer_init();
//	blt_soft_timer_add(&gpio_test1, 7000);
	#if(TLK_LED_ENABLE)
	blt_soft_timer_add(&display_button_proc, 100*1000);
	#endif
	#if(TLK_PEDOMETER_ENABLE)
	blt_soft_timer_add(&pedo_meter_proc, 650*1000);		//700ms,700*1000
	#endif
#endif


#if (TLK_HEARTRATE_ENABLE)


#endif

	//mark adv begin time

}


////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////
#define		SEND_DAT_HTML		"send to html"
#define		SEND_DAT_SERVER		"send to server"

extern int	wx_push_data_request (u8 *pd, int len, int html);

void proc_ui_weixin ()
{
	static u32 wx_dbg_sd;

	static u16 key_vol_up, key_vol_down;
	u32 kk = gpio_read (GPIO_PD4) && gpio_read (GPIO_PC5);
	if (key_vol_up && !kk)
	{
		wx_push_data_request ((u8*)SEND_DAT_HTML, sizeof (SEND_DAT_HTML), 1);
		wx_dbg_sd++;
	}
	key_vol_up = kk;

	kk = gpio_read (GPIO_PD5) && gpio_read (GPIO_PC6);
	if (key_vol_down && !kk)
	{
		wx_push_data_request ((u8*)SEND_DAT_SERVER, sizeof (SEND_DAT_SERVER), 0);
		wx_dbg_sd++;
	}
	key_vol_down = kk;
}

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////
u8 second = 0;
u8 minute  = 0;
u8 hour  = 0;

u32  tick_expected = 0;

void time_read_display()
{
	 if( clock_time_exceed(tick_expected,1*1000*1000) )
	 {
		 tick_expected += CLOCK_SYS_CLOCK_1S;  // 把实际1s（精确）的tick加上起始tick赋值给tick_expected，保证时间差永远在1s以内。
		 second++;
		 if( second >= 60 ) //60s
		 {
			 minute++;
			 second = 0;
			 if( minute >= 60 )//60m
			 {
				 hour++;
				 minute = 0;
				 if( hour >= 24 )//one day
				 {
					 hour = 0;
				 }
			 }

		 }
	 }
}
////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////
void led_display()
{

	u8 ab_digit[3] = {0};
	switch(bLed_State)
	{
	case LED_OFF:		//清除屏幕

					OLED_P16x16Ch(30,0,1);

					OLED_P12x24Ch(50,1,11);
					OLED_P12x24Ch(62,1,11);
					OLED_P12x24Ch(74,1,11);

					OLED_P12x24Ch(38,1,11);
					OLED_P12x24Ch(86,1,11);

					break;

	case	LED_TIME:		//显示时间

					//在这里计算
					data2array(hour, ab_digit, 2);

					OLED_P16x16Ch(30,0,1);

					OLED_P12x24Ch(38,1,ab_digit[0]);//1
					OLED_P12x24Ch(50,1,ab_digit[1]);//2

					OLED_P12x24Ch(62,1,10);//:

					data2array(minute, ab_digit, 2);

					OLED_P12x24Ch(74,1,ab_digit[0]);//2
					OLED_P12x24Ch(86,1,ab_digit[1]);//0


				break;


	case	LED_HEART_RATE:		// 显示心率，有心形 和数字
					//在这里计算
				data2array(tmp_data2, ab_digit, 3);
					//在这中间计算数字
				OLED_P12x24Ch(38,1,11);
				OLED_P12x24Ch(86,1,11);//清除

				OLED_P16x16Ch(30,0,0);

				OLED_P12x24Ch(50,1,ab_digit[0]);
				OLED_P12x24Ch(62,1,ab_digit[1]);
				OLED_P12x24Ch(74,1,ab_digit[2]);
		break;

	case	LED_PEDOMETER:		//显示计步，没有心形，只有数字
					//在这里计算
				data2array(tmp_data1, ab_digit, 3);
					//在这中间计算数字
				OLED_P12x24Ch(38,1,11);
				OLED_P12x24Ch(86,1,11);//清除

				OLED_P16x16Ch(30,0,1);
				OLED_P12x24Ch(50,1,ab_digit[0]);
				OLED_P12x24Ch(62,1,ab_digit[1]);
				OLED_P12x24Ch(74,1,ab_digit[2]);

				break;

	default:
			break;
	}
	return;
}
/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
u32 tick_loop;
unsigned short battValue[20];

void main_loop ()
{
	tick_loop ++;

	////////////////////////////////////// BLE entry /////////////////////////////////
	#if (BLT_SOFTWARE_TIMER_ENABLE)
		blt_soft_timer_process(MAINLOOP_ENTRY);
	#endif

	////////////////////////////////////// BLE entry ////////////////////////////
	//blt_slave_main_loop ();
	blt_sdk_main_loop();

	////////////////////////////////////// UI entry /////////////////////////////////
	/////////////weixin func is processed down here////////////////
#if(TLK_WEIXIN_ENABLE)
	task_weixin(0);

	proc_ui_weixin ();
#endif


	/////////////ancs func is processed down here////////////////
#if(TLK_ANCS_ENABLE)
	if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN ){
		u8 news_id = ancsHaveNews();
		if((news_id != 0 && news_id != -1) && (news_id != -2)){  // valid news
			ancs_notifySrc_type_t* news_ptr = ancs_get_news();
			proc_ancs_en(news_ptr->EventID, news_ptr->CategroyID);

		}

		if(clock_time_exceed(tick_test, 1 * 1000 * 1000)){
			ancs_getCurNotifyAttsData();///获取notifyTbl里最后一个notify的notifyAtts
			ancs_findCurNotifyIdentifier();///用于测试不同app发过来的notify的demo函数
			tick_test = clock_time();
		}
	}
#endif

	/////////////Led Display is processed down here////////////////
#if(TLK_LED_ENABLE)
	time_read_display();
	led_display();
#endif

	blt_pm_proc();  //power management
}







#endif  //end of  (__PROJECT_8267_BLE_REMOTE__ || __PROJECT_8261_BLE_REMOTE__)

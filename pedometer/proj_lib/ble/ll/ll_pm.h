/*
 * ll_pm.h
 *
 *  Created on: 2017-3-7
 *      Author: Administrator
 */

#ifndef LL_PM_H_
#define LL_PM_H_



////////////////// Power Management ///////////////////////
#define			SUSPEND_DISABLE			0
#define			SUSPEND_ADV				BIT(0)
#define			SUSPEND_CONN			BIT(1)
#define			DEEPSLEEP_ADV			BIT(2)
#define			DEEPSLEEP_CONN			BIT(3)
#define			MCU_STALL				BIT(6)



#define 		LOWPOWER_ADV			( SUSPEND_ADV  | DEEPSLEEP_ADV  )
#define 		LOWPOWER_CONN			( SUSPEND_CONN | DEEPSLEEP_CONN )
#define			DEEPSLEEP_ENTER			( DEEPSLEEP_ADV | DEEPSLEEP_CONN )



typedef struct {
	u8		suspend_mask;
	u8		wakeup_src;
	u8		advMcuStall_en;
	u8		conn_no_suspend;

	u32 	appWakeup_tick;

	u8		appWakeup_en;
	u8		appWakeup_flg;
	u8		latency_off;
	u8		no_latency;

	u16		sys_latency;
	u16 	user_latency;
	u16		valid_latency;
	u16 	latency_use;

}st_ll_pm_t;



typedef 	void (*ll_module_pm_callback_t)(void);


typedef 	void (*pm_appWakeupLowPower_callback_t)(int);




/******************************* User Interface  ************************************/
void 		blc_ll_initPowerManagement_module(void);


void		bls_pm_setSuspendMask (u8 mask);
u8 			bls_pm_getSuspendMask (void);
void 		bls_pm_setWakeupSource(u8 source);
u32 		bls_pm_getSystemWakeupTick(void);

void 		bls_pm_setManualLatency(u16 latency); //manual set latency to save power
void 		bls_pm_enableAdvMcuStall(u8 en);

void 		bls_pm_setAppWakeupLowPower(u32 wakeup_tick, u8 enable);
void 		bls_pm_registerAppWakeupLowPowerCb(pm_appWakeupLowPower_callback_t cb);

/************************* Stack Interface, user can not use!!! ***************************/




#endif /* LL_PM_H_ */

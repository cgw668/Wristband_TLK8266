
#include "../../proj/tl_common.h"

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"

#if (__PROJECT_8267_BLE_REMOTE__ || __PROJECT_8261_BLE_REMOTE__ || __PROJECT_8266_BLE_XW_DEMO__)

extern void user_init();
extern void deep_wakeup_proc(void);

_attribute_ram_code_ void irq_handler(void)
{

	irq_blt_sdk_handler ();

}
#if 0
u32 ab_value  = 0xFFFFFFFF;
u32 ab_value1 = 0xFFFFFFFF;
u32 ab_value2 = 0xFFFFFFFF;

u8  ab_data[4] = {0x11, 0x22, 0x33, 0x44};
#endif 
int main (void) {

	cpu_wakeup_init();

	clock_init();
	set_tick_per_us(CLOCK_SYS_CLOCK_HZ/1000000);

	gpio_init();



//	gpio_set_func(GPIO_PE7, AS_I2C);

	deep_wakeup_proc();

	rf_drv_init(CRYSTAL_TYPE);

	user_init ();
#if 0
	REG_ADDR8(0x5CA) = 0xCA;
	REG_ADDR8(0x5CB) = 0xCB;
	REG_ADDR8(0x5CC) = 0xCC;
	ab_value = REG_ADDR8(0x5CB);
	ab_value1 = REG_ADDR32(0x5CB);
	ab_value2 = REG_ADDR16(0x5CB);

	memcpy(&ab_value2, ab_data, 4);

	while(1);
#endif
    irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}



#endif

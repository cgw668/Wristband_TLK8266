/*
 * adc_8263.c
 *
 *  Created on: 2015-12-10
 *      Author: Telink
 */

#include "../tl_common.h"

#if(__TL_LIB_8263__ || (MCU_CORE_TYPE == MCU_CORE_8263))

#if (MODULE_ADC_ENABLE)


void adc_init ( ) {

	 reg_adc_chn_input = ADC_CHN_GP23;//ADC_CHNM_ANA_INPUT;// adc chn
	 reg_adc_ref = ADC_REF_VDDH;//ADC_REF_1P3V;//ADC_REF_VDDH;//ADC_CHNM_REF_SRC;  //ref

	 reg_adc_sample_clk_res = MASK_VAL(FLD_ADC_SAMPLE_CLOCK,ADC_CLOCK_CYCLE_3,
	   FLD_ADC_RESOLUTION,ADC_RES_10_BITS,
	   FLD_ADC_DATA_SIGN,ADC_DATA_SIGN_POSITIVE
	   );

	 //set clk  enable
	 reg_adc_step_l = 4;
	 reg_adc_mod = MASK_VAL(FLD_ADC_MOD, 192, FLD_ADC_CLK_EN, 1);

}

u16 adc_get () {
	 // Set a run signal
	 reg_adc_outp = FLD_ADC_OUTPUT_CHN_MANU_START;

	 // wait for data
	 sleep_us(5);

	 // read data
	 return (reg_adc_dat_byp_outp & 0x3FFF);
}

#endif

#endif

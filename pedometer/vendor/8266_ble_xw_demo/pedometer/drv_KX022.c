//#include "../../proj/drivers/spi.h"
#include "../../../proj/tl_common.h"

#include "drv_KX022.h"
#include "hal_ped.h"

u8 acc_id=0;

void drv_KX022_Init(void)
{
	u8  val,i;

	//soft reset kx022
	val = 0x80;
	i2c_burst_write(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_CTRL_REG2, &val ,1);
	delay(50000); //wait 50ms

	//Read out WHO_AM_I
	i2c_burst_read_midstart(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_WHO_AM_I,&val,1);
	acc_id = val;

	if(val!=0x14)	return;

	 //Change CTNL1
	 val = 0x14;   //disable sensor
	 i2c_burst_write(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_CTRL_REG1, &val ,1);

	 //Configure ODR, default 0x02:50Hz, 0x01:25Hz
	 val = 0x02;
	 i2c_burst_write(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_ODR_CNTL, &val ,1);

	 //Low power control
	 val = 0x0B;                         //9B:64 samples average, 2B:4samples
	 i2c_burst_write(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_LP_CNTL, &val ,1);

	 //TAP/DTAP control
	 val = 0x80;                         //TAP ODR 200Hz
	 i2c_burst_write(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_CTRL_REG3, &val ,1);

	 //Enable WMI, TAP and wakeup interrupt to INT1
	 val = 0x38;   //Active low, pulse
	 i2c_burst_write(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_INT_CTRL_REG1, &val ,1);

	 val = 0x20;   //0x10 for DRDY, 0x02 for Wake up, 0x20 for WMI, 0x04 for TAP
	 i2c_burst_write(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_INT_CTRL_REG1, &val ,1);

	 //Configure FIFO
	 val = 25;				//WMI counts
	 i2c_burst_write(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_BUF_CTRL1, &val ,1);

	 i2c_burst_write(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_BUF_CLEAR, &val ,1);	//Clear FIFO

	 val = 0xE0;
	 i2c_burst_write(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_BUF_CTRL2, &val ,1);	//Enable FIFO

	 //Turn on Sensor
	 val = ACC_REG_CTRL_PC | ACC_REG_CTRL_GSEL_TDTE;   //enable sensor
	 i2c_burst_write(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_CTRL_REG1, &val ,1);

	 //Clear the interrupt
	 i2c_burst_read_midstart(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_INT_REL, &val ,1);
}

void drv_KX022_ReadData(s16 *buf)
{
  u8 temp[6];

  i2c_burst_read(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_XOUT_L, temp ,6);
  buf[0] = ((s16)temp[1]<<8) | ((s16)temp[0]);
  buf[1] = ((s16)temp[3]<<8) | ((s16)temp[2]);
  buf[2] = ((s16)temp[5]<<8) | ((s16)temp[4]);;
}

u8  acc_data[41*6];
//u32 step_count = 0;
//float x_val, y_val, z_val;
u8 buffer_cnt = 0;
void drv_KX022_ReadFIFO(void)
{
  u8 val, i, acc_count;
  s16 new_x, new_y, new_z, temp;
//  float x_val, y_val, z_val;

  	  	i2c_burst_read_midstart(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_BUF_STATUS_REG1, &val ,1);
        buffer_cnt = val;

        if(val>0)
        {
          i2c_burst_read_midstart(KX022_I2C_SLAVE_ADDR, ACC_REG_ADDR_BUF_READ, acc_data ,val);

        }

#if 1
        HalaccUpdatePed(acc_data, val);
#else
      for(i=0; i<val; i++)
      {
        BrcmConsolePrintf("%4x",acc_data[i]);
        if(((i+1)%6) == 0)
        prinntf("\r\n");
      }
#endif

}

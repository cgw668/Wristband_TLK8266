#include"i2c_oled.h"
#include "codetab_oled.h"

//#include "../../proj/mcu_spec/Gpio_8266.h"
// ------------------  ASCII��ģ�����ݱ� ------------------------ //
// �����0x20~0x7e                                                //
// Asc12x24E.dat ����ȡģ���λ//
// -------------------------------------------------------------- //

void reset_ssd1306(void)
{
	//gpio_write(GPIO_PE4, 0);
	//sleep_us(100*1000);
	//gpio_write(GPIO_PE4, 1);

	gpio_write(GPIO_PA5, 0);
	sleep_us(100*1000);
	gpio_write(GPIO_PA5, 1);
}

static const unsigned char   nAsciiDot[] =              // ASCII
{
      /*--  ����:  0  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0x80,0xE0,0x70,0x10,0x10,0x10,0x70,0xE0,0x80,0x00,0x00,0x00,0x7F,0xFF,0x80,
      0x00,0x00,0x00,0x80,0xFF,0x7F,0x00,0x00,0x00,0x00,0x01,0x03,0x02,0x02,0x02,0x03,
      0x01,0x00,0x00,0x00,

      /*--  ����:  1  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0x00,0x00,0x20,0x20,0xF0,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x03,0x03,0x02,
      0x02,0x00,0x00,0x00,

      /*--  ����:  2  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0xE0,0xF0,0x10,0x10,0x10,0x10,0x30,0xE0,0xC0,0x00,0x00,0x00,0x01,0x81,0xC0,
      0x60,0x30,0x18,0x0E,0x87,0x81,0x00,0x00,0x00,0x03,0x03,0x02,0x02,0x02,0x02,0x02,
      0x03,0x03,0x00,0x00,

      /*--  ����:  3  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0xE0,0xF0,0x10,0x10,0x10,0x30,0xE0,0xC0,0x00,0x00,0x00,0x00,0xC0,0xC0,0x00,
      0x04,0x04,0x06,0x0F,0xF9,0xF0,0x00,0x00,0x00,0x01,0x03,0x02,0x02,0x02,0x02,0x03,
      0x01,0x00,0x00,0x00,

      /*--  ����:  4  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0x00,0x00,0x00,0x80,0xE0,0xF0,0xF8,0xF8,0x00,0x00,0x00,0x00,0x18,0x3C,0x27,
      0x23,0x20,0xFF,0xFF,0xFF,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x03,0x03,
      0x03,0x02,0x02,0x00,

      /*--  ����:  5  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0x00,0xF0,0xF0,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x00,0xC0,0xCF,0x0F,
      0x06,0x02,0x02,0x06,0xFC,0xF8,0x00,0x00,0x00,0x01,0x03,0x02,0x02,0x02,0x02,0x03,
      0x01,0x00,0x00,0x00,

      /*--  ����:  6  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0x00,0xC0,0xE0,0x30,0x10,0x10,0x10,0x70,0x60,0x00,0x00,0x00,0x7F,0xFF,0x86,
      0x02,0x02,0x02,0x06,0xFC,0xF8,0x00,0x00,0x00,0x00,0x01,0x03,0x02,0x02,0x02,0x03,
      0x01,0x00,0x00,0x00,

      /*--  ����:  7  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0x70,0x70,0x10,0x10,0x10,0xD0,0xF0,0x30,0x10,0x00,0x00,0x00,0x00,0x00,0xF0,
      0xFC,0x0F,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,

      /*--  ����:  8  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0xC0,0xE0,0x30,0x10,0x10,0x10,0x30,0xE0,0xC0,0x00,0x00,0x00,0xF1,0xFB,0x0E,
      0x04,0x04,0x04,0x0E,0xFB,0xF1,0x00,0x00,0x00,0x00,0x01,0x03,0x02,0x02,0x02,0x03,
      0x01,0x00,0x00,0x00,

      /*--  ����:  9  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0xC0,0xE0,0x30,0x10,0x10,0x10,0x70,0xE0,0x80,0x00,0x00,0x00,0x87,0x8F,0x18,
      0x10,0x10,0x10,0xD8,0xFF,0x3F,0x00,0x00,0x00,0x01,0x03,0x02,0x02,0x02,0x03,0x01,
      0x00,0x00,0x00,0x00,

      /*--  ����:  :  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x00,
      0x00,0x00,0x00,0x00,

      /*--  ����:  �����ǰλ��λ��  --*/
      /*--  @������16;  �������¶�Ӧ�ĵ���Ϊ����x��=12x21   --*/
      /*--  �߶Ȳ���8�ı������ֵ���Ϊ������x�߶�=12x24  --*/
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,

      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,
};

static const unsigned char   Battery_16x8[] =              // ASCII
{
         /*--  ������һ��ͼ��D:\Work\smartwatch\20%.bmp  --*/
        /*--  ����x�߶�=16x8  -- 0%*/ 
        0xFF,0xFF,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xFF,0xFF,0x3C,0x18,
        /*--  ������һ��ͼ��D:\Work\smartwatch\20%.bmp  --*/
        /*--  ����x�߶�=16x8  -- 20%*/ 
        0xFF,0xFF,0xFF,0xFF,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xFF,0xFF,0x3C,0x18,
        /*--  ������һ��ͼ��D:\Work\smartwatch\20%.bmp  --*/
        /*--  ����x�߶�=16x8  --40%*/
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xFF,0xFF,0x3C,0x18,
        /*--  ����x�߶�=16x8  --60%*/
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC3,0xC3,0xC3,0xC3,0xFF,0xFF,0x3C,0x18,
        /*--  ����x�߶�=16x8  --80%*/
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC3,0xC3,0xFF,0xFF,0x3C,0x18,
        /*--  ����x�߶�=16x8  --100%*/
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3C,0x18,
};

static const unsigned char   Charge_48x16[] =              // ASCII
{
           /*--  ������һ��ͼ���������½���ͼ��  --*/
      /*--  ����x�߶�=48x16  -- 0%*/
      0xFC,0xFE,0xFF,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
      0x07,0x07,0x07,0x07,0x07,0x07,0x07,0xFF,0xFE,0xFC,0xF8,0xF0,0x00,0x00,0x00,0x80,
      0xC0,0xE0,0xB0,0x98,0x8C,0x86,0x83,0x81,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x3F,0x7F,0xFF,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,
      0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xFF,0x7F,0x3F,0x1F,0x0F,0x00,0x00,0x00,0x01,
      0x81,0xC1,0x61,0x31,0x19,0x0D,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      /*--  ������һ��ͼ���������½���ͼ��  --*/
      /*--  ����x�߶�=48x16  --20%*/
      0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
      0x07,0x07,0x07,0x07,0x07,0x07,0x07,0xFF,0xFE,0xFC,0xF8,0xF0,0x00,0x00,0x00,0x80,
      0xC0,0xE0,0xB0,0x98,0x8C,0x86,0x83,0x81,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,
      0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xFF,0x7F,0x3F,0x1F,0x0F,0x00,0x00,0x00,0x01,
      0x81,0xC1,0x61,0x31,0x19,0x0D,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      /*--  ������һ��ͼ���������½���ͼ��  --*/
      /*--  ����x�߶�=48x16  --40%*/
      0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x07,0x07,0x07,0x07,0x07,
      0x07,0x07,0x07,0x07,0x07,0x07,0x07,0xFF,0xFE,0xFC,0xF8,0xF0,0x00,0x00,0x00,0x80,
      0xC0,0xE0,0xB0,0x98,0x8C,0x86,0x83,0x81,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE0,0xE0,0xE0,0xE0,0xE0,
      0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xFF,0x7F,0x3F,0x1F,0x0F,0x00,0x00,0x00,0x01,
      0x81,0xC1,0x61,0x31,0x19,0x0D,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      /*--  ������һ��ͼ���������½���ͼ��  --*/
      /*--  ����x�߶�=48x16  --60%*/
      0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x07,
      0x07,0x07,0x07,0x07,0x07,0x07,0x07,0xFF,0xFE,0xFC,0xF8,0xF0,0x00,0x00,0x00,0x80,
      0xC0,0xE0,0xB0,0x98,0x8C,0x86,0x83,0x81,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE0,
      0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xFF,0x7F,0x3F,0x1F,0x0F,0x00,0x00,0x00,0x01,
      0x81,0xC1,0x61,0x31,0x19,0x0D,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      /*--  ������һ��ͼ���������½���ͼ��  --*/
      /*--  ����x�߶�=48x16  --80%*/
      0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0x07,0x07,0x07,0x07,0xFF,0xFE,0xFC,0xF8,0xF0,0x00,0x00,0x00,0x80,
      0xC0,0xE0,0xB0,0x98,0x8C,0x86,0x83,0x81,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xE0,0xE0,0xE0,0xE0,0xFF,0x7F,0x3F,0x1F,0x0F,0x00,0x00,0x00,0x01,
      0x81,0xC1,0x61,0x31,0x19,0x0D,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      /*--  ������һ��ͼ���������½���ͼ��  --*/
      /*--  ����x�߶�=48x16  --100%*/
      0xFC,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFC,0xF8,0xF0,0x00,0x00,0x00,0x80,
      0xC0,0xE0,0xB0,0x98,0x8C,0x86,0x83,0x81,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x3F,0x1F,0x0F,0x00,0x00,0x00,0x01,
      0x81,0xC1,0x61,0x31,0x19,0x0D,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
////////////////////////////////////////////////////////////////////////////
#if 0//tiger.yang
void delay(unsigned int z)
{
	unsigned int x,y;
	for(x=z;x>0;x--)
		for(y=800;y>0;y--);
}
#endif
/**********************************************
//IIC Start
**********************************************/
void IIC_Start(void)
{
 
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop(void)
{
 
   
}

/**********************************************
// ͨ��I2C����дһ���ֽ�
**********************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
	
}

/*********************OLEDд����************************************/ 
void OLED_WrDat(unsigned char IIC_Data)
{
	//IIC_Start();
	//Write_IIC_Byte(0x78);
	//Write_IIC_Byte(0x40);			//write data
	//Write_IIC_Byte(IIC_Data);
	//IIC_Stop();

	i2c_burst_write(0x78, 0x40, (u8*)&IIC_Data, 1);
}
/*********************OLEDд����************************************/
void OLED_WrCmd(unsigned char IIC_Command)
{
	//IIC_Start();
	//Write_IIC_Byte(0x78);            //Slave addOLED_RESs,SA0=0
	//Write_IIC_Byte(0x00);			//write command
	//Write_IIC_Byte(IIC_Command);
	//IIC_Stop();

	i2c_burst_write(0x78, 0x00, (u8*)&IIC_Command, 1);
}
/*********************OLED ��������************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WrCmd(0xb0+y);
	OLED_WrCmd(((x&0xf0)>>4)|0x10);
	OLED_WrCmd((x&0x0f)|0x01);
} 
/*********************OLEDȫ��************************************/
void OLED_Fill(unsigned char bmp_dat) 
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		OLED_WrDat(bmp_dat);
	}
}
/*********************OLED��λ************************************/
void OLED_CLS(void)
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for(x=30;x<X_WIDTH;x++)
		OLED_WrDat(0);
	}
}

/*********************OLED��λ************************************/
void OLED_CLS_y(unsigned char y)
{
	unsigned char x;
	
	OLED_WrCmd(0xb0+y);
	OLED_WrCmd(0x01);
	OLED_WrCmd(0x10);
	for(x=32;x<128;x++)
	OLED_WrDat(0);
	
}


extern void reset_ssd1306(void);
/*********************OLED��ʼ��************************************/
void OLED_Init(void)
{
	#if 0
    OLED_RES = low;
	delay(1000);//��ʼ��֮ǰ����ʱ����Ҫ��
    OLED_RES = high;
    delay(1000);//��ʼ��֮ǰ����ʱ����Ҫ��
	#endif
    reset_ssd1306();
	sleep_us(100*1000);
	#if 0
        /* OLED_WrCmd(0xae);//--turn off oled panel
	OLED_WrCmd(0x00);//---set low column addOLED_RESs
	OLED_WrCmd(0x10);//---set high column addOLED_RESs
	OLED_WrCmd(0x40);//--set start line addOLED_RESs  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WrCmd(0x81);//--set contrast control register
	OLED_WrCmd(Brightness); // Set SEG Output Current Brightness
	OLED_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WrCmd(0xa6);//--set normal display
	OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3f);//--1/64 duty
	OLED_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WrCmd(0x00);//-not offset
	OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WrCmd(0xd9);//--set pre-charge period
	OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WrCmd(0xda);//--set com pins hardware configuration
	OLED_WrCmd(0x12);
	OLED_WrCmd(0xdb);//--set vcomh
	OLED_WrCmd(0x40);//Set VCOM Deselect Level
	OLED_WrCmd(0x20);//-Set Page AddOLED_RESsing Mode (0x00/0x01/0x02)
	OLED_WrCmd(0x02);//
	OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
	OLED_WrCmd(0x14);//--set(0x10) disable
	OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	OLED_WrCmd(0xaf);//--turn on oled panel
	OLED_Fill(0x00); //��ʼ����
	OLED_Set_Pos(0,0);*/
	OLED_WrCmd(0xae);//--turn off oled panel
	OLED_WrCmd(0x00);//---set low column addOLED_RESs
	OLED_WrCmd(0x12);//---set high column addOLED_RESs

	OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	
	OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
    OLED_WrCmd(0x1f);//--1/64 duty
    
	//OLED_WrCmd(0xa6);//--set normal display	
	//OLED_WrCmd(0x02);//	
	//OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
    OLED_WrCmd(0x00);//--set start line addOLED_RESs  Set Mapping RAM Display Start Line (0x00~0x3F)
             
    OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
	OLED_WrCmd(0x95);//--set(0x10) disable
	//OLED_WrCmd(0x10); //���㲻��

	OLED_WrCmd(0xb0);//-Set Page AddOLED_RESsing Mode (0x00/0x01/0x02)
        
    OLED_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����      
    OLED_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
        
       
	OLED_WrCmd(0xda);//--set com pins hardware configuration
	OLED_WrCmd(0x12);
         
	OLED_WrCmd(0x81);//--set contrast control register
	OLED_WrCmd(0x4f); // Set SEG Output Current Brightness
	
	OLED_WrCmd(0xd9);//--set pre-charge period
	OLED_WrCmd(0x1F);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	
	 OLED_WrCmd(0xdb);//--set vcomh
	OLED_WrCmd(0x40);//Set VCOM Deselect Level

	
    //OLED_WrCmd(0xa4);//--turn on oled panel//tiger.yang
    OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7)

	OLED_WrCmd(0xaf);//--turn on oled panel

    //delay(4000);//��ʼ��֮ǰ����ʱ����Ҫ��
    sleep_us(400*1000);//tiger.yang
    //OLED_WrCmd(0xa5);//--turn on oled panel

      
      
	OLED_Fill(0xFF); //��ʼ����//tiger.yang
	//OLED_Set_Pos(0,0);

	//////////test////////////////
	OLED_CLS();
	OLED_Set_Pos(0,0);
	OLED_P16x16Ch(32,0,0);
    sleep_us(400*1000);
    //////////test////////////////
	//OLED_CLS();
	//OLED_Set_Pos(0,0);
	//OLED_P16x16Ch(32,0,0);
	//OLED_P16x16Ch(48,0,0);
    sleep_us(400*1000);
    
#endif
#if 0   ////porter
	OLED_WrCmd(0xAE); //Set display off
	OLED_WrCmd(0x0e);//---set low column addOLED_RESs
	OLED_WrCmd(0x11);//---set high column addOLED_RESs
	
    OLED_WrCmd(0xD5); //Display divide ratio/osc. freq. mode
    OLED_WrCmd(0xF0); //Divide ratio 2 70 160 f0 200

    OLED_WrCmd(0xA8); //Multiplex ration mode:40
    //OLED_WrCmd(0x27);
    OLED_WrCmd(0x1f);

    OLED_WrCmd(0xD3); //Set Display Offset
    OLED_WrCmd(0x00); //Set vertical shift

    OLED_WrCmd(0x40); //Set Display Start Line

    OLED_WrCmd(0x8D); //Internal charge pump:
    OLED_WrCmd(0x95); // 0x95Enable Charge Pump/ 10h:Disable Charge Pump
	//OLED_WrCmd(0x10); //20161030,���㲻��

    OLED_WrCmd(0x20); //Set page address mode
    OLED_WrCmd(0x02);

    OLED_WrCmd(0xA1); //Segment Remap

    OLED_WrCmd(0xC8); //Set com output scan direction

    OLED_WrCmd(0xDA); //Common pads hardware: alternative
    OLED_WrCmd(0x12);

    OLED_WrCmd(0xAD); //Internal IREF Setting
    OLED_WrCmd(0x30); //Selectting: 30uA

    OLED_WrCmd(0x81); //Contrast control
    //OLED_WrCmd(0xFF);
    OLED_WrCmd(0xFF);//��������

    OLED_WrCmd(0xD9); //Set pre-charge period
    OLED_WrCmd(0x1f);//OLED_WrCmd(0x22);

    OLED_WrCmd(0xDB); //VCOM deselect level mode
    OLED_WrCmd(0x20); //VCOM:0.71*VCC

    OLED_WrCmd(0xA4); //Set Entire Display On/Off

    OLED_WrCmd(0xA6); //Set Normal Display

    //ssd1306_ram_fill(0x00);

    OLED_WrCmd(0xAF); //Set Display On
#endif

#if 0
	OLED_Fill(0x00); //��ʼ����//tiger.yang
    sleep_us(1000*1000);//tiger.yang
	
	OLED_CLS();
	OLED_Set_Pos(0,0);	
	sleep_us(1000*1000);//tiger.yang
	//OLED_P8x16Ch(0,2,0);
	OLED_P16x16Ch(30,0,0);

	//OLED_CLS_y(0);
	//OLED_CLS_y(1);
	//OLED_CLS_y(2);
	//OLED_CLS_y(3);
#endif

#if 0
	OLED_CLS();
	OLED_Set_Pos(0,0);	
	OLED_P16x16Ch_FF(32,0,0);
	//OLED_P8x16Ch(32,0,0);
	sleep_us(1000*1000);

	OLED_CLS();
	//OLED_P16x16Ch_FF(32,1,0);
	OLED_P8x16Ch(32,1,0);
	sleep_us(1000*1000);

	OLED_CLS();
	//OLED_P16x16Ch_FF(32,2,0);
	OLED_P8x16Ch(32,2,0);
	sleep_us(1000*1000);

	OLED_CLS();
	//OLED_P16x16Ch_FF(32,3,0);
	OLED_P8x16Ch(32,3,0);
    sleep_us(400*1000);
    
	//sleep_us(1000*1000);
	//OLED_Fill(0xFF); 
	
	//sleep_us(1000*1000);
	//OLED_Fill(0x00); //��ʼ����
	//OLED_Set_Pos(0,0);
#endif
#if 1  //16pin
    OLED_WrCmd(0xAE);	   //display off

    	OLED_WrCmd(0x40);	   //set start line,set to 0x40

    	OLED_WrCmd(0x81);	   //set contrast
    	OLED_WrCmd(0x80);     //0x25

    	OLED_WrCmd(0xA1);	   //set segment re-map

    	OLED_WrCmd(0xA4);	   //set entire display off

    	OLED_WrCmd(0xA6);	   //set normal display

    	OLED_WrCmd(0xA8);	   //�]�mCOM�}���Ҧ�,�q1-40,���B�]�m��40
    	OLED_WrCmd(0x27);

    	OLED_WrCmd(0xAD);	   //set DC-DC
    	OLED_WrCmd(0x8B);	   //8B:DC ON, 8A:DC OFF

    	OLED_WrCmd(0x33);     // set VPP=9V

    	OLED_WrCmd(0xC8);	   //set commom output scan direction

    	OLED_WrCmd(0xD3);	   //display offset,set to 0x00
    	OLED_WrCmd(0x0c);

    	OLED_WrCmd(0xD5);	   //set display clock divide ratio/osc frequency
    	OLED_WrCmd(0x80);

    	OLED_WrCmd(0xD9);	   //set dis-charge/pre-charge period
    	OLED_WrCmd(0x22);

    	OLED_WrCmd(0xDA);	   //Common pads hardware configuration:
    	OLED_WrCmd(0x12);

    	OLED_WrCmd(0xDB);	   //set VCOM deselect level
    	OLED_WrCmd(0x28);

    	OLED_WrCmd(0xAF);	   //display on

    	OLED_Fill(0xff);

    	sleep_us(1000*1000);//tiger.yang

    	OLED_CLS();
/*
   	 	OLED_Set_Pos(0,0);

   		sleep_us(1000*1000);//tiger.yang
    	OLED_P8x16Ch(0,2,0);
    	OLED_P16x16Ch(30,0,0);
    	sleep_us(1000*1000);
*/
#endif




    
} 
/***************������������ʾ6*8һ���׼ASCII�ַ���	��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>126){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
		OLED_WrDat(F6x8[c][i]);
		x+=6;
		j++;
	}
}
/*******************������������ʾ8*16һ���׼ASCII�ַ���	 ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/

/***************������������ʾ16*8һ���׼ASCII�ַ���	��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
void OLED_P16x8Str(unsigned char x,unsigned char y,unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=16*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(Battery_16x8[adder]);
		adder += 1;
	}
}
/*******************������������ʾ8*16һ���׼ASCII�ַ���	 ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/

void OLED_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120){x=0;y++;}
		OLED_Set_Pos(x,y);
		for(i=0;i<8;i++)
		OLED_WrDat(F8X16[c*16+i]);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		OLED_WrDat(F8X16[c*16+i+8]);
		x+=8;
		j++;
	}
}

/*******************������������ʾ12*24һ���׼ASCII�ַ���	 ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************/
void OLED_P12x24Ch(unsigned char x,unsigned char y,unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=36*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 12;wm++)
	{
		OLED_WrDat(nAsciiDot[adder]);
		adder += 1;
	}
	OLED_Set_Pos(x,y + 1);
	for(wm = 0;wm < 12;wm++)
	{
		OLED_WrDat(nAsciiDot[adder]);
		adder += 1;
	} 
       OLED_Set_Pos(x,y + 2);
	for(wm = 0;wm < 12;wm++)
	{
		OLED_WrDat(nAsciiDot[adder]);
		adder += 1;
	}     
}
/*****************������������ʾ16*16����  ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************************/
void OLED_P16x16Ch(unsigned char x,unsigned char y,unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16_xin[adder]);
		adder += 1;
	}

	OLED_Set_Pos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16_xin[adder]);
		adder += 1;
	} 

}
/*****************������������ʾ16*16����  ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************************/
void OLED_P16x16Ch_FF(unsigned char x,unsigned char y,unsigned char N)
{

	unsigned char wm=0;
	unsigned int adder=32*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16_FF[adder]);
		adder += 1;
	}
	//OLED_Set_Pos(x,y + 1);
	//for(wm = 0;wm < 16;wm++)
	//{
		//OLED_WrDat(F16x16_FF[adder]);
		//adder += 1;
	//} 
	
}
/*****************������������ʾ8*16����  ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************************/
void OLED_P8x16Ch(unsigned char x,unsigned char y,unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F8x16_H[adder]);
		adder += 1;
	}
	
}

/*****************������������ʾ16*16����  ��ʾ�����꣨x,y����yΪҳ��Χ0��7****************************/
void OLED_P48x16Ch(unsigned char x,unsigned char y,unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=96*N;
	OLED_Set_Pos(x , y);
	for(wm = 0;wm < 48;wm++)
	{
		OLED_WrDat(Charge_48x16[adder]);
		adder += 1;
	}
	OLED_Set_Pos(x,y + 1);
	for(wm = 0;wm < 48;wm++)
	{
		OLED_WrDat(Charge_48x16[adder]);
		adder += 1;
	} 	  	
}


/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WrDat(BMP[j++]);
	    }
	}
}


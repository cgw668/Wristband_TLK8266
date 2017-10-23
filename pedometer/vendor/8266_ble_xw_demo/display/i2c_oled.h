#ifndef _I2C_OLED_H_
#define _I2C_OLED_H_

#include "../../../proj/tl_common.h"

#define high 1
#define low 0

#define	Brightness	0xCF 
#define X_WIDTH 	148
#define Y_WIDTH 	64



///////////////////////////////////////////////////////////////////////////////
void OLED_Init(void);
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
void OLED_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[]);
void OLED_P16x16Ch(unsigned char x,unsigned char y,unsigned char N);
void OLED_P12x24Ch(unsigned char x,unsigned char y,unsigned char N);
void OLED_P16x8Str(unsigned char x,unsigned char y,unsigned char N);
void OLED_P48x16Ch(unsigned char x,unsigned char y,unsigned char N);
void OLED_P8x16Ch(unsigned char x,unsigned char y,unsigned char N);
void OLED_P16x16Ch_FF(unsigned char x,unsigned char y,unsigned char N);

#endif

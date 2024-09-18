#ifndef __EPAPER_H
#define __EPAPER_H

#include "cyhal.h"
#include "cybsp.h"

//400*300///////////////////////////////////////

#define MONOMSB_MODE 1
#define MONOLSB_MODE 2 
#define RED_MODE     3

#define MAX_LINE_BYTES 50		// =400/8
#define MAX_COLUMN_BYTES  300

#define ALLSCREEN_GRAGHBYTES	15000

#define MONO 1
#define RED  2

///////////////////////////GPIO Settings//////////////////////////////////////////////////////
#define EPD_W21_DC_0	cyhal_gpio_write(DC, 0)
#define EPD_W21_DC_1	cyhal_gpio_write(DC, 1)

#define EPD_W21_RST_0	cyhal_gpio_write(RST, 0)
#define EPD_W21_RST_1	cyhal_gpio_write(RST, 1)

#define EPD_W21_CS_0	cyhal_gpio_write(CYBSP_SPI_CS, 0)
#define EPD_W21_CS_1	cyhal_gpio_write(CYBSP_SPI_CS, 1)

#define EPD_W21_BUSY_LEVEL 0
#define isEPD_W21_BUSY cyhal_gpio_read(BUSY)

void driver_delay_xms(unsigned long xms);
void EpaperIO_Init(void);
void Epaper_READBUSY(void);
void Epaper_Spi_WriteByte(unsigned char TxData);
void Epaper_Write_Command(unsigned char cmd);
void Epaper_Write_Data(unsigned char data);

void EPD_HW_Init(void); //Electronic paper initialization
void EPD_Update(void);

void EPD_WhiteScreen_Red(void);
void EPD_WhiteScreen_Black(void);
void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);
//Display 

void EPD_ALL_image(const unsigned char *datas1, const unsigned char *datas2);

void EPD_Dis_Part(unsigned int xstart, unsigned int ystart,
		const unsigned char *datas, const unsigned char color_mode,
		unsigned int PART_COLUMN, unsigned int PART_LINE);
void EPD_Dis_Part_mult(unsigned int x_startA, unsigned int y_startA,
		const unsigned char *datasA1, const unsigned char *datasA2,
		unsigned int x_startB, unsigned int y_startB,
		const unsigned char *datasB1, const unsigned char *datasB2,
		unsigned int PART_COLUMN, unsigned int PART_LINE);

#endif


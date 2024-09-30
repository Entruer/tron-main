/*
 *----------------------------------------------------------------------
 *    micro T-Kernel 3.00.00
 *
 *    Copyright (C) 2006-2019 by Ken Sakamura.
 *    This software is distributed under the T-License 2.1.
 *----------------------------------------------------------------------
 *
 *    Released by TRON Forum(http://www.tron.org) at 2019/12/11.
 *
 *----------------------------------------------------------------------
 */
 

#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#include <string.h>
#include <stdio.h>

#include "cyhal.h"
#include "cybsp.h"
#include "UartDma.h"
#include "Epaper.h"
#include "Matrix.h"

/****************************************************
 * External variables
 ****************************************************/

extern uint8_t rx_dma_uart_buffer[BUFFER_SIZE];
extern uint8_t rx_dma_done;

/****************************************************
 * Variables
 ****************************************************/

uint32_t water_total_drink;
uint8_t water_now, water_last;
uint8_t water_total_drink_level;
uint8_t animal_sel;
bool epaper_need_update;
bool cup_connected;
uint8_t *datas1, *datas2;

/****************************************************
 * Mapping
 ****************************************************/

typedef struct {
	void *left;
	void *right;
} Mapping;


void right_shift_datas(unsigned char **datas, const Mapping *mappings);
void left_shift_datas(unsigned char **datas, const Mapping *mappings);
void increase_stage(unsigned char **datas, const Mapping *mappings);

const Mapping animal_switch_l0[] = {
	{(uint8_t *)cat_happy_stage1_layer0, (uint8_t *)elephant_happy_stage1_layer0},
	{(uint8_t *)cat_happy_stage2_layer0, (uint8_t *)elephant_happy_stage2_layer0},
	{(uint8_t *)cat_happy_stage3_layer0, (uint8_t *)elephant_happy_stage3_layer0},
	{(uint8_t *)cat_happy_stage4_layer0, (uint8_t *)elephant_happy_stage4_layer0},
	{(uint8_t *)cat_happy_stage5_layer0, (uint8_t *)elephant_happy_stage5_layer0},
	{(uint8_t *)cat_sad_stage1_layer0, (uint8_t *)elephant_sad_stage1_layer0},
	{(uint8_t *)cat_sad_stage2_layer0, (uint8_t *)elephant_sad_stage2_layer0},
	{(uint8_t *)cat_sad_stage3_layer0, (uint8_t *)elephant_sad_stage3_layer0},
	{(uint8_t *)cat_sad_stage4_layer0, (uint8_t *)elephant_sad_stage4_layer0},
	{(uint8_t *)cat_sad_stage5_layer0, (uint8_t *)elephant_sad_stage5_layer0},
	{(uint8_t *)elephant_happy_stage1_layer0, (uint8_t *)seal_happy_stage1_layer0},
	{(uint8_t *)elephant_happy_stage2_layer0, (uint8_t *)seal_happy_stage2_layer0},
	{(uint8_t *)elephant_happy_stage3_layer0, (uint8_t *)seal_happy_stage3_layer0},
	{(uint8_t *)elephant_happy_stage4_layer0, (uint8_t *)seal_happy_stage4_layer0},
	{(uint8_t *)elephant_happy_stage5_layer0, (uint8_t *)seal_happy_stage5_layer0},
	{(uint8_t *)elephant_sad_stage1_layer0, (uint8_t *)seal_sad_stage1_layer0},
	{(uint8_t *)elephant_sad_stage2_layer0, (uint8_t *)seal_sad_stage2_layer0},
	{(uint8_t *)elephant_sad_stage3_layer0, (uint8_t *)seal_sad_stage3_layer0},
	{(uint8_t *)elephant_sad_stage4_layer0, (uint8_t *)seal_sad_stage4_layer0},
	{(uint8_t *)elephant_sad_stage5_layer0, (uint8_t *)seal_sad_stage5_layer0},
	{(uint8_t *)seal_happy_stage1_layer0, (uint8_t *)tree_happy_stage1_layer0},
	{(uint8_t *)seal_happy_stage2_layer0, (uint8_t *)tree_happy_stage2_layer0},
	{(uint8_t *)seal_happy_stage3_layer0, (uint8_t *)tree_happy_stage3_layer0},
	{(uint8_t *)seal_happy_stage4_layer0, (uint8_t *)tree_happy_stage4_layer0},
	{(uint8_t *)seal_happy_stage5_layer0, (uint8_t *)tree_happy_stage5_layer0},
	{(uint8_t *)seal_sad_stage1_layer0, (uint8_t *)tree_sad_stage1_layer0},
	{(uint8_t *)seal_sad_stage2_layer0, (uint8_t *)tree_sad_stage2_layer0},
	{(uint8_t *)seal_sad_stage3_layer0, (uint8_t *)tree_sad_stage3_layer0},
	{(uint8_t *)seal_sad_stage4_layer0, (uint8_t *)tree_sad_stage4_layer0},
	{(uint8_t *)seal_sad_stage5_layer0, (uint8_t *)tree_sad_stage5_layer0},
	{(uint8_t *)tree_happy_stage1_layer0, (uint8_t *)cat_happy_stage1_layer0},
	{(uint8_t *)tree_happy_stage2_layer0, (uint8_t *)cat_happy_stage2_layer0},
	{(uint8_t *)tree_happy_stage3_layer0, (uint8_t *)cat_happy_stage3_layer0},
	{(uint8_t *)tree_happy_stage4_layer0, (uint8_t *)cat_happy_stage4_layer0},
	{(uint8_t *)tree_happy_stage5_layer0, (uint8_t *)cat_happy_stage5_layer0},
	{(uint8_t *)tree_sad_stage1_layer0, (uint8_t *)cat_sad_stage1_layer0},
	{(uint8_t *)tree_sad_stage2_layer0, (uint8_t *)cat_sad_stage2_layer0},
	{(uint8_t *)tree_sad_stage3_layer0, (uint8_t *)cat_sad_stage3_layer0},
	{(uint8_t *)tree_sad_stage4_layer0, (uint8_t *)cat_sad_stage4_layer0},
	{(uint8_t *)tree_sad_stage5_layer0, (uint8_t *)cat_sad_stage5_layer0},
};

const Mapping animal_switch_l1[] = {
	{(uint8_t *)cat_happy_stage1_layer1, (uint8_t *)elephant_happy_stage1_layer1},
	{(uint8_t *)cat_happy_stage2_layer1, (uint8_t *)elephant_happy_stage2_layer1},
	{(uint8_t *)cat_happy_stage3_layer1, (uint8_t *)elephant_happy_stage3_layer1},
	{(uint8_t *)cat_happy_stage4_layer1, (uint8_t *)elephant_happy_stage4_layer1},
	{(uint8_t *)cat_happy_stage5_layer1, (uint8_t *)elephant_happy_stage5_layer1},
	{(uint8_t *)cat_sad_stage1_layer1, (uint8_t *)elephant_sad_stage1_layer1},
	{(uint8_t *)cat_sad_stage2_layer1, (uint8_t *)elephant_sad_stage2_layer1},
	{(uint8_t *)cat_sad_stage3_layer1, (uint8_t *)elephant_sad_stage3_layer1},
	{(uint8_t *)cat_sad_stage4_layer1, (uint8_t *)elephant_sad_stage4_layer1},
	{(uint8_t *)cat_sad_stage5_layer1, (uint8_t *)elephant_sad_stage5_layer1},
	{(uint8_t *)elephant_happy_stage1_layer1, (uint8_t *)seal_happy_stage1_layer1},
	{(uint8_t *)elephant_happy_stage2_layer1, (uint8_t *)seal_happy_stage2_layer1},
	{(uint8_t *)elephant_happy_stage3_layer1, (uint8_t *)seal_happy_stage3_layer1},
	{(uint8_t *)elephant_happy_stage4_layer1, (uint8_t *)seal_happy_stage4_layer1},
	{(uint8_t *)elephant_happy_stage5_layer1, (uint8_t *)seal_happy_stage5_layer1},
	{(uint8_t *)elephant_sad_stage1_layer1, (uint8_t *)seal_sad_stage1_layer1},
	{(uint8_t *)elephant_sad_stage2_layer1, (uint8_t *)seal_sad_stage2_layer1},
	{(uint8_t *)elephant_sad_stage3_layer1, (uint8_t *)seal_sad_stage3_layer1},
	{(uint8_t *)elephant_sad_stage4_layer1, (uint8_t *)seal_sad_stage4_layer1},
	{(uint8_t *)elephant_sad_stage5_layer1, (uint8_t *)seal_sad_stage5_layer1},
	{(uint8_t *)seal_happy_stage1_layer1, (uint8_t *)tree_happy_stage1_layer1},
	{(uint8_t *)seal_happy_stage2_layer1, (uint8_t *)tree_happy_stage2_layer1},
	{(uint8_t *)seal_happy_stage3_layer1, (uint8_t *)tree_happy_stage3_layer1},
	{(uint8_t *)seal_happy_stage4_layer1, (uint8_t *)tree_happy_stage4_layer1},
	{(uint8_t *)seal_happy_stage5_layer1, (uint8_t *)tree_happy_stage5_layer1},
	{(uint8_t *)seal_sad_stage1_layer1, (uint8_t *)tree_sad_stage1_layer1},
	{(uint8_t *)seal_sad_stage2_layer1, (uint8_t *)tree_sad_stage2_layer1},
	{(uint8_t *)seal_sad_stage3_layer1, (uint8_t *)tree_sad_stage3_layer1},
	{(uint8_t *)seal_sad_stage4_layer1, (uint8_t *)tree_sad_stage4_layer1},
	{(uint8_t *)seal_sad_stage5_layer1, (uint8_t *)tree_sad_stage5_layer1},
	{(uint8_t *)tree_happy_stage1_layer1, (uint8_t *)cat_happy_stage1_layer1},
	{(uint8_t *)tree_happy_stage2_layer1, (uint8_t *)cat_happy_stage2_layer1},
	{(uint8_t *)tree_happy_stage3_layer1, (uint8_t *)cat_happy_stage3_layer1},
	{(uint8_t *)tree_happy_stage4_layer1, (uint8_t *)cat_happy_stage4_layer1},
	{(uint8_t *)tree_happy_stage5_layer1, (uint8_t *)cat_happy_stage5_layer1},
	{(uint8_t *)tree_sad_stage1_layer1, (uint8_t *)cat_sad_stage1_layer1},
	{(uint8_t *)tree_sad_stage2_layer1, (uint8_t *)cat_sad_stage2_layer1},
	{(uint8_t *)tree_sad_stage3_layer1, (uint8_t *)cat_sad_stage3_layer1},
	{(uint8_t *)tree_sad_stage4_layer1, (uint8_t *)cat_sad_stage4_layer1},
	{(uint8_t *)tree_sad_stage5_layer1, (uint8_t *)cat_sad_stage5_layer1},
};

const Mapping stage_switch_l0[] = {
	{(uint8_t *)cat_sad_stage1_layer0, (uint8_t *)cat_sad_stage2_layer0},
	{(uint8_t *)cat_sad_stage2_layer0, (uint8_t *)cat_sad_stage3_layer0},
	{(uint8_t *)cat_sad_stage3_layer0, (uint8_t *)cat_happy_stage4_layer0},
	{(uint8_t *)cat_happy_stage4_layer0, (uint8_t *)cat_happy_stage5_layer0},
	{(uint8_t *)cat_happy_stage5_layer0, (uint8_t *)cat_happy_stage5_layer0},
	{(uint8_t *)elephant_sad_stage1_layer0, (uint8_t *)elephant_sad_stage2_layer0},
	{(uint8_t *)elephant_sad_stage2_layer0, (uint8_t *)elephant_sad_stage3_layer0},
	{(uint8_t *)elephant_sad_stage3_layer0, (uint8_t *)elephant_happy_stage4_layer0},
	{(uint8_t *)elephant_happy_stage4_layer0, (uint8_t *)elephant_happy_stage5_layer0},
	{(uint8_t *)elephant_happy_stage5_layer0, (uint8_t *)elephant_happy_stage5_layer0},
	{(uint8_t *)seal_sad_stage1_layer0, (uint8_t *)seal_sad_stage2_layer0},
	{(uint8_t *)seal_sad_stage2_layer0, (uint8_t *)seal_sad_stage3_layer0},
	{(uint8_t *)seal_sad_stage3_layer0, (uint8_t *)seal_happy_stage4_layer0},
	{(uint8_t *)seal_happy_stage4_layer0, (uint8_t *)seal_happy_stage5_layer0},
	{(uint8_t *)seal_happy_stage5_layer0, (uint8_t *)seal_happy_stage5_layer0},
	{(uint8_t *)tree_sad_stage1_layer0, (uint8_t *)tree_sad_stage2_layer0},
	{(uint8_t *)tree_sad_stage2_layer0, (uint8_t *)tree_sad_stage3_layer0},
	{(uint8_t *)tree_sad_stage3_layer0, (uint8_t *)tree_happy_stage4_layer0},
	{(uint8_t *)tree_happy_stage4_layer0, (uint8_t *)tree_happy_stage5_layer0},
	{(uint8_t *)tree_happy_stage5_layer0, (uint8_t *)tree_happy_stage5_layer0},
};

const Mapping stage_switch_l1[] = {
	{(uint8_t *)cat_sad_stage1_layer1, (uint8_t *)cat_sad_stage2_layer1},
	{(uint8_t *)cat_sad_stage2_layer1, (uint8_t *)cat_sad_stage3_layer1},
	{(uint8_t *)cat_sad_stage3_layer1, (uint8_t *)cat_happy_stage4_layer1},
	{(uint8_t *)cat_happy_stage4_layer1, (uint8_t *)cat_happy_stage5_layer1},
	{(uint8_t *)cat_happy_stage5_layer1, (uint8_t *)cat_happy_stage5_layer1},
	{(uint8_t *)elephant_sad_stage1_layer1, (uint8_t *)elephant_sad_stage2_layer1},
	{(uint8_t *)elephant_sad_stage2_layer1, (uint8_t *)elephant_sad_stage3_layer1},
	{(uint8_t *)elephant_sad_stage3_layer1, (uint8_t *)elephant_happy_stage4_layer1},
	{(uint8_t *)elephant_happy_stage4_layer1, (uint8_t *)elephant_happy_stage5_layer1},
	{(uint8_t *)elephant_happy_stage5_layer1, (uint8_t *)elephant_happy_stage5_layer1},
	{(uint8_t *)seal_sad_stage1_layer1, (uint8_t *)seal_sad_stage2_layer1},
	{(uint8_t *)seal_sad_stage2_layer1, (uint8_t *)seal_sad_stage3_layer1},
	{(uint8_t *)seal_sad_stage3_layer1, (uint8_t *)seal_happy_stage4_layer1},
	{(uint8_t *)seal_happy_stage4_layer1, (uint8_t *)seal_happy_stage5_layer1},
	{(uint8_t *)seal_happy_stage5_layer1, (uint8_t *)seal_happy_stage5_layer1},
	{(uint8_t *)tree_sad_stage1_layer1, (uint8_t *)tree_sad_stage2_layer1},
	{(uint8_t *)tree_sad_stage2_layer1, (uint8_t *)tree_sad_stage3_layer1},
	{(uint8_t *)tree_sad_stage3_layer1, (uint8_t *)tree_happy_stage4_layer1},
	{(uint8_t *)tree_happy_stage4_layer1, (uint8_t *)tree_happy_stage5_layer1},
	{(uint8_t *)tree_happy_stage5_layer1, (uint8_t *)tree_happy_stage5_layer1},
};

/****************************************************
 * Functions
 ****************************************************/

void right_shift_datas(unsigned char **datas, const Mapping *mappings) {
    for (int i = 0; i < 40; ++i) {
        if (*datas == mappings[i].left) {
            *datas = mappings[i].right;
            break;
        }
    }
}

void left_shift_datas(unsigned char **datas, const Mapping *mappings) {
	for (int i = 0; i < 40; ++i) {
		if (*datas == mappings[i].right) {
			*datas = mappings[i].left;
			break;
		}
	}
}

void increase_stage(unsigned char **datas, const Mapping *mappings) {
	for (int i = 0; i < 20; ++i) {
		if (*datas == mappings[i].left) {
			*datas = mappings[i].right;
			break;
		}
	}
}

/****************************************************
 * Tasks
 ****************************************************/

LOCAL void task_epaper(INT stacd, void *exinf);
LOCAL ID tskid_epaper;
LOCAL T_CTSK ctsk_epaper = {
		.itskpri = 10,
		.stksz = 8192,
		.task = task_epaper,
		.tskatr = TA_HLNG | TA_RNG3,
};

LOCAL void task_epaper(INT stacd, void *exinf) {
	while (1) {
		if(epaper_need_update){
			EPD_HW_Init();
			EPD_ALL_image(datas1, datas2);
			epaper_need_update = false;
		}
		tk_dly_tsk(500);
	}
}

LOCAL void task_uart(INT stacd, void *exinf);
LOCAL ID tskid_uart;
LOCAL T_CTSK ctsk_uart = {
		.itskpri = 10,
		.stksz = 8192,
		.task = task_uart,
		.tskatr = TA_HLNG | TA_RNG3,
};

LOCAL void task_uart(INT stacd, void *exinf) {
	static bool send_update_flag;
	while (1) {
		char buffer[100];
		if (rx_dma_done == 1) {
			rx_dma_done = 0;
		}

		cup_connected = rx_dma_uart_buffer[0];
		water_now = rx_dma_uart_buffer[1];
		if(cup_connected && water_last > water_now){
			water_total_drink += (uint32_t)(water_last - water_now);
		}
		if(water_total_drink / 255 > water_total_drink_level){
			water_total_drink_level++;
			if(water_total_drink_level > 5) water_total_drink_level = 5;
			increase_stage(&datas1, stage_switch_l1);
			increase_stage(&datas2, stage_switch_l0);
			send_update_flag = true;
		}
		if(send_update_flag && isEPD_W21_BUSY == EPD_W21_BUSY_LEVEL){
			epaper_need_update = true;
			send_update_flag = false;
		}
		water_last = water_now;

		sprintf(buffer, "cup_connected: %d, water_now: %d, water_last: %d, water_total_drink: %d, water_total_drink_level: %d\n", cup_connected, water_now, water_last, water_total_drink, water_total_drink_level);

		Cy_SCB_UART_PutString(UART_RECEIVER_HW, buffer);
		tk_dly_tsk(100);
	}
}


LOCAL void task_btn(INT stacd, void *exinf);
LOCAL ID tskid_btn;
LOCAL T_CTSK ctsk_btn = {
		.itskpri = 10,
		.stksz = 8192,
		.task = task_btn,
		.tskatr = TA_HLNG,
};

LOCAL void task_btn(INT stacd, void *exinf) {
	static bool send_update_flag;
	while (1) {
		static bool btn_1_have_pressed, btn_2_have_pressed;
		if(cyhal_gpio_read(CYBSP_USER_BTN1) == CYBSP_BTN_PRESSED){
			if (btn_1_have_pressed) {
				tk_dly_tsk(50);
				continue;
			}
			btn_1_have_pressed = true;
			right_shift_datas(&datas1, animal_switch_l1);
			right_shift_datas(&datas2, animal_switch_l0);
			send_update_flag = true;
		} else {
			btn_1_have_pressed = false;
		}
		if(cyhal_gpio_read(CYBSP_USER_BTN2) == CYBSP_BTN_PRESSED){
			if (btn_2_have_pressed) {
				tk_dly_tsk(50);
				continue;
			}
			btn_2_have_pressed = true;
			left_shift_datas(&datas1, animal_switch_l1);
			left_shift_datas(&datas2, animal_switch_l0);
			send_update_flag = true;
		} else {
			btn_2_have_pressed = false;
		}
		if(send_update_flag && isEPD_W21_BUSY == EPD_W21_BUSY_LEVEL){
			epaper_need_update = true;
			send_update_flag = false;
		}

		tk_dly_tsk(50);
	}
}

/****************************************************
 * Main
 ****************************************************/

EXPORT INT usermain(void) {
	datas1 = (uint8_t *)cat_sad_stage1_layer1;
	datas2 = (uint8_t *)cat_sad_stage1_layer0;
	water_total_drink_level = 0;

    EPD_HW_Init();
    EPD_ALL_image(datas1, datas2);

	tskid_uart = tk_cre_tsk(&ctsk_uart);
	tk_sta_tsk(tskid_uart, 0);

	tskid_epaper = tk_cre_tsk(&ctsk_epaper);
	tk_sta_tsk(tskid_epaper, 0);

	tskid_btn = tk_cre_tsk(&ctsk_btn);
	tk_sta_tsk(tskid_btn, 0);

	tk_slp_tsk(TMO_FEVR);
	return 0;
}

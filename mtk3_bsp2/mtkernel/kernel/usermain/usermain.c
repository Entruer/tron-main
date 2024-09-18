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
//#include "Epaper.h"
//#include "Matrix.h"

extern uint8_t rx_dma_uart_buffer[BUFFER_SIZE];
extern uint8_t rx_dma_done;

/*
 * Entry routine for the user application.
 * At this point, Initialize and start the user application.
 *
 * Entry routine is called from the initial task for Kernel,
 * so system call for stopping the task should not be issued 
 * from the contexts of entry routine.
 * We recommend that:
 * (1)'usermain()' only generates the user initial task.
 * (2)initialize and start the user application by the user
 * initial task.
 */

//LOCAL void task_epaper(INT stacd, void *exinf);
//LOCAL ID tskid_epaper;
//LOCAL T_CTSK ctsk_epaper = {
//		.itskpri = 10,
//		.stksz = 8192,
//		.task = task_epaper,
//		.tskatr = TA_HLNG | TA_RNG3,
//};
//
//LOCAL void task_epaper(INT stacd, void *exinf) {
//	while (1) {
//		EPD_ALL_image(cat_happy_stage1_layer0, cat_happy_stage1_layer1);
//		cyhal_gpio_toggle(LED1);
//		tk_dly_tsk(1000);
//	}
//}

LOCAL void task_uart(INT stacd, void *exinf);
LOCAL ID tskid_uart;
LOCAL T_CTSK ctsk_uart = {
		.itskpri = 10,
		.stksz = 8192,
		.task = task_uart,
		.tskatr = TA_HLNG | TA_RNG3,
};

LOCAL void task_uart(INT stacd, void *exinf) {
	while (1) {
		char buffer[100];
		if (rx_dma_done == 1) {
			rx_dma_done = 0;
		}
		sprintf(buffer, "Data: %d %d\n\r", rx_dma_uart_buffer[0],
				rx_dma_uart_buffer[1]);
		Cy_SCB_UART_PutString(UART_RECEIVER_HW, buffer);
		cyhal_gpio_toggle(LED2);
		tk_dly_tsk(100);
	}
}

EXPORT INT usermain(void) {
	EPD_HW_Init();

	tskid_uart = tk_cre_tsk(&ctsk_uart);
	tk_sta_tsk(tskid_uart, 0);

//	tskid_epaper = tk_cre_tsk(&ctsk_epaper);
//	tk_sta_tsk(tskid_epaper, 0);

	tk_slp_tsk(TMO_FEVR);
	return 0;
}

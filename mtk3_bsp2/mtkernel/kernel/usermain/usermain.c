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

#include "cyhal.h"
#include "cybsp.h"

extern cyhal_uart_t uart_receiver;

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

LOCAL void task_uart(INT stacd, void *exinf);
LOCAL ID	tskid_uart;
LOCAL T_CTSK ctsk_uart = {
	.itskpri	= 10,
	.stksz		= 8192,
	.task		= task_uart,
	.tskatr		= TA_HLNG | TA_RNG3,
};

LOCAL void task_uart(INT stacd, void *exinf)
{
	while(1) {
		uint8_t rx_buffer[2];
		uint8_t length = 2;
		if(cyhal_uart_read(&uart_receiver, rx_buffer, &length) == CY_RSLT_SUCCESS){
			tm_printf((UB*)"uart task: %d %d\n", rx_buffer[0], rx_buffer[1]);
		}

		cyhal_gpio_toggle(LED1);
		tk_dly_tsk(500);
	}
}

EXPORT INT usermain( void )
{
	tm_putstring((UB*)"Start User-main program.\n");

	tskid_uart = tk_cre_tsk(&ctsk_uart);
	tk_sta_tsk(tskid_uart, 0);

	tk_slp_tsk(TMO_FEVR);
	return 0;
}

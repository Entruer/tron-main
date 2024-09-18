/*******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the Empty Application Example
*              for ModusToolbox.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2021-2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cyhal.h"
#include "cybsp.h"
#include "UartDma.h"
#include "Epaper.h"
#include "Matrix.h"

/******************************************************************************
* Macros
*******************************************************************************/

/*******************************************************************************
* Global Variables
*******************************************************************************/

uint8_t rx_dma_uart_buffer[BUFFER_SIZE];
uint8_t rx_dma_error;
uint8_t tx_dma_error;
uint8_t uart_error;
uint8_t rx_dma_done;

cyhal_spi_t mSPI;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

void handle_error(void);
void Isr_UART(void);

/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*******************************************************************************
* Function Name: main
*********************************************************************************
* Summary:
* This is the main function for CPU. It...
*    1.
*    2.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    cy_en_scb_uart_status_t init_status;
    cy_stc_scb_uart_context_t UART_RECEIVER_context;

#if defined (CY_DEVICE_SECURE)
    cyhal_wdt_t wdt_obj;

    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    cy_stc_sysint_t UART_RECEIVER_INT_cfg =
	{
		.intrSrc = ( (NvicMux3_IRQn << 16) | UART_RECEIVER_IRQ ),
		.intrPriority = 7u,
	};

	cy_stc_sysint_t RX_DMA_INT_cfg =
	{
		.intrSrc      = ( (NvicMux3_IRQn << 16) | (IRQn_Type)RxDma_IRQ ),
		.intrPriority = 6u,
	};

	cy_stc_sysint_t TX_DMA_INT_cfg =
	{
		.intrSrc      = ( (NvicMux2_IRQn << 16) | (IRQn_Type)TxDma_IRQ ),
		.intrPriority = 6u,
	};

    cyhal_gpio_configure(LED1, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG);
    cyhal_gpio_configure(LED2, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG);
    cyhal_gpio_configure(LED3, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG);
    init_cycfg_pins();
    cyhal_gpio_configure(DC, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG);
    cyhal_gpio_configure(RST, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG);
    cyhal_gpio_configure(BUSY, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE);
    cyhal_gpio_write(LED1, 1);
    cyhal_gpio_write(LED2, 1);
    cyhal_gpio_write(LED3, 1);
    configure_rx_dma(rx_dma_uart_buffer, &RX_DMA_INT_cfg);
	configure_tx_dma(rx_dma_uart_buffer, &TX_DMA_INT_cfg);
	Cy_SysInt_Init(&UART_RECEIVER_INT_cfg, &Isr_UART);
	NVIC_EnableIRQ((IRQn_Type) NvicMux3_IRQn);
	cyhal_spi_init_cfg(&mSPI, &SPI_PAPER_hal_config);

	init_status = Cy_SCB_UART_Init(UART_RECEIVER_HW, &UART_RECEIVER_config, &UART_RECEIVER_context);
	if (init_status!=CY_SCB_UART_SUCCESS)
	{
		handle_error();
	}
	Cy_SCB_UART_Enable(UART_RECEIVER_HW);

    rx_dma_error = 0;
	tx_dma_error = 0;
	uart_error = 0;
	rx_dma_done = 0;

    /* Enable global interrupts */
    __enable_irq();

//    void knl_start_mtkernel(void);
//	knl_start_mtkernel();

    EPD_HW_Init();
    EPD_ALL_image(&cat_happy_stage2_layer1[0][0], &cat_happy_stage2_layer0[0][0]);
//    EPD_WhiteScreen_Black();

    for (;;)
    {
    	cyhal_gpio_toggle(LED1);
    	cyhal_system_delay_ms(500);
    }
}

void handle_error(void)
{
     /* Disable all interrupts. */
    __disable_irq();

    CY_ASSERT(0);
}


void Isr_UART(void)
{
    uint32_t intrSrcRx;
    uint32_t intrSrcTx;

    /* Get RX interrupt sources */
    intrSrcRx = Cy_SCB_UART_GetRxFifoStatus(UART_RECEIVER_HW);
    Cy_SCB_UART_ClearRxFifoStatus(UART_RECEIVER_HW, intrSrcRx);

    /* Get TX interrupt sources */
    intrSrcTx = Cy_SCB_UART_GetTxFifoStatus(UART_RECEIVER_HW);
    Cy_SCB_UART_ClearTxFifoStatus(UART_RECEIVER_HW, intrSrcTx);

    /* RX overflow or RX underflow or RX overflow occured */
    uart_error = 1;
}

/* [] END OF FILE */

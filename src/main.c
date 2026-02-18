/*==================================================================================================
*   Project              : RTD AUTOSAR 4.9
*   Platform             : CORTEXM
*   Peripheral           : ADC_SAR
*   Dependencies         : none
*
*   Autosar Version      : 4.9.0
*   Autosar Revision     : ASR_REL_4_9_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 7.0.0
*   Build Version        : S32K3_RTD_7_0_0_QLP03_D2512_ASR_REL_4_9_REV_0000_20251210
*
*   Copyright 2020 - 2025 NXP
*
*   NXP Proprietary. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/

/**
*   @file main.c
*
*   @addtogroup main_module main module documentation
*   @{
*/

/*==================================================================================================
*                                        INCLUDE FILES
* 1) System and project includes
* 2) Needed interfaces from external units
* 3) Internal and external interfaces from this unit
==================================================================================================*/
#include "App.h"

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/* Macro for NODE selection: Select ETH_TX or ETH_RX and flash its corresponding profile */
#define ETH_TX				(1U)
#define ETH_RX				(0U)

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/
extern Adc_ValueGroupType AdcResult;
extern volatile uint16 AdcReceiveResult;

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/
volatile uint32 Adc_CallbackCounter = 0U;

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/* Adc Callback */
void Adc_Callback (void);

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/* Adc Callback */
void Adc_Callback (void)
{
	Adc_CallbackCounter++;
}

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/


/**
* @brief        Transmit & receive 128-byte frames in RMII mode at 100 Mbps using an external ETH PHY
*               with a polling based mechanism. Read potentiometer using ADC, SW Triggered, One-Shot
*               Conversion Mode with an interrupt based mechanism.
* @details
*/
int main (void)
{
	/*===================================== Status Variables =====================================*/
	Std_ReturnType AppStatus = E_NOT_OK;

	/*====================================== Initialization ======================================*/
	AppStatus = App_Init ( );

	/*====================================== Application =========================================*/

	/* Infinite loop */
    while (TRUE)
    {
    	AppStatus = E_NOT_OK; 						/* Reset auxiliary variables */

#if ETH_TX
    	/* Adc Read - Evaluation Board's Potentiometer */
    	AppStatus = App_AdcRead ( );

    	if (E_OK == AppStatus)
    	{
    		AppStatus = E_NOT_OK; 					/* Reset auxiliary variables */

    		/* Compare Adc value */
    		AppStatus = App_AdcCompare (AdcResult);

    		AppStatus = E_NOT_OK; 					/* Reset auxiliary variables */

    		/* Eth Transmit */
    		AppStatus = App_EthTransmit (AdcResult);

    		AppStatus = E_NOT_OK; 					/* Reset auxiliary variables */
    	}

		Adc_CallbackCounter = 0;

    	for (uint32 indx = 0; indx < 5000000; indx++);
#endif

#if ETH_RX
		/* Eth Transmit */
		AppStatus = App_EthReceive ( );

    	if (E_OK == AppStatus)
    	{
    		AppStatus = E_NOT_OK; 					/* Reset auxiliary variables */

			/* Compare Adc value */
			AppStatus = App_AdcCompare (AdcReceiveResult);

			AppStatus = E_NOT_OK; 					/* Reset auxiliary variables */
    	}
#endif

    }

    App_DeInit ( );

    return 0;
}

#ifdef __cplusplus
}
#endif

/** @} */

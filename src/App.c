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
*   @file
*
*   @addtogroup APP Application Driver
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "App.h"

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/* Ethernet Frame = Destination Address + Source Address + Ethernet Type + Payload + CRC */

/* Ethernet Frame = 128 bytes
* - Destination Address = 6   bytes
* - Source Address      = 6   bytes
* - Ethernet Type Frame = 2   bytes
* - Payload             = 110 bytes
* - CRC                 = 4   bytes
*/
#define ETH_FRAME_LENGTH		((uint8)128U)
#define ETH_PAYLOAD_LENGTH		((uint8)110U)

/* Ethernet Type */
#define ETH_TYPE_FRAME          	((uint16)0x8888U)

/* Adc Threshold Values */
#define POT_VALUE_HIGH			((uint32)10900U)
#define POT_VALUE_LOW			((uint32)5450U)

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      LOCAL VARIABLES
==================================================================================================*/

/* Ethernet Destination MAC Address */
const uint8 EthDstMacAddr [ ] = {0x01U, 0x00U, 0x5EU, 0x7FU, 0x00U, 0x01U};

/* Ethernet PHY Configuration */
EthPhy_TestMode          EthMode       = ETH_RMII_100_MBPS;
EthPhy_Duplex            EthDuplex     = ETH_FULL_DUPLEX;
EthPhy_TestConfiguration EthConfig     = ETH_NO_LOOPBACK;
EthPhy_MasterMode        EthMasterMode = ETH_MASTER_MODE;

/* NOTE: Should match the Ethernet controller configuration in the .mex file */

/*==================================================================================================
*                                      GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      GLOBAL VARIABLES
==================================================================================================*/
extern volatile uint32 EthIf_TxConfirmations;
extern volatile uint32 Adc_CallbackCounter;

Adc_ValueGroupType AdcResult = 0U;

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
Std_ReturnType App_Init (void)
{
	/*====================================== Initialization ======================================*/

    /* Initialize the Osif driver */
    OsIf_Init (NULL_PTR);

    /* Initialize the Mcu driver */
    Mcu_Init (NULL_PTR);

#ifdef PHY_INIT_BY_SOFTWARE

    /* ETH PHY hardware reset */
    EthPhy_PhyHwReset ( );

#endif

    /* Initialize all pins using the Port driver */
    Port_Init (NULL_PTR);

#ifdef PHY_INIT_BY_PIN_STRAPPING

    /* Initialize ETH PHY via pin strapping */
    EthPhy_PhyPinStrapping ( );

#endif

    /* Initialize the clock tree and apply PLL as system clock */
    Mcu_InitClock (McuClockSettingConfig_0);

#if (MCU_NO_PLL == STD_OFF)

    while (MCU_PLL_LOCKED != Mcu_GetPllStatus ( ))
    {
    	/* Busy wait until the System PLL is locked */
    }

    /* Activate PLL clock */
    Mcu_DistributePllClock ( );

#endif

    /* Initialize the Power driver. Set RUN Mode */
    Mcu_SetMode (McuModeSettingConf_0);

    /* Initialize Interrupts using Platform driver */
    Platform_Init (NULL_PTR);

    /* Initialize the Adc driver */
    Adc_Init (NULL_PTR);

    /* AdcResult is updated new data in Adc_Sar_0_Isr Handler */
    Adc_SetupResultBuffer (AdcGroup_0, &AdcResult);
    Adc_EnableGroupNotification (AdcGroup_0);

    /* Initialize the Gmac driver */
    Eth_43_GMAC_Init (NULL_PTR);

#ifdef PHY_INIT_BY_SOFTWARE

    /* Initialize ETH PHY via software */
    EthPhy_PhyInit (EthConf_EthCtrlConfig_EthCtrlConfig_0, EthConfig, EthMode, EthDuplex, EthMasterMode);

#endif

    /* Start the Gmac controller */
    Eth_43_GMAC_SetControllerMode (EthConf_EthCtrlConfig_EthCtrlConfig_0, ETH_MODE_ACTIVE);

    /* Initialization successful */
    return E_OK;

}

/*================================================================================================*/
Std_ReturnType App_DeInit (void)
{
    /* Disable the Adc driver */
    Adc_DeInit ( );

    /* Stop the Gmac controller */
    Eth_43_GMAC_SetControllerMode (EthConf_EthCtrlConfig_EthCtrlConfig_0, ETH_MODE_DOWN);

    /* De-Initialization successful */
    return E_OK;
}

/*================================================================================================*/
Std_ReturnType App_AdcRead (void)
{
	/*==================================== Auxiliary Variables ===================================*/
	sint32 Timeout = 0xFFFFFF;
	Adc_ValueGroupType AdcReadGroupResult = 0U;

	/*===================================== Status Variables =====================================*/
	Std_ReturnType AdcStatus = E_NOT_OK;

	/*====================================== Application =========================================*/
	do
	{
		/* Start channel. Wait for complete conversion */
		Adc_StartGroupConversion (AdcGroup_0);
	}
	while (0U == Adc_CallbackCounter && 0 < Timeout--);

	/* Get channel's conversion result */
	AdcStatus = Adc_ReadGroup (AdcGroup_0, &AdcReadGroupResult);

    /* Adc status */
    return AdcStatus;
}

/*================================================================================================*/
Std_ReturnType App_AdcCompare (uint16 AdcValue)
{
	/*===================================== Status Variables =====================================*/
	Std_ReturnType AdcStatus = E_NOT_OK;

	/*====================================== Application =========================================*/

	/* Check if AdcValue is less or equal than POT_VALUE_LOW */
	if (AdcValue <= POT_VALUE_LOW)
	{
		/* Turn-on RGB BLUE LED */
		Dio_WriteChannel (DioConf_DioChannel_RED_LED_PIN, STD_LOW);
		Dio_WriteChannel (DioConf_DioChannel_GREEN_LED_PIN, STD_LOW);
		Dio_WriteChannel (DioConf_DioChannel_BLUE_LED_PIN, STD_HIGH);
	}

	/* Check if AdcValue is less or equal than POT_VALUE_HIGH */
	else if (AdcValue <= POT_VALUE_HIGH)
	{
		/* Turn-on RGB GREEN LED */
		Dio_WriteChannel (DioConf_DioChannel_RED_LED_PIN, STD_LOW);
		Dio_WriteChannel (DioConf_DioChannel_GREEN_LED_PIN, STD_HIGH);
		Dio_WriteChannel (DioConf_DioChannel_BLUE_LED_PIN, STD_LOW);
	}

	/* Check if AdcValue is greater than POT_VALUE_HIGH */
	else
	{
		/* Turn-on RGB RED LED */
		Dio_WriteChannel (DioConf_DioChannel_RED_LED_PIN, STD_HIGH);
		Dio_WriteChannel (DioConf_DioChannel_GREEN_LED_PIN, STD_LOW);
		Dio_WriteChannel (DioConf_DioChannel_BLUE_LED_PIN, STD_LOW);
	}

    /* Adc status */
    return AdcStatus;
}

/*================================================================================================*/
Std_ReturnType App_EthTransmit (uint16 EthData)
{
	/*==================================== Auxiliary Variables ===================================*/
	sint32 Timeout = 0xFFFFFF;
	static uint8 SequenceId     = 0U;
	uint16 DesiredPayloadLength = ETH_PAYLOAD_LENGTH;

	uint8 *PayloadBuffer;
	Eth_BufIdxType BufferIndex;



	/*===================================== Status Variables =====================================*/
	Std_ReturnType GmacStatus = E_NOT_OK;

	/*====================================== Application =========================================*/

	/* Look for an available buffer */
	if ( BUFREQ_OK == Eth_43_GMAC_ProvideTxBuffer (EthConf_EthCtrlConfig_EthCtrlConfig_0, 0U,
			&BufferIndex, &PayloadBuffer, &DesiredPayloadLength) )
	{
		/* Fill payload */
		PayloadBuffer[0] = SequenceId;
		PayloadBuffer[1] = EthData >> 8U;
		PayloadBuffer[2] = EthData;

		/* Transmit frame */
		GmacStatus = Eth_43_GMAC_Transmit (EthConf_EthCtrlConfig_EthCtrlConfig_0, BufferIndex,
				(Eth_FrameType)ETH_TYPE_FRAME, TRUE, ETH_PAYLOAD_LENGTH, EthDstMacAddr);


		if (E_OK == GmacStatus)
		{
			do
			{
				/* Get transmission confirmation */
				Eth_43_GMAC_TxConfirmation (EthConf_EthCtrlConfig_EthCtrlConfig_0);
			}
			while (0U == EthIf_TxConfirmations && 0 < Timeout--);

			/* Increment sequence ID */
			SequenceId++;
		}

    	/* Reset auxiliary variables */
    	EthIf_TxConfirmations = 0U;
	}

	/* Restart Sequence ID */
	if (255U == SequenceId)
	{
		SequenceId = 0U;
	}

    /* Gmac status */
	return GmacStatus;
}

/*================================================================================================*/
Std_ReturnType App_EthReceive (void)
{
	/*==================================== Auxiliary Variables ===================================*/
	sint32 Timeout = 0xFFFFFF;

	/*===================================== Status Variables =====================================*/
	Std_ReturnType   GmacStatus = E_NOT_OK;
	Eth_RxStatusType GmacReceiveStatus = ETH_NOT_RECEIVED;

	/*====================================== Application =========================================*/

	do
	{
		/* Read packet */
		Eth_43_GMAC_Receive (EthConf_EthCtrlConfig_EthCtrlConfig_0, 0U, &GmacReceiveStatus);
	}
	while (ETH_RECEIVED != GmacReceiveStatus && 0 < Timeout--);

	if (ETH_RECEIVED == GmacReceiveStatus)
	{
		GmacStatus = E_OK;				/* Receive successful */
	}

	else
	{
		GmacStatus = E_NOT_OK; 			/* Receive not successful */
	}

    /* Gmac status */
	return GmacStatus;
}

#ifdef __cplusplus
}
#endif

/** @} */

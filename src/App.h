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

/*   @implements App.h_Artifact */
#ifndef APP_H_
#define APP_H_

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
*                          	        AUTHOR AND INSTRUCTIONS
==================================================================================================*/
/*
* Author: Alejandro Flores Triana (alejandro.flores@nxp.com)
*
*/

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Adc.h"
#include "Mcu.h"
#include "Dio.h"
#include "Port.h"
#include "OsIf.h"
#include "EthPhy.h"
#include "Platform.h"
#include "Eth_43_GMAC.h"

/*==================================================================================================
*                                          CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/* Macro for ETH PHY initialization: Select PHY_INIT_BY_PIN_STRAPPING or PHY_INIT_BY_SOFTWARE */
#define PHY_INIT_BY_PIN_STRAPPING
//#define PHY_INIT_BY_SOFTWARE

/*==================================================================================================
*                                            ENUMS
==================================================================================================*/

/*==================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/

/**
* @brief         Returns the initialization status of the Application.
*
* @api
* @return        Error status
* @retval        E_OK: Application initialization successful.
* @retval        E_NOT_OK: Application initialization not successful.
* implements     App_Init
*/
Std_ReturnType App_Init (void);

/**
* @brief         Returns the de-initialization status of the Application.
*
* @api
* @return        Error status
* @retval        E_OK: Application de-initialization successful.
* @retval        E_NOT_OK: Application de-initialization not successful.
* implements     App_DeInit
*/
Std_ReturnType App_DeInit (void);

/**
* @brief         Returns the reading status of the Adc.
*
* @api
* @return        Error status
* @retval        E_OK: Adc reading successful.
* @retval        E_NOT_OK: Adc reading not successful.
* implements     App_AdcRead
*/
Std_ReturnType App_AdcRead (void);

/**
* @brief         Returns the comparison status of the Adc based on a defined threshold.
*
* @api
* @param[in]     AdcValue Data to be compared.
* @return        Error status
* @retval        E_OK: Adc comparison successful.
* @retval        E_NOT_OK: Adc comparison not successful.
* implements     App_AdcCompare
*/
Std_ReturnType App_AdcCompare (uint16 AdcValue);

/**
* @brief         Returns the transmit status of the Gmac.
*
* @api
* @param[in]     EthData Data to be transmitted.
* @return        Error status
* @retval        E_OK: Gmac transmission successful.
* @retval        E_NOT_OK: Gmac transmission not successful.
* implements     App_EthTransmit
*/
Std_ReturnType App_EthTransmit (uint16 EthData);

/**
* @brief         Returns the receive status of the Gmac.
*
* @api
* @return        Error status
* @retval        E_OK: Gmac receive successful.
* @retval        E_NOT_OK: Gmac receive not successful.
* implements     App_EthReceive
*/
Std_ReturnType App_EthReceive (void);


/** @} */

#endif /* APP_H_ */

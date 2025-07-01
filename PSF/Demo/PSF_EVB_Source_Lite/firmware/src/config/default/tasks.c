/*******************************************************************************
 System Tasks File

  File Name:
    tasks.c

  Summary:
    This file contains source code necessary to maintain system's polled tasks.

  Description:
    This file contains source code necessary to maintain system's polled tasks.
    It implements the "SYS_Tasks" function that calls the individual "Tasks"
    functions for all polled MPLAB Harmony modules in the system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    polled in the system.  These handles are passed into the individual module
    "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) [2022] Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"


typedef enum
{
    APP_STATE_WAIT_FOR_CONTRACT,
    APP_STATE_CONTRACT_ESTABLISHED,
    APP_STATE_CONTRACT_UNESTABLISHED
} APP_STATES;

static APP_STATES appState = APP_STATE_WAIT_FOR_CONTRACT;
static UINT8 printOnceEstablished = 0;
static UINT8 printOnceUnestablished = 0;

void Explicit_Contract_Checker(void)
{
    switch (appState)
    {
        case APP_STATE_WAIT_FOR_CONTRACT:
            if (PE_GET_PD_CONTRACT(1))
            {
                appState = APP_STATE_CONTRACT_ESTABLISHED;
            } else {
                appState = APP_STATE_CONTRACT_UNESTABLISHED;
            }
            break;
            
        case APP_STATE_CONTRACT_ESTABLISHED:
            // static uint32_t lastTick = 0;
            if (!printOnceEstablished)
            {
                DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum," >>>>>>>>>> WenXian : EXPLICIT_CONTRACT_ESTABLISHED  <<<<<<<<<< \r\n");
                printOnceEstablished = 1;
                
                // DPM_RegisterInternalEvent (1, DPM_INT_EVT_INITIATE_VDEM);
                // gasCfgStatusData.sPerPortData[1].u32ClientRequest |= DPM_CLIENT_REQ_INITIATE_VDEM;
            }
            
            break;
        
        case APP_STATE_CONTRACT_UNESTABLISHED:
            if (!printOnceUnestablished)
            {
                DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum," >>>>>>>>>> WenXian : APP_STATE_CONTRACT_UNESTABLISHED  <<<<<<<<<< \r\n");
                printOnceUnestablished = 1;
            }
            appState = APP_STATE_WAIT_FOR_CONTRACT;
            break;
            
        default:
            break;
    }
}



// *****************************************************************************
// *****************************************************************************
// Section: System "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SYS_Tasks ( void )

  Remarks:
    See prototype in system/common/sys_module.h.
*/
void SYS_Tasks ( void )
{
    /* Maintain system services */
    

    /* Maintain Device Drivers */
    

    /* Maintain Middleware & Other Libraries */
    static UINT8 isInitDone = SET_TO_ZERO;
    
if(0 == isInitDone){    

    /* Disabled UPD IRQ Pins Initially*/
    EIC_InterruptDisable((EIC_PIN)PORT_PIN_PA14);
    EIC_InterruptDisable((EIC_PIN)PORT_PIN_PA15);

    /*PSF init called*/
    (void)MchpPSF_Init();
    isInitDone = SET_TO_ONE;
}

    /*PSF stack Run*/
    MchpPSF_RUN();

    // WenXian
    Explicit_Contract_Checker();

}

/*******************************************************************************
 End of File
 */


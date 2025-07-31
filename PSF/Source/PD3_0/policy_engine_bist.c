#include <psf_stdinc.h>

#if (TRUE == USB_BIST_DEVICE_TEST_DATA)
UINT8 send_count = SET_TO_ZERO;
UINT8 n = 2;
#endif

void PE_RunBistStateMachine (UINT8 u8PortNum)
{
	/* Transmit Message Type - SOP SOP' SOP" */
    // UINT8 u8TransmitSOP = PRL_SOP_TYPE;
    
	/* Transmit Message Header */
	UINT32 u32TransmitHeader = SET_TO_ZERO;

	/* Transmit Data Object */
	UINT32 *u32pTransmitDataObj = SET_TO_ZERO; 

	/* Transmit Call back */
	PRLTxCallback pfnTransmitCB = PE_StateChange_TransmitCB;

	/* Transmit Call back variables */
	UINT32 u32TransmitTmrIDTxSt = SET_TO_ZERO;

	/* Transmit Flag */
	UINT8 u8IsTransmit = FALSE;
    
    // ePolicyState eTxDoneSt;
    // ePolicySubState eTxDoneSS;
        
    // if (PD_ROLE_SOURCE == DPM_GET_CURRENT_POWER_ROLE(u8PortNum))
    // {
    //     eTxDoneSt = ePE_SRC_READY;
    //     eTxDoneSS = ePE_SRC_READY_END_AMS_SS;
    // }
    // else
    // {
    //     eTxDoneSt = ePE_SNK_READY;
    //     eTxDoneSS = ePE_SNK_READY_END_AMS_SS;
    // }        

    // UINT8 u8CurrentDataRole = DPM_GET_CURRENT_DATA_ROLE(u8PortNum);
    
    switch (gasPolicyEngine[u8PortNum].ePEState)
    {
        case ePE_BIST_CARRIER_MODE:
        {
            switch (gasPolicyEngine[u8PortNum].ePESubState)
            {
                case ePE_BIST_CARRIER_MODE_ENTRY_SS:
                {
                    
                    DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_BIST_CARRIER_MODE_ENTRY_SS\r\n");
                    
                    u32TransmitHeader = PRL_FormSOPTypeMsgHeader (u8PortNum, PE_DATA_BIST, PE_OBJECT_COUNT_1, PE_NON_EXTENDED_MSG);
            
                    /*The Protocol Layer indicates that the Message has not been sent , so
                    Send Soft reset since PD connected presently*/
                    u32TransmitTmrIDTxSt = PRL_BUILD_PKD_TXST_U32( ePE_BIST_CARRIER_MODE, \
                                           ePE_BIST_CARRIER_MODE_GOODCRC_RCVD_SS, \
                                           ePE_SEND_SOFT_RESET, ePE_SEND_SOFT_RESET_ENTRY_SS);   
                    
                    
                    // BIST Carrier Mode 
                    // *u32pTransmitDataObj = 1342177280;
                    UINT32 data = 0;
                    data |= (0x5 << 28);
                    
                    // See issues with 
                    // 1. *u32pTransmitDataObj = <some_value>
                    // 2. *u32pTransmitDataObj = SET_TO_ZERO
                    u32pTransmitDataObj = &data;
                    
                    u8IsTransmit = TRUE;
            
                    if (u8IsTransmit)
                    {
                        (void) PRL_TransmitMsg (u8PortNum, (UINT8) PRL_SOP_TYPE, u32TransmitHeader, \
                            (UINT8 *)u32pTransmitDataObj, pfnTransmitCB, u32TransmitTmrIDTxSt); 
                    }
            
                    gasPolicyEngine[u8PortNum].ePESubState = ePE_BIST_CARRIER_MODE_IDLE_SS;
                                        
                    break;                    
                }
                
                case ePE_BIST_CARRIER_MODE_IDLE_SS:
                {
                    // DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"ePE_BIST_CARRIER_MODE_IDLE_SS\r\n");
                    
                    break;
                }
                
                case ePE_BIST_CARRIER_MODE_GOODCRC_RCVD_SS:
                {
                    DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_BIST_CARRIER_MODE_GOODCRC_RCVD_SS\r\n");
                    
                    ///* Reset CapsCounter and HardReset Counter to 0 */
                    //gasPolicyEngine[u8PortNum].u8CapsCounter = RESET_TO_ZERO;
                    //gasPolicyEngine[u8PortNum].u8HardResetCounter = RESET_TO_ZERO;  

                    ///* Start Sender Response timer and Set the timer callback to transition to 
					//ePE_SRC_HARD_RESET sate and ePE_SRC_HARD_RESET_ENTRY_SS sub state if timeout happens */
                    //gasPolicyEngine[u8PortNum].u8PETimerID = PDTimer_Start (
                    //                                        PE_SENDERRESPONSE_TIMEOUT_MS,
                    //                                        PE_SSChngAndTimeoutValidate_TimerCB, u8PortNum,  
                    //                                        (UINT8)ePE_SRC_HARD_RESET_ENTRY_SS);
                    
                    break;
                }
                
                default:
                {
                    break;
                }
            }
                
        }
        
        
        
#if (TRUE == USB_BIST_DEVICE_TEST_DATA)
        case ePE_BIST_TEST_DATA:
        {
            switch (gasPolicyEngine[u8PortNum].ePESubState)
            {
                case ePE_BIST_TEST_DATA_ENTRY_SS:
                {
                                        
                    DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_BIST_TEST_DATA_ENTRY_SS\r\n");
                    
                    u32TransmitHeader = PRL_FormSOPTypeMsgHeader (u8PortNum, PE_DATA_BIST, 7, PE_NON_EXTENDED_MSG);
            
                    /*The Protocol Layer indicates that the Message has not been sent , so Send Soft reset since PD connected presently*/
                    u32TransmitTmrIDTxSt = PRL_BUILD_PKD_TXST_U32( ePE_BIST_TEST_DATA, \
                                         ePE_BIST_TEST_DATA_GOODCRC_RCVD_SS, \
                                         ePE_SEND_SOFT_RESET, ePE_SEND_SOFT_RESET_ENTRY_SS);                

                    // BIST Carrier Mode 
                    // *u32pTransmitDataObj = 1342177280;
                    UINT32 data[7] = {
                        0x80000000,  // First 32 bits: bit 31 set (0x1 << 31)
                        0x55555555,  // Next 32 bits: alternating pattern 01010101...
                        0xAAAAAAAA,  // Next 32 bits: alternating pattern 10101010...
                        0x55555555,  // Next 32 bits: alternating pattern 01010101...
                        0xAAAAAAAA,  // Next 32 bits: alternating pattern 10101010...
                        0x55555555,  // Next 32 bits: alternating pattern 01010101...
                        0xAAAAAAAA   // Last 32 bits: alternating pattern 10101010...                        
                    };
                    // data |= (0x1 << 31);
                    
                    // See issues with 
                    // 1. *u32pTransmitDataObj = <some_value>
                    // 2. *u32pTransmitDataObj = SET_TO_ZERO
                    u32pTransmitDataObj = data;
                    
                    u8IsTransmit = TRUE;
            
                    if (u8IsTransmit)
                    {
                        (void) PRL_TransmitMsg (u8PortNum, (UINT8) PRL_SOP_TYPE, u32TransmitHeader, \
                            (UINT8 *)u32pTransmitDataObj, pfnTransmitCB, u32TransmitTmrIDTxSt); 
                    }
                    
                    send_count++;
            
                    gasPolicyEngine[u8PortNum].ePESubState = ePE_BIST_CARRIER_MODE_IDLE_SS;

                    break;
                }
                
                // Second round of BIST Test Data Transmition
                case ePE_BIST_TEST_DATA_RESEND_SS:
                {
                    DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_BIST_TEST_DATA_ENTRY_SS\r\n");
                    
                    u32TransmitHeader = PRL_FormSOPTypeMsgHeader (u8PortNum, PE_DATA_BIST, 7, PE_NON_EXTENDED_MSG);
            
                    /*The Protocol Layer indicates that the Message has not been sent , so Send Soft reset since PD connected presently*/
                    u32TransmitTmrIDTxSt = PRL_BUILD_PKD_TXST_U32( ePE_BIST_TEST_DATA, \
                                         ePE_BIST_TEST_DATA_GOODCRC_RCVD_SS, \
                                         ePE_SEND_SOFT_RESET, ePE_SEND_SOFT_RESET_ENTRY_SS);                
                    
                                        //gasPolicyEngine[u8PortNum].u8PETimerID = PDTimer_Start (
                                        //                    PE_SENDERRESPONSE_TIMEOUT_MS,
                                        //                    PE_SubStateChange_TimerCB, u8PortNum,  
                                        //                    (UINT8)ePE_GET_SINK_CAP_NO_RESPONSE_SS); 
                    
                    
                    // BIST Carrier Mode 
                    // *u32pTransmitDataObj = 1342177280;
                    UINT32 data[7] = {
                        0x80000000,  // First 32 bits: bit 31 set (0x1 << 31)
                        0x55555555,  // Next 32 bits: alternating pattern 01010101...
                        0xAAAAAAAA,  // Next 32 bits: alternating pattern 10101010...
                        0x55555555,  // Next 32 bits: alternating pattern 01010101...
                        0xAAAAAAAA,  // Next 32 bits: alternating pattern 10101010...
                        0x55555555,  // Next 32 bits: alternating pattern 01010101...
                        0xAAAAAAAA   // Last 32 bits: alternating pattern 10101010...                        
                    };
                    // data |= (0x1 << 31);
                    
                    // See issues with 
                    // 1. *u32pTransmitDataObj = <some_value>
                    // 2. *u32pTransmitDataObj = SET_TO_ZERO
                    u32pTransmitDataObj = data;
                    
                    u8IsTransmit = TRUE;
            
                    if (u8IsTransmit)
                    {
                        (void) PRL_TransmitMsg (u8PortNum, (UINT8) PRL_SOP_TYPE, u32TransmitHeader, \
                            (UINT8 *)u32pTransmitDataObj, pfnTransmitCB, u32TransmitTmrIDTxSt); 
                    }
                    
                    send_count++;
            
                    gasPolicyEngine[u8PortNum].ePESubState = ePE_BIST_TEST_DATA_IDLE_SS;
                    
                    break;
                }
                
                case ePE_BIST_TEST_DATA_SEND_HARD_RESET_SS:
                {
                    DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_BIST_TEST_DATA_SEND_HARD_RESET_SS\r\n");
                    
                    PE_SendHardReset (u8PortNum);
                    break;
                }
                
                case ePE_BIST_TEST_DATA_GOODCRC_RCVD_SS:
                {
                    DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_BIST_TEST_DATA_GOODCRC_RCVD_SS\r\n");
                    
                    // Wait for 10ms
                    PDTimer_WaitforTicks(MILLISECONDS_TO_TICKS(10));
                    
                    // send_count increment every TEST_DATA entry with n = 2
                    if (send_count < n){
                        // First BIST_TEST_DATA Sent
                        DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_BIST_TEST_DATA_RESEND\r\n");
                        gasPolicyEngine[u8PortNum].ePESubState = ePE_BIST_TEST_DATA_ENTRY_SS;                        
                    } else{
                        DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_BIST_TEST_DATA_SEND_HARD_RESET_SS\r\n");
                        gasPolicyEngine[u8PortNum].ePESubState = ePE_BIST_TEST_DATA_SEND_HARD_RESET_SS;
                    }
                    
                    break;
                }
                
                case ePE_BIST_TEST_DATA_IDLE_SS:
                {
                    // DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_BIST_TEST_DATA_IDLE_SS\r\n");
                    
                    break;
                }
                
                default:
                {
                    break;
                }
                
            }
        }
#endif
        
        default:
        {
            break;
        }
                
    }

    
}
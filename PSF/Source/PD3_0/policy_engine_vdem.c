#include <psf_stdinc.h> 

#if (TRUE == INCLUDE_PD_VDEM)

/* Msg_Header[4:0] - Message Type */
#define PE_EXT_VDEM 0x00
/* MaxExtendedMsgLen = 260 */
#define MAX_VDEM_DATA_SIZE 260

/* Get the VDM Header and VDOs configured by the application */
#define VID_POS 	16
#define VDM_TYPE_POS 	15
#define VENDOR_USE_POS 	0

#define VID_MASK 	0xFFFF
#define VDM_TYPE_MASK 	0x1
#define VENDOR_USE_MASK 0x7FFF

#define VID 		0x1234
#define VDM_TYPE	0
#define VENDOR_DATA	0x5678				

void PE_RunVDEMStateMachine(UINT8 u8PortNum)
{
        /* VDM Data Object Array */
    	UINT32 u32aVDMDataObj[PRL_MAX_EXTN_MSG_LEN_IN_BYTES] = {SET_TO_ZERO};
	
    	/* VDM Data Object Count */
    	// UINT8 u8VDOCnt = SET_TO_ZERO;	// Set to 6 to mimic fwud

    	// Transmit Header and Data Pointer
    	// UINT32 u32TransmitHeader = SET_TO_ZERO;
        UINT16 u16Message_Header = SET_TO_ZERO;
        UINT16 u16Extended_Header = SET_TO_ZERO;
        // UINT32 pu32Msg_Header = SET_TO_ZERO;
        UINT32 u32TransmitHeader = SET_TO_ZERO;

    	/* Transmit Data Object */
    	UINT32 *u32pTransmitDataObj = SET_TO_ZERO;

    	/* Callback function? */
    	/* Definiation of PE_StateChange_TransmitCB can be found at line 2462 in policy_engine.c */
	/* Need to find the relationship between PE_StateChange_TransmitCB and PRL_TxOriginalCBfromCH (UINT8 u8PortNum, UINT8 u8TxStateforCB) in line 1513 protocol_layer.c */
	/* Inside the Chunk State Machine there is PRL_TCHChunkSMStateChange_TCHCB */
    	PRLTxCallback pfnTransmitCB = PE_StateChange_TransmitCB;
    
    	/* Transmit Timer ID for Tx State */
    	UINT32 u32TransmitTmrIDTxSt = 0;
    
    	/* Transmit flag */
    	UINT8 u8IsTransmit = FALSE;

    	ePolicyState eTxDoneSt;
    	ePolicySubState eTxDoneSS;

    	if (PD_ROLE_SOURCE == DPM_GET_CURRENT_POWER_ROLE(u8PortNum))
    	{
        	eTxDoneSt = ePE_SRC_READY;
		/*  Goto DPM_InitiateInternalEvts (u0PortNum) to proceed with remaining internal events */
        	eTxDoneSS = ePE_SRC_READY_END_AMS_SS;
    	}
    	else
    	{
        	eTxDoneSt = ePE_SNK_READY;
        	eTxDoneSS = ePE_SNK_READY_END_AMS_SS;
    	} 

    	switch (gasPolicyEngine[u8PortNum].ePEState)
    	{
            case ePE_VDEM_INITIATE_VDEM:
            {
                switch (gasPolicyEngine[u8PortNum].ePESubState)
                {
                    /* PE enter this sub-state when sending a VDEM request initiated by application */
                    case ePE_VDEM_INITIATE_VDEM_ENTRY_SS:
                    {
                        DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_VDEM_INITIATE_VDEM_ENTRY_SS\r\n");

              			/* Form VDM Header or use CFG_FORM_VDM_HEADER(svid,vdmType,svdmVersion,objPos,cmdType,cmd) */
                        u32aVDMDataObj[INDEX_0] = ((UINT32) (VID & VID_MASK) << VID_POS) | 
                                                  ((UINT32) (VDM_TYPE & VDM_TYPE_POS) << VDM_TYPE_POS) | 
                                                  ((UINT32) (VENDOR_DATA & VENDOR_USE_MASK) << VENDOR_USE_POS);

                        // u32aVDMDataObj[INDEX_0] = gasCfgStatusData.sVDMPerPortData[u8PortNum].u32VDMHeader;
                        u32pTransmitDataObj = u32aVDMDataObj;
				
                        /* Refer pg 116 */
                        /* Number of Data Objects indicate the number of 32-bit Data Objects that follow the Message Header~ */
                        /* ~ When both the Extended bit and chunked bit are set to one, the Number of Data Objects field shall indicate the number of Data Objects in the Message */
                        /* Object Count is incremented by 1 to include VDM Header.  Extended Message Header need to consider? */
                        /* PE_EXTENDED_MSG */
                        // u32TransmitHeader = PRL_FormSOPTypeMsgHeader (u8PortNum, (UINT8)PE_DATA_VENDOR_DEFINED, (u8VDOCnt + BYTE_LEN_1), PE_EXTENDED_MSG);

                        /* Refer to policy_engine_fwup.c */
                        u16Message_Header = PRL_FormSOPTypeMsgHeader (u8PortNum, PE_EXT_FW_UPDATE_RESPONSE, 7, PE_EXTENDED_MSG);
                        // u16Extended_Header = (1u << PRL_EXTMSG_CHUNKED_BIT_POS) | (PRL_EXTMSG_DATA_FIELD_MASK & gsPdfuInfo.u16PDFUResponseLength);
                        u16Extended_Header = (1u << PRL_EXTMSG_CHUNKED_BIT_POS) | 
                                             (PRL_EXTMSG_DATA_FIELD_MASK & PRL_MAX_EXTN_MSG_LEN_IN_BYTES);
                        // pu32Msg_Header = PRL_FORM_COMBINED_MSG_HEADER(u16Extended_Header, u16Message_Header);
                        u32TransmitHeader = PRL_FORM_COMBINED_MSG_HEADER(u16Extended_Header, u16Message_Header);

                
              			/* void PE_StateChange_TransmitCB (UINT8 u8PortNum, UINT8 u8TXDoneState, UINT8 u8TxDoneSubState, UINT8 u8TxFailedState, UINT8 u8TxFailedSubState) */
              			/* if gasPRL[u8PortNum].u8TxStateISR == PRL_TX_DONE_ST, set gasPolicyEngine[u8PortNum].ePEState = (ePolicyState) u8TXDoneState */
              			/*  To review hw to handle this for chunked messages */
              			u32TransmitTmrIDTxSt = PRL_BUILD_PKD_TXST_U32( ePE_VDEM_INITIATE_VDEM, ePE_VDEM_INITIATE_VDEM_MSG_DONE_SS, eTxDoneSt, eTxDoneSS);

                        u8IsTransmit = TRUE;

                        /* Move PE to an idle state to wait for Good CRC reception */
              			gasPolicyEngine[u8PortNum].ePESubState = ePE_VDM_INITIATE_VDM_IDLE_SS;                                       
                    
              			break;
                    }

                    /* Enter this state once chunked message is transmitted */
                    case ePE_VDEM_INITIATE_VDEM_MSG_DONE_SS:
                    {
                        DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_VDEM_INITIATE_VDEM_MSG_DONE_SS\r\n");

                        gasPolicyEngine [u8PortNum].ePEState = eTxDoneSt;
                        gasPolicyEngine [u8PortNum].ePESubState = eTxDoneSS;

                        break;
				
                    }

                    case ePE_VDEM_INITIATE_VDEM_IDLE_SS:
                    {
                        DEBUG_PRINT_PORT_STR (PSF_PE_LAYER_DEBUG_MSG,u8PortNum,"PE_VDM_INITIATE_VDEM_IDLE_SS\r\n");
                        /* Hook to notify PE state machine entry into idle sub-state - Not defined */
                        MCHP_PSF_HOOK_NOTIFY_IDLE(u8PortNum, eIDLE_PE_NOTIFY); 
                
                    break;
                    }
			
                    default:
                    {
                        break;
                    }
                }
            }
            /* Default case required to avoid enumeration value not handle in switch during during compilation */
            default:
            {
                break;
            }
        }

	/* Transmit the message if u8IsTransmit is set */
    if (u8IsTransmit)
    {
		(void) PRL_TransmitMsg (u8PortNum, (UINT8) PRL_SOP_TYPE, u32TransmitHeader, (UINT8 *)u32pTransmitDataObj, pfnTransmitCB, u32TransmitTmrIDTxSt); 
    }
}

#endif /* INCLUDE_PD_VDEM */
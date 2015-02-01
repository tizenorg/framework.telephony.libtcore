/*
 * libtcore
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Ja-young Gu <jygu@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "user_request.h"
#include "co_sms.h"

struct private_object_data {
	struct tcore_sms_operations *ops;
	gboolean b_readyStatus;
};

int _tcore_util_sms_encode_smsParameters(const struct telephony_sms_Params *incoming, unsigned char *data, int SMSPRecordLen)
{
	struct telephony_sms_Params *smsParams =  (struct telephony_sms_Params *)incoming;
	unsigned int nPIDIndex = 0;
	unsigned char nOffset = 0;

	if(incoming == NULL || data == NULL)
		return FALSE;

	memset(data, 0xff, SMSPRecordLen);//pSmsParam->RecordLen);

	dbg(" Record index = %d", (int) smsParams->recordIndex);
	dbg(" Alpha ID Len = %d", (int) smsParams->alphaIdLen);
	dbg(" Record Length : %d", SMSPRecordLen);//pSmsParam->RecordLen);

	if (SMSPRecordLen/*pSmsParam->RecordLen*/>= nDefaultSMSPWithoutAlphaId) {
		nPIDIndex = SMSPRecordLen
				/*pSmsParam->RecordLen*/- nDefaultSMSPWithoutAlphaId;
	}

	//dongil01.park(2008/12/27) - Check Point
	memcpy(data, smsParams->szAlphaId, (int) nPIDIndex/*pSmsParam->AlphaIdLen*/);

	dbg(" Alpha ID : %s", smsParams->szAlphaId);
	dbg(" nPIDIndex = %d", nPIDIndex);

	data[nPIDIndex] = smsParams->paramIndicator;

	dbg(" Param Indicator = %02x",	smsParams->paramIndicator);

	if ((smsParams->paramIndicator & SMSPValidDestAddr) == 0x00) {
		nOffset = nDestAddrOffset;

		data[nPIDIndex + (nOffset)] = smsParams->tpDestAddr.dialNumLen + 1;
		data[nPIDIndex + (++nOffset)] = ((smsParams->tpDestAddr.typeOfNum << 4) | smsParams->tpDestAddr.numPlanId) | 0x80;

		memcpy(&data[nPIDIndex + (++nOffset)], &smsParams->tpDestAddr.diallingNum, smsParams->tpDestAddr.dialNumLen);

		dbg(" nextIndex = %d", nPIDIndex);
	}

	if( (smsParams->paramIndicator & SMSPValidSvcAddr) == 0x00 )
	{
		dbg("TON [%d] / NPI [%d]", smsParams->tpSvcCntrAddr.typeOfNum, smsParams->tpSvcCntrAddr.numPlanId);

		nOffset = nSCAAddrOffset;

		dbg("SCA Length : %d", smsParams->tpSvcCntrAddr.dialNumLen);

		data[nPIDIndex + (nOffset)] = smsParams->tpSvcCntrAddr.dialNumLen + 1;
		data[nPIDIndex + (++nOffset)] = ((smsParams->tpSvcCntrAddr.typeOfNum << 4) | smsParams->tpSvcCntrAddr.numPlanId) | 0x80;

		memcpy(&data[nPIDIndex + (++nOffset)], &smsParams->tpSvcCntrAddr.diallingNum, smsParams->tpSvcCntrAddr.dialNumLen);

		dbg(" nextIndex = %d", nPIDIndex);
	}

	if ((smsParams->paramIndicator & SMSPValidPID) == 0x00) {
		nOffset = nPIDOffset;

		data[nPIDIndex + nOffset] = smsParams->tpProtocolId;
		dbg(" PID = %02x", smsParams->tpProtocolId);
		dbg(" nextIndex = %d", nPIDIndex);
	}

	if ((smsParams->paramIndicator & SMSPValidDCS) == 0x00) {
		nOffset = nDCSOffset;

		data[nPIDIndex + nOffset] = smsParams->tpDataCodingScheme;
		dbg(" DCS = %02x", smsParams->tpDataCodingScheme);
		dbg(" nextIndex = %d", nPIDIndex);
	}

	if ((smsParams->paramIndicator & SMSPValidVP) == 0x00) {
		nOffset = nVPOffset;

		data[nPIDIndex + nOffset] = smsParams->tpValidityPeriod;
		dbg(" VP = %02x", smsParams->tpValidityPeriod);
		dbg(" nextIndex = %d", nPIDIndex);
	}

	return TRUE;

}

void tcore_util_sms_semioctet_to_octect(int* nScLength) {
	if (*nScLength % 2) {
		*nScLength = (*nScLength / 2) + 1;
	} else {
		*nScLength = *nScLength / 2;
	}

	return;
}

TReturn tcore_util_sms_encode_submit_message(const struct telephony_sms_CdmaMsgInfo *pMsgInfo, uint8_t *output, uint32_t *pos)
{
	TReturn api_err = TCORE_RETURN_SUCCESS;

	struct telephony_sms_Is637OutSubmit *pSubmit = NULL;
	uint32_t temp_index = 0;
	int i = 0;

	// 1. check null pointer
	if(pMsgInfo == NULL || output == NULL || pos == NULL)
		return TCORE_RETURN_EINVAL;

	// 2. check manatory parameter in the TelSmsMsgInfo_t
	if(!((pMsgInfo->ParamMask & SMS_PARAM_TELESERVICE_MASK) &&
		(pMsgInfo->ParamMask & SMS_PARAM_ADDRESS_MASK) &&
		(pMsgInfo->ParamMask & SMS_PARAM_MESSAGE_ID_MASK) ))
				return TCORE_RETURN_EINVAL;

	pSubmit = (struct telephony_sms_Is637OutSubmit *)&(pMsgInfo->MsgData.outSubmit);

	dbg("SMS_PARAMID_TELESERVICE_ID\n");
	dbg("teleservice msg=%x\n", pSubmit->TeleService);
	// 3. teleservice
	output[temp_index++] = SMS_PARAMID_TELESERVICE_ID;
	output[temp_index++] = 2;
	memcpy(output+temp_index, &(pSubmit->TeleService), sizeof(unsigned short));
	temp_index += sizeof(unsigned short);

	dbg("SMS_PARAMID_ADDRESS\n");
	// 4. Destination address
	output[temp_index++] = SMS_PARAMID_ADDRESS;
	output[temp_index++] = pSubmit->DstAddr.szAddrLength + 5;
	output[temp_index++] = (uint8_t)pSubmit->DstAddr.Digit;
	output[temp_index++] = (uint8_t)pSubmit->DstAddr.NumberMode;
	output[temp_index++] = (uint8_t)pSubmit->DstAddr.NumberType;
	output[temp_index++] = (uint8_t)pSubmit->DstAddr.NumberPlan;
	output[temp_index++] = (uint8_t)pSubmit->DstAddr.szAddrLength;
	if(pSubmit->DstAddr.szAddrLength > SMS_MAXLENGTH_SMS_ADDRESS)
		api_err = TCORE_RETURN_EINVAL;
	else{
		memcpy(output+ temp_index, pSubmit->DstAddr.szAddress, pSubmit->DstAddr.szAddrLength);
		temp_index += pSubmit->DstAddr.szAddrLength;
	}

	dbg("SMS_PARAMID_SUBADDRESS\n");
	// 5. Subaddress (optional)
	if( (api_err == TCORE_RETURN_SUCCESS)  && (pMsgInfo->ParamMask & SMS_PARAM_SUBADDRESS_MASK)){
		output[temp_index++] = SMS_PARAMID_SUBADDRESS;
		output[temp_index++] = pSubmit->DstSubAddr.szAddrLength + 3;
		output[temp_index++] = pSubmit->DstSubAddr.SubType;
		output[temp_index++] = pSubmit->DstSubAddr.Odd;
		output[temp_index++] = pSubmit->DstSubAddr.szAddrLength;
		if(pSubmit->DstSubAddr.szAddrLength > SMS_MAXLENGTH_SMS_ADDRESS)
			api_err = TCORE_RETURN_EINVAL;
		else{
			memcpy(output+ temp_index, pSubmit->DstSubAddr.szAddress, pSubmit->DstSubAddr.szAddrLength);
			temp_index += pSubmit->DstSubAddr.szAddrLength;
		}
	}

	dbg("SMS_PARAMID_BEARER_REPLY\n");
	// 6. Bearer Reply Option
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_BEARER_REPLY_MASK)){
		output[temp_index++] = SMS_PARAMID_BEARER_REPLY;
		output[temp_index++] = 1;
		if(pSubmit->ReplySeqNumber >= 64)
			api_err = TCORE_RETURN_EINVAL;
		else output[temp_index++] = pSubmit->ReplySeqNumber;
	}

	dbg("SMS_PARAMID_MESSAGE_ID\n");
	dbg("Message ID msg=%x\n",pSubmit->MsgId);
	// 7. Message Id
	if((api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_MESSAGE_ID_MASK)){
		output[temp_index++] = SMS_PARAMID_MESSAGE_ID;
		output[temp_index++] = 3;
		output[temp_index++] = SMS_MESSAGETYPE_SUBMIT;
		memcpy(output+ temp_index, &(pSubmit->MsgId), sizeof(unsigned short));
		temp_index += sizeof(unsigned short);
	}

	dbg("SMS_PARAMID_USER_DATA\n");
	// 8. User Data
	if((api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_USER_DATA_MASK)){
		output[temp_index++] = SMS_PARAMID_USER_DATA;
		output[temp_index++] = 2 + pSubmit->MsgLength;
		output[temp_index++] = pSubmit->MsgEncoding;
		output[temp_index++] = pSubmit->MsgLength;
		if(pSubmit->MsgLength > SMS_MAXLENGTH_SMS_MO_USER_DATA)
			api_err = TCORE_RETURN_EINVAL;
		else{
			memcpy(output+ temp_index, pSubmit->szData, pSubmit->MsgLength);
			temp_index += pSubmit->MsgLength;
		}
	}

	// 9. Deferred DeliveryTime Absolute
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_DEFERRED_DELIVERY_ABS_MASK)){
		output[temp_index++] = SMS_PARAMID_DEFERRED_DELIVERY_ABS;
		output[temp_index++] = 6;
		output[temp_index++] = (uint8_t)pSubmit->DeferredDelTimeAbs.year;
		output[temp_index++] = (uint8_t)pSubmit->DeferredDelTimeAbs.month;
		output[temp_index++] = (uint8_t)pSubmit->DeferredDelTimeAbs.day;
		output[temp_index++] = (uint8_t)pSubmit->DeferredDelTimeAbs.hours;
		output[temp_index++] = (uint8_t)pSubmit->DeferredDelTimeAbs.minutes;
		output[temp_index++] = (uint8_t)pSubmit->DeferredDelTimeAbs.seconds;
	}

	// 10. Deferred DeliveryTime Relative
	if((api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_DEFERRED_DELIVERY_REL_MASK)){
		output[temp_index++] = SMS_PARAMID_DEFERRED_DELIVERY_REL;
		output[temp_index++] = 1;
		output[temp_index++] = (uint8_t)pSubmit->DeferredDelTimeRel;
	}

	// 11. Priority Indicator
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_PRIORITY_MASK)){
		output[temp_index++] = SMS_PARAMID_PRIORITY;
		output[temp_index++] = 1;
		if((int)pSubmit->Privacy < SMS_PRIVACY_NOT_RESTRICTED || pSubmit->Privacy > SMS_PRIVACY_SECRET)
			api_err = TCORE_RETURN_EINVAL;
		else output[temp_index++] = pSubmit->Priority;
	}

	// 12. Privacy Indicator
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_PRIVACY_MASK)){
		output[temp_index++] = SMS_PARAMID_PRIVACY;
		output[temp_index++] = 1;
		if((int)pSubmit->Priority < SMS_PRIORITY_NORMAL || pSubmit->Priority > SMS_PRIORITY_EMERGENCY)
			api_err = TCORE_RETURN_EINVAL;
		else output[temp_index++] = pSubmit->Privacy;
	}

	// 13. Reply Option
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_REPLY_OPTION_MASK)){
		output[temp_index++] = SMS_PARAMID_REPLY_OPTION;
		output[temp_index++] = 2;
		if(pSubmit->bUserAckRequest == 0 &&  pSubmit->bDeliveryAckRequest == 0)
			api_err = TCORE_RETURN_EINVAL;
		else {
			output[temp_index++] = (uint8_t)(pSubmit->bUserAckRequest);
			output[temp_index++] = (uint8_t)(pSubmit->bDeliveryAckRequest);
		}
	}

	// 14. Alert on Message Delivery
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_ALERT_ON_DELIVERY_MASK)){
		output[temp_index++] = SMS_PARAMID_ALERT_ON_DELIVERY;
		output[temp_index++] = 1;
		if((int)pSubmit->AlertPriority< SMS_ALERT_PRIORITY_DEFAULT || pSubmit->AlertPriority > SMS_ALERT_PRIORITY_HIGH)
			api_err = TCORE_RETURN_EINVAL;
		else output[temp_index++] = pSubmit->AlertPriority;
	}

	// 15. Language Indicator
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_LANGUAGE_MASK)){
		output[temp_index++] = SMS_PARAMID_LANGUAGE;
		output[temp_index++] = 1;
		if((int)pSubmit->MsgLang< SMS_LANG_UNKNOWN || pSubmit->MsgLang > SMS_LANG_HEBREW)
			api_err = TCORE_RETURN_EINVAL;
		else output[temp_index++] = pSubmit->MsgLang;
	}

	dbg("SMS_PARAMID_CALLBACK\n");
	// 16. Callback Number
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_CALLBACK_MASK)){
		output[temp_index++] = SMS_PARAMID_CALLBACK;
		output[temp_index++] = 4 + pSubmit->CallBackNumber.szAddrLength;
		output[temp_index++] = pSubmit->CallBackNumber.Digit;
		output[temp_index++] = pSubmit->CallBackNumber.NumberType;
		output[temp_index++] = pSubmit->CallBackNumber.NumberPlan;
		output[temp_index++] = (uint8_t)pSubmit->CallBackNumber.szAddrLength;
		dbg("temp_index before =%d, value=%x", (int)temp_index, output[temp_index-1]);
		if(pSubmit->CallBackNumber.szAddrLength > SMS_MAXLENGTH_SMS_ADDRESS)
			api_err = TCORE_RETURN_EINVAL;
		else{
			memcpy(output+ temp_index, pSubmit->CallBackNumber.szAddress, pSubmit->CallBackNumber.szAddrLength);

			dbg("index after =%d, value=%x\n", (int)temp_index, output[temp_index]);

			for (i=0;i<11;i++)
				dbg("szAddr[%d]=%x\n", i, output[temp_index+i]);
			temp_index += pSubmit->CallBackNumber.szAddrLength;
		}
	}

	dbg("output index: (0)=%x, (-1)=%x, (+1)=%x\n", output[temp_index], output[temp_index-1],output[temp_index+1]);

	*pos = temp_index;

	return api_err;
}

TReturn tcore_util_sms_encode_cancel_message(const struct telephony_sms_CdmaMsgInfo *pMsgInfo, uint8_t *output, uint32_t *pos)
{
	TReturn api_err = TCORE_RETURN_SUCCESS;
	struct telephony_sms_Is637OutCancel *pCancel = NULL;
	uint32_t temp_index = 0;

	// 1. check null pointer
	if(pMsgInfo == NULL || output == NULL || pos == NULL)
		return TCORE_RETURN_EINVAL;

	// 2. check manatory parameter in the TelSmsMsgInfo_t
	if(!((pMsgInfo->ParamMask & SMS_PARAM_TELESERVICE_MASK) &&
		(pMsgInfo->ParamMask & SMS_PARAM_ADDRESS_MASK) &&
		(pMsgInfo->ParamMask & SMS_PARAM_MESSAGE_ID_MASK) ))
		return TCORE_RETURN_EINVAL;

	pCancel = (struct telephony_sms_Is637OutCancel *)&(pMsgInfo->MsgData.outCancel);

	// 3. teleservice
	output[temp_index++] = SMS_PARAMID_TELESERVICE_ID;
	output[temp_index++] = 2;
	memcpy(output+temp_index, &pCancel->TeleService, sizeof(unsigned short));
	temp_index += sizeof(unsigned short);


	// 4. Destination address
	output[temp_index++] = SMS_PARAMID_ADDRESS;
	output[temp_index++] = pCancel->DstAddr.szAddrLength + 5;
	output[temp_index++] = (uint8_t)pCancel->DstAddr.Digit;
	output[temp_index++] = (uint8_t)pCancel->DstAddr.NumberMode;
	output[temp_index++] = (uint8_t)pCancel->DstAddr.NumberType;
	output[temp_index++] = (uint8_t)pCancel->DstAddr.NumberPlan;
	output[temp_index++] = (uint8_t)pCancel->DstAddr.szAddrLength;
	if(pCancel->DstAddr.szAddrLength > SMS_MAXLENGTH_SMS_ADDRESS)
		api_err = TCORE_RETURN_EINVAL;
	else{
		memcpy(output+ temp_index, pCancel->DstAddr.szAddress, pCancel->DstAddr.szAddrLength);
		temp_index += pCancel->DstAddr.szAddrLength;
	}

	// 5. Subaddress (optional)
	if( (api_err == TCORE_RETURN_SUCCESS)  && (pMsgInfo->ParamMask & SMS_PARAM_SUBADDRESS_MASK)){
		output[temp_index++] = SMS_PARAMID_SUBADDRESS;
		output[temp_index++] = pCancel->DstSubAddr.szAddrLength + 3;
		output[temp_index++] = pCancel->DstSubAddr.SubType;
		output[temp_index++] = pCancel->DstSubAddr.Odd;
		output[temp_index++] = pCancel->DstSubAddr.szAddrLength;
		if(pCancel->DstSubAddr.szAddrLength > SMS_MAXLENGTH_SMS_ADDRESS)
			api_err = TCORE_RETURN_EINVAL;
		else{
			memcpy(output+ temp_index, pCancel->DstSubAddr.szAddress, pCancel->DstSubAddr.szAddrLength);
			temp_index += pCancel->DstSubAddr.szAddrLength;
		}
	}

	// 6. Bearer Reply Option
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_BEARER_REPLY_MASK)){
		output[temp_index++] = SMS_PARAMID_BEARER_REPLY;
		output[temp_index++] = 1;
		if(pCancel->ReplySeqNumber >= 64)
			api_err = TCORE_RETURN_EINVAL;
		else output[temp_index++] = pCancel->ReplySeqNumber;
	}

	// 7. Message Id
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_MESSAGE_ID_MASK)){
		output[temp_index++] = SMS_PARAMID_MESSAGE_ID;
		output[temp_index++] = 3;
		output[temp_index++] = SMS_MESSAGETYPE_CANCEL;
		memcpy(output+ temp_index, &pCancel->MsgId, sizeof(unsigned short));
		temp_index += sizeof(unsigned short);
	}

	*pos = temp_index;

	return api_err;
}

TReturn tcore_util_sms_encode_user_ack_message(const struct telephony_sms_CdmaMsgInfo *pMsgInfo, uint8_t *output, uint32_t *pos)
{
	TReturn api_err = TCORE_RETURN_SUCCESS;
	struct telephony_sms_Is637OutAck *pUserAck = NULL;
	uint32_t temp_index = 0;

	// 1. check null pointer
	if(pMsgInfo == NULL || output == NULL || pos == NULL)
		return TCORE_RETURN_EINVAL;

	if(!((pMsgInfo->ParamMask & SMS_PARAM_TELESERVICE_MASK) &&
		(pMsgInfo->ParamMask & SMS_PARAM_ADDRESS_MASK) &&
		(pMsgInfo->ParamMask & SMS_PARAM_MESSAGE_ID_MASK) ))
				return TCORE_RETURN_EINVAL;

	pUserAck = (struct telephony_sms_Is637OutAck *)&(pMsgInfo->MsgData.outAck);

	// 3. teleservice
	output[temp_index++] = SMS_PARAMID_TELESERVICE_ID;
	output[temp_index++] = 2;
	memcpy(output+temp_index, &pUserAck->TeleService, sizeof(unsigned short));
	temp_index += sizeof(unsigned short);


	// 4. Destination address
	output[temp_index++] = SMS_PARAMID_ADDRESS;
	output[temp_index++] = pUserAck->DstAddr.szAddrLength + 5;
	output[temp_index++] = (uint8_t)pUserAck->DstAddr.Digit;
	output[temp_index++] = (uint8_t)pUserAck->DstAddr.NumberMode;
	output[temp_index++] = (uint8_t)pUserAck->DstAddr.NumberType;
	output[temp_index++] = (uint8_t)pUserAck->DstAddr.NumberPlan;
	output[temp_index++] = (uint8_t)pUserAck->DstAddr.szAddrLength;
	if(pUserAck->DstAddr.szAddrLength > SMS_MAXLENGTH_SMS_ADDRESS)
		api_err = TCORE_RETURN_EINVAL;
	else{
		memcpy(output+ temp_index, pUserAck->DstAddr.szAddress, pUserAck->DstAddr.szAddrLength);
		temp_index += pUserAck->DstAddr.szAddrLength;
	}

	// 5. Subaddress (optional)
	if( (api_err == TCORE_RETURN_SUCCESS)  && (pMsgInfo->ParamMask & SMS_PARAM_SUBADDRESS_MASK)){
		output[temp_index++] = SMS_PARAMID_SUBADDRESS;
		output[temp_index++] = pUserAck->DstSubAddr.szAddrLength + 3;
		output[temp_index++] = pUserAck->DstSubAddr.SubType;
		output[temp_index++] = pUserAck->DstSubAddr.Odd;
		output[temp_index++] = pUserAck->DstSubAddr.szAddrLength;
		if(pUserAck->DstSubAddr.szAddrLength > SMS_MAXLENGTH_SMS_ADDRESS)
			api_err = TCORE_RETURN_EINVAL;
		else{
			memcpy(output+ temp_index, pUserAck->DstSubAddr.szAddress, pUserAck->DstSubAddr.szAddrLength);
			temp_index += pUserAck->DstSubAddr.szAddrLength;
		}
	}

	// 6. Bearer Reply Option
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_BEARER_REPLY_MASK)){
		output[temp_index++] = SMS_PARAMID_BEARER_REPLY;
		output[temp_index++] = 1;
		if(pUserAck->ReplySeqNumber >= 64)
			api_err = TCORE_RETURN_EINVAL;
		else output[temp_index++] = pUserAck->ReplySeqNumber;
	}

	// 7. Message Id
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_MESSAGE_ID_MASK)){
		output[temp_index++] = SMS_PARAMID_MESSAGE_ID;
		output[temp_index++] = 3;
		output[temp_index++] = SMS_MESSAGETYPE_USER_ACK;
		memcpy(output+ temp_index, &pUserAck->MsgId, sizeof(unsigned short));
		temp_index += sizeof(unsigned short);
	}

	// 8. User Data
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_USER_DATA_MASK)){
		output[temp_index++] = SMS_PARAMID_USER_DATA;
		output[temp_index++] = 2 + pUserAck->MsgEncoding;
		output[temp_index++] = pUserAck->MsgLength;
		if(pUserAck->MsgLength > SMS_MAXLENGTH_SMS_MO_USER_DATA)
			api_err = TCORE_RETURN_EINVAL;
		else{
			memcpy(output+ temp_index, pUserAck->szData, pUserAck->MsgLength);
			temp_index += pUserAck->MsgLength;
		}
	}

	// 9. User Response Code
	if( (api_err == TCORE_RETURN_SUCCESS) && (pMsgInfo->ParamMask & SMS_PARAM_USER_RESPONSE_CODE_MASK)){
		output[temp_index++] = SMS_PARAMID_USER_RESPONSE_CODE;
		output[temp_index++] = 1;
		output[temp_index++] = pUserAck->UserResponseCode;
	}

	*pos = temp_index;

	return api_err;
}

int tcore_util_sms_decode_inDeliver_message(uint8_t *incoming, uint32_t length, struct telephony_sms_CdmaMsgInfo *pMsgInfo)
{
	int rtn = TRUE;
	uint32_t	temp_index = 0;
	struct telephony_sms_Is637InDeliver *InDeliver = NULL;

	dbg("Parsing Bearer Data below, Total length[0x%x]", (unsigned int)length);

	if(incoming == NULL || pMsgInfo == NULL)
		return FALSE;

	InDeliver = &(pMsgInfo->MsgData.inDeliver);

	do{
		if(incoming[temp_index] == SMS_PARAMID_USER_DATA){
			int i=0;
			dbg("ParamID[SMS_PARAMID_USER_DATA=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->MsgEncoding = incoming[++temp_index];
			InDeliver->MsgLength = incoming[++temp_index];
			memcpy(InDeliver->szData, incoming+ ++temp_index, InDeliver->MsgLength);
			temp_index += InDeliver->MsgLength;
			pMsgInfo->ParamMask |= SMS_PARAM_USER_DATA_MASK;
			dbg("MsgEnconding[0x%x], MsgLength[%d]",
				InDeliver->MsgEncoding, InDeliver->MsgLength);

			for(i = 0 ; i < InDeliver->MsgLength ; i++)
			{
				dbg("Index[%d] Char[0x%x]", i, InDeliver->szData[i]);
			}
			dbg("Final Index[0x%x]", (unsigned int)temp_index);


		}
		else if(incoming[temp_index] == SMS_PARAMID_VALIDITY_PERIOD_ABS){
			dbg("ParamID[SMS_PARAMID_VALIDITY_PERIOD_ABS=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->ValidityPeriodAbs.year = incoming[++temp_index];
			InDeliver->ValidityPeriodAbs.month = incoming[++temp_index];
			InDeliver->ValidityPeriodAbs.day = incoming[++temp_index];
			InDeliver->ValidityPeriodAbs.hours = incoming[++temp_index];
			InDeliver->ValidityPeriodAbs.minutes = incoming[++temp_index];
			InDeliver->ValidityPeriodAbs.seconds = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_VALIDITY_PERIOD_ABS_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_VALIDITY_PERIOD_REL){
			dbg("ParamID[SMS_PARAMID_VALIDITY_PERIOD_REL=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->ValidityPeriodRel = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_VALIDITY_PERIOD_REL_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_MC_TIME_STAMP){
			dbg("ParamID[SMS_PARAMID_MC_TIME_STAMP=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->MessageCenterTimeStamp.year = incoming[++temp_index];
			InDeliver->MessageCenterTimeStamp.month = incoming[++temp_index];
			InDeliver->MessageCenterTimeStamp.day = incoming[++temp_index];
			InDeliver->MessageCenterTimeStamp.hours = incoming[++temp_index];
			InDeliver->MessageCenterTimeStamp.minutes = incoming[++temp_index];
			InDeliver->MessageCenterTimeStamp.seconds = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_MC_TIME_STAMP_MASK;
		}

		else if(incoming[temp_index] == SMS_PARAMID_PRIORITY){
			dbg("ParamID[SMS_PARAMID_PRIORITY=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->Priority = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_PRIORITY_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_PRIVACY){
			dbg("ParamID[SMS_PARAMID_PRIVACY=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->Privacy = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_PRIVACY_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_NUMBER_OF_MESSAGE){
			dbg("ParamID[SMS_PARAMID_NUMBER_OF_MESSAGE=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->NumMsg = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_NUMBER_OF_MESSAGE_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_ALERT_ON_DELIVERY){
			dbg("ParamID[SMS_PARAMID_ALERT_ON_DELIVERY=0x%x], ParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->AlertPriority = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_ALERT_ON_DELIVERY_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_LANGUAGE){
			dbg("ParamID[SMS_PARAMID_LANGUAGE=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->MsgLang = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_LANGUAGE_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_REPLY_OPTION){
			dbg("ParamID[SMS_PARAMID_REPLY_OPTION=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->bUserAckRequest = (int)incoming[++temp_index];
			InDeliver->bDeliveryAckRequest = (int)incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_REPLY_OPTION_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_CALLBACK){
			dbg("ParamID[SMS_PARAMID_CALLBACK=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->CallBackNumer.Digit = incoming[++temp_index];
			InDeliver->CallBackNumer.NumberType= incoming[++temp_index];
			InDeliver->CallBackNumer.NumberPlan = incoming[++temp_index];
			InDeliver->CallBackNumer.szAddrLength = incoming[++temp_index];
			memcpy(InDeliver->CallBackNumer.szAddress, incoming+ ++temp_index, InDeliver->CallBackNumer.szAddrLength);
			temp_index+= InDeliver->CallBackNumer.szAddrLength;
			pMsgInfo->ParamMask |= SMS_PARAMID_CALLBACK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_DISPLAY_MODE){
			dbg("ParamID[SMS_PARAMID_DISPLAY_MODE=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			temp_index++; //ignore param length
			InDeliver->Display= incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_DISPLAY_MODE_MASK;
		}
		else
		{
			dbg("Undefined SMS Parameter ID [0x%x] in the Bearer Data", incoming[temp_index]);
			rtn = FALSE;
			break;
		}
	}while(!(temp_index == length));

	return rtn;
}

int tcore_util_sms_decode_inAck_message(uint8_t *incoming, uint32_t length, struct telephony_sms_CdmaMsgInfo *pMsgInfo)
{
	int rtn = TRUE;
	struct telephony_sms_Is637InAck *InAck = NULL;
	uint32_t	temp_index = 0;

	if(incoming == NULL || pMsgInfo == NULL)
		return FALSE;

	InAck = &(pMsgInfo->MsgData.inAck);
	do{
		if(incoming[temp_index] == SMS_PARAMID_USER_DATA){
			temp_index++; //ignore param length
			InAck->MsgEncoding = incoming[++temp_index];
			InAck->MsgLength = incoming[++temp_index];
			memcpy(InAck->szData, incoming+ ++temp_index, InAck->MsgLength);
			temp_index += InAck->MsgLength;
			pMsgInfo->ParamMask |= SMS_PARAM_USER_DATA_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_USER_RESPONSE_CODE){
			temp_index++; //ignore param length
			InAck->MsgEncoding = incoming[++temp_index];
			InAck->UserResponseCode = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_USER_RESPONSE_CODE_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_MC_TIME_STAMP){
			temp_index++; //ignore param length
			InAck->MessageCenterTimeStamp.year = incoming[++temp_index];
			InAck->MessageCenterTimeStamp.month = incoming[++temp_index];
			InAck->MessageCenterTimeStamp.day = incoming[++temp_index];
			InAck->MessageCenterTimeStamp.hours = incoming[++temp_index];
			InAck->MessageCenterTimeStamp.minutes = incoming[++temp_index];
			InAck->MessageCenterTimeStamp.seconds = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_MC_TIME_STAMP_MASK;
		}
		else{
			rtn = FALSE;
			break;
		}
	}while(!(temp_index == length));

	return rtn;
}

int tcore_util_sms_decode_inDeliverAck_message(uint8_t *incoming, uint32_t length, struct telephony_sms_CdmaMsgInfo *pMsgInfo)
{
	int rtn = TRUE;
	struct telephony_sms_Is637InDeliverAck *InDelAck = NULL;
	uint32_t	temp_index = 0;

	if(incoming == NULL || pMsgInfo == NULL)
		return FALSE;

	InDelAck = &(pMsgInfo->MsgData.inDeliverAck);
	do{
		if(incoming[temp_index] == SMS_PARAMID_USER_DATA){
			temp_index++; //ignore param length
			InDelAck->MsgEncoding = incoming[++temp_index];
			InDelAck->MsgLength = incoming[++temp_index];
			memcpy(InDelAck->szData, incoming+ ++temp_index, InDelAck->MsgLength);
			temp_index += InDelAck->MsgLength;
			pMsgInfo->ParamMask |= SMS_PARAM_USER_DATA_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_MC_TIME_STAMP){
			temp_index++; //ignore param length
			InDelAck->MessageCenterTimeStamp.year = incoming[++temp_index];
			InDelAck->MessageCenterTimeStamp.month = incoming[++temp_index];
			InDelAck->MessageCenterTimeStamp.day = incoming[++temp_index];
			InDelAck->MessageCenterTimeStamp.hours = incoming[++temp_index];
			InDelAck->MessageCenterTimeStamp.minutes = incoming[++temp_index];
			InDelAck->MessageCenterTimeStamp.seconds = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_MC_TIME_STAMP_MASK;
		}
		else{
			rtn = FALSE;
			break;
		}
	}while(!(temp_index == length));

	return rtn;
}

int tcore_util_sms_decode_ptp_message(uint8_t *incoming, uint32_t length, struct telephony_sms_CdmaMsgInfo *pMsgInfo)
{
	int rtn = TRUE;
	uint32_t	temp_index = 0;
	uint32_t	ParamLen = 0;
	struct telephony_sms_Is637InDeliver *pCommon = NULL;
	enum telephony_sms_CdmaMsgType type;

	if(incoming == NULL || pMsgInfo == NULL)
		return FALSE;

	pCommon = &(pMsgInfo->MsgData.inDeliver);

	do{
		if(incoming[temp_index] == SMS_PARAMID_TELESERVICE_ID)
		{
			dbg("ParamID[SMS_PARAMID_TELESERVICE_ID=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			ParamLen = incoming[++temp_index];	 //parameter length
			memcpy(&pCommon->TeleService, incoming+ ++temp_index, ParamLen);
			temp_index += ParamLen;
			pMsgInfo->ParamMask |= SMS_PARAM_TELESERVICE_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_ADDRESS)
		{
			dbg("ParamID[SMS_PARAMID_ADDRESS=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			ParamLen = incoming[++temp_index];	 //parameter length
			pCommon->OrigAddr.Digit = incoming[++temp_index];
			pCommon->OrigAddr.NumberMode = incoming[++temp_index];
			pCommon->OrigAddr.NumberType = incoming[++temp_index];
			pCommon->OrigAddr.NumberPlan = incoming[++temp_index];
			pCommon->OrigAddr.szAddrLength = incoming[++temp_index];
			memcpy(pCommon->OrigAddr.szAddress, incoming+ ++temp_index, pCommon->OrigAddr.szAddrLength);
			temp_index += pCommon->OrigAddr.szAddrLength;
			pMsgInfo->ParamMask |= SMS_PARAM_ADDRESS_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_SUBADDRESS)
		{
			dbg("ParamID[SMS_PARAMID_SUBADDRESS=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			ParamLen = incoming[++temp_index];	 //parameter length
			pCommon->OrigSubAddr.SubType =  incoming[++temp_index];
			pCommon->OrigSubAddr.Odd =  incoming[++temp_index];
			pCommon->OrigSubAddr.szAddrLength = incoming[++temp_index];
			memcpy(pCommon->OrigSubAddr.szAddress, incoming+ ++temp_index, pCommon->OrigSubAddr.szAddrLength);
			temp_index += pCommon->OrigSubAddr.szAddrLength;
			pMsgInfo->ParamMask |= SMS_PARAM_SUBADDRESS_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_BEARER_REPLY)
		{
			dbg("ParamID[SMS_PARAMID_BEARER_REPLY=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			ParamLen = incoming[++temp_index];	 //parameter length
			pCommon->bBearerReplySeqRequest = (int)TRUE;
			pCommon->ReplySeqNumber = incoming[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_BEARER_REPLY_MASK;
		}
		else if(incoming[temp_index] == SMS_PARAMID_MESSAGE_ID)
		{
			dbg("ParamID[SMS_PARAMID_MESSAGE_ID=0x%x]\tParamLen[%d]", incoming[temp_index], incoming[temp_index+1]);
			ParamLen = incoming[++temp_index];	 //parameter length
			type = incoming[++temp_index];
			pMsgInfo->MsgType = type;
			memcpy(&pCommon->MsgId, incoming+ ++temp_index, sizeof(unsigned short));
			temp_index += sizeof(unsigned short);
			pMsgInfo->ParamMask |= SMS_PARAM_MESSAGE_ID_MASK;
			switch(type)
			{
				case SMS_MESSAGETYPE_DELIVER:
					dbg("SubParamID[SMS_MESSAGETYPE_DELIVER=0x%x]", type);
					dbg("Current Index[0x%x], Parsing Length[0x%x], ParamLen[0x%x]", (unsigned int)temp_index, (unsigned int)length, (unsigned int)ParamLen);

					rtn = tcore_util_sms_decode_inDeliver_message(incoming+temp_index, length - temp_index, pMsgInfo);

					break;
				case SMS_MESSAGETYPE_DELIVERY_ACK:
					dbg("SubParamID[SMS_MESSAGETYPE_DELIVERY_ACK=0x%x]", type);

					rtn = tcore_util_sms_decode_inAck_message(incoming+temp_index, length - temp_index, pMsgInfo);

					break;
				case SMS_MESSAGETYPE_USER_ACK:
					dbg("SubParamID[SMS_MESSAGETYPE_USER_ACK=0x%x]", type);

					rtn = tcore_util_sms_decode_inDeliverAck_message(incoming+temp_index, length - temp_index, pMsgInfo);

					break;
				default:
					dbg("Unknown Incoming Message Type = %d", type);
					rtn = FALSE;
					break;
			}
			temp_index = length;
		}
		else
		{
			dbg("ParamID[Undefined]");
			//rtn = FALSE;
			break;
		}

	}while(!(temp_index == length));

	return rtn;
}

int tcore_util_sms_decode_broadcast_message(uint8_t *bcmsg, uint32_t length, struct telephony_sms_CdmaMsgInfo *pMsgInfo)
{
	int rtn = TRUE;
	uint32_t	temp_index = 0;
	uint32_t	ParamLen = 0;
	struct telephony_sms_Is637InBroadCast *pOutput = NULL;

	if(bcmsg == NULL || pMsgInfo == NULL)
		return FALSE;

	pOutput = &(pMsgInfo->MsgData.inBc);

	do{
		if(bcmsg[temp_index] == SMS_PARAMID_SERVICE_CATEGORY){
			ParamLen = bcmsg[++temp_index];	 //parameter length
			memcpy(&pOutput->ServiceCategory, bcmsg+ ++temp_index, ParamLen);
			temp_index += ParamLen;
			pMsgInfo->ParamMask |= SMS_PARAM_SERVICE_CATEGORY_MASK;
		}
		else if(bcmsg[temp_index] == SMS_PARAMID_BEARER_REPLY){
			ParamLen = bcmsg[++temp_index];	 //parameter length
			pOutput->bBearerReplySeqRequest = (int)TRUE;
			pOutput->ReplySeqNumber = bcmsg[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_BEARER_REPLY_MASK;
		}
		else if(bcmsg[temp_index] == SMS_PARAMID_MESSAGE_ID){
			ParamLen = bcmsg[++temp_index];	 //parameter length
			memcpy(&pOutput->MsgId, bcmsg+ ++temp_index, ParamLen);
			temp_index += ParamLen;
			pMsgInfo->ParamMask |= SMS_PARAM_MESSAGE_ID_MASK;
		}
		else if(bcmsg[temp_index] == SMS_PARAMID_VALIDITY_PERIOD_ABS){
			ParamLen = bcmsg[++temp_index];	 //parameter length
			pOutput->ValidityPeriodAbs.year = bcmsg[++temp_index];
			pOutput->ValidityPeriodAbs.month = bcmsg[++temp_index];
			pOutput->ValidityPeriodAbs.day = bcmsg[++temp_index];
			pOutput->ValidityPeriodAbs.hours = bcmsg[++temp_index];
			pOutput->ValidityPeriodAbs.minutes = bcmsg[++temp_index];
			pOutput->ValidityPeriodAbs.seconds = bcmsg[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_VALIDITY_PERIOD_ABS_MASK;
		}
		else if(bcmsg[temp_index] == SMS_PARAMID_VALIDITY_PERIOD_REL){
			ParamLen = bcmsg[++temp_index];	 //parameter length
			pOutput->ValidityPeriodRel = bcmsg[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_VALIDITY_PERIOD_REL_MASK;
		}
		else if(bcmsg[temp_index] == SMS_PARAMID_MC_TIME_STAMP){
			ParamLen = bcmsg[++temp_index];	 //parameter length
			pOutput->MessageCenterTimeStamp.year = bcmsg[++temp_index];
			pOutput->MessageCenterTimeStamp.month = bcmsg[++temp_index];
			pOutput->MessageCenterTimeStamp.day = bcmsg[++temp_index];
			pOutput->MessageCenterTimeStamp.hours = bcmsg[++temp_index];
			pOutput->MessageCenterTimeStamp.minutes = bcmsg[++temp_index];
			pOutput->MessageCenterTimeStamp.seconds = bcmsg[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_MC_TIME_STAMP_MASK;
		}
		else if(bcmsg[temp_index] == SMS_PARAMID_PRIORITY){
			ParamLen = bcmsg[++temp_index];	 //parameter length
			pOutput->Priority= bcmsg[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_PRIORITY_MASK;
		}
		else if(bcmsg[temp_index] == SMS_PARAMID_ALERT_ON_DELIVERY){
			ParamLen = bcmsg[++temp_index];	 //parameter length
			pOutput->AlertPriority = bcmsg[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_ALERT_ON_DELIVERY_MASK;
		}
		else if(bcmsg[temp_index] == SMS_PARAMID_LANGUAGE){
			ParamLen = bcmsg[++temp_index];	 //parameter length
			pOutput->MsgLang = bcmsg[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_LANGUAGE_MASK;
		}
		else if(bcmsg[temp_index] == SMS_PARAMID_DISPLAY_MODE){
			ParamLen = bcmsg[++temp_index];	 //parameter length
			pOutput->Display = bcmsg[++temp_index];
			temp_index++;
			pMsgInfo->ParamMask |= SMS_PARAM_DISPLAY_MODE_MASK;
		}
		else if(bcmsg[temp_index] == SMS_PARAMID_USER_DATA){
			ParamLen = bcmsg[++temp_index];	 //parameter length
			pOutput->MsgEncoding = bcmsg[++temp_index];
			pOutput->MsgLength = bcmsg[++temp_index];
			memcpy(pOutput->szData, bcmsg+ ++temp_index, pOutput->MsgLength);
			temp_index += pOutput->MsgLength;
			pMsgInfo->ParamMask |= SMS_PARAM_USER_DATA_MASK;
		}
		else{
			rtn = FALSE;
			break;
		}

	}while(!(temp_index == length));

	return rtn;
}

int tcore_util_sms_decode_smsParameters(uint8_t *incoming, uint32_t length, struct telephony_sms_Params *params)
{
	int alpha_id_len = 0;
	int i = 0;
	int nOffset = 0;

	dbg(" RecordLen = %d", length);

	if(incoming == NULL || params == NULL)
		return FALSE;

	alpha_id_len = length -SMS_SMSP_PARAMS_MAX_LEN;

	if ( alpha_id_len > 0 )
	{
		if(alpha_id_len > SMS_SMSP_ALPHA_ID_LEN_MAX)
		{
			alpha_id_len = SMS_SMSP_ALPHA_ID_LEN_MAX;
		}

		for( i=0 ; i < alpha_id_len ; i++)
		{
			if( 0xff == incoming[i])
			{
				dbg(" found");
				break;
			}
		}

		memcpy(params->szAlphaId, incoming, i);

		params->alphaIdLen = i;

		dbg(" Alpha id length = %d", i);

	}
	else
	{
		params->alphaIdLen = 0;
		dbg(" Alpha id length is zero");
	}

	//dongil01.park - start parse from here.
	params->paramIndicator = incoming[alpha_id_len];

	dbg(" Param Indicator = %02x", params->paramIndicator);

	//dongil01.park(2008/12/26) - DestAddr
	if( (params->paramIndicator & SMSPValidDestAddr) == 0)
	{
		nOffset = nDestAddrOffset;

		if( 0x00 == incoming[alpha_id_len + nOffset] || 0xff == incoming[alpha_id_len + nOffset])
		{
			params->tpDestAddr.dialNumLen = 0;

			dbg("DestAddr Length is 0");
		}
		else
		{
			if ( 0 < (int)incoming[alpha_id_len + nOffset] )
			{
				params->tpDestAddr.dialNumLen = (int)(incoming[alpha_id_len + nOffset] - 1 );

			        if(params->tpDestAddr.dialNumLen > SMS_SMSP_ADDRESS_LEN)
				        params->tpDestAddr.dialNumLen = SMS_SMSP_ADDRESS_LEN;
			}
			else
			{
				params->tpDestAddr.dialNumLen = 0;
			}

			params->tpDestAddr.numPlanId= incoming[alpha_id_len + (++nOffset)] & 0x0f ;
			params->tpDestAddr.typeOfNum= (incoming[alpha_id_len + nOffset] & 0x70 )>>4 ;

			memcpy( params->tpDestAddr.diallingNum, &incoming[alpha_id_len + (++nOffset)], (params->tpDestAddr.dialNumLen)) ;

			dbg("Dest TON is %d",params->tpDestAddr.typeOfNum);
			dbg("Dest NPI is %d",params->tpDestAddr.numPlanId);
			dbg("Dest Length = %d",params->tpDestAddr.dialNumLen);
			dbg("Dest Addr = %s",params->tpDestAddr.diallingNum);

		}
	}

	//dongil01.park(2008/12/26) - SvcAddr
	if( (params->paramIndicator & SMSPValidSvcAddr) == 0)
	{
		nOffset = nSCAAddrOffset;

		if( 0x00 == (int)incoming[alpha_id_len + nOffset] || 0xff == (int)incoming[alpha_id_len + nOffset] )
		{
			params->tpSvcCntrAddr.dialNumLen = 0;

			dbg(" SCAddr Length is 0");
		}
		else
		{
			if ( 0 < (int)incoming[alpha_id_len + nOffset]  )
			{
				params->tpSvcCntrAddr.dialNumLen = (int)(incoming[alpha_id_len + nOffset] - 1);

			        if(params->tpSvcCntrAddr.dialNumLen > SMS_SMSP_ADDRESS_LEN)
				        params->tpSvcCntrAddr.dialNumLen = SMS_SMSP_ADDRESS_LEN;

				params->tpSvcCntrAddr.numPlanId= incoming[alpha_id_len + (++nOffset)] & 0x0f ;
				params->tpSvcCntrAddr.typeOfNum= (incoming[alpha_id_len + nOffset] & 0x70) >>4 ;

				memcpy( params->tpSvcCntrAddr.diallingNum, &incoming[alpha_id_len + (++nOffset)], (params->tpSvcCntrAddr.dialNumLen));

				dbg("SCAddr Length = %d ",params->tpSvcCntrAddr.dialNumLen);
				dbg("SCAddr TON is %d",params->tpSvcCntrAddr.typeOfNum);
				dbg("SCAddr NPI is %d",params->tpSvcCntrAddr.numPlanId);

				for( i = 0 ; i < (int)params->tpSvcCntrAddr.dialNumLen ; i ++)
					dbg("SCAddr = %d [%02x]",i,params->tpSvcCntrAddr.diallingNum[i]);
			}
			else
			{
				params->tpSvcCntrAddr.dialNumLen = 0;
			}
		}
	}
	else if ( (0x00 < (int)incoming[alpha_id_len +nSCAAddrOffset] && (int)incoming[alpha_id_len +nSCAAddrOffset] <= 12 )
			|| 0xff != (int)incoming[alpha_id_len +nSCAAddrOffset])
	{
		nOffset = nSCAAddrOffset;

		if( 0x00 == (int)incoming[alpha_id_len + nOffset] || 0xff == (int)incoming[alpha_id_len + nOffset] )
		{
			params->tpSvcCntrAddr.dialNumLen = 0;
			dbg("SCAddr Length is 0");
		}
		else
		{

			if ( 0 < (int)incoming[alpha_id_len + nOffset]  )
			{
				params->tpSvcCntrAddr.dialNumLen = (int)(incoming[alpha_id_len + nOffset] - 1);

				params->tpSvcCntrAddr.dialNumLen = incoming[alpha_id_len + nOffset] -1;

			        if(params->tpSvcCntrAddr.dialNumLen > SMS_SMSP_ADDRESS_LEN)
				        params->tpSvcCntrAddr.dialNumLen = SMS_SMSP_ADDRESS_LEN;

				params->tpSvcCntrAddr.numPlanId= incoming[alpha_id_len + (++nOffset)] & 0x0f ;
				params->tpSvcCntrAddr.typeOfNum= (incoming[alpha_id_len + nOffset] & 0x70) >>4 ;

				memcpy( params->tpSvcCntrAddr.diallingNum, &incoming[alpha_id_len + (++nOffset)],
						(params->tpSvcCntrAddr.dialNumLen)) ;

				dbg("SCAddr Length = %d ",params->tpSvcCntrAddr.dialNumLen);
				dbg("SCAddr TON is %d",params->tpSvcCntrAddr.typeOfNum);
				dbg("SCAddr NPI is %d",params->tpSvcCntrAddr.numPlanId);

				for( i = 0 ; i < (int)params->tpSvcCntrAddr.dialNumLen ; i ++)
					dbg("SCAddr = %d [%02x]",i,params->tpSvcCntrAddr.diallingNum[i]);
			}
			else
			{
				params->tpSvcCntrAddr.dialNumLen = 0;
			}
		}

	}

	if( (params->paramIndicator & SMSPValidPID) == 0 &&	(alpha_id_len + nPIDOffset) < SMS_MAX_EFSMSP_RECORD_LENGTH)
	{
		params->tpProtocolId = incoming[alpha_id_len + nPIDOffset];
	}
	if( (params->paramIndicator & SMSPValidDCS) == 0 && (alpha_id_len + nDCSOffset) < SMS_MAX_EFSMSP_RECORD_LENGTH)
	{
		params->tpDataCodingScheme = incoming[alpha_id_len + nDCSOffset];
	}
	if( (params->paramIndicator & SMSPValidVP) == 0 && (alpha_id_len + nVPOffset) < SMS_MAX_EFSMSP_RECORD_LENGTH)
	{
		params->tpValidityPeriod = incoming[alpha_id_len + nVPOffset];
	}

	dbg(" Alpha Id(Len) = %d",(int)params->alphaIdLen);

	for (i=0; i< (int)params->alphaIdLen ; i++)
	{
		dbg(" Alpha Id = [%d] [%c]",i,params->szAlphaId[i]);
	}
	dbg(" PID = %d",params->tpProtocolId);
	dbg(" DCS = %d",params->tpDataCodingScheme);
	dbg(" VP = %d",params->tpValidityPeriod);

	return TRUE;
}


static TReturn _dispatcher(CoreObject *o, UserRequest *ur)
{
	enum tcore_request_command command;
	struct private_object_data *po = NULL;
	TReturn rtn = TCORE_RETURN_SUCCESS;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_SMS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po || !po->ops) {
		dbg("[tcore_SMS] ERR: private_object is NULL or ops is NULL");
		return TCORE_RETURN_ENOSYS;
	}

	if(po->b_readyStatus == FALSE) {
		dbg("[tcore_SMS] DEVICE_NOT_READY");
		return TCORE_RETURN_ENOSYS; /* TAPI_API_NETTEXT_DEVICE_NOT_READY */
	}

	command = tcore_user_request_get_command(ur);
	switch (command) {
		case TREQ_SMS_SEND_UMTS_MSG:
			if (!po->ops->send_umts_msg)
			{
				dbg("[tcore_SMS] ERR: po->ops->send_umts_msg is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->send_umts_msg(o, ur);

			break;
		case TREQ_SMS_READ_MSG:
			if (!po->ops->read_msg)
			{
				dbg("[tcore_SMS] ERR: po->ops->read_msg is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->read_msg(o, ur);

			break;
		case TREQ_SMS_SAVE_MSG:
			if (!po->ops->save_msg)
			{
				dbg("[tcore_SMS] ERR: po->ops->save_msg is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->save_msg(o, ur);

			break;
		case TREQ_SMS_DELETE_MSG:
			if (!po->ops->delete_msg)
			{
				dbg("[tcore_SMS] ERR: po->ops->delete_msg is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->delete_msg(o, ur);

			break;
		case TREQ_SMS_GET_COUNT:
			if (!po->ops->get_storedMsgCnt)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_storedMsgCnt is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_storedMsgCnt(o, ur);

			break;
		case TREQ_SMS_GET_SCA:
			if (!po->ops->get_sca)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_sca is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_sca(o, ur);

			break;
		case TREQ_SMS_SET_SCA:
			if (!po->ops->set_sca)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_sca is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_sca(o, ur);

			break;
		case TREQ_SMS_GET_CB_CONFIG:
			if (!po->ops->get_cb_config)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_cb_config is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_cb_config(o, ur);

			break;
		case TREQ_SMS_SET_CB_CONFIG:
			if (!po->ops->set_cb_config)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_cb_config is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_cb_config(o, ur);

			break;
		case TREQ_SMS_SET_MEM_STATUS:
			if (!po->ops->set_mem_status)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_mem_status is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_mem_status(o, ur);

			break;
		case TREQ_SMS_GET_PREF_BEARER:
			if (!po->ops->get_pref_brearer)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_pref_brearer is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_pref_brearer(o, ur);

			break;
		case TREQ_SMS_SET_PREF_BEARER:
			if (!po->ops->set_pref_brearer)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_pref_brearer is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_pref_brearer(o, ur);

			break;
		case TREQ_SMS_SET_DELIVERY_REPORT:
			if (!po->ops->set_delivery_report)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_delivery_report is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_delivery_report(o, ur);

			break;
		case TREQ_SMS_SET_MSG_STATUS:
			if (!po->ops->set_msg_status)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_msg_status is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_msg_status(o, ur);

			break;
		case TREQ_SMS_GET_PARAMS:
			if (!po->ops->get_sms_params)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_sms_params is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_sms_params(o, ur);

			break;
		case TREQ_SMS_SET_PARAMS:
			if (!po->ops->set_sms_params)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_sms_params is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_sms_params(o, ur);

			break;
		case TREQ_SMS_GET_PARAMCNT:
			if (!po->ops->get_paramcnt)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_paramcnt is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_paramcnt(o, ur);

			break;
		case TREQ_SMS_SEND_CDMA_MSG:
			if (!po->ops->send_cdma_msg)
			{
				dbg("[tcore_SMS] ERR: po->ops->send_cdma_msg is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->send_cdma_msg(o, ur);

			break;
		default:
			break;
	}

	dbg("[tcore_SMS] result = [0x%x], command = [0x%x]", rtn, command);

	return rtn;
}

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SMS);

	po = tcore_object_ref_object(o);
	if (po) {
		free(po);
		tcore_object_link_object(o, NULL);
	}
}

static void _clone_hook(CoreObject *src, CoreObject *dest)
{
	struct private_object_data *src_po = NULL;
	struct private_object_data *dest_po = NULL;

	if (!src || !dest)
		return;

	dest_po = calloc(1, sizeof(struct private_object_data));
	if (!dest_po) {
		tcore_object_link_object(dest, NULL);
		return;
	}

	src_po = tcore_object_ref_object(src);
	dest_po->ops = src_po->ops;

	tcore_object_link_object(dest, dest_po);
}

gboolean tcore_sms_get_ready_status(CoreObject *o)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	return po->b_readyStatus;
}

gboolean tcore_sms_set_ready_status(CoreObject *o, int status)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	po->b_readyStatus = status;

	return TRUE;
}

CoreObject *tcore_sms_new(TcorePlugin *p, const char *name,
		struct tcore_sms_operations *ops, TcoreHal *hal)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!p)
		return NULL;

	o = tcore_object_new(p, name, hal);
	if (!o)
		return NULL;

	po = calloc(1, sizeof(struct private_object_data));
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_SMS);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_clone_hook(o, _clone_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_sms_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SMS);
	tcore_object_free(o);
}

void tcore_sms_set_ops(CoreObject *o, struct tcore_sms_operations *ops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SMS);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		return;
	}

	po->ops = ops;
}

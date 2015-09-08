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

#ifndef __TCORE_CO_SMS_H__
#define __TCORE_CO_SMS_H__

#include <core_object.h>

__BEGIN_DECLS


#define nDefaultSMSPWithoutAlphaId	28

#define SMSPValidDestAddr			0x01
#define SMSPValidSvcAddr			0x02
#define SMSPValidPID				0x04
#define SMSPValidDCS				0x08
#define SMSPValidVP					0x10

#define nDestAddrOffset				1
#define nSCAAddrOffset				13
#define nPIDOffset					25
#define nDCSOffset					26
#define nVPOffset					27

#define SMS_PARAMID_TELESERVICE_ID				0x01	/* Teleservice Identifier */
#define SMS_PARAMID_SERVICE_CATEGORY			0x02	/* Broadcast Service Category */
#define SMS_PARAMID_ADDRESS					0x03	/* Address */
#define SMS_PARAMID_SUBADDRESS				0x04	/* Subaddress */
#define SMS_PARAMID_BEARER_REPLY				0x05	/* Bearer Reply Option */
#define SMS_PARAMID_CAUSE_CODES				0x06	/* Cause Codes */
#define SMS_PARAMID_MESSAGE_ID				0x07	/* Message Identifier */
#define SMS_PARAMID_USER_DATA					0x08	/* User Data */
#define SMS_PARAMID_USER_RESPONSE_CODE		0x09	/* User Response Code */
#define SMS_PARAMID_MC_TIME_STAMP			0x0A	/* Message Center Time Stamp */
#define SMS_PARAMID_VALIDITY_PERIOD_ABS		0x0B	/* Validity Period - Absolute */
#define SMS_PARAMID_VALIDITY_PERIOD_REL		0x0C	/* Validiry Period - Relative */
#define SMS_PARAMID_DEFERRED_DELIVERY_ABS	0x0D	/* Deferred Delivery Time - Absolute */
#define SMS_PARAMID_DEFERRED_DELIVERY_REL	0x0E	/* Deferred Delivery Time - Relative */
#define SMS_PARAMID_PRIORITY					0x0F	/* Priority Indicator */
#define SMS_PARAMID_PRIVACY					0x10	/* Privacy Indicator */
#define SMS_PARAMID_REPLY_OPTION				0x11	/* Reply Option */
#define SMS_PARAMID_NUMBER_OF_MESSAGE		0x12	/* Number of Messages : Voice Mail Count */
#define SMS_PARAMID_ALERT_ON_DELIVERY			0x13	/* Alert on Message Delivery */
#define SMS_PARAMID_LANGUAGE					0x14	/* Langauge Indicator */
#define SMS_PARAMID_CALLBACK					0x15	/* Call Back Number */
#define SMS_PARAMID_DISPLAY_MODE				0x16	/* Display Mode */
#define SMS_PARAMID_MULTI_ENCODING_USER_DATA	0x17	/* Multiply Encoding User Data */

struct property_sms_info {
	int	g_trans_id;
	int	SMSPRecordLen;
};

struct tcore_sms_operations {
	TReturn (*send_umts_msg)(CoreObject *o, UserRequest *ur);
	TReturn (*read_msg)(CoreObject *o, UserRequest *ur);
	TReturn (*save_msg)(CoreObject *o, UserRequest *ur);
	TReturn (*delete_msg)(CoreObject *o, UserRequest *ur);
	TReturn (*get_storedMsgCnt)(CoreObject *o, UserRequest *ur);
	TReturn (*get_sca)(CoreObject *o, UserRequest *ur);
	TReturn (*set_sca)(CoreObject *o, UserRequest *ur);
	TReturn (*get_cb_config)(CoreObject *o, UserRequest *ur);
	TReturn (*set_cb_config)(CoreObject *o, UserRequest *ur);
	TReturn (*set_mem_status)(CoreObject *o, UserRequest *ur);
	TReturn (*get_pref_brearer)(CoreObject *o, UserRequest *ur);
	TReturn (*set_pref_brearer)(CoreObject *o, UserRequest *ur);
	TReturn (*set_delivery_report)(CoreObject *o, UserRequest *ur);
	TReturn (*set_msg_status)(CoreObject *o, UserRequest *ur);
	TReturn (*get_sms_params)(CoreObject *o, UserRequest *ur);
	TReturn (*set_sms_params)(CoreObject *o, UserRequest *ur);
	TReturn (*get_paramcnt)(CoreObject *o, UserRequest *ur);
	TReturn (*send_cdma_msg)(CoreObject *o, UserRequest *ur);
};

/**
 * This function is used to encode SMS Parameters to TPDU on EFsmsp
 *
 * @return		length of string
 * @param[in]		incoming - telephony_sms_Params_t
 * @param[in]		data - TPDU data
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
int         _tcore_util_sms_encode_smsParameters(const struct telephony_sms_Params *incoming, unsigned char *data, int SMSPRecordLen);

void tcore_util_sms_semioctet_to_octect(int* nScLength);
TReturn tcore_util_sms_encode_submit_message(const struct telephony_sms_CdmaMsgInfo *pMsgInfo, uint8_t *output, uint32_t *pos);
TReturn tcore_util_sms_encode_cancel_message(const struct telephony_sms_CdmaMsgInfo *pMsgInfo, uint8_t *output, uint32_t *pos);
TReturn tcore_util_sms_encode_user_ack_message(const struct telephony_sms_CdmaMsgInfo *pMsgInfo, uint8_t *output, uint32_t *pos);
int tcore_util_sms_decode_inDeliver_message(uint8_t *incoming, uint32_t length, struct telephony_sms_CdmaMsgInfo *pMsgInfo);
int tcore_util_sms_decode_inAck_message(uint8_t *incoming, uint32_t length, struct telephony_sms_CdmaMsgInfo *pMsgInfo);
int tcore_util_sms_decode_inDeliverAck_message(uint8_t *incoming, uint32_t length, struct telephony_sms_CdmaMsgInfo *pMsgInfo);
int tcore_util_sms_decode_ptp_message(uint8_t *incoming, uint32_t length, struct telephony_sms_CdmaMsgInfo *pMsgInfo);
int tcore_util_sms_decode_broadcast_message(uint8_t *bcmsg, uint32_t length, struct telephony_sms_CdmaMsgInfo *pMsgInfo);
int tcore_util_sms_decode_smsParameters(uint8_t *incoming, uint32_t length, struct telephony_sms_Params *params);

gboolean    tcore_sms_get_ready_status(CoreObject *o);
gboolean    tcore_sms_set_ready_status(CoreObject *o, int status);
CoreObject* tcore_sms_new(TcorePlugin *p, const char *name, struct tcore_sms_operations *ops, TcoreHal *hal);
void        tcore_sms_free(CoreObject * n);

void tcore_sms_set_ops(CoreObject *o, struct tcore_sms_operations *ops);

__END_DECLS

#endif


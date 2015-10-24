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

#ifndef __TYPE_REQUEST_H__
#define __TYPE_REQUEST_H__

#include <type/common.h>

__BEGIN_DECLS

/*
 * SET/REQ: async
 * GET: sync
 */
enum tcore_request_command {
	TREQ_UNKNOWN = 0,

	TREQ_SERVER = TCORE_REQUEST | TCORE_TYPE_SERVER,
	TREQ_SERVER_GET_PLUGINS,

	TREQ_MODEM = TCORE_REQUEST | TCORE_TYPE_MODEM,
	TREQ_MODEM_POWER_ON,
	TREQ_MODEM_POWER_OFF,
	TREQ_MODEM_POWER_RESET,
	TREQ_MODEM_POWER_LOW,
	TREQ_MODEM_SET_FLIGHTMODE,
	TREQ_MODEM_GET_FLIGHTMODE,
	TREQ_MODEM_GET_IMEI,
	TREQ_MODEM_GET_VERSION,
	TREQ_MODEM_GET_SN,
	TREQ_MODEM_SET_DUN_PIN_CONTROL,
	TREQ_MODEM_GET_DEVICE_INFO,

	TREQ_CALL = TCORE_REQUEST | TCORE_TYPE_CALL,
	TREQ_CALL_DIAL,
	TREQ_CALL_ANSWER,
	TREQ_CALL_END,
	TREQ_CALL_HOLD,
	TREQ_CALL_ACTIVE,
	TREQ_CALL_SWAP,
	TREQ_CALL_JOIN,
	TREQ_CALL_SPLIT,
	TREQ_CALL_DEFLECT,
	TREQ_CALL_TRANSFER,
	TREQ_CALL_START_CONT_DTMF,
	TREQ_CALL_STOP_CONT_DTMF,
	TREQ_CALL_SEND_BURST_DTMF,
	TREQ_CALL_SET_PRIVACY_MODE,
	TREQ_CALL_GET_PRIVACY_MODE,
	TREQ_CALL_SET_SOUND_PATH,
	TREQ_CALL_GET_SOUND_VOLUME_LEVEL,
	TREQ_CALL_SET_SOUND_VOLUME_LEVEL,
	TREQ_CALL_SET_SOUND_MUTE_STATUS,
	TREQ_CALL_GET_SOUND_MUTE_STATUS,
	TREQ_CALL_SET_SOUND_RECORDING,
	TREQ_CALL_SET_SOUND_EQUALIZATION,
	TREQ_CALL_SET_SOUND_NOISE_REDUCTION,
	TREQ_CALL_SET_SOUND_CLOCK_STATUS,
	TREQ_CALL_SET_PREFERRED_VOICE_SUBSCRIPTION,
	TREQ_CALL_GET_PREFERRED_VOICE_SUBSCRIPTION,
	TREQ_CALL_MODIFY,
	TREQ_CALL_CONFIRM_MODIFY,

	TREQ_CALL_SET_ACTIVE_LINE,
	TREQ_CALL_GET_ACTIVE_LINE,
	TREQ_CALL_ACTIVATE_CCBS,
	TREQ_CALL_CUSTOM_SERVICE,

	TREQ_SS = TCORE_REQUEST | TCORE_TYPE_SS,
	TREQ_SS_BARRING_ACTIVATE,
	TREQ_SS_BARRING_DEACTIVATE,
	TREQ_SS_BARRING_CHANGE_PASSWORD,
	TREQ_SS_BARRING_GET_STATUS,
	TREQ_SS_FORWARDING_ACTIVATE,
	TREQ_SS_FORWARDING_DEACTIVATE,
	TREQ_SS_FORWARDING_REGISTER,
	TREQ_SS_FORWARDING_DEREGISTER,
	TREQ_SS_FORWARDING_GET_STATUS,
	TREQ_SS_WAITING_ACTIVATE,
	TREQ_SS_WAITING_DEACTIVATE,
	TREQ_SS_WAITING_GET_STATUS,
	TREQ_SS_CLI_ACTIVATE,
	TREQ_SS_CLI_DEACTIVATE,
	TREQ_SS_CLI_SET_STATUS,
	TREQ_SS_CLI_GET_STATUS,
	TREQ_SS_SEND_USSD,
	TREQ_SS_SET_AOC,
	TREQ_SS_GET_AOC,

	TREQ_SIM = TCORE_REQUEST | TCORE_TYPE_SIM,
	TREQ_SIM_VERIFY_PINS,
	TREQ_SIM_VERIFY_PUKS,
	TREQ_SIM_CHANGE_PINS,
	TREQ_SIM_GET_FACILITY_STATUS,
	TREQ_SIM_DISABLE_FACILITY,
	TREQ_SIM_ENABLE_FACILITY,
	TREQ_SIM_TRANSMIT_APDU,
	TREQ_SIM_GET_ATR,
	TREQ_SIM_GET_ECC,
	TREQ_SIM_GET_LANGUAGE,
	TREQ_SIM_SET_LANGUAGE,
	TREQ_SIM_GET_ICCID,
	TREQ_SIM_GET_MAILBOX,
	TREQ_SIM_SET_MAILBOX,
	TREQ_SIM_GET_CALLFORWARDING,
	TREQ_SIM_SET_CALLFORWARDING,
	TREQ_SIM_GET_MESSAGEWAITING,
	TREQ_SIM_SET_MESSAGEWAITING,
	TREQ_SIM_GET_CPHS_INFO,
	TREQ_SIM_GET_SERVICE_TABLE,
	TREQ_SIM_GET_MSISDN,
	TREQ_SIM_GET_SPN,
	TREQ_SIM_GET_SPDI,
	TREQ_SIM_GET_OPL,
	TREQ_SIM_GET_PNN,
	TREQ_SIM_GET_CPHS_NETNAME,
	TREQ_SIM_GET_OPLMNWACT,
	TREQ_SIM_REQ_AUTHENTICATION,
	TREQ_SIM_GET_LOCK_INFO,
	TREQ_SIM_GET_ICON,
	TREQ_SIM_SET_POWERSTATE,
	TREQ_SIM_GET_GID,
#if defined TIZEN_GLOBALCONFIG_ENABLE_CSP
	TREQ_SIM_GET_CPHS_CSP_INFO,
	TREQ_SIM_SET_CPHS_CSP_INFO,
#endif
	TREQ_SIM_GET_IMPI,
	TREQ_SIM_GET_IMPU,
	TREQ_SIM_GET_DOMAIN,
	TREQ_SIM_GET_PCSCF,
	TREQ_SIM_GET_ISIM_SERVICE_TABLE,
	TREQ_SIM_SET_PROVISIONING,

	TREQ_SAT = TCORE_REQUEST | TCORE_TYPE_SAT,
	TREQ_SAT_REQ_ENVELOPE,
	TREQ_SAT_REQ_TERMINALRESPONSE,
	TREQ_SAT_REQ_USERCONFIRMATION,

	TREQ_SAP = TCORE_REQUEST | TCORE_TYPE_SAP,
	TREQ_SAP_REQ_CONNECT,
	TREQ_SAP_REQ_DISCONNECT,
	TREQ_SAP_REQ_STATUS,
	TREQ_SAP_REQ_ATR,
	TREQ_SAP_TRANSFER_APDU,
	TREQ_SAP_SET_PROTOCOL,
	TREQ_SAP_SET_POWER,
	TREQ_SAP_REQ_CARDREADERSTATUS,

	TREQ_NETWORK = TCORE_REQUEST | TCORE_TYPE_NETWORK,
	TREQ_NETWORK_SEARCH,
	TREQ_NETWORK_SET_PLMN_SELECTION_MODE,
	TREQ_NETWORK_GET_PLMN_SELECTION_MODE,
	TREQ_NETWORK_SET_SERVICE_DOMAIN,
	TREQ_NETWORK_GET_SERVICE_DOMAIN,
	TREQ_NETWORK_SET_BAND,
	TREQ_NETWORK_GET_BAND,
	TREQ_NETWORK_SET_PREFERRED_PLMN,
	TREQ_NETWORK_GET_PREFERRED_PLMN,
	TREQ_NETWORK_SET_ORDER,
	TREQ_NETWORK_GET_ORDER,
	TREQ_NETWORK_SET_POWER_ON_ATTACH,
	TREQ_NETWORK_GET_POWER_ON_ATTACH,
	TREQ_NETWORK_SET_CANCEL_MANUAL_SEARCH,
	TREQ_NETWORK_GET_SERVING_NETWORK,
	TREQ_NETWORK_SET_MODE,
	TREQ_NETWORK_GET_MODE,
	TREQ_NETWORK_SET_NEIGHBORING_CELL_INFO,
	TREQ_NETWORK_GET_NEIGHBORING_CELL_INFO,
	TREQ_NETWORK_SET_DEFAULT_DATA_SUBSCRIPTION,
	TREQ_NETWORK_GET_DEFAULT_DATA_SUBSCRIPTION,
	TREQ_NETWORK_SET_DEFAULT_SUBSCRIPTION,
	TREQ_NETWORK_GET_DEFAULT_SUBSCRIPTION,
	TREQ_NETWORK_SET_EMERGENCY_CALLBACK_MODE,
	TREQ_NETWORK_SET_ROAMING_PREFERENCE,
	TREQ_NETWORK_GET_ROAMING_PREFERENCE,
	TREQ_NETWORK_GET_SUBSCRIPTION_INFO,
	TREQ_NETWORK_GET_SUBSCRIPTION_SOURCE,
	TREQ_NETWORK_SEARCH_ECC_RAT,
	TREQ_NETWORK_IMS_DEREGISTER,

	TREQ_PS = TCORE_REQUEST | TCORE_TYPE_PS,
	TREQ_PS_SET_PDP_ACTIVATE,
	TREQ_PS_SET_PDP_DEACTIVATE,

	TREQ_SMS = TCORE_REQUEST | TCORE_TYPE_SMS,
	TREQ_SMS_SEND_UMTS_MSG, /* Send the SMS-SUBMIT, SMS-COMMAND or SMS-SUBMIT-REPORT message */
	TREQ_SMS_READ_MSG, /* Read the message from the phone flash or SIM */
	TREQ_SMS_SAVE_MSG, /* Stores a message to memory storage */
	TREQ_SMS_DELETE_MSG, /* Deletes message from preferred message storage */
	TREQ_SMS_GET_COUNT, /* Get SMS Count Stored in Memory */
	TREQ_SMS_GET_SCA, /* Get SMS Service Center Address */
	TREQ_SMS_SET_SCA, /* Set SMS Service Center Address */
	TREQ_SMS_GET_CB_CONFIG, /* Get Cell Broadcast Configuration */
	TREQ_SMS_SET_CB_CONFIG, /* Set Cell Broadcast Configuration */
	TREQ_SMS_SET_MEM_STATUS, /* Memory Storage Status message*/
	TREQ_SMS_GET_PREF_BEARER, /* Get SMS Service Option like CS/PS bearer */
	TREQ_SMS_SET_PREF_BEARER, /* Set SMS Service Option like CS/PS bearer */
	TREQ_SMS_SET_DELIVERY_REPORT, /* Send the SMS-DELIVER-REPORT message to the network for the SMS-DELIVER message. */
	TREQ_SMS_SET_MSG_STATUS, /* SMS Status Stored in Memory message */
	TREQ_SMS_GET_PARAMS,
	TREQ_SMS_SET_PARAMS,
	TREQ_SMS_GET_PARAMCNT,
	TREQ_SMS_SEND_CDMA_MSG,

	TREQ_PHONEBOOK = TCORE_REQUEST | TCORE_TYPE_PHONEBOOK,
	TREQ_PHONEBOOK_GETCOUNT,
	TREQ_PHONEBOOK_GETMETAINFO,
	TREQ_PHONEBOOK_GETUSIMINFO,
	TREQ_PHONEBOOK_READRECORD,
	TREQ_PHONEBOOK_UPDATERECORD,
	TREQ_PHONEBOOK_DELETERECORD,

	TREQ_SOUND = TCORE_REQUEST | TCORE_TYPE_SOUND,

	TREQ_GPS = TCORE_REQUEST | TCORE_TYPE_GPS,
	TREQ_GPS_CONFIRM_MEASURE_POS,
	TREQ_GPS_SET_FREQUENCY_AIDING,
	TREQ_ENABLE_SMART_ASSISTANT,
	TREQ_DISABLE_SMART_ASSISTANT,
	TREQ_SYNC_SMART_ASSISTANT_AREA_LIST,
	TREQ_DEL_SMART_ASSISTANT_AREA_LIST,
	TREQ_ADD_SMART_ASSISTANT_AREA,
	TREQ_MODIFY_SMART_ASSISTANT_AREA,
	TREQ_SET_SMART_ASSISTANT_INFO,

	TREQ_CUSTOM = TCORE_REQUEST | TCORE_TYPE_CUSTOM,
};

struct treq_server_get_plugins {
};

__END_DECLS

#endif

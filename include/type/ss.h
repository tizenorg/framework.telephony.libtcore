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

#ifndef __TYPE_SS_H__
#define __TYPE_SS_H__

__BEGIN_DECLS

#include <glib.h>

enum telephony_ss_class {

	SS_CLASS_NONE=0x00,                               /* 0x00 */

	/* TELESERVICE */
	SS_CLASS_ALL_TELE=0x10,                        /* 0x10 : All Teleservices */
	SS_CLASS_VOICE=0x11,                              /* 0x11 : All Voice ( telephony ) */
	SS_CLASS_ALL_DATA_TELE=0x12,             /* 0x12 : All Data Teleservices */
	SS_CLASS_FAX=0x13,                                /* 0x13 : All Fax Service */
	SS_CLASS_SMS=0x16,                                /* 0x16 : SMS service	 */
	SS_CLASS_VGCS=0x17,                              /* 0x17 : Voice Group Call Service */
	SS_CLASS_VBS=0x18,                                /* 0x18 : Voice Broadcast */
	SS_CLASS_ALL_TELE_EXPT_SMS=0x19,    /* 0x19 : All teleservice except SMS */

	/* BEARER SERVICE */
	SS_CLASS_ALL_BEARER=0x20,                  /* 0X20 : all bearer services */
	SS_CLASS_ALL_ASYNC=0x21,                    /* 0x21 : All Async services */
	SS_CLASS_ALL_SYNC=0x22,                      /* 0x22 : All sync services*/
	SS_CLASS_ALL_CS_SYNC=0x24,                /* 0x24 : All Circuit switched sync */
	SS_CLASS_ALL_CS_ASYNC=0x25,              /* 0x25 : All Circuit switched async */
	SS_CLASS_ALL_DEDI_PS=0x26,                /* 0x26 : All Dedicated packet Access */
	SS_CLASS_ALL_DEDI_PAD=0x27,              /* 0x27 : All Dedicated PAD Access */
	SS_CLASS_ALL_DATA_CDA=0x28,		/*0x28 : All Data CDA*/  

	/* PLMN SPECIFIC TELESERVICE */
	SS_CLASS_PLMN_TELE_ALL = 0x50,         /*0x50 : PLMN specific teleservices*/
	SS_CLASS_PLMN_TELE_1 = 0x51,              /*0x51 :PLMN specific teleservice 1*/
	SS_CLASS_PLMN_TELE_2 = 0x52,             /*0x52 : PLMN specific teleservice 2*/
	SS_CLASS_PLMN_TELE_3 = 0x53,             /*0x53 : PLMN specific teleservice 3*/
	SS_CLASS_PLMN_TELE_4 = 0x54,             /*0x54 : PLMN specific teleservice 4*/
	SS_CLASS_PLMN_TELE_5 = 0x55,             /*0x55 : PLMN specific teleservice 5*/
	SS_CLASS_PLMN_TELE_6 = 0x56,             /*0x56 : PLMN specific teleservice 6*/
	SS_CLASS_PLMN_TELE_7 = 0x57,             /*0x57 : PLMN specific teleservice 7*/
	SS_CLASS_PLMN_TELE_8 = 0x58,             /*0x58 : PLMN specific teleservice 8*/
	SS_CLASS_PLMN_TELE_9 = 0x59,             /*0x59 : PLMN specific teleservice 9*/
	SS_CLASS_PLMN_TELE_A = 0x60,           /*0x60 :PLMN specific teleservice 10*/
	SS_CLASS_PLMN_TELE_B = 0x61,           /*0x61 :PLMN specific teleservice 11*/
	SS_CLASS_PLMN_TELE_C = 0x62,             /*0x62 : PLMN specific teleservice 12*/
	SS_CLASS_PLMN_TELE_D = 0x63,             /*0x63 : PLMN specific teleservice 13*/
	SS_CLASS_PLMN_TELE_E = 0x64,             /*0x64 : PLMN specific teleservice 14*/
	SS_CLASS_PLMN_TELE_F = 0x65,             /*0x65 : PLMN specific teleservice 15*/

	/* PLMN SPECIFIC BEARER SERVICE */
	SS_CLASS_PLMN_BEAR_ALL = 0x70,         /*0x70 : All PLMN specific bearer services*/
	SS_CLASS_PLMN_BEAR_1 = 0x71,              /*0x71 :PLMN specific bearer service 1*/
	SS_CLASS_PLMN_BEAR_2 = 0x72,             /*0x72 : PLMN specific bearer service  2*/
	SS_CLASS_PLMN_BEAR_3 = 0x73,             /*0x73 : PLMN specific bearer service  3*/
	SS_CLASS_PLMN_BEAR_4 = 0x74,             /*0x74 : PLMN specific bearer service  4*/
	SS_CLASS_PLMN_BEAR_5 = 0x75,             /*0x75 : PLMN specific bearer service  5*/
	SS_CLASS_PLMN_BEAR_6 = 0x76,             /*0x76 : PLMN specific bearer service  6*/
	SS_CLASS_PLMN_BEAR_7 = 0x77,             /*0x77 : PLMN specific bearer service  7*/
	SS_CLASS_PLMN_BEAR_8 = 0x78,             /*0x78 : PLMN specific bearer service  8*/
	SS_CLASS_PLMN_BEAR_9 = 0x79,             /*0x79 : PLMN specific bearer service  9*/
	SS_CLASS_PLMN_BEAR_A = 0x80,            /*0x80 : PLMN specific bearer service  10*/
	SS_CLASS_PLMN_BEAR_B = 0x81,             /*0x81 : PLMN specific bearer service  11*/
	SS_CLASS_PLMN_BEAR_C = 0x82,            /*0x82 : PLMN specific bearer service  12*/
	SS_CLASS_PLMN_BEAR_D = 0x83,            /*0x83 : PLMN specific bearer service  13*/
	SS_CLASS_PLMN_BEAR_E = 0x84,             /*0x84 : PLMN specific bearer service  14*/
	SS_CLASS_PLMN_BEAR_F = 0x85,             /*0x85 : PLMN specific bearer service  15*/

	/* CPHS - AUXILIARY SERVICE */
	SS_CLASS_AUX_VOICE = 0x89,			/* 0x89 : All Auxiliary Voice ( Auxiliary telephony ) */

	SS_CLASS_ALL_GPRS_BEARER=0x99,       /* 0x99 : All GPRS bearer services */
	SS_CLASS_ALL_TELE_BEARER=0xFF,        /* 0xFF : all tele and bearer services */
};

enum telephony_ss_status {
	SS_STATUS_REG=0x01,         /* 0x01 : Registration */
	SS_STATUS_DEREG,              /* 0x02 : De-registration( erase ) */
	SS_STATUS_ACTIVATE,        /* 0x03 : Activation */
	SS_STATUS_DEACTIVATE,    /* 0x04 : De-activation */
	SS_STATUS_MAX
};

enum telephony_ss_barring_mode {
	SS_BARR_MODE_NONE,
	SS_BARR_MODE_BAOC,                /* 0x01 : Barring All Outgoing Calls */
	SS_BARR_MODE_BOIC,                /* 0x02 : Barring Outgoing International Calls */
	SS_BARR_MODE_BOIC_NOT_HC, /* 0x03 : Barring Outgoing International Calls
								 except to Home Country */
	SS_BARR_MODE_BAIC,                /* 0x04 : Barring All Incoming Calls */
	SS_BARR_MODE_BIC_ROAM,       /* 0x05 : Barring Incoming Calls when roam, 
									outside of the Home Country */
	SS_BARR_MODE_AB,                   /* 0x06 : All Barring Services */
	SS_BARR_MODE_AOB,                 /* 0x07 : All Outgoing Barring Services */
	SS_BARR_MODE_AIB,                  /* 0x08 : All Incoming Barring Services */
	SS_BARR_MODE_BIC_NOT_SIM, /* 0x09 : Barring Incoming Calls which is
								 not stored in the SIM memory */
	SS_BARR_MODE_MAX
};

enum telephony_ss_forwarding_mode {
	SS_CF_MODE_CFU = 0x01, /* 0x01 : Call Forwarding Unconditional */
	SS_CF_MODE_CFB,        /* 0x02 : Call Forwarding Mobile Busy */
	SS_CF_MODE_CFNRy,      /* 0x03 : Call Forwarding No Reply */
	SS_CF_MODE_CFNRc,      /* 0x04 : Call Forwarding Not Reachable */
	SS_CF_MODE_CF_ALL,     /* 0x05 : All Call Forwarding */
	SS_CF_MODE_CFC,        /* 0x06 : All Conditional Call Forwarding */
	SS_CF_MODE_MAX         /* 0x07 : Max */
};

enum telephony_ss_forwarding_no_reply_time {
	SS_CF_NO_REPLY_5_SEC	= 5,
	SS_CF_NO_REPLY_10_SEC	= 10,
	SS_CF_NO_REPLY_15_SEC	= 15,
	SS_CF_NO_REPLY_20_SEC	= 20,
	SS_CF_NO_REPLY_25_SEC	= 25,
	SS_CF_NO_REPLY_30_SEC	= 30,
};

enum telephony_ss_cli_type {
	SS_CLI_TYPE_NONE,
	SS_CLI_TYPE_CLIP,       /* 0x01 : Calling Line Identification Presentation */
	SS_CLI_TYPE_CLIR,       /* 0x02 : Calling Line Identification Restriction */
	SS_CLI_TYPE_COLP,      /* 0x03 : Connected Line Identification Presentation */
	SS_CLI_TYPE_COLR,      /* 0x04 : Connected Line Identification Restriction */
	SS_CLI_TYPE_CDIP,       /* 0x05 : Called Line Identification Presentation */
	SS_CLI_TYPE_CNAP,      /* 0x06 : Calling Name Presentation */
	SS_CLI_TYPE_MAX
};

enum telephony_ss_ussd_type {
	SS_USSD_TYPE_USER_INITIATED=0x01,   /* User Initiated USSD Message */
	SS_USSD_TYPE_USER_RES,                       /* User Response to Network Initiated Message */
	SS_USSD_TYPE_USER_RELEASE,               /* SS Termination by user */
	SS_USSD_TYPE_MAX,
};

enum telephony_ss_ussd_status {
	SS_USSD_NO_ACTION_REQUIRE = 0x01,  /* 0x01 : no further user action required
											  information needed after mobile initiated operation) */
	SS_USSD_ACTION_REQUIRE,                     /* 0x02 : further user action required
													   (network initiated USSD Request, or further
													   information needed after mobile initiated operation) */
	SS_USSD_TERMINATED_BY_NET,              /* 0x03 : USSD terminated by network */
	SS_USSD_OTHER_CLIENT,                         /* 0x04 : other local client has responded */
	SS_USSD_NOT_SUPPORT,                          /* 0x05 : operation not supported */
	SS_USSD_TIME_OUT,                                 /* 0x06 : network time out */
	SS_USSD_MAX
};

enum telephony_ss_aoc_type {
	SS_AOC_TYPE_RESET		=0x00,		/* AoC Reset Message */
	SS_AOC_TYPE_ACM			=0x01,		/* Accumulated call meter Message */
	SS_AOC_TYPE_CCM			=0x02,		/* Current call meter Message */
	SS_AOC_TYPE_MAXACM	=0x04,		/* Max Accumulated call meter Message */
	SS_AOC_TYPE_PUC			=0x08,		/* Price per unit and currency Message */
	SS_AOC_TYPE_MAX		=0x10
};

#define MAX_SS_BARRING_PASSWORD_LEN 4
struct treq_ss_barring {
	enum telephony_ss_class class;
	enum telephony_ss_barring_mode mode;
	char password[ MAX_SS_BARRING_PASSWORD_LEN ];
};

struct treq_ss_barring_change_password {
	char password_old[ MAX_SS_BARRING_PASSWORD_LEN ];
	char password_new[ MAX_SS_BARRING_PASSWORD_LEN ];
	char password_confirm[ MAX_SS_BARRING_PASSWORD_LEN ];
};

#define MAX_SS_FORWARDING_NUMBER_LEN 32
struct treq_ss_forwarding {
	enum telephony_ss_class class;
	enum telephony_ss_forwarding_mode mode;
	enum telephony_ss_forwarding_no_reply_time time;
	char number[ MAX_SS_FORWARDING_NUMBER_LEN ];
};

struct treq_ss_waiting {
	enum telephony_ss_class class;

};

struct treq_ss_cli {
	enum telephony_ss_cli_type type;
};

#define MAX_SS_USSD_LEN 208
struct treq_ss_ussd {
	enum telephony_ss_ussd_type type;
	char str[ MAX_SS_USSD_LEN ];
};

// response

struct tresp_ss_general {
	gboolean err;
};

struct tresp_ss_barring {
	int record_num;
	struct barring_info {
		enum telephony_ss_class class;
		enum telephony_ss_status status;
		enum telephony_ss_barring_mode mode;
	} *record;
	gboolean err;
};

struct tresp_ss_forwarding {
	int record_num;
	struct forwarding_info {
		enum telephony_ss_class class;
		enum telephony_ss_status status;
		enum telephony_ss_forwarding_mode mode;
		enum telephony_ss_forwarding_no_reply_time time;
		gboolean number_present;
		int		 number_type;
		char	 number[ MAX_SS_FORWARDING_NUMBER_LEN ];
	} *record;
	gboolean err;
};

struct tresp_ss_waiting {
	int record_num;
	struct waiting_info {
		enum telephony_ss_class class;
		enum telephony_ss_status status;
	} *record;
	gboolean err;
};

struct tresp_ss_cli {
	enum telephony_ss_cli_type type;
	gboolean status;
	gboolean err;
};

struct tresp_ss_ussd {
	enum telephony_ss_ussd_type type;
	enum telephony_ss_ussd_status status;
	char str[ MAX_SS_USSD_LEN ];
	gboolean err;
};



// notification

struct tnoti_ss_ussd {
	enum telephony_ss_ussd_status status;
	char str[ MAX_SS_USSD_LEN ];
};

__END_DECLS

#endif

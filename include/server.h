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

#ifndef __TCORE_SERVER_H__
#define __TCORE_SERVER_H__

__BEGIN_DECLS

typedef enum tcore_hook_return (*tcore_server_request_hook)(Server *s,
		UserRequest *ur, void *user_data);
typedef enum tcore_hook_return (*tcore_server_notification_hook)(Server *s,
		CoreObject *source, enum tcore_notification_command command,
		unsigned int data_len, void *data, void *user_data);

Server *tcore_server_new();
void tcore_server_free(Server *s);

TReturn tcore_server_run(Server *s);
TReturn tcore_server_exit(Server *s);

TReturn tcore_server_add_plugin(Server *s, TcorePlugin *plugin);
GSList *tcore_server_ref_plugins(Server *s);
TcorePlugin *tcore_server_find_plugin(Server *s, const char *name);

TReturn tcore_server_add_communicator(Server *s, Communicator *comm);
GSList *tcore_server_ref_communicators(Server *s);
Communicator *tcore_server_find_communicator(Server *s, const char *name);

TReturn tcore_server_add_storage(Server *s, Storage *strg);
GSList *tcore_server_ref_storages(Server *s);
Storage * tcore_server_find_storage(Server *s, const char *name);

TReturn tcore_server_add_template_object(Server *s, CoreObject *template_co);
GSList *tcore_server_ref_template_object(Server *s);
CoreObject *tcore_server_find_template_object(Server *s, unsigned int type);

TReturn tcore_server_link_udev(Server *s, TcoreUdev *udev);
TcoreUdev *tcore_server_ref_udev(Server *s);

TReturn tcore_server_dispatch_request(Server *s, UserRequest *ur);
TReturn tcore_server_send_notification(Server *s, CoreObject *source,
			enum tcore_notification_command command,
			unsigned int data_len, void *data);

TReturn tcore_server_add_request_hook(Server *s,
			enum tcore_request_command command,
			tcore_server_request_hook func, void *user_data);
TReturn tcore_server_remove_request_hook(Server *s,
			tcore_server_request_hook func);
TReturn tcore_server_add_notification_hook(Server *s,
			enum tcore_notification_command command,
			tcore_server_notification_hook func, void *user_data);
TReturn tcore_server_remove_notification_hook(Server *s,
			tcore_server_notification_hook func);

gboolean tcore_server_register_modem(Server *s, TcorePlugin *modem_iface_plugin);
void tcore_server_unregister_modem(Server *s, TcorePlugin *modem_iface_plugin);
gboolean tcore_server_update_modem_plugin(TcorePlugin *modem_iface_plugin,
							TcorePlugin *modem_plugin);
GSList *tcore_server_get_cp_name_list(Server *s);
const char *tcore_server_get_cp_name_by_plugin(TcorePlugin *plugin);

gboolean tcore_server_add_cp_mapping_tbl_entry(TcorePlugin *modem_iface_plugin,
					unsigned int co_type, TcoreHal *hal);
void tcore_server_remove_cp_mapping_tbl(TcorePlugin *modem_iface_plugin);
void tcore_server_remove_cp_mapping_tbl_entry(TcorePlugin *modem_iface_plugin,
					TcoreHal *hal);
void *tcore_server_get_cp_mapping_tbl(TcorePlugin *modem_plugin);

void tcore_server_print_modems(TcorePlugin *plugin);

TReturn tcore_server_load_modem_plugin(Server *s,
					TcorePlugin *modem_if_plugin,
					const char *name);
void tcore_server_unload_modem_plugin(Server *s, TcorePlugin *modem_if_plugin);


__END_DECLS

#endif

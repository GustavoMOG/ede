/*
 * $Id: Bugzilla.cpp 3445 2012-11-07 12:49:43Z karijes $
 *
 * ede-bug-report, a tool to report bugs on EDE bugzilla instance
 * Part of Equinox Desktop Environment (EDE).
 * Copyright (c) 2009 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>
#include <edelib/Debug.h>
#include <edelib/String.h>

#include "Bugzilla.h"

EDELIB_NS_USING(String)

/* must be static; don't ask me why */
static struct xmlrpc_clientparms     gparms;
static struct xmlrpc_curl_xportparms gparms_curl;

struct BugzillaData {
	xmlrpc_env      xenv;
	xmlrpc_client  *xcli;
	String          url;

	BugzillaErrorCallback cb;
	void                  *cb_data;
};

BugzillaData *bugzilla_new(const char *url, BugzillaErrorCallback cb, void *cb_data) {
	BugzillaData *data = new BugzillaData;
	bugzilla_set_error_callback(data, cb, cb_data);

	xmlrpc_env_init(&data->xenv);
	xmlrpc_client_setup_global_const(&data->xenv);
	data->url = url;

	/* 
	 * to allow https connections; curl by default refuse connections without valid certificate
	 * (at least docs says that ;))
	 */
	gparms_curl.no_ssl_verifypeer = 1;
	gparms_curl.no_ssl_verifyhost = 1;

	/* only curl transport is supported */
	gparms.transport = "curl";
	gparms.transportparmsP = &gparms_curl;

	xmlrpc_client_create(&data->xenv, XMLRPC_CLIENT_NO_FLAGS, "ede-bug-report", "0.1", &gparms, sizeof(gparms), &data->xcli);
	if(data->xenv.fault_occurred) {
		E_WARNING(E_STRLOC ": Unable to init xmlrpc client data (%s)\n", data->xenv.fault_string);
		if(cb) cb(data->xenv.fault_string, cb_data);
		delete data;
		data = NULL;
	}

	return data;
}

void bugzilla_free(BugzillaData *data) {
	if(data == NULL)
		return;

	xmlrpc_client_destroy(data->xcli);
	xmlrpc_env_clean(&data->xenv);

	delete data;
}

void bugzilla_set_error_callback(BugzillaData *data, BugzillaErrorCallback cb, void *cb_data) {
	E_ASSERT(data != NULL);
	data->cb = cb;
	data->cb_data = cb_data;
}

char *bugzilla_get_version(BugzillaData *data) {
	E_ASSERT(data != NULL);

	xmlrpc_value *result;
	xmlrpc_client_call2f(&data->xenv, data->xcli, data->url.c_str(), "Bugzilla.version", &result, "()");

	if(data->xenv.fault_occurred) {
		E_WARNING(E_STRLOC ": Unable to call xmlrpc function (%s)\n", data->xenv.fault_string);
		if(data->cb) (data->cb)(data->xenv.fault_string, data->cb_data);
		return (char*)"";
	}

	/* this value will be malloc()-ated by xmlrpc_decompose_value() and should be freeed by user */
	char *ret;
	xmlrpc_decompose_value(&data->xenv, result, "{s:s,*}", "version", &ret);
	xmlrpc_DECREF(result);

	return ret;
}

int bugzilla_login(BugzillaData *data, const char *user, const char *passwd) {
	E_ASSERT(data != NULL);

	xmlrpc_value *result;
	int remember = 0;  /* remember login data */
	int id = -1; /* bugzilla user id */

	xmlrpc_client_call2f(&data->xenv, data->xcli, data->url.c_str(), "User.login", &result,
			"({s:s,s:s,s:b})",
			"login", user,
			"password", passwd,
			"remember", remember);

	if(data->xenv.fault_occurred) {
		E_WARNING(E_STRLOC ": Unable to perform login function (%s)\n", data->xenv.fault_string);
		if(data->cb) (data->cb)(data->xenv.fault_string, data->cb_data);
		return id;
	}

	xmlrpc_decompose_value(&data->xenv, result, "{s:i,*}", "id", &id);
	xmlrpc_DECREF(result);

	return id;
}

void bugzilla_logout(BugzillaData *data) {
	E_ASSERT(data != NULL);

	xmlrpc_value *result;
	xmlrpc_client_call2f(&data->xenv, data->xcli, data->url.c_str(), "User.logout", &result, "()");

	if(data->xenv.fault_occurred) {
		E_WARNING(E_STRLOC ": Unable to call xmlrpc function (%s)\n", data->xenv.fault_string);
		if(data->cb) (data->cb)(data->xenv.fault_string, data->cb_data);
		return;
	}

	xmlrpc_DECREF(result);
}

int bugzilla_submit_bug(BugzillaData *data,
						const char *product,
						const char *component,
						const char *summary,
						const char *version,
						const char *description,
						const char *op_sys,
						const char *platform,
						const char *priority,
						const char *severity,
						const char *cc)
{
	E_ASSERT(data != NULL);

	int bug_id = -1;
	xmlrpc_value *result;

	/* if given CC, add it so bugzilla assign that email as notification receiver */
	if(cc) {
		/* as for now we only support single email address */
		xmlrpc_value *cc_array, *cc_str;

		cc_array = xmlrpc_array_new(&data->xenv);
		cc_str = xmlrpc_string_new(&data->xenv, (const char *const)cc);
		xmlrpc_array_append_item(&data->xenv, cc_array, cc_str);
		xmlrpc_DECREF(cc_str);

		xmlrpc_client_call2f(&data->xenv, data->xcli, data->url.c_str(), "Bug.create", &result,
							 "({s:s,s:s,s:s,s:s,s:s,s:s,s:s,s:s,s:s,s:A})",
							 "product", product,
							 "component", component,
							 "summary", summary,
							 "version", version,
							 "description", description,
							 "op_sys", op_sys,
							 "platform", platform,
							 "priority", priority,
							 "severity", severity,
							 "cc", cc_array);
		xmlrpc_DECREF(cc_array);
	} else {
		xmlrpc_client_call2f(&data->xenv, data->xcli, data->url.c_str(), "Bug.create", &result,
							 "({s:s,s:s,s:s,s:s,s:s,s:s,s:s,s:s,s:s})",
							 "product", product,
							 "component", component,
							 "summary", summary,
							 "version", version,
							 "description", description,
							 "op_sys", op_sys,
							 "platform", platform,
							 "priority", priority,
							 "severity", severity);
	}

	if(data->xenv.fault_occurred) {
		E_WARNING(E_STRLOC ": Unable to perform submit function (%s)\n", data->xenv.fault_string);
		if(data->cb) (data->cb)(data->xenv.fault_string, data->cb_data);
		return bug_id;
	}

	xmlrpc_decompose_value(&data->xenv, result, "{s:i,*}", "id", &bug_id);
	xmlrpc_DECREF(result);

	return bug_id;
}

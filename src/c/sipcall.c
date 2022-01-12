#include "sipcall.h"

//#include <fcntl.h>
#include <pjsua-lib/pjsua.h>
//#include <pthread.h>
#include <signal.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>

#include "utility.h"

static void callbackCallStateChange(pjsua_call_id callId, pjsip_event *event);

static void sipCall(const sip_data *config, const pjsua_acc_id accId);
static void sipHangup();
static pjsua_acc_id sipRegister(const sip_data *config);
static void sipSetup();

////////////////////////////////////////////////////////////////////////////////////

// main application
int callSip(const sip_data *config) {
	if (config->domain == NULL)
		return -1;

	if (config->user == NULL || config->password == NULL)
		return -2;

	if (config->number == NULL)
		return -3;

	struct sigaction sig = {
		.sa_handler = &sipHangup,
		.sa_flags = SA_RESTART,
	};
	sigaction(SIGINT, &sig, NULL);

	// Set up pjsua library
	sipSetup();
	// Create account and register to SIP server
	pjsua_acc_id accId = sipRegister(config);
	// Call
	sipCall(config, accId);

	//pthread_mutex_t mutex;
	//pthread_cond_t cond;

	//pthread_mutex_init(&mutex, NULL);
	//pthread_cond_init(&cond, NULL);

	//pthread_mutex_lock(&mutex);
	//pthread_cond_wait(&mutex, &cond);
	//pthread_mutex_unlock(&mutex);

	//pthread_cond_destroy(&mutex);
	//pthread_mutex_destroy(&mutex);

	sleep(120);

	sipHangup();

	return 0;
}

static void callbackCallStateChange(pjsua_call_id callId, pjsip_event *event) {
	(void) event;

	pjsua_call_info ci;
	pjsua_call_get_info(callId, &ci);

	// check call state
	if (ci.state == PJSIP_INV_STATE_CONFIRMED) {
		printf("Call confirmed.\n");
	}

	if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
		printf("Call disconnected.\n");
		sipHangup();
	}
}

static void sipCall(const sip_data *config, const pjsua_acc_id accId) {
	char sip_target_url[strlen(config->number) + strlen(config->domain) + strlen("sip:@") + 1];

	sprintf(sip_target_url, "sip:%s@%s", config->number, config->domain);

	pjsua_call_setting set;
	pjsua_call_setting_default(&set);

	pj_str_t uri = pj_str(sip_target_url);
	if (pjsua_call_make_call(accId, &uri, &set, NULL, NULL, NULL) != PJ_SUCCESS)
		die("pjsua_call_make_call");
}

static void sipHangup() {
	pjsua_call_hangup_all();
	pjsua_destroy();
}

static pjsua_acc_id sipRegister(const sip_data *config) {
	pjsua_acc_config cfg;
	pjsua_acc_config_default(&cfg);

	char sip_user_url[strlen(config->user) + strlen(config->domain) + strlen("sip:@") + 1];
	char sip_provider_url[strlen(config->domain) + strlen("sip:") + 1];

	sprintf(sip_user_url, "sip:%s@%s", config->user, config->domain);
	sprintf(sip_provider_url, "sip:%s", config->domain);

	cfg.id = pj_str(sip_user_url);
	cfg.reg_uri = pj_str(sip_provider_url);
	cfg.cred_count = 1;
	cfg.cred_info[0].realm = pj_str(config->domain);
	cfg.cred_info[0].scheme = pj_str("digest");
	cfg.cred_info[0].username = pj_str(config->user);
	cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	cfg.cred_info[0].data = pj_str(config->password);

	pjsua_acc_id accId;
	if (pjsua_acc_add(&cfg, PJ_TRUE, &accId) != PJ_SUCCESS)
		die("pjsua_acc_add");

	return accId;
}

static void sipSetup() {
	// Create pjsua
	if (pjsua_create() != PJ_SUCCESS)
		die("pjsua_create");

	// configure pjsua
	pjsua_config cfg;
	pjsua_config_default(&cfg);

	// enable one simultaneous call
	cfg.max_calls = 1;

	// callback configuration
	cfg.cb.on_call_state = &callbackCallStateChange;

	// logging configuration
	pjsua_logging_config log_cfg;
	pjsua_logging_config_default(&log_cfg);
	log_cfg.console_level = 0;	// no logging

	// initialize pjsua
	if (pjsua_init(&cfg, &log_cfg, NULL) != PJ_SUCCESS)
		die("pjsua_init");

	// add UDP transport
	pjsua_transport_config udpcfg;
	pjsua_transport_config_default(&udpcfg);
	udpcfg.port = 5060;

	if (pjsua_transport_create(PJSIP_TRANSPORT_UDP, &udpcfg, NULL) != PJ_SUCCESS)
		die("pjsua_transport_create");

	// start pjsua
	if (pjsua_start() != PJ_SUCCESS)
		die("pjsua_start");

	// disable sound - use null sound device
	if (pjsua_set_null_snd_dev() != PJ_SUCCESS)
		die("pjsua_set_null_snd_dev");
}

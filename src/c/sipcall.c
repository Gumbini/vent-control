#include "sipcall.h"

#define PJ_AUTOCONF 1

#include <pjsua-lib/pjsua.h>
#include <signal.h>	 // sigaction()
#include <stdio.h>	 // snprintf()
#include <unistd.h>	 // sleep()

#include "utility.h"  // die()

static void callbackCallStateChange(pjsua_call_id callId, pjsip_event *event);

static void sipCall(const sipConfig_t *config, const pjsua_acc_id accId);
static void sipHangup();
static pjsua_acc_id sipRegister(const sipConfig_t *config);
static void sipSetup();

////////////////////////////////////////////////////////////////////////////////////

// main application
int callSip(const sipConfig_t *config) {
	if (config->host == NULL)
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

	sipCall(config, accId);
	sleep(120);
	sipHangup();

	return 0;
}

static void callbackCallStateChange(pjsua_call_id callId, pjsip_event *event) {
	(void) event;

	pjsua_call_info callInfo;
	pjsua_call_get_info(callId, &callInfo);

	// check call state
	if (callInfo.state == PJSIP_INV_STATE_CONFIRMED) {
		printf("Call confirmed.\n");
	}

	if (callInfo.state == PJSIP_INV_STATE_DISCONNECTED) {
		printf("Call disconnected.\n");
		sipHangup();
	}
}

static void sipCall(const sipConfig_t *config, const pjsua_acc_id accId) {
	char sipTargetUrl[strlen(config->number) + strlen(config->host) + strlen("sip:@") + 1];

	sprintf(sipTargetUrl, "sip:%s@%s", config->number, config->host);

	pjsua_call_setting callSetting;
	pjsua_call_setting_default(&callSetting);

	pj_str_t sipTargetUri = pj_str(sipTargetUrl);
	if (pjsua_call_make_call(accId, &sipTargetUri, &callSetting, NULL, NULL, NULL) != PJ_SUCCESS)
		die("pjsua_call_make_call");
}

static void sipHangup() {
	pjsua_call_hangup_all();
	pjsua_destroy();
}

static pjsua_acc_id sipRegister(const sipConfig_t *config) {
	pjsua_acc_config accCfg;
	pjsua_acc_config_default(&accCfg);

	char sipUserUrl[strlen(config->user) + strlen(config->host) + strlen("sip:@") + 1];
	char sipProviderUrl[strlen(config->host) + strlen("sip:") + 1];

	sprintf(sipUserUrl, "sip:%s@%s", config->user, config->host);
	sprintf(sipProviderUrl, "sip:%s", config->host);

	accCfg.id = pj_str(sipUserUrl);
	accCfg.reg_uri = pj_str(sipProviderUrl);
	accCfg.cred_count = 1;
	accCfg.cred_info[0].realm = pj_str(config->host);
	accCfg.cred_info[0].scheme = pj_str("digest");
	accCfg.cred_info[0].username = pj_str(config->user);
	accCfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	accCfg.cred_info[0].data = pj_str(config->password);

	pjsua_acc_id accId;
	if (pjsua_acc_add(&accCfg, PJ_TRUE, &accId) != PJ_SUCCESS)
		die("pjsua_acc_add");

	return accId;
}

static void sipSetup() {
	// Create pjsua
	if (pjsua_create() != PJ_SUCCESS)
		die("pjsua_create");

	// configure pjsua
	pjsua_config pjsuaCfg;
	pjsua_config_default(&pjsuaCfg);

	// enable one simultaneous call
	pjsuaCfg.max_calls = 1;

	// callback configuration
	pjsuaCfg.cb.on_call_state = &callbackCallStateChange;

	// logging configuration
	pjsua_logging_config logCfg;
	pjsua_logging_config_default(&logCfg);
	logCfg.console_level = 0;  // no logging

	// initialize pjsua
	if (pjsua_init(&pjsuaCfg, &logCfg, NULL) != PJ_SUCCESS)
		die("pjsua_init");

	// add UDP transport
	pjsua_transport_config udpCfg;
	pjsua_transport_config_default(&udpCfg);
	udpCfg.port = 5060;

	if (pjsua_transport_create(PJSIP_TRANSPORT_UDP, &udpCfg, NULL) != PJ_SUCCESS)
		die("pjsua_transport_create");

	// start pjsua
	if (pjsua_start() != PJ_SUCCESS)
		die("pjsua_start");

	// disable sound - use null sound device
	if (pjsua_set_null_snd_dev() != PJ_SUCCESS)
		die("pjsua_set_null_snd_dev");
}

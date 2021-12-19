#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "utility.h"

void callSip(const char *number) {
	const size_t len = 512;
	char command[len];

	snprintf(command, len,
			 "./sipcmd/sipcmd -P sip -u %s -c %s -w %s -x \"c%s;w5000;h;\"",
			 "USERNAME",
			 "PASSWORD",
			 "fritz.box",
			 number);

	pid_t child = fork();

	if (child == -1) {	// Error
		die("fork");
	}
	if (child == 0) {  // Child
		execl("/bin/sh", "sh", "-c", command, (char *) NULL);
		return;
	}

	// Parent

	sleep(20);
	//kill(child, SIGKILL);
	execl("/bin/sh", "sh", "-c", "pkill -SIGKILL sipcmd", (char *) NULL);
}

#include <ogc/system.h>

struct DWC_PatchList {
	char base_url[32];
	char naswii_url[32];
	char conntest_url[32];
	char pr_url[32];
	char dls_url[32];
	char gamespy_url[32];
	char req_method[9];
	char dns[16];
	u8 padding[7];
	char title[32];
};

#define SERVER_WIIMMFI 0
#define SERVER_TEST_WIIMMFI 1
#define SERVER_ALTWFC 2
#define SERVER_BEANJRFI 3
#define SERVER_LUMAFI 4
#define SERVER_NOSSL 5
#define SERVER_NOPATCHES 6
#define NUM_OF_SERVERS 7

const struct DWC_PatchList dwcPatchOptions[NUM_OF_SERVERS];

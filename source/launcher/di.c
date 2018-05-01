
#include "../wdvd.h"
#include "di.h"

/*
check_disc(): Reads the game ID of the currently inserted disc.
Returns the game ID; 1 if there's no disc inserted or 0 for read errors.
*/

u32 check_disc() {
	if (DiscInserted()) {
		WDVD_LowReadDiskId();
		u32 *gid = (u32*)0x80000000;
		return *gid;
	}
	else {
		return 1;
	}
}

bool DiscInserted(void) {
	bool cover;
	if (!WDVD_VerifyCover(&cover))
		return cover;
	return false;
}


#include "../wdvd.h"
#include "di.h"

/*
check_disc(): Reads the game ID of the currently inserted disc.
Returns the game ID; 1 if there's no disc inserted or 0 for read errors.
*/

u32 check_disc() {
	bool cover;
	WDVD_Reset();
	if (!WDVD_VerifyCover(&cover)) {
		WDVD_LowReadDiskId();
		u32 *gid = (u32*)0x80000000;
		return *gid;
	}
	else {
		return 1;
	}
}
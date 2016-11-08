#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <malloc.h>
#include <ogc/consol.h>
#include <ogc/lwp.h>
#include <ogc/video.h>

#include "core.h"
#include "apploader/apploader.c"
#include "library/event.h"
#include "library/dolphin_os.h"
#include "di/di.h"

static const char *regions[] = {"NTSC-U", "NTSC-J", "PAL", "NTSC-K"};
static const char *gid[] = {"RMCE", "RMCJ", "RMCP", "RMCK"};
int gm_region;

int core_init(void) {
	printf("link to core.c sucessful\n");
	
    if (!Apploader_Init())
        goto exit_error;
	printf("Checking disc...\n");

	Event_Wait(&apploader_event_disk_id);
	
	if (os0->disc.gamename==gid[3]) {
		printf("Korean support is not implemented!\n");
		Event_Wait(&apploader_event_complete);
		printf("\nPress Home to exit.\n");
		return 2;
	}
	else if (os0->disc.gamename!=gid[0] || os0->disc.gamename!=gid[1] || os0->disc.gamename!=gid[2]) {
		printf("Excuse me, princess! This isn't Mario Kart Wii!\n");
		Event_Wait(&apploader_event_complete);
		printf("\nPress Home to exit.\n");
		return 1;
	}
	else {
		if (os0->disc.gamename==gid[0]) {
			gm_region=0;
		}
		else if(os0->disc.gamename==gid[1]) {
			gm_region=1;
		}
		else if(os0->disc.gamename==gid[2]) {
			gm_region=2;
		}
		printf("MKW detected, starting patch process (%s)...",regions[gm_region]);
		Event_Wait(&apploader_event_complete);
		printf("\nPress Home to exit.\n");
		
		return 0;
	}
	exit_error:
		return -1;
}
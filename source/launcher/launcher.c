#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <malloc.h>
#include <ogc/consol.h>
#include <ogc/lwp.h>
#include <ogc/video.h>
#include "../wdvd.h"
#include "di.h"
#include "launcher.h"
#include "../version.h"

#define HOME_EXIT() { \
	WPAD_ScanPads(); \
	if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) { \
		if (*(vu32*)0x80001804 != 0x53545542) \
			SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0); \
		else \
			return 0; \
	} \
	VIDEO_WaitVSync(); \
}

static const char* regions[4] = {"NTSC-U", "NTSC-J", "PAL", "NTSC-K"};
static const u32 debug_gids[4] = {0x524d4345, 0x52534245, 0x534f5545, 0x525a4445};
static const u32 release_gids[4] = {0x524d4345, 0x524d434a, 0x524d4350, 0x524d434b};
u32 game_id;
int gm_region;

int launch() {
	u32 gids[4];
	if (debug_build) {
		gids[0] = debug_gids[0];
		gids[1] = debug_gids[1];
		gids[2] = debug_gids[2];
		gids[3] = debug_gids[3];
	}
	else {
		gids[0] = release_gids[0];
		gids[1] = release_gids[1];
		gids[2] = release_gids[2];
		gids[3] = release_gids[3];
	}

	printf("Init DI...\n");
	
	WDVD_Init();

	if (!DiscInserted()) {
		printf("Please insert a Mario Kart Wii Game Disc.\n\n");
	}
	while (!DiscInserted()) {
		HOME_EXIT();
	}

	printf("Checking disc...\n");

	WDVD_Reset();
	
	game_id=check_disc();

	static const u32 disc_error=0;
	static const u32 no_disc=1;
		
	if (game_id==disc_error) {
		printf("Disc read error!\n\n");
		return -1;
	}
	else if (game_id==no_disc) {
		printf("No disc is inserted.\n\n");
		return 1;
	}
	else if ((game_id != gids[REGION_AMERICA] && game_id != gids[REGION_JAPAN] && game_id != gids[REGION_EUROPE] && game_id != gids[REGION_KOREA])) {
		printf("Excuse me, princess! This isn't Mario Kart Wii!\n\n");
		return 2;
	}
	else if (game_id==gids[REGION_KOREA]) {
		printf("Korean support is not implemented!\n\n");
		return 3;
	}
	else {
		if (game_id==gids[REGION_AMERICA]) {
			gm_region=REGION_AMERICA;
		}
		else if(game_id==gids[REGION_JAPAN]) {
			gm_region=REGION_JAPAN;
		}
		else if(game_id==gids[REGION_EUROPE]) {
			gm_region=REGION_EUROPE;
		}
		else if(game_id==gids[REGION_KOREA]) {
			gm_region=REGION_KOREA;
		}
		else {
			return -1;
		}
		printf("MKW detected, starting patch process (%s)...",regions[gm_region]);
		return 0;
	}
	return -1;
}

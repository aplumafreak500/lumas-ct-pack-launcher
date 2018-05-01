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

static const char regions[][6] = {"NTSC-U", "NTSC-J", "PAL", "NTSC-K"};

char game_id[4];

int gm_region;

int launch() {
	static const char* gids;
	static const char debug_gids[4][4] = {"RMCE", "RMCJ", "RMCP", "RMCK"};
	static const char release_gids[4][4] = {"RMCE", "RSBE", "SOUE", "RZDE"};
	if (debug_build) {
		gids = *debug_gids;
	}
	else {
		gids = *release_gids;
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
	
	char game_id=(char) check_disc();

	static const char disc_error[4]="\x00\x00\x00\x00";
	static const char no_disc[4]="\x00\x00\x00\x01";
		
	if (&game_id==disc_error) {
		printf("Disc read error!\n\n");
		return -1;
	}
	else if (&game_id==no_disc) {
		printf("No disc is inserted.\n\n");
		return 1;
	}
	else if ((&game_id!=&gids[REGION_AMERICA] && &game_id!=&gids[REGION_JAPAN] && &game_id!=&gids[REGION_EUROPE] && &game_id !=&gids[REGION_KOREA]) && !debug_build) {
		printf("Excuse me, princess! This isn't Mario Kart Wii!\n\n");
		return 2;
	}
	else if (&game_id==&gids[REGION_KOREA]) {
		printf("Korean support is not implemented!\n\n");
		return 3;
	}
	else {
		if (&game_id==&gids[REGION_AMERICA]) {
			gm_region=REGION_AMERICA;
		}
		else if(&game_id==&gids[REGION_JAPAN]) {
			gm_region=REGION_JAPAN;
		}
		else if(&game_id==&gids[REGION_EUROPE]) {
			gm_region=REGION_EUROPE;
		}
		else if(&game_id==&gids[REGION_KOREA]) {
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

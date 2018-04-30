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
#include "../patch/dwc.h"

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

bool DiscInserted(void) {
	bool cover;
	if (!WDVD_VerifyCover(&cover))
		return cover;
	return false;
}

int launch() {
	const char* gids;
	const char release_gids[4][4] = {"RMCE", "RMCJ", "RMCP", "RMCK"};
	const char debug_gids[4][4] = {"RMCE", "RSBE", "SOUE", "RZDE"};
	if (debug_build) {
		gids = *debug_gids;
	}
	else {
		gids = *release_gids;
	}
	printf("Init DI...\n");
	
reinsert_disc:
	
	WDVD_Init();

	if (!DiscInserted()) {
		printf("Please insert a Mario Kart Wii Game Disc.\n\n");
	}
	while (!DiscInserted()) {
		HOME_EXIT();
	}

	printf("Checking disc...\n");
	
	char game_id=(char) check_disc();

	char disc_error[4]="\x00\x00\x00\x00";
	char no_disc[4]="\x00\x00\x00\x01";
	
	// int read_try=0;
		
	if (&game_id==disc_error) {
		printf("Disc read error!\n\n");
		/*
		read_try++;
		if (read_try==3) {
			//  todo: eject the disc
		}
		*/
		goto reinsert_disc;
	}
	else if (&game_id==no_disc) {
		// We would've printf()'d here but it would result in a dupe message.
		//  todo: eject the disc
		goto reinsert_disc;
	}
	else if ((&game_id!=&gids[REGION_AMERICA] && &game_id!=&gids[REGION_JAPAN] && &game_id!=&gids[REGION_EUROPE] && &game_id !=&gids[REGION_KOREA]) && !debug_build) {
		printf("Excuse me, princess! This isn't Mario Kart Wii!\n\n");
		//  todo: eject the disc
		goto reinsert_disc;
	}
	else if (&game_id==&gids[REGION_KOREA]) {
		printf("Korean support is not implemented!\n\n");
		//  todo: eject the disc
		goto reinsert_disc;
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

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

static const char* regions[] = {"NTSC-U", "NTSC-J", "PAL", "NTSC-K"};

u32 game_id;

int gm_region;

bool DiscInserted(void) {
	bool cover;
	if (!WDVD_VerifyCover(&cover))
		return cover;
	return false;
}

int launch() {
	u32 gids[3];
	if (debug_build) {
		gids[0]=0x524d4345; //RMCE
		gids[1]=0x52534245; //RSBE
		gids[2]=0x534f5545; //SOUE
		gids[3]=0x525a4445; //RZDE
	}
	else {
		gids[0]=0x524d4345; //RMCE
		gids[1]=0x524d434a; //RMCJ
		gids[2]=0x524d4350; //RMCP
		gids[3]=0x524d434b; //RMCK
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
	
	game_id=check_disc();
	
	// int read_try=0;
		
	if (game_id==0x0) {
		printf("Disc read error!\n\n");
		/*
		read_try++;
		if (read_try==3) {
			//  todo: eject the disc
		}
		*/
		goto reinsert_disc;
	}
	else if (game_id==0x1) {
		// We would've printf()'d here but it would result in a dupe message.
		//  todo: eject the disc
		goto reinsert_disc;
	}
	else if ((game_id!=gids[0] && game_id!=gids[1] && game_id!=gids[2] && game_id !=gids[3]) && !debug_build) {
		printf("Excuse me, princess! This isn't Mario Kart Wii!\n\n");
		//  todo: eject the disc
		goto reinsert_disc;
	}
	else if (game_id==gids[3]) {
		printf("Korean support is not implemented!\n\n");
		//  todo: eject the disc
		goto reinsert_disc;
	}
	else {
		if (game_id==gids[0]) {
			gm_region=0;
		}
		else if(game_id==gids[1]) {
			gm_region=1;
		}
		else if(game_id==gids[2]) {
			gm_region=2;
		}
		else if(game_id==gids[3]) {
			gm_region=3;
		}
		else {
			return -1;
		}
		printf("MKW detected, starting patch process (%s)...",regions[gm_region]);
		return 0;
	}
	return -1;
}
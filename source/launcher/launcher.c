#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <string.h>
#include <wiiuse/wpad.h>

#include "di.h"
#include "launcher.h"
#include "version.h"
#include "apploader/apploader.h"
#include "library/dolphin_os.h"
#include "threads.h"
#include "error.h"

const char* regions[4] = {
		[REGION_AMERICA] = "NTSC-U", 
		[REGION_JAPAN] = "NTSC-J", 
		[REGION_EUROPE] = "PAL", 
		[REGION_KOREA] = "NTSC-K"
	};
const char* debug_gids[4] = {
		[REGION_AMERICA] = "RMCE", 
		[REGION_JAPAN] = "RMGE", 
		[REGION_EUROPE] = "SB4E", 
		[REGION_KOREA] = "SOUE"
	};
const char* release_gids[4] = {
		[REGION_AMERICA] = "RMCE", 
		[REGION_JAPAN] = "RMCJ", 
		[REGION_EUROPE] = "RMCP", 
		[REGION_KOREA] = "RMCK"
	};

int launch() {
	char* game_id;
	int gm_region;
	const char* gids[4];
	if (debug_build) {
		gids[REGION_AMERICA] = debug_gids[REGION_AMERICA];
		gids[REGION_JAPAN] = debug_gids[REGION_JAPAN];
		gids[REGION_EUROPE] = debug_gids[REGION_EUROPE];
		gids[REGION_KOREA] = debug_gids[REGION_KOREA];
	}
	else {
		gids[REGION_AMERICA] = release_gids[REGION_AMERICA];
		gids[REGION_JAPAN] = release_gids[REGION_JAPAN];
		gids[REGION_EUROPE] = release_gids[REGION_EUROPE];
		gids[REGION_KOREA] = release_gids[REGION_KOREA];
	}

	printf("Init DI...\n");
	
	Event_Wait(&apploader_event_disk_id);
/*
	if (!DiscInserted()) {
		printf("Please insert a Mario Kart Wii Game Disc.\n\n");
	}
	while (!DiscInserted()) {
		HOME_EXIT();
	}

	printf("Checking disc...\n");
	
*/
	game_id=os0->disc.gamename;
/*
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
*/
	if (strcmp(game_id, gids[REGION_AMERICA]) && strcmp(game_id, gids[REGION_JAPAN]) && strcmp(game_id, gids[REGION_EUROPE]) && strcmp(game_id, gids[REGION_KOREA])) {
		printf("Excuse me, princess! This isn't Mario Kart Wii!\n\n");
		return EWRONGDISC;
	}
	else if (strcmp(game_id, gids[REGION_KOREA])) {
		printf("Korean support is not implemented!\n\n");
		return EDISCNOTSUPPORTED;
	}
	else {
		if (strcmp(game_id, gids[REGION_AMERICA])) {
			gm_region=REGION_AMERICA;
		}
		else if (strcmp(game_id, gids[REGION_JAPAN])) {
			gm_region=REGION_JAPAN;
		}
		else if (strcmp(game_id, gids[REGION_EUROPE])) {
			gm_region=REGION_EUROPE;
		}
		else if (strcmp(game_id, gids[REGION_KOREA])) {
			gm_region=REGION_KOREA;
		}
		else {
			return EUNDEFINEDERROR;
		}
		printf("MKW detected, starting patch process (%s)...",regions[gm_region]);
		// patches go here
		
		apploader_game_entry_fn();

	}
	return 0;
}

int HOME_EXIT() {
	WPAD_ScanPads();
	if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) {
		if (*(vu32*)0x80001804 != 0x53545542) {
			SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
		}
		else {
			return true;
		}
	}
	VIDEO_WaitVSync();
	return false;
}

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
	[REGION_AMERICA] = "NTSC-U\0",
	[REGION_JAPAN] = "NTSC-J\0",
	[REGION_EUROPE] = "PAL\0",
	[REGION_KOREA] = "NTSC-K\0"
};
const char* debug_gids[4] = {
	[REGION_AMERICA] = "RMCE\0",
	[REGION_JAPAN] = "RMGE\0",
	[REGION_EUROPE] = "SB4E\0",
	[REGION_KOREA] = "SOUE\0"
};
const char* release_gids[4] = {
	[REGION_AMERICA] = "RMCE\0",
	[REGION_JAPAN] = "RMCJ\0",
	[REGION_EUROPE] = "RMCP\0",
	[REGION_KOREA] = "RMCK\0"
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

	printf("Please insert a Mario Kart Wii Game Disc.\n\n");

	game_id=os0->disc.gamename;

	if (memcmp(game_id, gids[REGION_AMERICA], 4) && memcmp(game_id, gids[REGION_JAPAN], 4) && memcmp(game_id, gids[REGION_EUROPE], 4) && memcmp(game_id, gids[REGION_KOREA], 4)) {
		printf("Excuse me, princess! This isn't Mario Kart Wii!\n\n");
		return EWRONGDISC;
	}
	else if (memcmp(game_id, gids[REGION_KOREA], 4)) {
		printf("Korean support is not implemented!\n\n");
		return EDISCNOTSUPPORTED;
	}
	else {
		if (memcmp(game_id, gids[REGION_AMERICA], 4)) {
			gm_region=REGION_AMERICA;
		}
		else if (memcmp(game_id, gids[REGION_JAPAN], 4)) {
			gm_region=REGION_JAPAN;
		}
		else if (memcmp(game_id, gids[REGION_EUROPE], 4)) {
			gm_region=REGION_EUROPE;
		}
		else if (memcmp(game_id, gids[REGION_KOREA], 4)) {
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
void HOME_EXIT() {
	while (true) {
		WPAD_ScanPads(); // todo: GC, DRC, physical power button
		if (WPAD_ButtonsDown(0) & (WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME)) {
			if (*(vu32*)0x80001804 != 0x53545542) {
				SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
			}
			// else return to loader - nothing else needed here
			break;
		}
		VIDEO_WaitVSync();
	}
}

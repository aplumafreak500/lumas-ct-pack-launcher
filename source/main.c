#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <malloc.h>
#include <ogc/consol.h>
#include <ogc/lwp.h>
#include <ogc/video.h>

#include "wdvd.h"

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

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

static u32 gid[] = {0x524d4345,0x524d4350,0x524d434a,0x524d434b};
static const char* regions[] = {"NTSC-U", "NTSC-J", "PAL", "NTSC-K"};

u32* game_id=(u32*)0x80000000;

int gm_region;

bool DiscInserted(void) {
	bool cover;
	if (!WDVD_VerifyCover(&cover))
		return cover;
	return false;
}

int main(int argc, char **argv) {

	VIDEO_Init();
	WPAD_Init();

	rmode = VIDEO_GetPreferredMode(NULL);

	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
	printf("\x1b[2;0H");
	
	printf("Press Home to abort launch process...\n");
	
	WDVD_Init();
	printf("Checking disc...\n");

reinsert_disc:
	
	printf("Please insert a Mario Kart Wii Game Disc.\n");
	WDVD_Reset();
	while (!DiscInserted()) {
		HOME_EXIT();
	}
	WDVD_LowReadDiskId();
	if (*game_id==gid[3]) {
		printf("Korean support is not implemented!\n\n");
		goto reinsert_disc;
	}
	else if (*game_id!=gid[0] && *game_id!=gid[1] && *game_id!=gid[2]) {
		printf("Excuse me, princess! This isn't Mario Kart Wii!\n\n");
		goto reinsert_disc;
	}
	else {
		if (*game_id!=gid[0]) {
			gm_region=0;
		}
		else if(*game_id!=gid[0]) {
			gm_region=1;
		}
		else if(*game_id!=gid[0]) {
			gm_region=2;
		}
		printf("MKW detected, starting patch process (%s)...",regions[gm_region]);
		// file patches go here
	}
	while (true)
		HOME_EXIT();
	return 0;
}

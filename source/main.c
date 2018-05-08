#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <malloc.h>
#include <ogc/consol.h>
#include <ogc/lwp.h>
#include <ogc/video.h>

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

#include "launcher/launcher.h"
#include "version.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

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
	
	printf("Luma's CT Pack\nLauncher v%d.%d.%d.%d",version,subversion,revision,build);
	
	if (debug_build) printf(" DEBUG BUILD");
	printf("\n");
	
	u32 launch_status=launch();

	if (launch_status!=0) {
		printf("Press Home to exit...");
		while (true) HOME_EXIT();	
	}
	return 0;
}

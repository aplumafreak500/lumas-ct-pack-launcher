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
	
	int ret=core_init();
	
	while(1) {
		WPAD_ScanPads();

		u32 pressed = WPAD_ButtonsDown(0);

		if ( pressed & WPAD_BUTTON_HOME ) exit(0);

		VIDEO_WaitVSync();
	}

	return ret;
}

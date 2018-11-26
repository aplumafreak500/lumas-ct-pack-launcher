#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <malloc.h>
#include <ogc/consol.h>
#include <ogc/lwp.h>
#include <ogc/video.h>

#include "launcher/launcher.h"
#include "apploader/apploader.h"
#include "library/event.h"
#include "version.h"
#include "threads.h"
#include "error.h"

int main(void) {

	s16 ret;

	void *xfb = NULL;
	GXRModeObj *rmode = NULL;
	
	/* The game's boot loader is statically loaded at 0x81200000, so we'd better
	 * not start mallocing there! */
	SYS_SetArena1Hi((void *)0x81200000);
	
	/* initialise all subsystems */
//	if (!Event_Init(&main_event_fat_loaded)) {
//		goto exit;
//	}
	if (!Apploader_Init()) {
		ret=EAPPLOADERERR;
		goto exit;
	}
//	if (!Module_Init()) {
//		goto exit;
//	}
//	if (!Search_Init()) {
//		goto exit;
//	}

	/* main thread is UI, so set thread prior to UI */
	LWP_SetThreadPriority(LWP_GetSelf(), THREAD_PRIO_UI);

	/* configure the video */
	VIDEO_Init();
	
	/* init wiimotes */
	WPAD_Init();

	rmode = VIDEO_GetPreferredMode(NULL);

	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	if (!xfb) {
		ret=EXFBERR;
		goto exit;
	}
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	/* spawn lots of worker threads to do stuff */
	if (!Apploader_RunBackground()) {
		ret=EAPPLOADERERR;
		goto exit;
	}
//	if (!Module_RunBackground()) {
//		goto exit;
//	}
//	if (!Search_RunBackground()) {
//		goto exit;
//	}
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE){
		VIDEO_WaitVSync();
	}
	printf("\x1b[2;0H");
	
	printf("Luma's CT Pack\nLauncher v%d.%d.%d.%d",version,subversion,revision,build);
	
	if (debug_build) {
		printf(" DEBUG BUILD");
	}
	printf("\n");
	
	ret=launch();

exit:

	if (ret!=0) {
		printf("Press Home to exit...");
	}
	HOME_EXIT();
	VIDEO_SetBlack(true);
	VIDEO_Flush();
	VIDEO_WaitVSync();
    
	free(xfb);
        
	return ret;
}

#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <malloc.h>
#include <ogc/consol.h>
#include <ogc/lwp.h>
#include <ogc/video.h>

static const char *regions[];
static const char *gid[];
int gm_region;

int core_init(void);
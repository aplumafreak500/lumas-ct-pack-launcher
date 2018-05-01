#include "../wdvd.h"

#define REGION_AMERICA 0
#define REGION_JAPAN 1
#define REGION_EUROPE 2
#define REGION_KOREA 3

static const char regions[][6];
char game_id[4];
int gm_region;
int launch();

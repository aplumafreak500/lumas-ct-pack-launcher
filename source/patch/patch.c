#include <stdlib.h>
#include <stdio.h>
// #include <assert.h>

#include "patch.h"

void Main_PrintSize(size_t size) {
    static const char *suffix[] = { "bytes", "KiB", "MiB", "GiB" };
    unsigned int magnitude, precision;
    float sizef;

    sizef = size;
    magnitude = 0;
    while (sizef > 512) {
        sizef /= 1024.0f;
        magnitude++;
    }
    
    // assert(magnitude < 4);
    
    if (magnitude == 0)
        precision = 0;
    else if (sizef >= 100)
        precision = 0;
    else if (sizef >= 10)
        precision = 1;
    else
        precision = 2;
        
    printf("%.*f %s", precision, sizef, suffix[magnitude]);
}

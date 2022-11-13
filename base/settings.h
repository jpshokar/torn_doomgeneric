#ifndef SETTINGS_H
#define SETTINGS_H


#ifndef MAX_BATCHABLE_SIZE
#define MAX_BATCHABLE_SIZE 4096
#endif
#ifndef TORN_Error
#define TORN_Error(contents, title) OS_MessageBox(0, contents, title, OS_MessageBox_Error)
#endif

// You must provide a C-like extended formated printf function
// Otherwise, it will not print anything. (%p, %i, %s, %d) 

#ifdef TORN_VERBOSE
#ifndef TORN_LOG
#include <stdio.h>
#define TORN_Log printf
#else
#define TORN_Log
#endif
#else
#define TORN_Log
#endif

// UI
#define COLOR_BTN_G1_SELECTED  v4(60,60,60,255)
#define COLOR_BTN_G2_SELECTED  v4(60,60,60,255)

#define COLOR_BTN_G1_PRESSED   v4(30,30,30,255)
#define COLOR_BTN_G2_PRESSED   v4(30,30,30,255)

#define COLOR_BTN_G1_INACTIVE  v4(10,10,10,255)
#define COLOR_BTN_G2_INACTIVE  v4(10,10,10,255)




#endif

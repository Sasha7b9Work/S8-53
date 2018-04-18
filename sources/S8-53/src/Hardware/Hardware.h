#pragma once
#include "defines.h"


#ifdef __cplusplus
extern "C" {
#endif
    
void Hardware_Init();
    
#ifdef __cplusplus
}
#endif
    
uint Hardware_CalculateCRC32(uint address = 0x08020000, uint numBytes = 128 * 1024 * 3 / 4);


// #define HARDWARE_ERROR HardwareErrorHandler(__FILE__, __FUNCTION__, __LINE__);
// void HardwareErrorHandler(const char *file, const char *function, int line) {};

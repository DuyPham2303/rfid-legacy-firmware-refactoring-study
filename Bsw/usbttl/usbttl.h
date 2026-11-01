#ifndef USBTTL_H
#define USBTTL_H
#include <stdint.h>

void Usbttl_Init();
void Terminal_InitDebug(uint8_t totalID, const char *name, const char *car_id);
void Terminal_IdleDebug();
void Terminal_RunDebug(const char *name, const char *car_id);
void Terminal_ErrorDebug();

#endif
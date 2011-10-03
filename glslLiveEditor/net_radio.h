#ifndef _NET_RADIO_H_
#define _NET_RADIO_H_

#include <Windows.h>

void net_radio_init(HWND hwnd);
void net_radio_openUrl(const char* url);
bool net_radio_updateBuffering();
void net_radio_free();
void net_radio_getFftData1024Floats(float* data);

#endif

#pragma once

#include <dxgi1_4.h>

#ifdef __cplusplus
extern "C" {
#endif

void *CreateDCompContextFor(HWND hwnd, IDXGISwapChain3 *swapChain);

#ifdef __cplusplus
}
#endif

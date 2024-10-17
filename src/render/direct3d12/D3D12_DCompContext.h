#pragma once

#ifdef SDL_VIDEO_RENDER_D3D12

#include <dxgi1_4.h>

#ifdef __cplusplus
extern "C" {
#endif

void *CreateDCompContextFor(HWND hwnd, IDXGISwapChain3 *swapChain);

#ifdef __cplusplus
}
#endif

#endif // SDL_VIDEO_RENDER_D3D12

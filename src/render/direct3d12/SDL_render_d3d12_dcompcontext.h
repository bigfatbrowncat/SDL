#pragma once

#ifdef SDL_VIDEO_RENDER_D3D12

#include <dxgi1_4.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DCompContext DCompContext;

DCompContext *CreateDCompContextFor(HWND hwnd, IDXGISwapChain3 *swapChain);
void DestroyDCompContext(DCompContext* context);

#ifdef __cplusplus
}
#endif

#endif // SDL_VIDEO_RENDER_D3D12

/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2024 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_internal.h"

#include "SDL_hacks_gpu.h"

#if defined(SDL_VIDEO_RENDER_D3D12) || defined(SDL_VIDEO_RENDER_D3D11)

#include "core/windows/SDL_windows.h"

#if defined(_MSC_VER) && !defined(__clang__)
#define SDL_COMPOSE_ERROR(str) __FUNCTION__ ", " str
#else
#define SDL_COMPOSE_ERROR(str) SDL_STRINGIFY_ARG(__FUNCTION__) ", " str
#endif

#define DCOMP_SAFE_RELEASE(X)      \
    if (X) {                     \
        (X)->lpVtbl->Release(X); \
        X = NULL;                \
    }

// Finding the FIRST output of the adapter.
// It has been found experimentally, we need the first adapter output from the list.
bool HACK_FindFirstAdapterOutput(IDXGIAdapter* dxgiAdapter, IDXGIOutput** dxgiAdapterFirstOutput) {
    if (dxgiAdapter != NULL) {
        IDXGIOutput* output;
        for (UINT i = 0; dxgiAdapter->lpVtbl->EnumOutputs(dxgiAdapter, i, &output) != DXGI_ERROR_NOT_FOUND; i++) {
            DXGI_OUTPUT_DESC outputDesc;
            HRESULT result = output->lpVtbl->GetDesc(output, &outputDesc);
            if (FAILED(result)) {
                return WIN_SetErrorFromHRESULT(SDL_COMPOSE_ERROR("HACK_FindFirstAdapterOutput"), result);
            }

            if (outputDesc.Monitor != NULL) {
                *dxgiAdapterFirstOutput = output;
                break;
            } else {
                output->lpVtbl->Release(output);
            }
        }
    }
    return true;
}

bool HACK_SyncFirstAdapterOutput(IDXGIOutput** dxgiAdapterFirstOutput, bool freeOutput) {
    // Before doing the swapChain->Present(), waiting for the VBlank on the previously
    // found first GPU output. That almostly (99%) guarantees that there wouldn't be
    // a flicker after window resizing.
    if (*dxgiAdapterFirstOutput != NULL) {
        HRESULT result = (*dxgiAdapterFirstOutput)->lpVtbl->WaitForVBlank(*dxgiAdapterFirstOutput);
        if (FAILED(result)) {
            return WIN_SetErrorFromHRESULT(SDL_COMPOSE_ERROR("HACK_SyncFirstAdapterOutput"), result);
        }
        if (freeOutput) {
            DCOMP_SAFE_RELEASE(*dxgiAdapterFirstOutput); // Resetting the found adapter before the next resize
        }
        return true;
    }
    return false;
}

#endif // defined(SDL_VIDEO_RENDER_D3D12) || defined(SDL_VIDEO_RENDER_D3D11)
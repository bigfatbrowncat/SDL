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

bool HACK_LookForIntelOutput(IDXGIFactory2* dxgiFactory, IDXGIAdapter** intelAdapter, IDXGIOutput** intelAdapterFirstOutput) {
	//D3D12_RenderData *data = (D3D12_RenderData *)renderer->internal;

	// Intel iGPU driver is sometimes buggy in terms of frame synchronization,
	// specifically, it doesn't process swapChain->Present() well after the buffer
	// was just resized -- a flicker occurs. So we have to do a trick:

	// 1. Finding the Intel iGPU in the system
	if (*intelAdapter == NULL) {

		const UINT INTEL_VENDOR_ID = 0x8086;
		IDXGIAdapter* adapter;
		for (UINT i = 0; dxgiFactory->lpVtbl->EnumAdapters(dxgiFactory, i, &adapter) != DXGI_ERROR_NOT_FOUND; i++) {
			DXGI_ADAPTER_DESC desc;

			HRESULT res = adapter->lpVtbl->GetDesc(adapter, &desc);
			if (res != S_OK) { return false; }

			if (desc.VendorId == INTEL_VENDOR_ID) {
				*intelAdapter = adapter;
				break;
			}
		}
	}

	// 2. Finding the FIRST output of the adapter that has a display connected.
	//    It has been found experimentally, we need the first adapter output from the list.
	// (the next step is in the D3D12_SyncIntelOutput() function)
	if (*intelAdapter != NULL) {
		IDXGIOutput* output;
		for (UINT i = 0; (*intelAdapter)->lpVtbl->EnumOutputs(*intelAdapter, i, &output) != DXGI_ERROR_NOT_FOUND; i++) {
			DXGI_OUTPUT_DESC outputDesc;
			HRESULT result = output->lpVtbl->GetDesc(output, &outputDesc);
			if (FAILED(result)) {
				return WIN_SetErrorFromHRESULT(SDL_COMPOSE_ERROR("HACK_LookForIntelOutput"), result);
			}

			if (outputDesc.Monitor != NULL) {
				*intelAdapterFirstOutput = output;
				break;
			} else {
				output->lpVtbl->Release(output);
			}
		}
	}
	return true;
}

bool HACK_SyncIntelOutputIfPrepared(IDXGIOutput** intelAdapterFirstOutput, bool freeOutput) {
	// (the previous steps are in the D3D12_LookForIntelOutput() function)
	// 3. Before doing the swapChain->Present(), waiting for the VBlank on the previously
	//    found first iGPU output. That almostly (99%) guarantees that there wouldn't be
	//    a flicker after window resizing.
	if (*intelAdapterFirstOutput != NULL) {
		HRESULT result = (*intelAdapterFirstOutput)->lpVtbl->WaitForVBlank(*intelAdapterFirstOutput);
		if (FAILED(result)) {
			return WIN_SetErrorFromHRESULT(SDL_COMPOSE_ERROR("HACK_SyncIntelOutput"), result);
		}
		if (freeOutput) {
			DCOMP_SAFE_RELEASE(*intelAdapterFirstOutput); // Resetting the found adapter before the next resize
		}
		return true;
	}
	return false;
}

#endif // defined(SDL_VIDEO_RENDER_D3D12) || defined(SDL_VIDEO_RENDER_D3D11)
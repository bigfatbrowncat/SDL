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

#ifdef SDL_VIDEO_RENDER_D3D12

#include "SDL_render_d3d12_dcompcontext.h"

// OS headers
#include <dcomp.h>
//#include <Windows.h>

// C++ stl
#include <memory>

struct DCompContext {
	IDCompositionDevice* dcomp;
	IDCompositionTarget* target;
	IDCompositionVisual* visual;

	// Call this function immediately after the CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP, ...)
	DCompContext(HWND hwnd, IDXGISwapChain3* swapChain);

	// Crash if hr != S_OK.
	static void hr_check(HRESULT hr)
	{
		// Ignore the "occluded" state as a success
		if (hr == DXGI_STATUS_OCCLUDED) return;

		if (hr == S_OK) return;
		while (true) __debugbreak();
	}

	virtual ~DCompContext();
};

DCompContext::~DCompContext() {
    if (visual != nullptr) { visual->Release(); visual = nullptr; }
    if (target != nullptr) { target->Release(); target = nullptr; }
    if (dcomp != nullptr) { dcomp->Release(); dcomp = nullptr; }
}

DCompContext::DCompContext(HWND hwnd, IDXGISwapChain3* swapChain): dcomp(nullptr), target(nullptr), visual(nullptr) {
	// Bind our swap chain to the window.
	// TODO: Determine what DCompositionCreateDevice(nullptr, ...) actually does.
	// I assume it creates a minimal IDCompositionDevice for use with D3D that can't actually
	// do any adapter-specific resource allocations itself, but I'm yet to verify this.
	hr_check(DCompositionCreateDevice(nullptr, IID_PPV_ARGS(&dcomp)));
	hr_check(dcomp->CreateTargetForHwnd(hwnd, FALSE, &target));
	hr_check(dcomp->CreateVisual(&visual));
	hr_check(target->SetRoot(visual));
	hr_check(visual->SetContent(swapChain));
	hr_check(dcomp->Commit());
}

extern "C" {
	DCompContext* CreateDCompContextFor(HWND hwnd, IDXGISwapChain3* swapChain) {
		return new DCompContext(hwnd, swapChain);
	}

	void DestroyDCompContext(DCompContext* context) {
		delete context;
	}
}

#endif // SDL_VIDEO_RENDER_D3D12

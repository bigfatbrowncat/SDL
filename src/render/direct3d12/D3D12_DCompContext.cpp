#include "D3D12_DCompContext.h"

// OS headers
#include <dcomp.h>
#include <Windows.h>

// C++ stl
#include <memory>

struct DCompContext {
	IDCompositionDevice* dcomp;
	IDCompositionTarget* target;
	IDCompositionVisual* visual;

	// This function should NOT be called from the destructor, instead it has to be called in WM_DESTROY
	void unbind();

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

};

void DCompContext::unbind() {
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

extern "C" void* CreateDCompContextFor(HWND hwnd, IDXGISwapChain3* swapChain) {
	return (void*)(new DCompContext(hwnd, swapChain));
}
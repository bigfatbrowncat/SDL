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

#ifndef SDL_hacks_gpu_h_
#define SDL_hacks_gpu_h_

#include "SDL_internal.h"

#include "SDL_sysrender.h"

#if defined(SDL_VIDEO_RENDER_D3D12) || defined(SDL_VIDEO_RENDER_D3D11)

#ifdef __cplusplus
extern "C" {
#endif

#include <dxgi1_6.h>

bool HACK_FindFirstAdapterOutput(IDXGIAdapter* dxgiAdapter, IDXGIOutput** dxgiAdapterFirstOutput);

/*
  The function waits for VBlank on a passed adapter and frees it if freeOutput is true
  Returns true if the passed (*intelAdapterFirstOutput) is not NULL
 */
bool HACK_SyncFirstAdapterOutput(IDXGIOutput** dxgiAdapterFirstOutput, bool freeOutput);

#ifdef __cplusplus
}
#endif

#endif // defined(SDL_VIDEO_RENDER_D3D12) || defined(SDL_VIDEO_RENDER_D3D11)

#endif // SDL_hacks_gpu_h_
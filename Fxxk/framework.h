// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <pathcch.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <dxgi.h>
#include <WICTextureLoader.h>
#include <VertexTypes.h>
#include <ResourceUploadBatch.h>
#include <DescriptorHeap.h>
#include <DirectXHelpers.h>
#include <CommonStates.h>
#include <wil/result.h>

// C RunTime Header Files
#include <cstdlib>
#include <memory.h>
#include <tchar.h>
#include <cmath>

#include <cassert>
#include <string>
#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <initializer_list>


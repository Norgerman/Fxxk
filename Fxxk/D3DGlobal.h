#pragma once
#include <array>
#include <DirectXMath.h>
#include <d3d12.h>

class D3DScene;

namespace D3DGlobal {
    const std::array<float, 4> backgroundColour = { 1.0f, 1.0f, 1.0f, 1.0f };
    const D3D12_RASTERIZER_DESC defaultRasterizerStateDesc = {
            D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID,
            D3D12_CULL_MODE::D3D12_CULL_MODE_BACK,
            TRUE,
            0,
            0.0f,
            0.0f,
            TRUE,
            FALSE,
            FALSE,
            FALSE
    };
    DirectX::XMMATRIX project(D3DScene& scene);
}

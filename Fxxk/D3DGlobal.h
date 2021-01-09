#pragma once
#include <array>
#include <DirectXMath.h>
#include <d3d11.h>

class D3DScene;

namespace D3DGlobal {
    const std::array<float, 4> backgroundColour = { 1.0f, 1.0f, 1.0f, 1.0f };
    const D3D11_RASTERIZER_DESC defaultRasterizerState = {
            D3D11_FILL_MODE::D3D11_FILL_SOLID,
            D3D11_CULL_MODE::D3D11_CULL_BACK,
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

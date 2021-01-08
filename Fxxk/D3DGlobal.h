#pragma once
#include <array>
#include <DirectXMath.h>
#include <d3d11.h>

class D3DScene;

namespace D3DGlobal {
    const std::array<float, 4> backgroundColour = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMMATRIX project(D3DScene& scene);
}

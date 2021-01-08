#pragma once
#include <DirectXMath.h>
#include <array>

class D3DScene;

namespace D3DGlobal {
    const std::array<float, 4> background_colour = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMMATRIX project(D3DScene& scene);
}

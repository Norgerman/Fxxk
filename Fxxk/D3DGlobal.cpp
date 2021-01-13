#include "D3DGlobal.h"
#include "D3DScene.h"

DirectX::XMMATRIX D3DGlobal::Project(D3DScene& scene)
{
    float projectionMatrixData[] = {
           1 / scene.Viewport().Width * 2, 0.0f,  0.0f, 0.0f,
           0.0f, 1 / scene.Viewport().Height * 2,  0.0f, 0.0f,
           0.0f, 0.0f,  1.0f, 0.0f,
           -1.0f, -1.0f,  0.0f, 1.0f,
    };

    return DirectX::XMMATRIX(projectionMatrixData);
}

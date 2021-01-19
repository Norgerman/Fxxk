#include <D3DGlobal.h>
#include <D3DScene.h>

void DX::Global::Project(D3DScene& scene)
{
    float projectionMatrixData[] = {
           1 / scene.Viewport().Width * 2, 0.0f,  0.0f, 0.0f,
           0.0f, 1 / scene.Viewport().Height * 2,  0.0f, 0.0f,
           0.0f, 0.0f,  1.0f, 0.0f,
           -1.0f, -1.0f,  0.0f, 1.0f,
    };

    scene.UpdateProjection(DirectX::XMMATRIX(projectionMatrixData));
}

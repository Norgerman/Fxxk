#pragma once
#pragma unmanaged
#include <DirectXMath.h>

#ifdef _WIN32
#define CALLCONV __stdcall
#else
#define CALLCONV __fastcall
#endif

namespace DX 
{
    namespace Sharp
    {
        namespace SimpleMath
        {
            void CALLCONV setMatrix(DirectX::XMMATRIX& target,
                float m00, float m01, float m02, float m03,
                float m10, float m11, float m12, float m13,
                float m20, float m21, float m22, float m23,
                float m30, float m31, float m32, float m33
            );
            void CALLCONV copyMatrix(DirectX::XMMATRIX& target, const DirectX::XMMATRIX& src);
            void CALLCONV setIdentity(DirectX::XMMATRIX& target);
            void CALLCONV multiplyVector(DirectX::XMVECTOR& a, DirectX::XMVECTOR& b);
            void CALLCONV multiplyVector(DirectX::XMVECTOR& a, DirectX::XMVECTOR&& b);
            void CALLCONV multiplyRow(DirectX::XMMATRIX& target, size_t index, float x, float y, float z, float w);
            void CALLCONV multiplyMatrix(DirectX::XMMATRIX& target, DirectX::XMMATRIX& b);
            void CALLCONV rotationX(DirectX::XMMATRIX& target, float angle);
            void CALLCONV rotationY(DirectX::XMMATRIX& target, float angle);
            void CALLCONV rotationZ(DirectX::XMMATRIX& target, float angle);
            void CALLCONV rotationAxis(DirectX::XMMATRIX& target, float x, float y, float z, float angle);
            void CALLCONV rotationQuaternion(DirectX::XMMATRIX& target, float x, float y, float z, float w);
            void CALLCONV inverseMatrix(DirectX::XMMATRIX& target, DirectX::XMMATRIX& m);
        }
    }
}
#pragma managed

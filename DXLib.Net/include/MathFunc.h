#pragma once
#pragma unmanaged
#include <DirectXMath.h>

namespace DX 
{
    namespace Sharp
    {
        namespace SimpleMath
        {
            void __fastcall setMatrix(DirectX::XMMATRIX& target,
                float m00, float m01, float m02, float m03,
                float m10, float m11, float m12, float m13,
                float m20, float m21, float m22, float m23,
                float m30, float m31, float m32, float m33
            );
            void __fastcall copyMatrix(DirectX::XMMATRIX& target, const DirectX::XMMATRIX& src);
            void __fastcall setIdentity(DirectX::XMMATRIX& target);
            void __fastcall multiplyVector(DirectX::XMVECTOR& a, DirectX::XMVECTOR& b);
            void __fastcall multiplyVector(DirectX::XMVECTOR& a, DirectX::XMVECTOR&& b);
            void __fastcall multiplyRow(DirectX::XMMATRIX& target, size_t index, float x, float y, float z, float w);
            void __fastcall multiplyMatrix(DirectX::XMMATRIX& target, DirectX::XMMATRIX& b);
            void __fastcall rotationX(DirectX::XMMATRIX& target, float angle);
            void __fastcall rotationY(DirectX::XMMATRIX& target, float angle);
            void __fastcall rotationZ(DirectX::XMMATRIX& target, float angle);
            void __fastcall rotationAxis(DirectX::XMMATRIX& target, float x, float y, float z, float angle);
            void __fastcall rotationQuaternion(DirectX::XMMATRIX& target, float x, float y, float z, float w);
            void __fastcall inverseMatrix(DirectX::XMMATRIX& target, DirectX::XMMATRIX& m);
        }
    }
}
#pragma managed

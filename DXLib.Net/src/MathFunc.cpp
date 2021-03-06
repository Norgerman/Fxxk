#pragma unmanaged
#include <DirectXMath.h>
#include <MathFunc.h>

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
            )
            {
                target = DirectX::XMMatrixSet(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33);
            }

            void CALLCONV copyMatrix(DirectX::XMMATRIX& target, const DirectX::XMMATRIX& src)
            {
                target = src;
            }

            void CALLCONV setIdentity(DirectX::XMMATRIX& target)
            {
                target.r[0] = DirectX::g_XMIdentityR0;
                target.r[1] = DirectX::g_XMIdentityR1;
                target.r[2] = DirectX::g_XMIdentityR2;
                target.r[3] = DirectX::g_XMIdentityR3;
            }

            void CALLCONV multiplyVector(DirectX::XMVECTOR& a, DirectX::XMVECTOR& b)
            {
                a = DirectX::XMVectorMultiply(a, b);
            }

            void CALLCONV multiplyVector(DirectX::XMVECTOR& a, DirectX::XMVECTOR&& b)
            {
                a = DirectX::XMVectorMultiply(a, b);
            }

            void CALLCONV multiplyRow(DirectX::XMMATRIX& target, size_t index, float x, float y, float z, float w)
            {
                multiplyVector(target.r[index], { x, y, z, w });
            }

            void CALLCONV multiplyMatrix(DirectX::XMMATRIX& target, DirectX::XMMATRIX& b)
            {
                target = DirectX::XMMatrixMultiply(target, b);
            }

            void CALLCONV rotationX(DirectX::XMMATRIX& target, float angle)
            {
                target = DirectX::XMMatrixRotationX(angle);
            }

            void CALLCONV rotationY(DirectX::XMMATRIX& target, float angle)
            {
                target = DirectX::XMMatrixRotationY(angle);
            }

            void CALLCONV rotationZ(DirectX::XMMATRIX& target, float angle)
            {
                target = DirectX::XMMatrixRotationZ(angle);
            }

            void CALLCONV rotationAxis(DirectX::XMMATRIX& target, float x, float y, float z, float angle)
            {
                target = DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR({ x, y, z }), angle);
            }

            void CALLCONV rotationQuaternion(DirectX::XMMATRIX& target, float x, float y, float z, float w)
            {
                target = DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR({ x, y, z, w }));
            }

            void CALLCONV inverseMatrix(DirectX::XMMATRIX& target, DirectX::XMMATRIX& m)
            {
                target = DirectX::XMMatrixInverse(nullptr, m);
            }
        }
    }
}

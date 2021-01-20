#pragma once
#pragma unmanaged
#include <DirectXMath.h>
#pragma managed

namespace DX 
{
    namespace Sharp
    {
        namespace SimpleMath
        {
            public ref class XMMatrix
            {
            public:
                property DirectX::XMMATRIX* Value
                {
                    DirectX::XMMATRIX* get()
                    {
                        return m_value;
                    }
                }

                XMMatrix();
                void Set(
                    float m00, float m01, float m02, float m03,
                    float m10, float m11, float m12, float m13,
                    float m20, float m21, float m22, float m23,
                    float m30, float m31, float m32, float m33
                );
                void Set(XMMatrix^ other);
                void MultiplyRow(size_t row, float x, float y, float z, float w);
                void Multiply(XMMatrix^ other);
                void Scaling(float x, float y, float z);
                void Translation(float x, float y, float z);
                void Identity();
                void RotationX(float angle);
                void RotationY(float angle);
                void RotationZ(float angle);
                void RotationAxis(float x, float y, float z, float angle);
                void RotationQuaternion(float x, float y, float z, float w);
                void Inverse(XMMatrix^ other);
                array<float>^ ToArray();
                !XMMatrix();
                ~XMMatrix();
            private:
                DirectX::XMMATRIX* m_value;
            };
        }
    }
}


#include <cstring>
#include <XMMatrix.h>
#include <MathFunc.h>

namespace DX
{
    namespace Sharp
    {
        namespace SimpleMath
        {
            XMMatrix::XMMatrix()
            {
                m_value = static_cast<DirectX::XMMATRIX*>(_mm_malloc(sizeof(DirectX::XMMATRIX), 16));
                Identity();
            }

            void XMMatrix::Set(
                float m00, float m01, float m02, float m03,
                float m10, float m11, float m12, float m13,
                float m20, float m21, float m22, float m23,
                float m30, float m31, float m32, float m33
            )
            {
                setMatrix(*m_value, m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33);
            }

            void XMMatrix::Set(XMMatrix^ other)
            {
                copyMatrix(*m_value, *other->m_value);
            }

            void XMMatrix::MultiplyRow(size_t row, float x, float y, float z, float w)
            {
                multiplyRow(*m_value, row, x, y, z, w);
            }

            void XMMatrix::Multiply(XMMatrix^ other)
            {
                multiplyMatrix(*m_value, *(other->m_value));
            }

            void XMMatrix::Scaling(float x, float y, float z)
            {
                Set(
                    x, 0.0f, 0.0f, 0.0f,
                    0.0f, y, 0.0f, 0.0f,
                    0.0f, 0.0f, z, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f
                );
            }

            void XMMatrix::Translation(float x, float y, float z)
            {
                Set(
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    x, y, z, 1.0f
                );
            }

            void XMMatrix::Identity()
            {
                setIdentity(*m_value);
            }

            void XMMatrix::RotationX(float angle)
            {
                rotationX(*m_value, angle);
            }

            void XMMatrix::RotationY(float angle)
            {
                rotationY(*m_value, angle);
            }

            void XMMatrix::RotationZ(float angle)
            {
                rotationZ(*m_value, angle);
            }

            void XMMatrix::RotationAxis(float x, float y, float z, float angle)
            {
                rotationAxis(*m_value, x, y, z, angle);
            }

            void XMMatrix::RotationQuaternion(float x, float y, float z, float w)
            {
                rotationQuaternion(*m_value, x, y, z, w);
            }

            void XMMatrix::Inverse(XMMatrix^ other)
            {
                inverseMatrix(*m_value, *other->m_value);
            }

            array<float>^ XMMatrix::ToArray()
            {
                auto data = gcnew array<float>(16);

                pin_ptr<float> ptr = &data[0];

                memcpy(ptr, m_value, 4 * 16);

                return data;
            }

            XMMatrix::!XMMatrix()
            {
                if (m_value)
                {
                    _mm_free(m_value);
                    m_value = nullptr;
                }
            }

            XMMatrix::~XMMatrix()
            {
                this->!XMMatrix();
            }
        }
    }
}

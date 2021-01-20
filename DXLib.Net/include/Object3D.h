#pragma once
#include <D3DObject.h>
#include <Attribute.h>
#include <Constant.h>
#include <Index.h>
#include <Enums.hpp>
#include <DescriptorHeapX.h>
#include <XMMatrix.h>

namespace DX
{
    namespace Sharp
    {
        ref class Scene3D;

        public ref class Object3D
        {
        public:
            property DX::D3DObject* Value
            {
                DX::D3DObject* get()
                {
                    return m_value;
                }
            }
            Object3D(
                System::Collections::Generic::IEnumerable<Attribute^>^ attributes,
                Index^ index,
                System::Collections::Generic::IEnumerable<Constant^>^ constants,
                DescriptorHeap^ textureHeap,
                DescriptorHeap^ samplerHeap,
                Direct3D::PrimitiveTopology primitiveTopology
            );
            void UpdateTransform(DX::Sharp::SimpleMath::XMMatrix^ transform);
            void UpdateIndices(void* data);
            void UpdateConstant(size_t index, void* data);
            void UpdateAttribute(size_t index, void* data);
            generic<typename T> where T : value class
                void UpdateIndices(System::Memory<T> data);
            generic<typename T> where T : value class
                void UpdateConstant(size_t index, System::Memory<T> data);
            generic<typename T> where T : value class
                void UpdateAttribute(size_t index, System::Memory<T> data);
            void Initialize(
                Scene3D^ scene,
                System::Memory<System::Byte> vertexShader,
                System::Memory<System::Byte> pixelShader,
                Direct3D12::BlendDescription% blend,
                Direct3D12::DepthStencilDescription% depthStencil,
                Direct3D12::RasteriazerDescription% rasterizer,
                DXGI::Format rtFormat,
                DXGI::Format dsFormat,
                Direct3D12::PrimitiveTopologyType primitiveTopology,
                Direct3D12::IndexBufferStripCutValue stripCutValue);
            void Initialize(
                Scene3D^ scene,
                System::Memory<System::Byte> vertexShader,
                System::Memory<System::Byte> pixelShader,
                Direct3D12::BlendDescription% blend,
                Direct3D12::DepthStencilDescription% depthStencil,
                Direct3D12::RasteriazerDescription% rasterizer,
                DXGI::Format rtFormat,
                DXGI::Format dsFormat
            );
            ~Object3D();
            !Object3D();
        private:
            DX::D3DObject* m_value;
            System::Collections::Generic::IEnumerable<Attribute^>^ m_attributes;
            Index^ m_index;
            System::Collections::Generic::IEnumerable<Constant^>^ m_constants;
            DescriptorHeap^ m_textureHeap;
            DescriptorHeap^ m_samplerHeap;
        };
    }
}


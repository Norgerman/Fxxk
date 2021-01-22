#pragma once
#include <Structs.hpp>
#include <Enums.hpp>
#include <D3DEffect.h>
#include <Constant.h>

namespace DX
{
    namespace Sharp
    {
        ref class Scene3D;

        public value struct InputElement
        {
            System::String^ SemanticName;
            System::UInt32 SemanticIndex;
            DX::Sharp::DXGI::Format Format;
            System::UInt32 InputSolt;
            System::UInt32 AlignedByteOffset;
            DX::Sharp::Direct3D12::InputClassification InputSlotClass;
            System::UInt32 InstanceDataStepRate;

            DX::InputElement ToNative();

            static System::UInt32 AppendAlignedElement = D3D12_APPEND_ALIGNED_ELEMENT;
        };

        public ref class Effect3D
        {
        public:
            property DX::D3DEffect* Value
            {
                DX::D3DEffect* get()
                {
                    return m_value;
                }
            }

            Effect3D(System::Collections::Generic::IEnumerable<InputElement>^ inputElements,
                System::Collections::Generic::IEnumerable<Constant^>^ effectContants,
                System::Int32 objectConstantCount, System::Int32 textureCont, System::Int32 samplerCount);
            
            virtual void Initialize(
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
            
            !Effect3D();
            virtual ~Effect3D();

        private:
            System::Collections::Generic::IEnumerable<Constant^>^ m_constants;
            DX::D3DEffect* m_value;
        };
    }
}


#include <vector>
#include <RenderTargetState.h>
#include <Effect3D.h>
#include <Scene3D.h>

namespace DX
{
    namespace Sharp
    {
        using namespace System;
        using namespace System::Buffers;
        using namespace System::Collections::Generic;
        using namespace System::Runtime::InteropServices;
        using namespace DX::Sharp::Direct3D12;
        using namespace DX::Sharp::DXGI;
        using namespace DirectX;
        using namespace std;

        DX::InputElement InputElement::ToNative()
        {
            IntPtr ptr = Marshal::StringToHGlobalAnsi(SemanticName);

            DX::InputElement r = {
                static_cast<const char*>(ptr.ToPointer()),
                SemanticIndex,
                static_cast<DXGI_FORMAT>(Format),
                InputSolt,
                AlignedByteOffset,
                static_cast<D3D12_INPUT_CLASSIFICATION>(InputSlotClass),
                InstanceDataStepRate
            };

            Marshal::FreeHGlobal(ptr);
            return r;
        }

        Effect3D::Effect3D(System::Collections::Generic::IEnumerable<InputElement>^ inputElements,
            System::Collections::Generic::IEnumerable<Constant^>^ effectContants,
            System::Int32 objectConstantCount, System::Int32 textureCont, System::Int32 samplerCount) :
            m_constants(effectContants)
        {
            vector<DX::InputElement> elements;
            vector<D3DConstant*> constants;

            for each (auto % e in inputElements)
            {
                elements.push_back(e.ToNative());
            }

            for each (auto constant in effectContants)
            {
                constants.push_back(constant->Value);
            }

            m_value = new D3DEffect(move(elements), move(constants), objectConstantCount, textureCont, samplerCount);
        }


        void Effect3D::Initialize(
            Scene3D^ scene,
            Memory<Byte> vertexShader,
            Memory<Byte> pixelShader,
            BlendDescription% blend,
            DepthStencilDescription% depthStencil,
            RasteriazerDescription% rasterizer,
            Format rtFormat,
            Format dsFormat,
            PrimitiveTopologyType primitiveTopology,
            IndexBufferStripCutValue stripCutValue)
        {
            auto renderTarget = RenderTargetState(static_cast<DXGI_FORMAT>(rtFormat), static_cast<DXGI_FORMAT>(dsFormat));
            pin_ptr<BlendDescription> blendPtr = &blend;
            pin_ptr<DepthStencilDescription> depthPtr = &depthStencil;
            pin_ptr<RasteriazerDescription> rasterizerPtr = &rasterizer;
            MemoryHandle^ vertexShaderData = vertexShader.Pin();
            MemoryHandle^ pixelShaderData = pixelShader.Pin();
            D3D12_SHADER_BYTECODE vertex = { vertexShaderData->Pointer, static_cast<unsigned int>(vertexShader.Length) };
            D3D12_SHADER_BYTECODE pixel = { pixelShaderData->Pointer,  static_cast<unsigned int>(pixelShader.Length) };

            try {
                m_value->Initialize(
                    *(scene->Value),
                    vertex, pixel,
                    *reinterpret_cast<D3D12_BLEND_DESC*>(blendPtr),
                    *reinterpret_cast<D3D12_DEPTH_STENCIL_DESC*>(depthPtr),
                    *reinterpret_cast<D3D12_RASTERIZER_DESC*>(rasterizerPtr),
                    renderTarget,
                    static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(primitiveTopology),
                    static_cast<D3D12_INDEX_BUFFER_STRIP_CUT_VALUE>(stripCutValue)
                );
            }
            finally
            {
                delete vertexShaderData;
                delete pixelShaderData;
            }
        }

        void Effect3D::Initialize(
            Scene3D^ scene,
            Memory<Byte> vertexShader,
            Memory<Byte> pixelShader,
            BlendDescription% blend,
            DepthStencilDescription% depthStencil,
            RasteriazerDescription% rasterizer,
            Format rtFormat,
            Format dsFormat
        )
        {
            Initialize(scene, vertexShader, pixelShader, blend, depthStencil, rasterizer, rtFormat, dsFormat, PrimitiveTopologyType::Triangle, IndexBufferStripCutValue::Disabled);
        }


        Effect3D::!Effect3D()
        {
            delete m_value;
            m_value = nullptr;
        }

        Effect3D::~Effect3D()
        {
            this->!Effect3D();
        }
    }
}
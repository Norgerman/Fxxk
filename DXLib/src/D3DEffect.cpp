#include <winrt/base.h>
#include <D3DEffect.h>
#include <EffectPipelineStateDescription.h>
#include <wil/result.h>
#include <DirectXHelpers.h>
#include <D3DScene.h>
#include "d3dx12.h"

using namespace std;
using namespace DirectX;

namespace DX
{
    class D3DEffect::Impl
    {
    public:
        Impl(
            const vector<InputElement>& inputElements,
            const vector<D3DConstant*>& effectContants,
            size_t objectConstantCount, size_t textureCont, size_t samplerCount) :
            m_elements(inputElements),
            m_constants(effectContants),
            m_objectConstantCount(objectConstantCount),
            m_textureCount(textureCont),
            m_samplerCount(samplerCount)
        {

        }

        Impl(
            vector<InputElement>&& inputElements,
            vector<D3DConstant*>&& effectContants,
            size_t objectConstantCount, size_t textureCont, size_t samplerCount) :
            m_elements(move(inputElements)),
            m_constants(move(effectContants)),
            m_objectConstantCount(objectConstantCount),
            m_textureCount(textureCont),
            m_samplerCount(samplerCount)
        {

        }

        void Initialize(
            D3DScene& scene,
            D3D12_SHADER_BYTECODE& vertexShader,
            D3D12_SHADER_BYTECODE& pixelShader,
            const D3D12_BLEND_DESC& blend,
            const D3D12_DEPTH_STENCIL_DESC& depthStencil,
            const D3D12_RASTERIZER_DESC& rasterizer,
            const DirectX::RenderTargetState& renderTarget,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            D3D12_INDEX_BUFFER_STRIP_CUT_VALUE stripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED)
        {
            vector<D3D12_INPUT_ELEMENT_DESC> elements;
            uint32_t idx = 0;
            for (auto& element : m_elements)
            {
                elements.push_back(
                    {
                        element.SemanticName.data(),
                        element.SemanticIndex,
                        element.Format,
                        element.InputSlot,
                        element.AlignedByteOffset,
                        element.InputSlotClass,
                        element.InstanceDataStepRate
                    });
                idx++;
            }
            D3D12_INPUT_LAYOUT_DESC layoutDesc = {
                elements.data(),
                static_cast<uint32_t>(elements.size())
            };

            EffectPipelineStateDescription pd = {
                &layoutDesc,
                blend,
                depthStencil,
                rasterizer,
                renderTarget,
                primitiveTopology,
                stripCutValue
            };

            D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

            // effect consant count + object constants + texture count + sampler count + projection + transform
            vector<CD3DX12_ROOT_PARAMETER> rootParameters(m_constants.size() + m_objectConstantCount + m_textureCount + m_samplerCount + 2);
            vector<CD3DX12_DESCRIPTOR_RANGE> textureRange(m_textureCount);
            vector<CD3DX12_DESCRIPTOR_RANGE> samplerRange(m_samplerCount);
            CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};

            rootParameters[0].InitAsConstantBufferView(0);
            rootParameters[1].InitAsConstantBufferView(1);
            idx = 2;

            // constants
            auto count = m_constants.size() + m_objectConstantCount;
            for (size_t i = 0; i < count; i++, idx++)
            {
                rootParameters[idx].InitAsConstantBufferView(static_cast<uint32_t>(i + 2));
            }

            // textures
            for (size_t i = 0; i < m_textureCount; i++, idx++)
            {
                textureRange[i] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, static_cast<uint32_t>(i));
                rootParameters[idx].InitAsDescriptorTable(1, &textureRange[i], D3D12_SHADER_VISIBILITY_PIXEL);
            }

            // samplers
            for (size_t i = 0; i < m_samplerCount; i++, idx++)
            {
                samplerRange[i] = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, static_cast<uint32_t>(i));
                rootParameters[idx].InitAsDescriptorTable(1, &samplerRange[i], D3D12_SHADER_VISIBILITY_PIXEL);
            }

            rootSignatureDesc.Init(static_cast<uint32_t>(rootParameters.size()), rootParameters.data(), 0, nullptr, rootSignatureFlags);
            auto hr = CreateRootSignature(scene.Device(), &rootSignatureDesc, m_rootSignature.put());

            THROW_IF_FAILED(hr);

            pd.CreatePipelineState(scene.Device(), m_rootSignature.get(), vertexShader, pixelShader, m_pipelineState.put());
        }

        size_t ObjectConstantStartSolt() const
        {
            return m_constants.size() + 1; // projection from scene;
        }


        void Apply(D3DScene& scene)
        {
            auto commandList = scene.CommandList();
            commandList->SetGraphicsRootSignature(m_rootSignature.get());
            commandList->SetPipelineState(m_pipelineState.get());
            commandList->SetGraphicsRootConstantBufferView(0, scene.Projection().GpuAddress());

            auto idx = 1;
            for (auto& constant : m_constants)
            {
                commandList->SetGraphicsRootConstantBufferView(idx, constant->GpuAddress());
                idx++;
            }
        }

        void Reset()
        {
            for (auto& constant : m_constants)
            {
                constant->Reset();
            }
            m_pipelineState = nullptr;
            m_rootSignature = nullptr;
        }
    private:
        winrt::com_ptr<ID3D12PipelineState> m_pipelineState;
        winrt::com_ptr<ID3D12RootSignature> m_rootSignature;
        vector<InputElement> m_elements;
        vector<D3DConstant*> m_constants;
        size_t m_objectConstantCount;
        size_t m_textureCount;
        size_t m_samplerCount;
    };


    D3DEffect::D3DEffect(
        const std::vector<InputElement>& inputElements,
        const std::vector<D3DConstant*>& effectContants,
        size_t objectConstantCount, size_t textureCont, size_t samplerCount) :
        m_impl(make_unique<Impl>(inputElements, effectContants, objectConstantCount, textureCont, samplerCount))
    {

    }

    D3DEffect::D3DEffect(
        std::vector<InputElement>&& inputElements,
        std::vector<D3DConstant*>&& effectContants,
        size_t objectConstantCount, size_t textureCont, size_t samplerCount) : 
        m_impl(make_unique<Impl>(move(inputElements), move(effectContants), objectConstantCount, textureCont, samplerCount))
    {

    }

    void D3DEffect::Initialize(
        D3DScene& scene,
        D3D12_SHADER_BYTECODE& vertexShader,
        D3D12_SHADER_BYTECODE& pixelShader,
        const D3D12_BLEND_DESC& blend,
        const D3D12_DEPTH_STENCIL_DESC& depthStencil,
        const D3D12_RASTERIZER_DESC& rasterizer,
        const DirectX::RenderTargetState& renderTarget,
        D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology,
        D3D12_INDEX_BUFFER_STRIP_CUT_VALUE stripCutValue)
    {
        return m_impl->Initialize(scene, vertexShader, pixelShader, blend, depthStencil, rasterizer, renderTarget, primitiveTopology, stripCutValue);
    }

    size_t D3DEffect::ObjectConstantStartSolt() const
    {
        return m_impl->ObjectConstantStartSolt();
    }

    void D3DEffect::Apply(D3DScene& scene)
    {
        return m_impl->Apply(scene);
    }

    void D3DEffect::Reset()
    {
        return m_impl->Reset();
    }

    D3DEffect::~D3DEffect()
    {

    }
}

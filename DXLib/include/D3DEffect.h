#pragma once
#include <vector>
#include <memory>
#include <string>
#include <d3d12.h>
#include <RenderTargetState.h>
#include <export.h>
#include <D3DConstant.h>

namespace DX
{
    class D3DScene;

    typedef struct dllexport InputElement
    {
        std::string SemanticName = "";
        uint32_t SemanticIndex = 0;
        DXGI_FORMAT Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
        uint32_t InputSlot = 0;
        uint32_t AlignedByteOffset = 0;
        D3D12_INPUT_CLASSIFICATION InputSlotClass = D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        uint32_t InstanceDataStepRate = 0;
    } InputElement;

    class dllexport D3DEffect
    {
    public:
        D3DEffect(
            const std::vector<InputElement>& inputElements,
            const std::vector<D3DConstant*>& effectContants,
            size_t objectConstantCount, size_t textureCont, size_t samplerCount);

        D3DEffect(
            std::vector<InputElement>&& inputElements,
            std::vector<D3DConstant*>&& effectContants,
            size_t objectConstantCount, size_t textureCont, size_t samplerCount);

        virtual void Initialize(
            D3DScene& scene,
            D3D12_SHADER_BYTECODE& vertexShader,
            D3D12_SHADER_BYTECODE& pixelShader,
            const D3D12_BLEND_DESC& blend,
            const D3D12_DEPTH_STENCIL_DESC& depthStencil,
            const D3D12_RASTERIZER_DESC& rasterizer,
            const DirectX::RenderTargetState& renderTarget,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            D3D12_INDEX_BUFFER_STRIP_CUT_VALUE stripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED);

        size_t ObjectConstantStartSolt() const;
        void Apply(D3DScene& scene);
        void Reset();
        virtual ~D3DEffect();
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}


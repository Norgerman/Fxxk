#pragma once
#include <vector>
#include <initializer_list>
#include <cstdint>
#include <d3d11.h>
#include <DirectXMath.h>
#include "D3DGlobal.h"
#include "D3DAttribute.h"
#include "D3DConstant.h"

class D3DScene;

typedef struct D3DShader 
{
    const void* byteCode;
    size_t size;
} D3DShader;

typedef struct D3DBlend {
    D3D11_BLEND_DESC desc;
    float blendFactor[4];
    uint32_t sampleMask;
} D3DBlend;

class D3DObject
{
    friend class D3DScene;
public:
    D3DObject() = default;

    template<
        typename T1 = std::vector<D3DAttribute>,
        typename T2 = std::vector<D3DConstant>,
        typename T3 = D3D11_RASTERIZER_DESC,
        typename T4 = std::vector<ID3D11Resource*>,
        typename T5 = std::vector<ID3D11ShaderResourceView*>,
        typename T6 = std::vector<D3D11_SAMPLER_DESC>
    >
    D3DObject(
        T1&& attributes,
        T2&& vsConstants,
        T2&& psConstants,
        D3D_PRIMITIVE_TOPOLOGY topopogy,
        T3&& rasterizerDesc,
        T4&& textures = {},
        T5&& textureViews = {},
        T6&& samplerDesc = {}
    ) :
        m_indexBuffer(nullptr),
        m_transformBuffer(nullptr),
        m_firstVertexAttribute(nullptr),
        m_indexAttribute(nullptr),
        m_indexPosition(-1),
        m_layout(nullptr),
        m_vs(nullptr),
        m_ps(nullptr),
        m_rs(nullptr),
        m_blendState(nullptr),
        m_blendNeedUpdate(false),
        m_blend({ {}, { 0 }, 0xffffffff }),
        m_attributes(attributes),
        m_vsConstants(vsConstants),
        m_psConstants(psConstants),
        m_rasterizerDesc(rasterizerDesc),
        m_samplerDesc(samplerDesc),
        m_topology(topopogy),
        m_textures(textures),
        m_textureViews(textureViews),
        m_transform(DirectX::XMMatrixIdentity()),
        m_inited(false)
    {
        m_transformDesc = D3DConstant(&m_transform, 4, 16);
    }
    void updateTransform(const DirectX::XMMATRIX& transform);
    void updateTransform(DirectX::XMMATRIX&& transform);
    const DirectX::XMMATRIX& getTransform() const;
    void updateAttribute(size_t index, const void* data);
    void updateVSConstant(size_t index, const void* data);
    void updatePSConstant(size_t index, const void* data);
    void updateIndex(const void* data);
    void enableBlend(const D3DBlend& blend);
    void enableBlend(D3DBlend&& blend);
    void init(D3DScene& scene, D3DShader vertexShader, D3DShader pixelShader);
    void disableBlend();
    void dispose(bool releaseTexture = true);
    ~D3DObject();
    
    template<typename T1 = std::vector<ID3D11Resource*>, typename T2 = std::vector<ID3D11ShaderResourceView*>>
    void setTexture(T1&& textures, T2&& textureViews)
    {
        m_textures = textures;
        m_textureViews = textureViews;
    }

private:
    void render(D3DScene& scene);
    void uploadTransform(D3DScene& scene);
    void uploadVertexBuffer(D3DScene& scene);
    void uploadConstantBuffer(D3DScene& scene);
    void uploadIndex(D3DScene& scene);
    void updateBlend(D3DScene& scene);

    ID3D11Buffer* m_indexBuffer;
    ID3D11Buffer* m_transformBuffer;
    D3DAttribute* m_indexAttribute;
    D3DAttribute* m_firstVertexAttribute;
    D3DConstant m_transformDesc;
    ID3D11InputLayout* m_layout;
    ID3D11VertexShader* m_vs;
    ID3D11PixelShader* m_ps;
    ID3D11RasterizerState* m_rs;
    std::vector<ID3D11SamplerState*> m_ss;
    std::vector<ID3D11Buffer*> m_vertexBuffer;
    std::vector<ID3D11Buffer*> m_psConstantBuffer;
    std::vector<ID3D11Buffer*> m_vsConstantBuffer;
    std::vector<ID3D11Resource*> m_textures;
    std::vector<ID3D11ShaderResourceView*> m_textureViews;
    std::vector<D3DAttribute> m_attributes;
    std::vector<D3DConstant> m_vsConstants;
    std::vector<D3DConstant> m_psConstants;
    DirectX::XMMATRIX m_transform;
    D3D11_RASTERIZER_DESC m_rasterizerDesc;
    D3D_PRIMITIVE_TOPOLOGY m_topology;
    std::vector<D3D11_SAMPLER_DESC> m_samplerDesc;
    std::vector<uint32_t> m_strides;
    std::vector<uint32_t> m_offsets;
    size_t m_indexPosition;
    D3DBlend m_blend;
    ID3D11BlendState* m_blendState;
    bool m_blendNeedUpdate;
    bool m_inited;
};

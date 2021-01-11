#pragma once
#include <vector>
#include <initializer_list>
#include <cstdint>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl\client.h>
#include "D3DGlobal.h"
#include "D3DAttribute.h"
#include "D3DConstant.h"

class D3DScene;

typedef struct D3DShader
{
    const void* byteCode;
    size_t size;
} D3DShader;

class D3DObject
{
    friend class D3DScene;
public:
    D3DObject() = default;

    template<
        typename T1 = std::vector<D3DAttribute>,
        typename T2 = std::vector<D3DConstant>,
        typename T3 = std::vector<Microsoft::WRL::ComPtr<ID3D11Resource>>,
        typename T4 = std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>,
        typename T5 = std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>>
    >
        D3DObject(
            T1&& attributes,
            T2&& vsConstants,
            T2&& psConstants,
            D3D_PRIMITIVE_TOPOLOGY topopogy,
            Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState = nullptr,
            T3&& textures = {},
            T4&& textureViews = {},
            T5&& samplerState = {},
            Microsoft::WRL::ComPtr<ID3D11DepthStencilState> stencilState = nullptr,
            uint32_t stencilRef = 0
        ) :
        m_indexBuffer(nullptr),
        m_transformBuffer(nullptr),
        m_firstVertexAttribute(nullptr),
        m_indexAttribute(nullptr),
        m_indexPosition(-1),
        m_layout(nullptr),
        m_vs(nullptr),
        m_ps(nullptr),
        m_rs(rasterizerState),
        m_stencilState(stencilState),
        m_stencilRef(0),
        m_blendState(nullptr),
        m_blendFactor{ 0 },
        m_sampleMask(0xffffffff),
        m_attributes(std::forward<T1>(attributes)),
        m_vsConstants(std::forward<T2>(vsConstants)),
        m_psConstants(std::forward<T2>(psConstants)),
        m_topology(topopogy),
        m_textures(std::forward<T3>(textures)),
        m_textureViews(std::forward<T4>(textureViews)),
        m_ss(std::forward<T5>(samplerState)),
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
    void setRasterizerState(Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState);
    void setStencilState(Microsoft::WRL::ComPtr<ID3D11DepthStencilState> stencilState, uint32_t stencilRef);
    void enableBlend(Microsoft::WRL::ComPtr<ID3D11BlendState> blendState, const float* blendFactor, uint32_t sampleMask);
    void init(D3DScene& scene, D3DShader& vertexShader, D3DShader& pixelShader);
    void init(D3DScene& scene, D3DShader&& vertexShader, D3DShader&& pixelShader);
    void disableBlend();
    ~D3DObject();

    template<typename T1 = std::vector<Microsoft::WRL::ComPtr<ID3D11Resource>>, typename T2 = std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>>
    void setTexture(T1&& textures, T2&& textureViews)
    {
        m_textures = std::forward<T1>(textures);
        m_textureViews = std::forward<T2>(textureViews);
    }

    template<typename T = std::vector<Microsoft::WRL::ComPtr<ID3D11Resource>>>
    void setSamplerState(T&& samplerStates)
    {
        m_ss = std::forward<T>(samplerStates);
    }

private:
    void render(D3DScene& scene);
    void uploadTransform(D3DScene& scene);
    void uploadVertexBuffer(D3DScene& scene);
    void uploadConstantBuffer(D3DScene& scene);
    void uploadIndex(D3DScene& scene);

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_transformBuffer;
    D3DAttribute* m_indexAttribute;
    D3DAttribute* m_firstVertexAttribute;
    D3DConstant m_transformDesc;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rs;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_stencilState;
    std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> m_ss;
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_vertexBuffer;
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_psConstantBuffer;
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_vsConstantBuffer;
    std::vector<Microsoft::WRL::ComPtr<ID3D11Resource>> m_textures;
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textureViews;
    std::vector<D3DAttribute> m_attributes;
    std::vector<D3DConstant> m_vsConstants;
    std::vector<D3DConstant> m_psConstants;
    DirectX::XMMATRIX m_transform;
    D3D_PRIMITIVE_TOPOLOGY m_topology;
    std::vector<uint32_t> m_strides;
    std::vector<uint32_t> m_offsets;
    size_t m_indexPosition;
    float m_blendFactor[4];
    uint32_t m_sampleMask;
    uint32_t m_stencilRef;
    bool m_inited;
};

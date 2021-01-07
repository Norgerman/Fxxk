#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <array>
#include <initializer_list>
#include <string>

class D3DScene;

class D3DObject
{
    friend class D3DScene;
public:
    D3DObject(
        std::initializer_list<float>&& vertices, 
        std::initializer_list<float>&& normals,
        std::initializer_list<float>&& textcoord, 
        std::initializer_list<uint32_t>&& indices,
        std::wstring&& vertexShader,
        std::wstring&& pixelShader,
        std::initializer_list<std::wstring>&& textureFiles,
        D3D11_RASTERIZER_DESC&& rasterizerDesc, 
        std::initializer_list<D3D11_SAMPLER_DESC>&& samplerDesc,
        D3D_PRIMITIVE_TOPOLOGY topopogy
        );
    void updateTransform(DirectX::XMMATRIX&& transform);
    void updateTransform(DirectX::XMMATRIX& transform);
    void init(D3DScene& scene);
    void dispose();
    ~D3DObject();
private:
    void render(D3DScene& scene);
    void updateTransform(D3DScene& scene);

    ID3D11Buffer* m_indexBuffer;
    ID3D11InputLayout* m_layout;
    ID3D11VertexShader* m_vs;
    ID3D11PixelShader* m_ps;
    ID3D11RasterizerState* m_rs;
    std::vector<ID3D11SamplerState*> m_ss;
    std::array<ID3D11Buffer*, 4> m_vertexBuffer;
    std::vector<ID3D11Resource*> m_textures;
    std::vector<ID3D11ShaderResourceView*> m_textureViews;
    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<float> m_texcoord;
    std::vector<uint32_t> m_incides;
    DirectX::XMMATRIX m_transform;
    std::wstring m_vertexShader;
    std::wstring m_pixelShader;
    std::vector<std::wstring> m_textureFiles;
    D3D11_RASTERIZER_DESC m_rasterizerDesc;
    std::vector<D3D11_SAMPLER_DESC> m_samplerDesc;
    D3D_PRIMITIVE_TOPOLOGY m_topology;
    bool m_inited;
    bool m_matrixDirty;
};

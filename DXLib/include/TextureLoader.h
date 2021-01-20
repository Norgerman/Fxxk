#pragma once
#include <memory>
#include <d3d12.h>
#include <export.h>

namespace DX
{
    class dllexport Waitable
    {
    public:
        virtual void Wait() = 0;
        virtual ~Waitable();
    };

    class dllexport TextureLoader
    {
    public:
        TextureLoader(ID3D12Device* device, ID3D12CommandQueue* commandQueue);
        void Begin();
        Waitable& End();
        void CreateTexture(const wchar_t* fileName, ID3D12Resource** texture, bool generateMips = false, size_t maxsize = 0);
        void CreateTexture(const uint8_t* data, size_t size, ID3D12Resource** texture, bool generateMips = false, size_t maxsize = 0);
        void CreateShaderResourceView(ID3D12Resource* tex, D3D12_CPU_DESCRIPTOR_HANDLE descriptor, bool isCubeMap = false);
        ~TextureLoader();
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}


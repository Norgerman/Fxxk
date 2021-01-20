#include <future>
#include <TextureLoader.h>
#include <WICTextureLoader.h>
#include <DirectXHelpers.h>
#include <ResourceUploadBatch.h>
#include <wil/result.h>

using namespace std;
using namespace DirectX;

namespace DX
{
    Waitable::~Waitable()
    {

    }

    class FutureWaitable : public Waitable
    {
    public:
        FutureWaitable()
            : m_future()
        {
        }

        FutureWaitable(future<void>&& future)
            : m_future(move(future))
        {

        }

        FutureWaitable(FutureWaitable&) = delete;
        FutureWaitable& operator=(FutureWaitable&) = delete;

        FutureWaitable& operator=(FutureWaitable&&) = default;

        virtual void Wait()
        {
            m_future.wait();
            m_future.get();
        }

    private:
        future<void> m_future;
    };

    class TextureLoader::Impl
    {
    public:
        Impl(ID3D12Device* device, ID3D12CommandQueue* commandQueue) :
            m_device(device),
            m_commandQueue(commandQueue),
            m_uploadBatch(make_unique<ResourceUploadBatch>(device))
        {

        }
        void Begin()
        {
            m_uploadBatch->Begin();
        }

        FutureWaitable& End()
        {
            m_waitable = m_uploadBatch->End(m_commandQueue);
            return m_waitable;
        }

        void CreateTexture(const wchar_t* fileName, ID3D12Resource** texture, bool generateMips, size_t maxsize)
        {
            THROW_IF_FAILED(CreateWICTextureFromFile(m_device, *m_uploadBatch, fileName, texture, generateMips, maxsize));
        }

        void CreateTexture(const uint8_t* data, size_t size, ID3D12Resource** texture, bool generateMips, size_t maxsize)
        {
            THROW_IF_FAILED(CreateWICTextureFromMemory(m_device, *m_uploadBatch, data, size, texture, generateMips, maxsize));
        }

        void CreateShaderResourceView(ID3D12Resource* tex, D3D12_CPU_DESCRIPTOR_HANDLE descriptor, bool isCubeMap)
        {
            ::CreateShaderResourceView(m_device, tex, descriptor, isCubeMap);
        }

    private:
        ID3D12Device* m_device;
        ID3D12CommandQueue* m_commandQueue;
        unique_ptr<ResourceUploadBatch> m_uploadBatch;
        FutureWaitable m_waitable;
    };

    TextureLoader::TextureLoader(ID3D12Device* device, ID3D12CommandQueue* commandQueue)
        : m_impl(make_unique<Impl>(device, commandQueue))
    {

    }

    void TextureLoader::Begin()
    {
        return m_impl->Begin();
    }

    Waitable& TextureLoader::End()
    {
        return m_impl->End();
    }

    void TextureLoader::CreateTexture(const wchar_t* fileName, ID3D12Resource** texture, bool generateMips, size_t maxsize)
    {
        return m_impl->CreateTexture(fileName, texture, generateMips, maxsize);
    }

    void TextureLoader::CreateTexture(const uint8_t* data, size_t size, ID3D12Resource** texture, bool generateMips, size_t maxsize)
    {
        return m_impl->CreateTexture(data, size, texture, generateMips, maxsize);
    }

    void TextureLoader::CreateShaderResourceView(ID3D12Resource* tex, D3D12_CPU_DESCRIPTOR_HANDLE descriptor, bool isCubeMap)
    {
        return m_impl->CreateShaderResourceView(tex, descriptor, isCubeMap);
    }

    TextureLoader::~TextureLoader()
    {

    }
}

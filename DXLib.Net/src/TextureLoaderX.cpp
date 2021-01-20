#include <vcclr.h>
#include <TextureLoaderX.h>

namespace DX
{
    namespace Sharp
    {
        using namespace System;
        using namespace System::Threading::Tasks;
        using namespace System::Buffers;
        using namespace DX::Sharp::Direct3D12;

        TextureLoader::TextureLoader(Device device, CommandQueue commandQueue)
        {
            m_loader = new DX::TextureLoader(device.Pointer, commandQueue.Pointer);
        }

        void TextureLoader::Begin()
        {
            m_loader->Begin();
        }

        void TextureLoader::End()
        {
            m_loader->End().Wait();
        }

        Task^ TextureLoader::EndAsync()
        {
            return Task::Factory->StartNew(gcnew Action(this, &TextureLoader::End));
        }

        void TextureLoader::CreateTexture(String^ filename, Resource^ resource, bool generateMips, size_t maxsize)
        {
            pin_ptr<const wchar_t> ptr = PtrToStringChars(filename);
            pin_ptr<ID3D12Resource*> texPtr = &resource->Value;
            m_loader->CreateTexture(ptr, texPtr, generateMips, maxsize);
        }

        void TextureLoader::CreateTexture(String^ filename, Resource^ resource)
        {
            CreateTexture(filename, resource, false, 0);
        }

        void TextureLoader::CreateTexture(Memory<Byte> data, Resource^ resource, bool generateMips, size_t maxsize)
        {
            MemoryHandle^ handle = data.Pin();
            pin_ptr<ID3D12Resource*> texPtr = &resource->Value;
            try
            {
                m_loader->CreateTexture(static_cast<uint8_t*>(handle->Pointer), data.Length, texPtr, generateMips, maxsize);
            }
            finally
            {
                delete handle;
            }
        }

        void TextureLoader::CreateTexture(Memory<Byte> data, Resource^ resource)
        {
            CreateTexture(data, resource, false, 0);
        }

        void TextureLoader::CreateShaderResourceView(Resource^ tex, CpuDescriptorHandle descriptor, bool isCubeMap)
        {
            m_loader->CreateShaderResourceView(tex->Value, descriptor.ToNative(), isCubeMap);
        }

        void TextureLoader::CreateShaderResourceView(Resource^ tex, CpuDescriptorHandle descriptor)
        {
            CreateShaderResourceView(tex, descriptor, false);
        }
        
        TextureLoader::!TextureLoader()
        {
            delete m_loader;
            m_loader = nullptr;
        }

        TextureLoader::~TextureLoader()
        {
            this->!TextureLoader();
        }
    }
}

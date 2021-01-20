#pragma once
#include <TextureLoader.h>
#include <Structs.hpp>

namespace DX
{
    namespace Sharp
    {
        public ref class TextureLoader
        {
        public:
            TextureLoader(DX::Sharp::Direct3D12::Device device, DX::Sharp::Direct3D12::CommandQueue commandQueue);
            void Begin();
            void End();
            System::Threading::Tasks::Task^ EndAsync();
            void CreateTexture(System::String^ filename, DX::Sharp::Direct3D12::Resource^ resource, bool generateMips, size_t maxsize);
            void CreateTexture(System::String^ filename, DX::Sharp::Direct3D12::Resource^ resource);
            void CreateTexture(System::Memory<System::Byte> data, DX::Sharp::Direct3D12::Resource^ resource, bool generateMips, size_t maxsize);
            void CreateTexture(System::Memory<System::Byte> data, DX::Sharp::Direct3D12::Resource^ resource);
            void CreateShaderResourceView(DX::Sharp::Direct3D12::Resource^ tex, DX::Sharp::Direct3D12::CpuDescriptorHandle descriptor, bool isCubeMap);
            void CreateShaderResourceView(DX::Sharp::Direct3D12::Resource^ tex, DX::Sharp::Direct3D12::CpuDescriptorHandle descriptor);
            !TextureLoader();
            ~TextureLoader();
        private:
            DX::TextureLoader* m_loader;
        };
    }
}

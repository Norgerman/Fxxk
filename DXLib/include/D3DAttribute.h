#pragma once
#include <string>
#include <d3d12.h>
#include <vector>
#include <D3DData.h>
#include <export.h>

namespace DX {
    struct dllexport InputElement
    {
        std::string SemanticName;
        uint32_t SemanticIndex;
        DXGI_FORMAT Format;
        uint32_t AlignedByteOffset;
        D3D12_INPUT_CLASSIFICATION InputSlotClass;
        uint32_t InstanceDataStepRate;
    };

    class dllexport D3DAttribute :
        public D3DData
    {
    public:
        template<typename T>
        D3DAttribute(
            D3DScene* scene,
            uint32_t elementSize,
            uint32_t size,
            uint32_t stride,
            T&& elements
        ) :
            D3DData(scene, elementSize, size),
            m_elements(std::forward<T>(elements)),
            m_stride(stride),
            m_view()
        {
            m_view.SizeInBytes = ByteSize();
            m_view.StrideInBytes = m_stride;
        }


        void AppendElement(uint32_t solt, std::vector<D3D12_INPUT_ELEMENT_DESC>& output) const;
        const D3D12_VERTEX_BUFFER_VIEW& BufferView() const;
    protected:
        virtual void Alloc();
    private:
        uint32_t m_stride;
        D3D12_VERTEX_BUFFER_VIEW m_view;
        std::vector<InputElement> m_elements;
    };
}

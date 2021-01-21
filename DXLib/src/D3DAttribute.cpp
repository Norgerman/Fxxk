#include <D3DAttribute.h>
#include <D3DScene.h>

namespace DX {
    void D3DAttribute::AppendElement(uint32_t solt, std::vector<D3D12_INPUT_ELEMENT_DESC>& output) const
    {
        for (auto& element : m_elements)
        {
            output.push_back(
                {
                    element.SemanticName.data(),
                    element.SemanticIndex,
                    element.Format,
                    solt,
                    element.AlignedByteOffset,
                    element.InputSlotClass,
                    element.InstanceDataStepRate
                });
        }
    }

    void D3DAttribute::Alloc()
    {
        D3DData::Alloc(ByteSize());
        m_view.BufferLocation = GpuAddress();
    }

    const D3D12_VERTEX_BUFFER_VIEW& D3DAttribute::BufferView() const
    {
        return m_view;
    }
}

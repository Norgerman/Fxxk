#include "D3DAttribute.h"


D3DAttribute::D3DAttribute() : 
    D3DAttribute(nullptr, 0, 0, "", 0, 0, 0, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R8_UINT)
{

}
uint32_t D3DAttribute::getStride() const
{
    return m_stride;
}

uint32_t D3DAttribute::getOffset() const
{
    return m_offset;
}

DXGI_FORMAT D3DAttribute::getFormat() const
{
    return m_format;
}

uint32_t D3DAttribute::getRowByteSize() const
{
    return m_size / m_rowCount * m_elementSize;
}

D3D11_INPUT_CLASSIFICATION D3DAttribute::getInputSoltClass() const
{
    return m_inputSoltClass;
}

void D3DAttribute::appendLayout(uint32_t solt, std::vector<D3D11_INPUT_ELEMENT_DESC>& output) const
{
    auto rowByteSize = getRowByteSize();

    for (uint32_t i = 0; i < m_rowCount; i++)
    {
        output.push_back(
        {
            m_semanticName.data(),
            i,
            m_format,
            solt,
            rowByteSize * i,
            m_inputSoltClass,
            m_instanceDataStepRate
        });
    }
}

bool D3DAttribute::shouldUpload() const
{
    return m_dirty && m_usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC && (m_CPUAccessFlags & D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE) != 0;
}

bool D3DAttribute::isIndex() const
{
    return m_bindFlags & D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
}

D3D11_BUFFER_DESC D3DAttribute::buildBufferDesc()
{
    D3D11_BUFFER_DESC bufferDescr = {};
    bufferDescr.ByteWidth = getByteSize();
    bufferDescr.BindFlags = m_bindFlags;
    bufferDescr.Usage = m_usage;
    bufferDescr.CPUAccessFlags = m_CPUAccessFlags;
    return bufferDescr;
}

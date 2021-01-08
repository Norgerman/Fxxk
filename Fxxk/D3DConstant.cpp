#include "D3DConstant.h"

D3DConstant::D3DConstant() 
    : D3DConstant(nullptr, 0, 0)
{

}

D3DConstant::D3DConstant(const void* data, uint32_t elementSize, uint32_t size) : D3DData(data, elementSize, size)
{

}

D3D11_BUFFER_DESC D3DConstant::buildBufferDesc()
{
    D3D11_BUFFER_DESC bufferDescr = {};
    bufferDescr.ByteWidth = getByteSize();
    bufferDescr.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
    bufferDescr.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
    bufferDescr.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
    return bufferDescr;
}
bool D3DConstant::shouldUpload() const
{
    return m_dirty;
}

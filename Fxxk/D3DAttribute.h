#pragma once
#include <string>
#include <d3d11.h>
#include <vector>
#include "D3DData.h"

class D3DAttribute :
    public D3DData
{
public:
    D3DAttribute()
        : D3DAttribute(nullptr, 0, 0, "", 0, 0, 0, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R8_UINT)
    {

    }

    template<typename T = std::string>
    D3DAttribute(
        const void* data, 
        uint32_t elementSize, 
        uint32_t size, 
        T&& semanticName, 
        uint32_t stride,
        uint32_t offset,
        uint32_t bindFlags,
        D3D11_USAGE usage,
        uint32_t CPUAccessFlags,
        DXGI_FORMAT format,
        D3D11_INPUT_CLASSIFICATION inputSoltClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
        uint32_t rowCount = 1,
        uint32_t instanceDataStepRate = 0
        ):
        D3DData(data, elementSize, size),
        m_semanticName(std::forward<T>(semanticName)),
        m_stride(stride),
        m_offset(offset),
        m_bindFlags(bindFlags),
        m_usage(usage),
        m_CPUAccessFlags(CPUAccessFlags),
        m_format(format),
        m_inputSoltClass(inputSoltClass),
        m_rowCount(rowCount),
        m_instanceDataStepRate(instanceDataStepRate)
    {

    }
    uint32_t getStride() const;
    uint32_t getOffset() const;
    DXGI_FORMAT getFormat() const;
    D3D11_INPUT_CLASSIFICATION getInputSoltClass() const;
    bool isIndex() const;
    void appendLayout(uint32_t solt, std::vector<D3D11_INPUT_ELEMENT_DESC>& output) const;
    virtual D3D11_BUFFER_DESC buildBufferDesc();
protected:
    virtual bool shouldUpload() const;
private:
    uint32_t getRowByteSize() const;

    std::string m_semanticName;
    uint32_t m_rowCount;
    uint32_t m_stride;
    uint32_t m_offset;
    uint32_t m_bindFlags;
    D3D11_USAGE m_usage;
    uint32_t m_CPUAccessFlags;
    DXGI_FORMAT m_format;
    D3D11_INPUT_CLASSIFICATION m_inputSoltClass;
    uint32_t m_instanceDataStepRate;
};


#pragma once
// D3D structs wrapper

#include <d3d12.h>
#include <inline_array.hpp>
#include <Enums.hpp>

namespace DX
{
    namespace Sharp
    {
        public value struct Color
        {
        public:
            float R;
            float G;
            float B;
            float A;

            static Color White = { 1.0f, 1.0f, 1.0f, 1.0f };
        };

        namespace Direct3D12
        {
            public ref struct InputElement
            {
            public:
                System::String^ SemanticName;
                System::UInt32 SemanticIndex;
                DX::Sharp::DXGI::Format Format;
                System::UInt32 InputSlot;
                System::UInt32 AlignedByteOffset;
                InputClassification InputSlotClass;
                System::UInt32 InstanceDataStepRate;

                !InputElement()
                {
                    if (m_ptr != System::IntPtr::Zero)
                    {
                        System::Runtime::InteropServices::Marshal::FreeHGlobal(m_ptr);
                        m_ptr = System::IntPtr::Zero;
                    }
                }

                ~InputElement()
                {
                    this->!InputElement();
                }

                static operator D3D12_INPUT_ELEMENT_DESC(InputElement% value)
                {
                    if (value.m_ptr == System::IntPtr::Zero)
                    {
                        value.m_ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(value.SemanticName);
                    }
                    return D3D12_INPUT_ELEMENT_DESC{
                        static_cast<LPCSTR>((void*)value.m_ptr),
                        value.SemanticIndex,
                        static_cast<DXGI_FORMAT>(value.Format),
                        value.InputSlot,
                        value.AlignedByteOffset,
                        static_cast<D3D12_INPUT_CLASSIFICATION>(value.InputSlotClass),
                        value.InstanceDataStepRate
                    };
                }

                static System::UInt32 AppendAlignedElement = D3D12_APPEND_ALIGNED_ELEMENT;
            private:
                System::IntPtr m_ptr;
            };

            [System::Runtime::InteropServices::StructLayout(System::Runtime::InteropServices::LayoutKind::Sequential)]
            public value struct Viewport
            {
                float TopLeftX;
                float TopLeftY;
                float Width;
                float Height;
                float MinDepth;
                float MaxDepth;

                static Viewport FromNative(const D3D12_VIEWPORT& value)
                {
                    return *reinterpret_cast<const Viewport*>(static_cast<const void*>(&value));
                }
            };

            public value struct GpuDescriptorHandle
            {
                System::UInt64 Pointer;

                D3D12_GPU_DESCRIPTOR_HANDLE ToNative()
                {
                    return { Pointer };
                }

                static GpuDescriptorHandle FromNative(const D3D12_GPU_DESCRIPTOR_HANDLE& value)
                {
                    return { value.ptr };
                }

                static GpuDescriptorHandle FromNative(D3D12_GPU_DESCRIPTOR_HANDLE&& value)
                {
                    return { value.ptr };
                }
            };

            public value struct CpuDescriptorHandle
            {
                size_t Pointer;

                D3D12_CPU_DESCRIPTOR_HANDLE ToNative()
                {
                    return { Pointer };
                }

                static CpuDescriptorHandle FromNative(const D3D12_CPU_DESCRIPTOR_HANDLE& value)
                {
                    return { value.ptr };
                }

                static CpuDescriptorHandle FromNative(D3D12_CPU_DESCRIPTOR_HANDLE&& value)
                {
                    return { value.ptr };
                }
            };

            [System::Runtime::InteropServices::StructLayout(System::Runtime::InteropServices::LayoutKind::Sequential)]
            public value struct RenderTargetBlendDescription
            {
                int BlendEnable;
                int LogicOpEnable;
                Blend SrcBlend;
                Blend DestBlend;
                BlendOperation BlendOp;
                Blend SrcBlendAlpha;
                Blend DestBlendAlpha;
                BlendOperation BlendOpAlpha;
                LogicOperation LogicOp;
                ColorWriteEnable RenderTargetWriteMask;
            };

            [System::Runtime::InteropServices::StructLayout(System::Runtime::InteropServices::LayoutKind::Sequential)]
            public value struct BlendDescription
            {
                int AlphaToCoverageEnable;
                int IndependentBlendEnable;
                inline_array<RenderTargetBlendDescription, 8 * sizeof(D3D12_RENDER_TARGET_BLEND_DESC)> RenderTarget;

                BlendDescription(bool alphaToCoverageEnable, bool independentBlendEnable, System::Memory<RenderTargetBlendDescription> renderTarget) :
                    AlphaToCoverageEnable(alphaToCoverageEnable),
                    IndependentBlendEnable(independentBlendEnable)
                {
                    Clear();

                    int maxIndex = min(renderTarget.Length, 8);
                    auto span = renderTarget.Span;
                    for (int i = 0; i < maxIndex; i++)
                    {
                        RenderTarget[i] = span[i];
                    }
                }

                BlendDescription(bool alphaToCoverageEnable, bool independentBlendEnable, System::Collections::Generic::IEnumerable<RenderTargetBlendDescription>^ renderTarget) :
                    AlphaToCoverageEnable(alphaToCoverageEnable),
                    IndependentBlendEnable(independentBlendEnable)
                {
                    Clear();

                    int index = 0;
                    for each (auto % e in renderTarget)
                    {
                        if (index < 8)
                        {
                            RenderTarget[index] = e;
                        }
                        else
                        {
                            break;
                        }
                        index++;
                    }
                }

                static BlendDescription Opaque = BlendDescription(1);

            private:
                BlendDescription(int) :
                    AlphaToCoverageEnable(false),
                    IndependentBlendEnable(false)
                {
                    Clear();
                    RenderTarget[0] = {
                        false, false, Blend::One, Blend::Zero, BlendOperation::Add, Blend::One, Blend::Zero, BlendOperation::Add, LogicOperation::Noop, ColorWriteEnable::All
                    };
                }

                void Clear()
                {
                    pin_ptr<RenderTargetBlendDescription> a = interior_ptr<RenderTargetBlendDescription>(&RenderTarget);
                    memset(a, 0, 8 * sizeof(D3D12_RENDER_TARGET_BLEND_DESC));
                }
            };

            [System::Runtime::InteropServices::StructLayout(System::Runtime::InteropServices::LayoutKind::Sequential)]
            public value struct DepthStencilOperationDescription
            {
                StencilOperation StencilFailOp;
                StencilOperation StencilDepthFailOp;
                StencilOperation StencilPassOp;
                ComparisonFunc StencilFunc;
            };

            [System::Runtime::InteropServices::StructLayout(System::Runtime::InteropServices::LayoutKind::Sequential)]
            public value struct DepthStencilDescription
            {
                int DepthEnable;
                DepthWriteMask DepthWriteMask;
                ComparisonFunc DepthFunc;
                int StencilEnable;
                unsigned char StencilReadMask;
                unsigned char StencilWriteMask;
                DepthStencilOperationDescription FrontFace;
                DepthStencilOperationDescription BackFace;

                static DepthStencilDescription Default =
                {
                    true,
                    DepthWriteMask::All,
                    ComparisonFunc::LessEqual,
                    false,
                    D3D12_DEFAULT_STENCIL_READ_MASK,
                    D3D12_DEFAULT_STENCIL_WRITE_MASK,
                    {
                        StencilOperation::Keep,
                        StencilOperation::Keep,
                        StencilOperation::Keep,
                        ComparisonFunc::Always
                    },
                    {
                        StencilOperation::Keep,
                        StencilOperation::Keep,
                        StencilOperation::Keep,
                        ComparisonFunc::Always
                    }
                };
            };

            [System::Runtime::InteropServices::StructLayout(System::Runtime::InteropServices::LayoutKind::Sequential)]
            public value struct RasteriazerDescription
            {
                FillMode FillMode;
                CullMode CullMode;
                int FrontCounterClockwise;
                int DepthBias;
                float DepthBiasClamp;
                float SlopeScaledDepthBias;
                int DepthClipEnable;
                int MultisampleEnable;
                int AntialiasedLineEnable;
                unsigned int ForcedSampleCount;
                ConservativeRasterizationMode ConservativeRaster;

                static RasteriazerDescription Default =
                {
                    FillMode::Solid,
                    CullMode::Back,
                    true,
                    0,
                    0.0f,
                    0.0f,
                    true,
                    false,
                    false,
                    false,
                    ConservativeRasterizationMode::Off
                };
            };

            [System::Runtime::InteropServices::StructLayout(System::Runtime::InteropServices::LayoutKind::Sequential)]
            public value struct SamplerDescrption
            {
                Filter Filter;
                TextureAddressMode AddressU;
                TextureAddressMode AddressV;
                TextureAddressMode AddressW;
                float MipLODBias;
                unsigned int MaxAnisotropy;
                ComparisonFunc ComparisonFunc;
                inline_array<float, 16> BorderColor;
                float MinLOD;
                float MaxLOD;

                SamplerDescrption(
                    Direct3D12::Filter filter,
                    TextureAddressMode addressU,
                    TextureAddressMode addressV,
                    TextureAddressMode addressW,
                    float mipLODBias,
                    unsigned int maxAnisotropy,
                    Direct3D12::ComparisonFunc comparisonFunc,
                    System::Memory<float> borderColor,
                    float minLOD,
                    float maxLOD
                ) :
                    Filter(filter),
                    AddressU(addressU),
                    AddressV(addressV),
                    AddressW(addressW),
                    MipLODBias(mipLODBias),
                    MaxAnisotropy(maxAnisotropy),
                    ComparisonFunc(comparisonFunc),
                    MinLOD(minLOD),
                    MaxLOD(maxLOD)
                {
                    Clear();

                    int maxIndex = min(borderColor.Length, 4);
                    auto span = borderColor.Span;
                    for (int i = 0; i < maxIndex; i++)
                    {
                        BorderColor[i] = span[i];
                    }
                }

                SamplerDescrption(
                    Direct3D12::Filter filter,
                    TextureAddressMode addressU,
                    TextureAddressMode addressV,
                    TextureAddressMode addressW,
                    float mipLODBias,
                    unsigned int maxAnisotropy,
                    Direct3D12::ComparisonFunc comparisonFunc,
                    System::Collections::Generic::IEnumerable<float>^ borderColor,
                    float minLOD,
                    float maxLOD
                ) :
                    Filter(filter),
                    AddressU(addressU),
                    AddressV(addressV),
                    AddressW(addressW),
                    MipLODBias(mipLODBias),
                    MaxAnisotropy(maxAnisotropy),
                    ComparisonFunc(comparisonFunc),
                    MinLOD(minLOD),
                    MaxLOD(maxLOD)
                {
                    Clear();

                    int index = 0;
                    for each (auto e in borderColor)
                    {
                        if (index < 8)
                        {
                            BorderColor[index] = e;
                        }
                        else
                        {
                            break;
                        }
                        index++;
                    }
                }

                static SamplerDescrption LinearWrap = SamplerDescrption(
                    Filter::MinMagMipLinear,
                    TextureAddressMode::Wrap, // AddressU
                    TextureAddressMode::Wrap, // AddressV
                    TextureAddressMode::Wrap, // AddressW
                    0, // MipLODBias
                    D3D12_MAX_MAXANISOTROPY,
                    ComparisonFunc::Never,
                    { 0, 0, 0, 0 }, // BorderColor
                    0, // MinLOD
                    FLT_MAX // MaxLOD
                );

            private:
                SamplerDescrption(
                    Direct3D12::Filter filter,
                    TextureAddressMode addressU,
                    TextureAddressMode addressV,
                    TextureAddressMode addressW,
                    float mipLODBias,
                    unsigned int maxAnisotropy,
                    Direct3D12::ComparisonFunc comparisonFunc,
                    std::initializer_list<float>&& borderColor,
                    float minLOD,
                    float maxLOD
                ) :
                    Filter(filter),
                    AddressU(addressU),
                    AddressV(addressV),
                    AddressW(addressW),
                    MipLODBias(mipLODBias),
                    MaxAnisotropy(maxAnisotropy),
                    ComparisonFunc(comparisonFunc),
                    MinLOD(minLOD),
                    MaxLOD(maxLOD)
                {
                    Clear();

                    int index = 0;
                    for (auto& e : borderColor)
                    {
                        if (index < 8)
                        {
                            BorderColor[index] = e;
                        }
                        else
                        {
                            break;
                        }
                        index++;
                    }
                }

                void Clear()
                {
                    pin_ptr<float> a = interior_ptr<float>(&BorderColor);
                    memset(a, 0, 4 * sizeof(float));
                }
            };

            public value struct Device
            {
                ID3D12Device* Pointer;
                Device(ID3D12Device* pointer) : Pointer(pointer)
                {

                }

                void CreateSampler(SamplerDescrption% desc, CpuDescriptorHandle handle)
                {
                    pin_ptr<SamplerDescrption> ptr = &desc;
                    Pointer->CreateSampler(reinterpret_cast<D3D12_SAMPLER_DESC*>(ptr), handle.ToNative());
                }
            };

            public value struct CommandQueue
            {
                ID3D12CommandQueue* Pointer;
                CommandQueue(ID3D12CommandQueue* pointer) : Pointer(pointer)
                {

                }
            };

            public value struct CommandList
            {
                ID3D12CommandList* Pointer;
                CommandList(ID3D12CommandList* pointer) : Pointer(pointer)
                {

                }
            };

            public ref class Resource
            {
            public:
                ID3D12Resource* Value;

                Resource()
                {
                    Value = nullptr;
                }

                Resource(ID3D12Resource* value)
                {
                    Value = value;
                    if (Value != nullptr)
                    {
                        Value->AddRef();
                    }
                }

                void Release()
                {
                    if (Value != nullptr)
                    {
                        Value->Release();
                    }
                    Value = nullptr;
                }

                ~Resource()
                {
                    this->!Resource();
                }

                !Resource()
                {
                    Release();
                }
            };

        }
    }
}
#pragma once
// D3D enums for clr
namespace DX
{
    namespace Sharp
    {
        namespace Direct3D
        {
            public enum class FeatureLevel
            {
                Level_1_0_Core = 0x1000,
                Level_9_1 = 0x9100,
                Level_9_2 = 0x9200,
                Level_9_3 = 0x9300,
                Level_10_0 = 0xa000,
                Level_10_1 = 0xa100,
                Level_11_0 = 0xb000,
                Level_11_1 = 0xb100,
                Level_12_0 = 0xc000,
                Level_12_1 = 0xc100
            };

            public enum class PrimitiveTopology
            {
                Undefined = 0,
                PointList = 1,
                LineList = 2,
                LineStrip = 3,
                TriangleList = 4,
                TriangleStrip = 5,
                LineListWithAdjacency = 10,
                LineStripWithAdjacency = 11,
                TriangleListWithAdjacency = 12,
                TriangleStripWithAdjacency = 13,
                PatchListWith1ControlPoints = 33,
                PatchListWith2ControlPoints = 34,
                PatchListWith3ControlPoints = 35,
                PatchListWith4ControlPoints = 36,
                PatchListWith5ControlPoints = 37,
                PatchListWith6ControlPoints = 38,
                PatchListWith7ControlPoints = 39,
                PatchListWith8ControlPoints = 40,
                PatchListWith9ControlPoints = 41,
                PatchListWith10ControlPoints = 42,
                PatchListWith11ControlPoints = 43,
                PatchListWith12ControlPoints = 44,
                PatchListWith13ControlPoints = 45,
                PatchListWith14ControlPoints = 46,
                PatchListWith15ControlPoints = 47,
                PatchListWith16ControlPoints = 48,
                PatchListWith17ControlPoints = 49,
                PatchListWith18ControlPoints = 50,
                PatchListWith19ControlPoints = 51,
                PatchListWith20ControlPoints = 52,
                PatchListWith21ControlPoints = 53,
                PatchListWith22ControlPoints = 54,
                PatchListWith23ControlPoints = 55,
                PatchListWith24ControlPoints = 56,
                PatchListWith25ControlPoints = 57,
                PatchListWith26ControlPoints = 58,
                PatchListWith27ControlPoints = 59,
                PatchListWith28ControlPoints = 60,
                PatchListWith29ControlPoints = 61,
                PatchListWith30ControlPoints = 62,
                PatchListWith31ControlPoints = 0x3F,
                PatchListWith32ControlPoints = 0x40
            };
        }

        namespace Direct3D12
        {
            public enum class DescriptorHeapType
            {
                ConstantBufferViewShaderResourceViewUnorderedAccessView = 0,
                Sampler = (ConstantBufferViewShaderResourceViewUnorderedAccessView + 1),
                RenderTargetView = (Sampler + 1),
                DepthStencilView = (RenderTargetView + 1),
                NumTypes = (DepthStencilView + 1)
            };

            [System::Flags]
            public enum class DescriptorHeapFlags
            {
                None = 0x0,
                ShaderVisible = 0x1
            };

            public enum class PrimitiveTopologyType
            {
                Undefined = 0,
                Point = 1,
                Line = 2,
                Triangle = 3,
                Patch = 4
            };

            public enum class IndexBufferStripCutValue
            {
                Disabled = 0,
                Value0xFFFF = 1,
                Value0xFFFFFFFF = 2
            };

            public enum class Blend
            {
                Zero = 1,
                One = 2,
                SourceColor = 3,
                InverseSourceColor = 4,
                SourceAlpha = 5,
                InverseSourceAlpha = 6,
                DestinationAlpha = 7,
                InverseDestinationAlpha = 8,
                DestinationColor = 9,
                InverseDestinationColor = 10,
                SourceAlphaSaturate = 11,
                BlendFactor = 14,
                InverseBlendFactor = 15,
                SecondarySourceColor = 16,
                InverseSecondarySourceColor = 17,
                SecondarySourceAlpha = 18,
                InverseSecondarySourceAlpha = 19
            };

            public enum class BlendOperation
            {
                Add = 1,
                Subtract = 2,
                ReverseSubtract = 3,
                Minimum = 4,
                Maximum = 5
            };

            public enum class LogicOperation
            {
                Clear = 0,
                Set = (Clear + 1),
                Copy = (Set + 1),
                CopyInverted = (Copy + 1),
                Noop = (CopyInverted + 1),
                Invert = (Noop + 1),
                And = (Invert + 1),
                Nand = (And + 1),
                Or = (Nand + 1),
                Nor = (Or + 1),
                Xor = (Nor + 1),
                Equiv = (Xor + 1),
                AndReverse = (Equiv + 1),
                AndInverted = (AndReverse + 1),
                OrReverse = (AndInverted + 1),
                OrInverted = (OrReverse + 1)
            };

            [System::Flags]
            public enum class ColorWriteEnable : System::Byte
            {
                Red = 1,
                Green = 2,
                Blue = 4,
                Alpha = 8,
                All = (((Red | Green) | Blue) | Alpha)
            };

            public enum class DepthWriteMask
            {
                Zero = 0,
                All = 1
            };

            public enum class ComparisonFunc
            {
                Never = 1,
                Less = 2,
                Equal = 3,
                LessEqual = 4,
                Greater = 5,
                NotEqual = 6,
                GreaterEqual = 7,
                Always = 8
            };

            public enum class StencilOperation
            {
                Keep = 1,
                Zero = 2,
                Replace = 3,
                IncrementAndClamp = 4,
                DecrementAndClamp = 5,
                Invert = 6,
                Increment = 7,
                Decrement = 8
            };


            public enum class FillMode
            {
                Wireframe = 2,
                Solid = 3
            };

            public enum class CullMode
            {
                None = 1,
                Front = 2,
                Back = 3
            };

            public enum class ConservativeRasterizationMode
            {
                Off = 0,
                On = 1
            };

            public enum class InputClassification
            {
                PerVertexData = 0,
                PerInstanceData = 1
            };

            public enum class TextureAddressMode
            {
                Wrap = 1,
                Mirror = 2,
                Clamp = 3,
                Border = 4,
                MirrorOnce = 5
            };

            public enum class Filter
            {
                MinMagMipPoint = 0,
                MinMagPointMipLinear = 1,
                MinPointMagLinearMipPoint = 4,
                MinPointMagMipLinear = 5,
                MinLinearMagMipPoint = 0x10,
                MinLinearMagPointMipLinear = 17,
                MinMagLinearMipPoint = 20,
                MinMagMipLinear = 21,
                Anisotropic = 85,
                ComparisonMinMagMipPoint = 0x80,
                ComparisonMinMagPointMipLinear = 129,
                ComparisonMinPointMagLinearMipPoint = 132,
                ComparisonMinPointMagMipLinear = 133,
                ComparisonMinLinearMagMipPoint = 144,
                ComparisonMinLinearMagPointMipLinear = 145,
                ComparisonMinMagLinearMipPoint = 148,
                ComparisonMinMagMipLinear = 149,
                ComparisonAnisotropic = 213,
                MinimumMinMagMipPoint = 0x100,
                MinimumMinMagPointMipLinear = 257,
                MinimumMinPointMagLinearMipPoint = 260,
                MinimumMinPointMagMipLinear = 261,
                MinimumMinLinearMagMipPoint = 272,
                MinimumMinLinearMagPointMipLinear = 273,
                MinimumMinMagLinearMipPoint = 276,
                MinimumMinMagMipLinear = 277,
                MinimumAnisotropic = 341,
                MaximumMinMagMipPoint = 384,
                MaximumMinMagPointMipLinear = 385,
                MaximumMinPointMagLinearMipPoint = 388,
                MaximumMinPointMagMipLinear = 389,
                MaximumMinLinearMagMipPoint = 400,
                MaximumMinLinearMagPointMipLinear = 401,
                MaximumMinMagLinearMipPoint = 404,
                MaximumMinMagMipLinear = 405,
                MaximumAnisotropic = 469
            };
        }

        namespace DXGI {
            public enum class Format : System::UInt32
            {
                Unknown = 0,
                R32G32B32A32Typeless = 1,
                R32G32B32A32Float = 2,
                R32G32B32A32UInt = 3,
                R32G32B32A32SInt = 4,
                R32G32B32Typeless = 5,
                R32G32B32Float = 6,
                R32G32B32UInt = 7,
                R32G32B32SInt = 8,
                R16G16B16A16Typeless = 9,
                R16G16B16A16Float = 10,
                R16G16B16A16UNorm = 11,
                R16G16B16A16UInt = 12,
                R16G16B16A16SNorm = 13,
                R16G16B16A16SInt = 14,
                R32G32Typeless = 0xF,
                R32G32Float = 0x10,
                R32G32UInt = 17,
                R32G32SInt = 18,
                R32G8X24Typeless = 19,
                D32FloatS8X24UInt = 20,
                R32FloatX8X24Typeless = 21,
                X32TypelessG8X24UInt = 22,
                R10G10B10A2Typeless = 23,
                R10G10B10A2UNorm = 24,
                R10G10B10A2UInt = 25,
                R11G11B10Float = 26,
                R8G8B8A8Typeless = 27,
                R8G8B8A8UNorm = 28,
                R8G8B8A8UNormSRgb = 29,
                R8G8B8A8UInt = 30,
                R8G8B8A8SNorm = 0x1F,
                R8G8B8A8SInt = 0x20,
                R16G16Typeless = 33,
                R16G16Float = 34,
                R16G16UNorm = 35,
                R16G16UInt = 36,
                R16G16SNorm = 37,
                R16G16SInt = 38,
                R32Typeless = 39,
                D32Float = 40,
                R32Float = 41,
                R32UInt = 42,
                R32SInt = 43,
                R24G8Typeless = 44,
                D24UNormS8UInt = 45,
                R24UNormX8Typeless = 46,
                X24TypelessG8UInt = 47,
                R8G8Typeless = 48,
                R8G8UNorm = 49,
                R8G8UInt = 50,
                R8G8SNorm = 51,
                R8G8SInt = 52,
                R16Typeless = 53,
                R16Float = 54,
                D16UNorm = 55,
                R16UNorm = 56,
                R16UInt = 57,
                R16SNorm = 58,
                R16SInt = 59,
                R8Typeless = 60,
                R8UNorm = 61,
                R8UInt = 62,
                R8SNorm = 0x3F,
                R8SInt = 0x40,
                A8UNorm = 65,
                R1UNorm = 66,
                R9G9B9E5Sharedexp = 67,
                R8G8B8G8UNorm = 68,
                G8R8G8B8UNorm = 69,
                BC1Typeless = 70,
                BC1UNorm = 71,
                BC1UNormSRgb = 72,
                BC2Typeless = 73,
                BC2UNorm = 74,
                BC2UNormSRgb = 75,
                BC3Typeless = 76,
                BC3UNorm = 77,
                BC3UNormSRgb = 78,
                BC4Typeless = 79,
                BC4UNorm = 80,
                BC4SNorm = 81,
                BC5Typeless = 82,
                BC5UNorm = 83,
                BC5SNorm = 84,
                B5G6R5UNorm = 85,
                B5G5R5A1UNorm = 86,
                B8G8R8A8UNorm = 87,
                B8G8R8X8UNorm = 88,
                R10G10B10XrBiasA2UNorm = 89,
                B8G8R8A8Typeless = 90,
                B8G8R8A8UNormSRgb = 91,
                B8G8R8X8Typeless = 92,
                B8G8R8X8UNormSRgb = 93,
                BC6HTypeless = 94,
                BC6HUf16 = 95,
                BC6HSf16 = 96,
                BC7Typeless = 97,
                BC7UNorm = 98,
                BC7UNormSRgb = 99,
                AYUV = 100,
                Y410 = 101,
                Y416 = 102,
                NV12 = 103,
                P010 = 104,
                P016 = 105,
                Opaque420 = 106,
                YUY2 = 107,
                Y210 = 108,
                Y216 = 109,
                NV11 = 110,
                AI44 = 111,
                IA44 = 112,
                P8 = 113,
                A8P8 = 114,
                B4G4R4A4UNorm = 115,
                P208 = 130,
                V208 = 131,
                V408 = 132,
                SamplerFeedbackMinMipOpaque = 189,
                SamplerFeedbackMipRegionUsedOpaque = 190,
                ForceUint = 0xffffffff
            };
        }
    }
}
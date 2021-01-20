#pragma once
namespace DX
{
    namespace Sharp
    {
        template<typename T, int byteSize>
        [System::Runtime::CompilerServices::UnsafeValueType]
        [System::Runtime::InteropServices::StructLayout
        (
            System::Runtime::InteropServices::LayoutKind::Explicit,
            Size = byteSize
        )
        ]
        public value struct inline_array {
        private:
            [System::Runtime::InteropServices::FieldOffset(0)]
            T dummy_item;

        public:
            T% operator[](int index) {
                return *((&dummy_item) + index);
            }

            static operator interior_ptr<T>(inline_array<T, byteSize>% ia) {
                return &ia.dummy_item;
            }
        };
    }
}
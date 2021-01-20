using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Fxxk.Net
{
    public unsafe struct Vector2U32
    {
        private fixed uint data[2];

        public uint X
        {
            set
            {
                data[0] = value;
            }
            get
            {
                return data[0];
            }
        }

        public uint Y
        {
            set
            {
                data[1] = value;
            }
            get
            {
                return data[1];
            }
        }
    }

    public unsafe struct Vector2I32
    {
        private fixed int data[2];

        public int X
        {
            set
            {
                data[0] = value;
            }
            get
            {
                return data[0];
            }
        }

        public int Y
        {
            set
            {
                data[1] = value;
            }
            get
            {
                return data[1];
            }
        }
    }

    public unsafe struct Vector3F32
    {
        private fixed float data[3];

        public float X
        {
            set
            {
                data[0] = value;
            }
            get
            {
                return data[0];
            }
        }

        public float Y
        {
            set
            {
                data[1] = value;
            }
            get
            {
                return data[1];
            }
        }

        public float Z
        {
            set
            {
                data[2] = value;
            }
            get
            {
                return data[2];
            }
        }
    }

    public unsafe struct Vector2F32
    {
        private fixed float data[2];

        public float X
        {
            set
            {
                data[0] = value;
            }
            get
            {
                return data[0];
            }
        }

        public float Y
        {
            set
            {
                data[1] = value;
            }
            get
            {
                return data[1];
            }
        }
    }

    public struct Vertex
    {
        public Vector3F32 Position;
        public Vector3F32 Normal;
        public Vector2F32 TexCoord;
    }
}

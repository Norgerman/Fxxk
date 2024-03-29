using DX.Sharp;
using DX.Sharp.Direct3D12;
using DX.Sharp.DXGI;
using DX.Sharp.SimpleMath;
using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;

namespace Fxxk.Net
{
    unsafe static class Program
    {
        static void Transform(XMMatrix target, Viewport viewport, float angle, float scale, float w, float h)
        {
            using var r = new XMMatrix();
            using var b = new XMMatrix();
            using var s = new XMMatrix();
            using var t = new XMMatrix();

            r.RotationZ((float)(angle / 180 * Math.PI));
            s.Scaling(scale, scale, 1.0f);
            t.Translation(viewport.Width / 2 - w * scale / 2, viewport.Height / 2 - h * scale / 2, 0.0f);
            b.Translation(-viewport.Width / 2, -viewport.Height / 2, 0.0f);
            b.Multiply(r);
            r.Set(b);
            b.Identity();
            b.Translation(viewport.Width / 2, viewport.Height / 2, 0.0f);
            r.Multiply(b);

            s.Multiply(t);
            s.Multiply(r);

            target.Set(s);
        }

        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [MTAThread]
        static void Main()
        {
            Application.SetCompatibleTextRenderingDefault(false);
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.EnableVisualStyles();

            var texturePathA = Path.Combine(Path.GetDirectoryName(AppContext.BaseDirectory), "a.png");
            var texturePathB = Path.Combine(Path.GetDirectoryName(AppContext.BaseDirectory), "b.png");
            var textureDataA = File.ReadAllBytes(texturePathA);
            var textureDataB = File.ReadAllBytes(texturePathB);
            var vertexShader = File.ReadAllBytes(Path.Combine(Path.GetDirectoryName(AppContext.BaseDirectory), "VertexShader.cso"));
            var pixelShader = File.ReadAllBytes(Path.Combine(Path.GetDirectoryName(AppContext.BaseDirectory), "PixelShader.cso"));

            using var form = new Form();
            using var scene = new Scene3D();

#if DEBUG
            scene.EnableDebug();
#endif
            scene.Initialize(form.Handle, 0.0f, 0.0f, form.Size.Width, form.Size.Height);
            float w = 1600;
            float h = 450;
            var indices = new ushort[6] { 0, 1, 2, 0, 2, 3 };
            var vertices = new Vertex[4]
            {
                new Vertex()
                {
                    Position = new Vector3F32(),
                    Normal = new Vector3F32 { Z = 1.0f },
                    TexCoord = new Vector2F32 { X = 0.0f, Y = 1.0f }
                },
                new Vertex()
                {
                    Position = new Vector3F32 { X = w },
                    Normal = new Vector3F32 { Z = 1.0f },
                    TexCoord = new Vector2F32 { X = 2.0f, Y = 1.0f }
                },
                new Vertex()
                {
                    Position = new Vector3F32 { X = w, Y = h },
                    Normal = new Vector3F32 { Z = 1.0f },
                    TexCoord = new Vector2F32 { X = 2.0f, Y = 0.0f }
                },
                new Vertex()
                {
                    Position = new Vector3F32 { Y = h },
                    Normal = new Vector3F32 { Z = 1.0f},
                    TexCoord = new Vector2F32()
                }
            };
            var textureMatrixX = stackalloc float[9];
            var textureMatrixY = stackalloc float[12];
            var color = stackalloc float[4] {
                0.0f,
                0.0f,
                0.0f,
                0.0f,
            };

            textureMatrixX[0] = textureMatrixX[4] = textureMatrixX[8] = 1.0f;
            textureMatrixY[0] = textureMatrixY[5] = textureMatrixY[10] = 1.0f;


            var inputs = new List<InputElement>()
            {
                new InputElement()
                {
                    SemanticName = "POS",
                    SemanticIndex = 0,
                    Format = Format.R32G32B32Float,
                    InputSolt = 0,
                    AlignedByteOffset = InputElement.AppendAlignedElement,
                    InputSlotClass = InputClassification.PerVertexData,
                    InstanceDataStepRate = 0
                },
                new InputElement()
                {
                    SemanticName = "NORMAL",
                    SemanticIndex = 0,
                    Format = Format.R32G32B32Float,
                    InputSolt = 0,
                    AlignedByteOffset = InputElement.AppendAlignedElement,
                    InputSlotClass = InputClassification.PerVertexData,
                    InstanceDataStepRate = 0
                },
                new InputElement()
                {
                    SemanticName = "TEXCOORD",
                    SemanticIndex = 0,
                    Format = Format.R32G32Float,
                    InputSolt = 0,
                    AlignedByteOffset = InputElement.AppendAlignedElement,
                    InputSlotClass = InputClassification.PerVertexData,
                    InstanceDataStepRate = 0
                },
                new InputElement()
                {
                    SemanticName = "TEXMATRIX",
                    SemanticIndex = 0,
                    Format = Format.R32G32B32Float,
                    InputSolt = 1,
                    AlignedByteOffset = InputElement.AppendAlignedElement,
                    InputSlotClass = InputClassification.PerInstanceData,
                    InstanceDataStepRate = 0
                },
                new InputElement()
                {
                    SemanticName = "TEXMATRIX",
                    SemanticIndex = 1,
                    Format = Format.R32G32B32Float,
                    InputSolt = 1,
                    AlignedByteOffset = InputElement.AppendAlignedElement,
                    InputSlotClass = InputClassification.PerInstanceData,
                    InstanceDataStepRate = 0
                },
                new InputElement()
                {
                    SemanticName = "TEXMATRIX",
                    SemanticIndex = 2,
                    Format = Format.R32G32B32Float,
                    InputSolt = 1,
                    AlignedByteOffset = InputElement.AppendAlignedElement,
                    InputSlotClass = InputClassification.PerInstanceData,
                    InstanceDataStepRate = 0
                }
            };

            using var index = new MemoryIndex<ushort>(scene, indices);
            using var attribute1 = new MemoryAttribute<Vertex>(scene, vertices, (uint)sizeof(Vertex));
            using var attribute2 = new DX.Sharp.Attribute(scene, textureMatrixX, 4, 9, 36);
            using var constant = new Constant(scene, textureMatrixY, 4, 12);
            using var constant2 = new Constant(scene, color, 4, 4);
            using var textureHeap1 = new DescriptorHeap(scene.Device, 2);
            using var textureHeap2 = new DescriptorHeap(scene.Device, 2);
            using var samplerHeap = new DescriptorHeap(scene.Device, DescriptorHeapType.Sampler, DescriptorHeapFlags.ShaderVisible, 1);
            using var effect = new Effect3D(inputs, new Constant[] { constant2 }, 1, 2, 1);
            using var loader = new TextureLoader(scene.Device, scene.CommandQueue);

            effect.Initialize(scene, vertexShader, pixelShader, ref BlendDescription.Opaque, ref DepthStencilDescription.Default, ref RasteriazerDescription.Default, Format.B8G8R8A8UNorm, Format.D32Float);

            var textures = new List<Resource>()
            {
                new Resource(),
                new Resource()
            };


            scene.Device.CreateSampler(ref SamplerDescrption.LinearWrap, samplerHeap.GetCpuHandle(0));

            loader.Begin();
            loader.CreateTexture(textureDataA, textures[0]);
            loader.CreateShaderResourceView(textures[0], textureHeap1.GetCpuHandle(0));
            loader.CreateShaderResourceView(textures[0], textureHeap2.GetCpuHandle(1));
            loader.CreateTexture(textureDataB, textures[1]);
            loader.CreateShaderResourceView(textures[1], textureHeap1.GetCpuHandle(1));
            loader.CreateShaderResourceView(textures[1], textureHeap2.GetCpuHandle(0));
            loader.End();


            using var obj1 = new Object3D(new List<DX.Sharp.Attribute> { attribute1, attribute2 }, index, new List<Constant> { constant }, effect, textureHeap1, samplerHeap);
            using var obj2 = new Object3D(new List<DX.Sharp.Attribute> { attribute1, attribute2 }, index, new List<Constant> { constant }, effect, textureHeap2, samplerHeap);

            obj1.Initialize(scene);
            obj2.Initialize(scene);

            scene.SetRenderList(new List<Object3D> { obj1, obj2 });
            using var transform = new XMMatrix();
            var minimized = false;

            scene.SetInactiveTargetUpdateTimeout(true, 1.0 / 30);

            form.Resize += (sender, e) =>
            {
                if (form.WindowState == FormWindowState.Minimized)
                {
                    scene.OnSuspending();
                    minimized = true;
                }
                else if (minimized)
                {
                    scene.OnResuming();
                    minimized = false;
                }

                scene.OnWindowSizeChanged(0.0f, 0.0f, form.Size.Width, form.Size.Height);
                scene.Tick();
            };

            form.Activated += (sender, e) =>
            {
                scene.OnActivated();
            };

            form.Deactivate += (sender, e) =>
            {
                scene.OnDeactivated();
            };

            uint count = 0;
            float scale = 1;
            float step = 0.01f;
            float angle = 0;
            float angleStep = 1;
            var random = new Random();
            var viewMatrix = new XMMatrix();

            scene.OnUpdate += (scene, second, frame) =>
            {
                if (frame > 1)
                {
                    scale += step;
                    angle += angleStep;
                    count++;
                }

                if (angle > 360)
                {
                    angle -= 360;
                }

                if (scale > 1.0f || scale < 0.1)
                {
                    step = -step;
                }

                textureMatrixX[0] = scale;
                textureMatrixY[5] = scale;

                if (count == 10)
                {
                    count = 0;
                    color[0] = (float)random.NextDouble();
                    color[1] = (float)random.NextDouble();
                    color[2] = (float)random.NextDouble();
                    color[3] = (float)random.NextDouble();
                }

                attribute2.Update(textureMatrixX);
                constant.Update(textureMatrixY);
                constant2.Update(color);

                Transform(transform, scene.Viewport, angle, scale, w, h);
                obj1.UpdateModelView(transform, viewMatrix);

                Transform(transform, scene.Viewport, angle * 2, scale, w, h);
                obj2.UpdateModelView(transform, viewMatrix);
            };

            RenderLoop.Run(form, scene.Tick);
        }
    }
}

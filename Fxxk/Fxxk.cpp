// Fxxk.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Fxxk.h"
#include "D3DScene.h"
#include "D3DObject.h"
#include "D3DConstant.h"
#include "D3DAttribute.h"
#include "D3DIndex.h"

#define M_PI acosf(-1.0)

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

// Global Variables:
constexpr auto MAX_LOADSTRING = 100;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
//D3DScene* g_scene;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND& hwnd);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
unique_ptr<D3DScene> g_scene;

inline float Randf()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

inline DirectX::XMMATRIX Transform(const D3D12_VIEWPORT& viewport, float angle, float scale, float w, float h)
{
    auto r = DirectX::XMMatrixRotationZ(angle / 180 * M_PI);
    auto base = DirectX::XMMatrixTranslation(-viewport.Width / 2, -viewport.Height / 2, 0.0f);
    r = DirectX::XMMatrixMultiply(base, r);
    base.r[3] = DirectX::XMVectorMultiply(base.r[3], DirectX::XMVECTOR({ -1.0f, -1.0f, 0.0f, 1.0f }));
    r = DirectX::XMMatrixMultiply(r, base);

    auto s = DirectX::XMMatrixScaling(scale, scale, 1.0f);
    auto t = DirectX::XMMatrixTranslation(viewport.Width / 2 - w * scale / 2, viewport.Height / 2 - h * scale / 2, 0.0f);

    auto result = DirectX::XMMatrixMultiply(s, t);

    return DirectX::XMMatrixMultiply(result, r);

    //return DirectX::XMMatrixIdentity();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    wchar_t path[MAX_PATH];

    GetModuleFileNameW(NULL, path, MAX_PATH);
    PathCchRemoveFileSpec(path, MAX_PATH);

    wstring s = path;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    HWND hwnd = NULL;


    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow, hwnd))
    {
        return FALSE;
    }

    g_scene = make_unique<D3DScene>();
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_scene.get()));


    RECT winRect;
    GetClientRect(hwnd, &winRect);
    g_scene->Initialize(hwnd, 0.0f, 0.0f, static_cast<float>(winRect.right - winRect.left), static_cast<float>(winRect.bottom - winRect.top));
    D3D12_SAMPLER_DESC samplerState =
    {
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        0.0f,
        1,
        D3D12_COMPARISON_FUNC_ALWAYS,
        { 0, 0, 0, 0},
        0,
        D3D12_FLOAT32_MAX,
    };

    ID3DBlob* vs = nullptr, * ps = nullptr, * error = nullptr;
#if _DEBUG
    auto compileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
    auto compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

    auto hr = D3DCompileFromFile(
        (s + L"\\VertexShader.hlsl").data(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_5_0",
        compileFlags,
        0,
        &vs,
        &error);
    assert(SUCCEEDED(hr));

    hr = D3DCompileFromFile(
        (s + L"\\PixelShader.hlsl").data(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_5_0",
        compileFlags,
        0,
        &ps,
        &error);
    assert(SUCCEEDED(hr));

    if (error) {
        error->Release();
    }

    float w = 1600;
    float h = 450;

    std::vector<ComPtr<ID3D12Resource>> textures;
    ComPtr<ID3D12Resource> texture = nullptr;
    std::vector<wstring> textureFiles = { s + L"\\a.png", s + L"\\b.png" };
    std::shared_ptr<DescriptorHeap> textureHeap = make_shared<DescriptorHeap>(g_scene->Device().Get(), 2);
    std::shared_ptr<DescriptorHeap> textureHeap2 = make_shared<DescriptorHeap>(g_scene->Device().Get(), 2);
    std::shared_ptr<DescriptorHeap> samplerHeap = make_shared<DescriptorHeap>(g_scene->Device().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);
    size_t idx = 0;

    ResourceUploadBatch resourceUpload(g_scene->Device().Get());
    resourceUpload.Begin();


    for (auto& element : textureFiles)
    {
        hr = DirectX::CreateWICTextureFromFile(g_scene->Device().Get(), resourceUpload, element.data(), &texture, false);
        assert(SUCCEEDED(hr));
        textures.push_back(texture);
        CreateShaderResourceView(g_scene->Device().Get(), texture.Get(), textureHeap->GetCpuHandle(idx));
        CreateShaderResourceView(g_scene->Device().Get(), texture.Get(), textureHeap2->GetCpuHandle((idx + 1) % 2));
        idx++;
    }
    resourceUpload.End(g_scene->CommandQueue().Get()).wait();
    g_scene->Device()->CreateSampler(&samplerState, samplerHeap->GetCpuHandle(0));

    DirectX::VertexPositionNormalTexture vertices[] = {
        { XMFLOAT3({ 0.0f, 0.0f, 0.0f }), XMFLOAT3({ 0.0f, 0.0f, 1.0f }), XMFLOAT2({ 0.0f, 1.0f }) },
        { XMFLOAT3({ w, 0.0f, 0.0f }), XMFLOAT3({ 0.0f, 0.0f, 1.0f }), XMFLOAT2({  2.0f, 1.0f }) },
        { XMFLOAT3({ w, h, 0.0f }), XMFLOAT3({ 0.0f, 0.0f, 1.0f }), XMFLOAT2({ 2.0f, 0.0f }) },
        { XMFLOAT3({ 0.0f, h, 0.0f }), XMFLOAT3({ 0.0f, 0.0f, 1.0f }), XMFLOAT2({ 0.0f, 0.0f }) },
    };

    vector<D3D12_INPUT_ELEMENT_DESC> verticesDesc =
    {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    D3D12_SHADER_BYTECODE vertexShader = {
        vs->GetBufferPointer(),
        vs->GetBufferSize()
    };

    D3D12_SHADER_BYTECODE pixelShader = {
      ps->GetBufferPointer(),
      ps->GetBufferSize()
    };

    uint32_t indices[] = {
         0, 1, 2, 0, 2, 3
    };

    float textureMatrixX[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
    vector<D3D12_INPUT_ELEMENT_DESC> textureMatrixDesc =
    {
        { "TEXMATRIX", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0},
        { "TEXMATRIX", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0},
        { "TEXMATRIX", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0}
    };


    float textureMatrixY[] =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
    };

    float color[] = {
        0.0f, 0.0f, 0.0f, 0.0f,
    };

    RenderTargetState renderTargetState(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT);

    D3DObject obj1(
        initializer_list {
            D3DAttribute(&vertices, sizeof(VertexPositionNormalTexture), 4, sizeof(VertexPositionNormalTexture), verticesDesc),
            D3DAttribute(&textureMatrixX, sizeof(float), 9, sizeof(float) * 9, textureMatrixDesc)
        },
        D3DIndex(&indices, 4, 6),
        initializer_list{
            D3DConstant(&textureMatrixY, 4, 16),
            D3DConstant(&color, 4, 4)
        },
        textureHeap,
        samplerHeap
    );

    D3DObject obj2(
        initializer_list {
            D3DAttribute(&vertices, sizeof(VertexPositionNormalTexture), 4, sizeof(VertexPositionNormalTexture), verticesDesc),
            D3DAttribute(&textureMatrixX, sizeof(float), 9, sizeof(float) * 9, textureMatrixDesc)
        },
        D3DIndex(&indices, 4, 6),
        initializer_list {
            D3DConstant(&textureMatrixY, 4, 16),
            D3DConstant(&color, 4, 4)
        },
        textureHeap2,
        samplerHeap
    );

    obj1.Initialize(*g_scene, vertexShader, pixelShader, CommonStates::Opaque, CommonStates::DepthDefault, D3DGlobal::defaultRasterizerStateDesc, renderTargetState);
    obj2.Initialize(*g_scene, vertexShader, pixelShader, CommonStates::Opaque, CommonStates::DepthDefault, D3DGlobal::defaultRasterizerStateDesc, renderTargetState);

    ps->Release();
    vs->Release();

    g_scene->SetRenderList(std::initializer_list<D3DObject*>{ &obj1, &obj2 });

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg = {};
    uint32_t count = 0;
    float scale = 1;
    float step = 0.00f;
    float angle = 0;
    float angleStep = 1;

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            scale += step;
            angle += angleStep;
            count++;

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
                color[0] = Randf();
                color[1] = Randf();
                color[2] = Randf();
                color[3] = Randf();
            }

            obj1.UpdateAttribute(1, textureMatrixX);
            obj1.UpdateConstant(0, textureMatrixY);
            obj1.UpdateConstant(1, color);
            obj2.UpdateAttribute(1, textureMatrixX);
            obj2.UpdateConstant(0, textureMatrixY);
            obj2.UpdateConstant(1, color);

            obj1.UpdateTransform(Transform(g_scene->Viewport(), angle, scale, w, h));
            obj2.UpdateTransform(Transform(g_scene->Viewport(), angle * 2, scale, w, h));
            g_scene->Tick();
        }
    }
    g_scene.reset();
    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND& hwnd)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    //UpdateWindow(hWnd);

    hwnd = hWnd;

    return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;
    static bool s_fullscreen = false;

    auto scene = reinterpret_cast<D3DScene*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        if (s_in_sizemove && scene)
        {
            scene->Tick();
        }
        else
        {
            PAINTSTRUCT ps;
            (void)BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
    }
    break;
    case WM_SIZE:
    {
        if (wParam == SIZE_MINIMIZED)
        {
            if (!s_minimized)
            {
                s_minimized = true;
                if (!s_in_suspend && scene)
                    scene->OnSuspending();
                s_in_suspend = true;
            }
        }
        else if (s_minimized)
        {
            s_minimized = false;
            if (s_in_suspend && scene)
                scene->OnResuming();
            s_in_suspend = false;
        }
        else if (!s_in_sizemove && scene)
        {
            scene->OnWindowSizeChanged(0.0f, 0.0f, static_cast<float>(LOWORD(lParam)), static_cast<float>(HIWORD(lParam)));
        }
    }
    break;
    case WM_ENTERSIZEMOVE:
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;
        if (scene)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);

            scene->OnWindowSizeChanged(0.0f, 0.0f, static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top));
        }
        break;
    case WM_ACTIVATEAPP:
        if (scene)
        {
            if (wParam)
            {
                scene->OnActivated();
            }
            else
            {
                scene->OnDeactivated();
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

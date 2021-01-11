// Fxxk.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Fxxk.h"
#include "D3DObject.h"
#include "D3DScene.h"
#include "D3DAttribute.h"
#include "D3DConstant.h"

#pragma comment(lib, "pathcch.lib")
#pragma comment(lib, "d3dcompiler.lib" )

#define M_PI acosf(-1.0)

using namespace std;
using namespace Microsoft::WRL;

// Global Variables:
constexpr auto MAX_LOADSTRING = 100;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
D3DScene* g_scene;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND& hwnd);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

inline float randf()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

inline DirectX::XMMATRIX transform(float angle, float scale, float w, float h)
{
    const auto& viewport = g_scene->getViewport();
    auto r = DirectX::XMMatrixRotationZ(angle / 180 * M_PI);
    auto base = DirectX::XMMatrixTranslation(-viewport.Width / 2, -viewport.Height / 2, 0.0f);
    r = DirectX::XMMatrixMultiply(base, r);
    base.r[3] = DirectX::XMVectorMultiply(base.r[3], DirectX::XMVECTOR({ -1.0f, -1.0f, 0.0f, 1.0f }));
    r = DirectX::XMMatrixMultiply(r, base);

    auto s = DirectX::XMMatrixScaling(scale, scale, 1.0f);
    auto t = DirectX::XMMatrixTranslation(viewport.Width / 2 - w * scale / 2, viewport.Height / 2 - h * scale / 2, 0.0f);

    auto result = DirectX::XMMatrixMultiply(s, t);

    return DirectX::XMMatrixMultiply(result, r);
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

    DXGI_SWAP_CHAIN_DESC swapChainDescr =
    {
        { 0, 1, DXGI_FORMAT_B8G8R8A8_UNORM },
        { 1, 0 },
        DXGI_USAGE_RENDER_TARGET_OUTPUT,
        1,
        hwnd,
        true
    };

    RECT winRect;
    GetClientRect(hwnd, &winRect);
    D3D11_SAMPLER_DESC samplerState =
    {
        D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        0.0f,
        1,
        D3D11_COMPARISON_ALWAYS,
        { 0, 0, 0, 0},
        0,
        D3D11_FLOAT32_MAX,
    };

    g_scene = new D3DScene();
    g_scene->init
    (
        {
            { 0, 0, { 0, 1 }, DXGI_FORMAT_B8G8R8A8_UNORM },
            { 1, 0 },
            DXGI_USAGE_RENDER_TARGET_OUTPUT,
            1,
            hwnd,
            true
        },
        0.0f, 0.0f,
        static_cast<float>(winRect.right - winRect.left),
        static_cast<float>(winRect.bottom - winRect.top)
    );
    ComPtr<ID3D11RasterizerState> rs = nullptr;
    ComPtr<ID3D11SamplerState> ss = nullptr;

    g_scene->getDevice()->CreateRasterizerState(&D3DGlobal::defaultRasterizerStateDesc, &rs);
    g_scene->getDevice()->CreateSamplerState(&samplerState, &ss);

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

    float vertices[] = {
         0.0f, 0.0f, 0.0f,
         w, 0.0f, 0.0f,
         w, h, 0.0f,
         0.0f, h, 0.0f
    };
    float normals[] = {
        0.0f, 0.0f, 0.0f,
        w, 0.0f, 0.0f,
        w, h, 0.0f,
        0.0f, h, 0.0f
    };
    float texcoords[] = {
        0.0f, 1.0f,
        2.0f, 1.0f,
        2.0f, 0.0f,
        0.0f, 0.0f,
    };
    uint32_t indices[] = {
         0, 1, 2, 0, 2, 3
    };
    float textureMatrixX[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    float textureMatrixY[] =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
    };

    float color[] = {
        1.0f, 1.0f, 1.0f, 0.5f,
    };

    // load textures
    std::vector<ComPtr<ID3D11Resource>> textures;
    std::vector<ComPtr<ID3D11ShaderResourceView>> textureViews;
    std::vector<wstring> textureFiles = { s + L"\\a.png", s + L"\\b.png" };
    ComPtr<ID3D11Resource> texture = nullptr;
    ComPtr<ID3D11ShaderResourceView> textureView = nullptr;

    for (auto& element : textureFiles)
    {
        hr = DirectX::CreateWICTextureFromFile(g_scene->getDevice().Get(), element.data(), &texture, &textureView);
        assert(SUCCEEDED(hr));
        textures.push_back(texture);
        textureViews.push_back(textureView);
    }

    D3DObject obj(
        {
            D3DAttribute(vertices, sizeof(float), 12, "POS", sizeof(float) * 3, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT),
            D3DAttribute(normals, sizeof(float), 12, "NORMAL", sizeof(float) * 3, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT),
            D3DAttribute(texcoords, sizeof(float), 8, "TEXCOORD", sizeof(float) * 2, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT),
            D3DAttribute(textureMatrixX, sizeof(float), 9, "TEXMATRIX", sizeof(float) * 9, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 3),
            D3DAttribute(indices, sizeof(uint32_t), 6, "INDICES", sizeof(uint32_t) * 2, 0, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32_UINT),
        },
        {
            D3DConstant(textureMatrixY, sizeof(float), 12)
        },
        {
             D3DConstant(color, sizeof(float), 4)
        },
        D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        rs,
        textures,
        textureViews,
        {
            ss
        }
    );

    std::reverse(textures.begin(), textures.end());
    std::reverse(textureViews.begin(), textureViews.end());

    D3DObject obj2(
        {
            D3DAttribute(vertices, sizeof(float), 12, "POS", sizeof(float) * 3, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT),
            D3DAttribute(normals, sizeof(float), 12, "NORMAL", sizeof(float) * 3, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT),
            D3DAttribute(texcoords, sizeof(float), 8, "TEXCOORD", sizeof(float) * 2, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT),
            D3DAttribute(textureMatrixX, sizeof(float), 9, "TEXMATRIX", sizeof(float) * 9, 0, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 3),
            D3DAttribute(indices, sizeof(uint32_t), 6, "INDICES", sizeof(uint32_t) * 2, 0, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, D3D11_USAGE::D3D11_USAGE_DEFAULT, 0, DXGI_FORMAT::DXGI_FORMAT_R32_UINT),
        },
        {
            D3DConstant(textureMatrixY, sizeof(float), 12),
            D3DConstant(color, sizeof(float), 4)
        },
        {
             D3DConstant(color, sizeof(float), 4)
        },
        D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        rs,
        textures,
        textureViews,
        {
            ss
        }
    );

    float angle = 0;
    float scale = 1.0f;
    float step = 0.01f;
    uint8_t count = 0;

    obj.updateTransform(DirectX::XMMatrixTranslation(g_scene->getViewport().Width / 2 - w / 2, g_scene->getViewport().Height / 2 - h / 2, 0.0f));
    obj2.updateTransform(DirectX::XMMatrixTranslation(g_scene->getViewport().Width / 2 - w / 2, g_scene->getViewport().Height / 2 - h / 2, 0.0f));
    obj.init(*g_scene, { vs->GetBufferPointer(), vs->GetBufferSize() }, { ps->GetBufferPointer(), ps->GetBufferSize() });
    obj2.init(*g_scene, { vs->GetBufferPointer(), vs->GetBufferSize() }, { ps->GetBufferPointer(), ps->GetBufferSize() });

    ps->Release();
    vs->Release();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg = {};

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
            angle += 1;
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
                color[0] = randf();
                color[1] = randf();
                color[2] = randf();
                color[3] = randf();
            }

            obj.updateAttribute(3, textureMatrixX);
            obj.updateVSConstant(0, textureMatrixY);
            obj.updatePSConstant(0, color);
            obj2.updateAttribute(3, textureMatrixX);
            obj2.updateVSConstant(0, textureMatrixY);
            obj2.updatePSConstant(0, color);

            obj.updateTransform(transform(angle, scale, w, h));
            obj2.updateTransform(transform(angle * 2, scale, w, h));
            g_scene->render({ &obj, &obj2 });
        }
    }

#if _DEBUG
    // obj clean check
    ss = nullptr;
    rs = nullptr;
    texture = nullptr;
    textureView = nullptr;
    textures.clear();
    textureViews.clear();

    obj.~D3DObject();
    obj2.~D3DObject();

    delete g_scene;
#endif

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
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_SIZE:
    {
        if (g_scene)
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            if (width == 0 || height == 0)
            {
            }
            else
            {
                g_scene->resize(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
            }
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

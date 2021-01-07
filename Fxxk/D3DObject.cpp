#include <d3dcompiler.h>
#include <WICTextureLoader.h>
#include "D3DObject.h"
#include "D3DScene.h"

#pragma comment(lib, "d3dcompiler.lib" )

using namespace std;

const uint32_t vertexStride[] = { 3 * sizeof(float), 2 * sizeof(float), sizeof(DirectX::XMMATRIX) };
const uint32_t vertexOffset[] = { 0, 0, 0 };

const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
	{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 0 },
	{ "MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 16, D3D11_INPUT_PER_INSTANCE_DATA, 0 },
	{ "MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 32, D3D11_INPUT_PER_INSTANCE_DATA, 0 },
	{ "MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 48, D3D11_INPUT_PER_INSTANCE_DATA, 0 }
};

D3DObject::D3DObject(
	initializer_list<float>&& vertices,
	initializer_list<float>&& texcoord,
	initializer_list<uint32_t>&& indices,
	std::wstring&& vertexShader,
	std::wstring&& pixelShader,
	initializer_list<std::wstring>&& textureFiles,
	D3D11_RASTERIZER_DESC&& rasterizerDesc,
	initializer_list<D3D11_SAMPLER_DESC>&& samplerDesc
) :
	m_indexBuffer(nullptr),
	m_vertexBuffer{ { nullptr, nullptr, nullptr } },
	m_layout(nullptr),
	m_vs(nullptr),
	m_ps(nullptr),
	m_rs(nullptr),
	m_vertices(vertices),
	m_texcoord(texcoord),
	m_incides(indices),
	m_vertexShader(vertexShader),
	m_pixelShader(pixelShader),
	m_textureFiles(textureFiles),
	m_rasterizerDesc(rasterizerDesc),
	m_samplerDesc(samplerDesc),
	m_transform(DirectX::XMMatrixIdentity()),
	m_inited(false),
	m_matrixDirty(false)
{
}
void D3DObject::updateTransform(DirectX::XMMATRIX&& transform)
{
	m_transform = transform;
	m_matrixDirty = true;
}

void D3DObject::updateTransform(DirectX::XMMATRIX& transform)
{
	m_transform = transform;
	m_matrixDirty = true;
}

void D3DObject::updateTransform(D3DScene& scene)
{
	if (m_matrixDirty)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		auto hr = scene.getContext()->Map(m_vertexBuffer[2], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert(SUCCEEDED(hr));
		memcpy(mappedResource.pData, &m_transform, sizeof(m_transform));
		scene.getContext()->Unmap(m_vertexBuffer[2], 0);
		m_matrixDirty = false;
	}
}


void D3DObject::render(D3DScene& scene)
{
	updateTransform(scene);
	scene.getContext()->RSSetState(m_rs);
	scene.getContext()->PSSetSamplers(0, m_ss.size(), m_ss.data());
	scene.getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	scene.getContext()->IASetInputLayout(m_layout);
	scene.getContext()->IASetVertexBuffers(0, m_vertexBuffer.size(), m_vertexBuffer.data(), vertexStride, vertexOffset);
	scene.getContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	scene.getContext()->PSSetShaderResources(0, m_textureViews.size(), m_textureViews.data());
	scene.getContext()->VSSetShader(m_vs, NULL, 0);
	scene.getContext()->PSSetShader(m_ps, NULL, 0);
	scene.getContext()->DrawIndexed(m_incides.size(), 0, 0);
}

void D3DObject::init(D3DScene& scene)
{
	ID3DBlob* vs = nullptr, * ps = nullptr, * error = nullptr;
#if _DEBUG
	auto compileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
	auto compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

	auto hr = D3DCompileFromFile(
		m_vertexShader.data(),
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
		m_pixelShader.data(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		compileFlags,
		0,
		&ps,
		&error);
	assert(SUCCEEDED(hr));

	hr = scene.getDevice()->CreateVertexShader(
		vs->GetBufferPointer(),
		vs->GetBufferSize(),
		NULL,
		&m_vs);
	assert(SUCCEEDED(hr));

	hr = scene.getDevice()->CreatePixelShader(
		ps->GetBufferPointer(),
		ps->GetBufferSize(),
		NULL,
		&m_ps);

	assert(SUCCEEDED(hr));

	if (error) {
		error->Release();
	}

	hr = scene.getDevice()->CreateInputLayout(
		inputElementDesc,
		ARRAYSIZE(inputElementDesc),
		vs->GetBufferPointer(),
		vs->GetBufferSize(),
		&m_layout);

	ps->Release();
	vs->Release();

	assert(SUCCEEDED(hr));


	// create buffer
	D3D11_SUBRESOURCE_DATA sr = { 0 };

	D3D11_BUFFER_DESC vertexBufferDescr = {};
	vertexBufferDescr.ByteWidth = sizeof(float) * m_vertices.size();
	vertexBufferDescr.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDescr.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	sr.pSysMem = m_vertices.data();
	hr = scene.getDevice()->CreateBuffer(
		&vertexBufferDescr,
		&sr,
		&m_vertexBuffer[0]);
	assert(SUCCEEDED(hr));

	D3D11_BUFFER_DESC texBufferDescr = {};
	texBufferDescr.ByteWidth = sizeof(float) * m_texcoord.size();
	texBufferDescr.Usage = D3D11_USAGE_DEFAULT;
	texBufferDescr.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	sr.pSysMem = m_texcoord.data();
	hr = scene.getDevice()->CreateBuffer(
		&texBufferDescr,
		&sr,
		&m_vertexBuffer[1]);
	assert(SUCCEEDED(hr));

	D3D11_BUFFER_DESC matrixBufferDescr = {};
	matrixBufferDescr.ByteWidth = sizeof(m_transform);
	matrixBufferDescr.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDescr.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDescr.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	sr.pSysMem = &m_transform;
	hr = scene.getDevice()->CreateBuffer(
		&matrixBufferDescr,
		&sr,
		&m_vertexBuffer[2]);
	assert(SUCCEEDED(hr));

	D3D11_BUFFER_DESC indexBufferDesc = { };
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * m_incides.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	sr.pSysMem = m_incides.data();

	hr = scene.getDevice()->CreateBuffer(
		&indexBufferDesc,
		&sr,
		&m_indexBuffer);
	assert(SUCCEEDED(hr));

	// load textures
	ID3D11Resource* texture = nullptr;
	ID3D11ShaderResourceView* textureView = nullptr;

	for (auto& element : m_textureFiles)
	{
		hr = DirectX::CreateWICTextureFromFile(scene.getDevice(), element.data(), &texture, &textureView);
		assert(SUCCEEDED(hr));
		m_textures.push_back(texture);
		m_textureViews.push_back(textureView);
	}

	hr = scene.getDevice()->CreateRasterizerState(&m_rasterizerDesc, &m_rs);
	assert(SUCCEEDED(hr));

	ID3D11SamplerState* sampleState = nullptr;

	for (auto& element : m_samplerDesc)
	{
		hr = scene.getDevice()->CreateSamplerState(&element, &sampleState);
		assert(SUCCEEDED(hr));
		m_ss.push_back(sampleState);
	}

	m_inited = true;
	m_matrixDirty = false;
}

void D3DObject::dispose()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
	}
	if (m_layout)
	{
		m_layout->Release();
	}
	if (m_vs)
	{
		m_vs->Release();
	}
	if (m_ps)
	{
		m_ps->Release();
	}
	if (m_rs)
	{
		m_rs->Release();
	}
	m_indexBuffer = nullptr;
	m_layout = nullptr;
	m_vs = nullptr;
	m_ps = nullptr;
	for (auto& element : m_vertexBuffer)
	{
		if (element)
		{
			element->Release();
			element = nullptr;
		}
	}
	for (auto& element : m_textures)
	{
		if (element)
		{
			element->Release();
		}
	}
	for (auto& element : m_textureViews)
	{
		if (element)
		{
			element->Release();
		}
	}
	for (auto& element : m_ss)
	{
		if (element)
		{
			element->Release();
		}
	}
	m_textures.clear();
	m_textureViews.clear();
	m_ss.clear();
}

D3DObject::~D3DObject()
{
	this->dispose();
}

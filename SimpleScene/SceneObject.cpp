#include "pch.h"
#include "SceneObject.h"

#include "Common\DirectXHelper.h"

using namespace SimpleScene;
using namespace DirectX;
using namespace Windows::Foundation;

SceneObject::SceneObject() { }

void SceneObject::Draw(std::shared_ptr<DX::DeviceResources> deviceResources)
{
	auto context = deviceResources->GetD3DDeviceContext();

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0
		);

	// Each vertex is one instance of the VertexPositionNormal struct.
	UINT stride = sizeof(VertexPositionNormal);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
		);

	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
		);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
		);

	// Send the constant buffers to the graphics device.
	context->VSSetConstantBuffers(
		0,
		1,
		m_constantBuffer.GetAddressOf()
		);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
		);

	context->PSSetConstantBuffers(
		0,
		1,
		m_constantBuffer.GetAddressOf()
		);
	
	// Draw the objects.
	context->DrawIndexed(
		m_indexCount,
		0,
		0
		);
}

void SceneObject::DrawDepthMap(std::shared_ptr<DX::DeviceResources> deviceResources)
{
	auto context = deviceResources->GetD3DDeviceContext();

	context->UpdateSubresource(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0
		);

	// Each vertex is one instance of the VertexPositionNormal struct.
	UINT stride = sizeof(VertexPositionNormal);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
		);

	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
		);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_depthVertexShader.Get(),
		nullptr,
		0
		);

	context->VSSetConstantBuffers(
		0,
		1,
		m_constantBuffer.GetAddressOf()
		);

	// Attach our pixel shader.
	ID3D11PixelShader* nullPS = nullptr;
	context->PSSetShader(
		//m_depthPixelShader.Get(),
		nullPS,
		nullptr,
		0
		);
	
	// Draw the objects.
	context->DrawIndexed(
		m_indexCount,
		0,
		0
		);
}

void SceneObject::LoadVS(
	std::shared_ptr<DX::DeviceResources> deviceResources,
	const std::vector<byte>& fileData)
{
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateVertexShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_vertexShader
			)
		);

	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateInputLayout(
			vertexDesc,
			ARRAYSIZE(vertexDesc),
			&fileData[0],
			fileData.size(),
			&m_inputLayout
			)
		);
}

void SceneObject::LoadDepthVS(
	std::shared_ptr<DX::DeviceResources> deviceResources,
	const std::vector<byte>& fileData)
{
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateVertexShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_depthVertexShader
			)
		);
}

void SceneObject::LoadPS(
	std::shared_ptr<DX::DeviceResources> deviceResources,
	const std::vector<byte>& fileData)
{
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_pixelShader
			)
		);
}

void SceneObject::LoadCB(
	std::shared_ptr<DX::DeviceResources> deviceResources)
{
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(MyConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&m_constantBuffer
			)
		);
}

SceneObject::~SceneObject()
{
	m_inputLayout.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
	m_vertexShader.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
}

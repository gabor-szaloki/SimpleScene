#include "pch.h"
#include "PointLight.h"
#include "Common\DirectXHelper.h"


PointLight::PointLight(XMFLOAT4 position, XMFLOAT4 color)
{
	this->m_position = position;
	this->m_color = color;
}

void PointLight::LoadLightViewProjectionBuffer(
	std::shared_ptr<DX::DeviceResources> deviceResources)
{
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&m_viewProjectionBuffer
			)
		);

	XMMATRIX lightPerspectiveMatrix = XMMatrixPerspectiveFovRH(
		XM_PIDIV2,
		1.0f,
		0.1f,
		50.f
		);

	XMStoreFloat4x4(
		&m_viewProjectionBufferData.projection,
		XMMatrixTranspose(lightPerspectiveMatrix)
		);

	static const XMVECTORF32 eye = { m_position.x, m_position.y, m_position.z, m_position.w };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(
		&m_viewProjectionBufferData.view,
		XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up))
		);

	// Store the light position to help calculate the shadow offset.
	m_viewProjectionBufferData.pos = m_position;
}

void PointLight::UpdateBuffer()
{
	static const XMVECTORF32 eye = { m_position.x, m_position.y, m_position.z, m_position.w };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(
		&m_viewProjectionBufferData.view,
		XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up))
		);

	// Store the light position to help calculate the shadow offset.
	m_viewProjectionBufferData.pos = m_position;
}

PointLight::~PointLight()
{
}

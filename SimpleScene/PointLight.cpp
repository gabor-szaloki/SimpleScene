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
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(LightViewsConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc,
			nullptr,
			&m_viewsBuffer
			)
		);

	UpdateBuffer();
}

void PointLight::UpdateBuffer()
{
	XMVECTORF32 eye = { m_position.x, m_position.y, m_position.z, m_position.w };
	XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMVECTORF32 backAt = { m_position.x * 2, m_position.y * 2, m_position.z * 2, 1};
	XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(
		&m_viewsBufferData.frontView,
		XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up))
		);
	XMStoreFloat4x4(
		&m_viewsBufferData.backView,
		XMMatrixTranspose(XMMatrixLookAtRH(eye, backAt, up))
		);

	// Store the light position to help calculate the shadow offset.
	m_viewsBufferData.pos = m_position;
}

PointLight::~PointLight()
{
}

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
	XMMATRIX lightPerspectiveMatrix = XMMatrixPerspectiveFovLH(
		XM_PIDIV2,
		1.0f,
		0.1f,
		50.f
		);

	XMStoreFloat4x4(
		&m_projection,
		XMMatrixTranspose(lightPerspectiveMatrix)
		);

	UpdateBuffer();
}

void PointLight::UpdateBuffer()
{
	XMVECTORF32 eye = { m_position.x, m_position.y, m_position.z, m_position.w };

	XMVECTORF32 ats[6] = {
		{ m_position.x + 1.f, m_position.y, m_position.z, m_position.w },
		{ m_position.x - 1.f, m_position.y, m_position.z, m_position.w },
		{ m_position.x, m_position.y + 1.f, m_position.z, m_position.w },
		{ m_position.x, m_position.y - 1.f, m_position.z, m_position.w },
		{ m_position.x, m_position.y, m_position.z + 1.f, m_position.w },
		{ m_position.x, m_position.y, m_position.z - 1.f, m_position.w }
	};
	XMVECTORF32 ups[6] = {
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 0.0f }
	};

	for (int i = 0; i < 6; i++)
	{
		XMStoreFloat4x4(
			&m_views[i],
			XMMatrixTranspose(XMMatrixLookAtLH(eye, ats[i], ups[i]))
			);
	}
}

PointLight::~PointLight()
{
}

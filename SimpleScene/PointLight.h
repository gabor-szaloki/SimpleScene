#pragma once

#include "Common\DeviceResources.h"
#include "Content\ShaderStructures.h"

using namespace DirectX;
using namespace SimpleScene;

class PointLight
{
public:
	PointLight(XMFLOAT4 position, XMFLOAT4 color);
	void LoadLightViewProjectionBuffer(std::shared_ptr<DX::DeviceResources> deviceResources);
	void UpdateBuffer();
	~PointLight();

	XMFLOAT4 m_position;
	XMFLOAT4 m_color;

	ViewProjectionConstantBuffer m_viewProjectionBufferData;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_viewProjectionBuffer;
};


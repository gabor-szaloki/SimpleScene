#pragma once

using namespace DirectX;

class PointLight
{
public:
	PointLight(XMFLOAT4 position, XMFLOAT4 color);
	~PointLight();

	XMFLOAT4 m_position;
	XMFLOAT4 m_color;
};


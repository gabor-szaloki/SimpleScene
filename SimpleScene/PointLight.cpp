#include "pch.h"
#include "PointLight.h"


PointLight::PointLight(XMFLOAT4 position, XMFLOAT4 color)
{
	this->m_position = position;
	this->m_color = color;
}


PointLight::~PointLight()
{
}

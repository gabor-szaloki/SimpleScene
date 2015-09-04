#pragma once
#include "SceneObject.h"
class Sphere : public SceneObject
{
public:
	Sphere();
	void GenerateMesh(std::shared_ptr<DX::DeviceResources> deviceResources) override;
	~Sphere();
};


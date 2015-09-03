#pragma once
#include "SceneObject.h"

#include "Common\DeviceResources.h"
#include "Common\DirectXHelper.h"
#include "Content\ShaderStructures.h"

class Cube : public SceneObject
{
public:
	Cube();
	void GenerateMesh(std::shared_ptr<DX::DeviceResources> deviceResources) override;
	~Cube();
};


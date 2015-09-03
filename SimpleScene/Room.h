#pragma once
#include "Cube.h"
class Room : public SceneObject
{
public:
	Room();
	void GenerateMesh(std::shared_ptr<DX::DeviceResources> deviceResources) override;
	~Room();
};


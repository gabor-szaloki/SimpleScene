#pragma once

#include "Common\DeviceResources.h"
#include "Content\ShaderStructures.h"

#include <vector>

using namespace SimpleScene;
using namespace DirectX;

class SceneObject
{
public:
	SceneObject();

	virtual void Draw(std::shared_ptr<DX::DeviceResources> deviceResources);
	virtual void DrawDepthMap(std::shared_ptr<DX::DeviceResources> deviceResources);

	virtual void LoadVS(
		std::shared_ptr<DX::DeviceResources> deviceResources,
		const std::vector<byte>& fileData);
	virtual void LoadDepthVS(
		std::shared_ptr<DX::DeviceResources> deviceResources,
		const std::vector<byte>& fileData);
	void LoadDepthGS(
		std::shared_ptr<DX::DeviceResources> deviceResources,
		const std::vector<byte>& fileData);
	virtual void LoadPS(
		std::shared_ptr<DX::DeviceResources> deviceResources,
		const std::vector<byte>& fileData);
	void LoadDepthPS(
		std::shared_ptr<DX::DeviceResources> deviceResources,
		const std::vector<byte>& fileData);
	virtual void LoadCBs(
		std::shared_ptr<DX::DeviceResources> deviceResources);
	virtual void GenerateMesh(std::shared_ptr<DX::DeviceResources> deviceResources) = 0;

	~SceneObject();

	ShadowVSConstantBuffer m_shadowVSConstantBufferData;
	ShadowGSConstantBuffer m_shadowGSConstantBufferData;
	SceneVSConstantBuffer m_sceneVSConstantBufferData;

protected:
	// Direct3D resources for geometry.
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	     m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		     m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		     m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	     m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	     m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	     m_depthVertexShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	 m_depthGeometryShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	     m_depthPixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		     m_shadowVSConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		     m_shadowGSConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		     m_sceneVSConstantBuffer;

	// System resources for geometry.
	uint32	m_indexCount;

};


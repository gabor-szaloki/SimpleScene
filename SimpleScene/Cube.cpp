#include "pch.h"
#include "Cube.h"


Cube::Cube() : SceneObject()
{
}

void Cube::GenerateMesh(std::shared_ptr<DX::DeviceResources> deviceResources)
{
	// Load mesh vertices. Each vertex has a position and a color.
	static const VertexPositionNormal cubeVertices[] =
	{
		// -x
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		// +x
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		// -y
		{ XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		// +y
		{ XMFLOAT3( 0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3( 0.5f, 0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, 0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

		// -z
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

		// +z
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3( 0.5f,  0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = cubeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_vertexBuffer
			)
		);

	// Load mesh indices. Each trio of indices represents
	// a triangle to be rendered on the screen.
	// For example: 0,2,1 means that the vertices with indexes
	// 0, 2 and 1 from the vertex buffer compose the 
	// first triangle of this mesh.
	static const unsigned short cubeIndices[] =
	{
		0,2,1, // -x
		1,2,3,

		4,5,6, // +x
		5,7,6,

		8,10,9, // -y
		9,10,11,

		12,13,14, // +y
		13,15,14,

		16,17,18, // -z
		17,19,18,
		
		20,22,21, // +z
		21,22,23
	};

	m_indexCount = ARRAYSIZE(cubeIndices);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = cubeIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&m_indexBuffer
			)
		);
}

Cube::~Cube()
{
}

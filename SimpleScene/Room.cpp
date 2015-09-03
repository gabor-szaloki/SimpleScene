#include "pch.h"
#include "Room.h"


Room::Room()
{
}

void Room::GenerateMesh(std::shared_ptr<DX::DeviceResources> deviceResources)
{
	// Load mesh vertices. Each vertex has a position and a color.
	static const VertexPositionNormal cubeVertices[] =
	{
		// -x
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

		// +x
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
		{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

		// -y
		{ XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

		// +y
		{ XMFLOAT3( 0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3( 0.5f, 0.5f,  0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
		{ XMFLOAT3(-0.5f, 0.5f,  0.5f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

		// -z
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },

		// +z
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3( 0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-0.5f,  0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3( 0.5f,  0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
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
		0,1,2, // -x
		1,3,2,

		4,6,5, // +x
		5,6,7,

		8,9,10, // -y
		9,11,10,

		12,14,13, // +y
		13,14,15,

		16,18,17, // -z
		17,18,19,

		20,21,22, // +z
		21,23,22
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

Room::~Room()
{
}

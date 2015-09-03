// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer MyConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 lightPosition;
	float4 lightColor;
	float4 cameraPosition;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 worldPos : POSITION;
	float3 normal : NORMAL;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	output.worldPos = mul(input.pos, world);
	output.normal = mul(input.normal, world);
	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	float4x4 WVP = mul(mul(world, view), projection);
	//float4x4 WVP = mul(mul(projection, view), world);
	output.pos = mul(pos, WVP);

	return output;
}

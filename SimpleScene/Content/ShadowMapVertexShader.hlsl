// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer WorldViewProjEyeConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 direction;
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
	float clipDepth : TEXCOORD1;
	float depth : TEXCOORD2;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	// Transform the vertex postition to paraboloid space
	float4 pos = float4(input.pos, 1.0f);
	pos = mul(mul(pos, world), view);
	pos /= pos.w;

	// For the back-map z has to be inverted
	pos.z *= direction.x;

	// normalize
	float fLength = length(pos.xyz);
	pos /= fLength;

	output.clipDepth = pos.z;

	// calc "normal" on intersection, by adding the 
	// reflection-vector(0,0,1) and divide through 
	// his z to get the texture coords
	pos.x /= pos.z + 1.0f;
	pos.y /= pos.z + 1.0f;

	float nearClippingPane = 0.1f;
	float farClippingPane = 1000.f;

	// set z for z-buffering and neutralize w
	pos.z = (fLength - nearClippingPane) / (farClippingPane - nearClippingPane);
	pos.w = 1.0f;

	output.pos = pos;
	output.depth = pos.z;

	return output;
}

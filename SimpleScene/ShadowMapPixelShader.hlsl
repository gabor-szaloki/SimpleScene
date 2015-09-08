// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float clipDepth : TEXCOORD1;
	float depth : TEXCOORD2;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	// clipping
	clip(input.clipDepth);

	return input.depth;
}
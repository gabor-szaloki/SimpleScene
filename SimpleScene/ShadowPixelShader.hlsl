struct PixelShaderInput
{
	float4	clipSpacePosition : SV_Position;
	uint	shadowMapIndex : SV_RenderTargetArrayIndex;
	float3  worldLightDir : Attrib0;
};

float main(PixelShaderInput input) : SV_Depth
{
	const float lightRange = 30.f;
	return length(input.worldLightDir) / lightRange;
}
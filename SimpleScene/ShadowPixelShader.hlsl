struct PixelShaderInput
{
	float4	clipSpacePosition : SV_Position;
	uint	shadowMapIndex : SV_RenderTargetArrayIndex;
	float3  worldLightDir : Attrib0;
};

float main(PixelShaderInput input) : SV_Depth
{
	return length(input.worldLightDir) / 30.0f;
}
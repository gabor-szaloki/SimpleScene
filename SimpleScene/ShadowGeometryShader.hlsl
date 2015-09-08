static const uint ShadowMapCount = 6;

cbuffer ShadowGSConstantBuffer : register(b0)
{
	matrix views[ShadowMapCount];
	matrix projection;
};

struct GeometryShaderInput
{
	float4 worldPos : SV_POSITION;
	float3 worldLightDir : Attrib0;
};

struct GeometryShaderOutput
{
	float4 clipSpacePosition : SV_POSITION;
	uint   shadowMapIndex : SV_RenderTargetArrayIndex;
	float3 worldLightDir : Attrib0;
};

[instance(ShadowMapCount)]
[maxvertexcount(3)]
void main(
	uint shadowMapID : SV_GSInstanceID,
	triangle GeometryShaderInput input[3],
	inout TriangleStream<GeometryShaderOutput> outputStream)
{
	GeometryShaderOutput output;
	output.shadowMapIndex = shadowMapID;

	for (uint vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
	{
		output.worldLightDir = input[vertexIndex].worldLightDir;
		output.clipSpacePosition = mul(input[vertexIndex].worldPos, mul(views[shadowMapID], projection));

		outputStream.Append(output);
	}

	outputStream.RestartStrip();
}
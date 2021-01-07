Texture2D shaderTexture1: register(t0);
Texture2D shaderTexture2: register(t1);
SamplerState SampleType: register(s0);

struct PS_IN {
	float4 position: SV_POSITION;
	float3 normal: NORMAL;
	float2 tex: TEXCOORD;
};

float4 main(PS_IN input) : SV_TARGET
{
	return shaderTexture1.Sample(SampleType, input.tex);
}

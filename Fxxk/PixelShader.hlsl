Texture2D shaderTexture1: register(t0);
Texture2D shaderTexture2: register(t1);
SamplerState SampleType: register(s0);

struct PS_IN {
	float4 position: SV_POSITION;
	float3 world_position: POS;
	float3 normal: NORMAL;
	float2 tex: TEXCOORD;
};

cbuffer PS_COLOR {
	float4 color;
};

float4 main(PS_IN input) : SV_TARGET
{
	float4 c1 = shaderTexture1.Sample(SampleType, input.tex);

	return float4((mul(c1, 1 - color.a) +  mul(color, color.a)).xyz, c1.a);
}

struct VS_IN {
    float3 position: POS;
    float3 normal: NORMAL;
    float2 tex: TEXCOORD;
    float3x3 tex_transform: TEXMATRIX;
};
cbuffer VS_PROJECTION_BUFFER: register(b0) {
    matrix projection;
};

cbuffer VS_TRANSFORM_BUFFER: register(b2) {
    matrix transform;
    matrix view;
    matrix modelView;
    matrix modelViewInverse;
};

cbuffer VS_TEX_BUFFER : register(b3) {
    float3x3 tex_mY;
}

struct VS_OUT {
    float4 position: SV_POSITION;
    float3 world_position: POS;
    float3 normal: NORMAL;
    float2 tex: TEXCOORD;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    float4 f = mul(transform, float4(input.position, 1.0f));
    output.world_position = f.xyz;
    output.position = mul(projection, mul(modelView, float4(input.position, 1.0f)));
    output.normal = normalize(mul(float4(input.normal, 0.0f), modelViewInverse).xyz);
    output.tex = mul(tex_mY, mul(input.tex_transform, float3(input.tex, 1.0f))).xy;
    return output;
}

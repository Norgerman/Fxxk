struct VS_IN {
    float3 position: POS;
    float2 tex: TEXCOORD;
    matrix transform: MATRIX;
};
cbuffer VS_CONSTANT_BUFFER: register(b0) {
    matrix projection;
};

struct VS_OUT {
    float4 position: SV_POSITION;
    float2 tex: TEXCOORD;
};

VS_OUT main(VS_IN input){
    VS_OUT output;
    float4 f = mul(input.transform, float4(input.position, 1.0f));
    output.position = mul(projection, f);
    output.tex = input.tex;
    return output;
}

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    float4 color;
};
struct PS_IN
{
    float4 PosH : SV_Position;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};
Texture2D tex;
SamplerState samp;

float4 main(PS_IN pi) : SV_TARGET
{    
    return tex.Sample(samp, pi.uv) * pi.color * color;
}

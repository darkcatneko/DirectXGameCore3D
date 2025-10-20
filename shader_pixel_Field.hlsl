struct PS_IN
{
    float4 PosH : SV_Position;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};
Texture2D tex0 : register(t0);
Texture2D tex1 : register(t1);

SamplerState samp;

float4 main(PS_IN pi) : SV_TARGET
{    
    pi.uv = pi.uv * 2;
    return tex0.Sample(samp, pi.uv) * pi.color;
}

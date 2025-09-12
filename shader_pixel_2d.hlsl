cbuffer cbGamma : register(b2)
{
    float gamma_in;
    float gamma_out;
    float padding1;
    float padding2;
}
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
    float4 texColor = tex.Sample(samp, pi.uv);
    texColor.rgb = pow(texColor.rgb, gamma_in); // ‰ğáù
    texColor *= pi.color;
    texColor.rgb = pow(texColor.rgb, 1.0 / gamma_out); // Ä•Òáùi‹ù‹j
    return texColor;
    //return tex.Sample(samp, pi.uv) * pi.color;
}

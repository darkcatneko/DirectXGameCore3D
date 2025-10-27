cbuffer PS_CONSTANT_BUFFER : register(b0)
{
    float4 color;
};
cbuffer PS_CONSTANT_BUFFER : register(b1)
{
    float4 ambient_color;
};
cbuffer PS_CONSTANT_BUFFER : register(b2)
{
    float4 directional_world_vector;
    float4 directional_color;
    float3 eyePosW;
    //float gSpecularPower;
};
struct PS_IN
{
    float4 posH : SV_Position;
    float4 posW : POSITION0;
    float4 normalW : NORMAL0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};
Texture2D tex;
SamplerState samp;

float4 main(PS_IN pi) : SV_TARGET
{    
    float4 normalW = normalize(pi.normalW);
    float dl = max(0.0f, dot(-directional_world_vector, normalW));
    
    float3 toEye = normalize(eyePosW - pi.posW.xyz);
    float3 r = reflect(directional_world_vector, pi.normalW).xyz;
    float t = pow(max(dot(r, toEye), 0.0f), 10.0f);
    
    float3 lcolor = pi.color.rgb * directional_color.rgb * dl + ambient_color.rgb * pi.color.rgb;
    lcolor += float3(1.0f, 1.0f, 1.0f) * t;
    
    return tex.Sample(samp, pi.uv) * float4(lcolor, 1.0f) * color;
}

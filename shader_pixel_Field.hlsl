cbuffer PS_CONSTANT_BUFFER : register(b0)
{
    float4 diffuse_color;
};
cbuffer PS_CONSTANT_BUFFER : register(b1)
{
    float4 ambient_color;
};
cbuffer PS_CONSTANT_BUFFER : register(b2)
{
    float4 directional_world_vector;
    float4 directional_color = { 1.0f, 1.0f, 1.0f, 1.0f };
};
cbuffer PS_CONSTANT_BUFFER : register(b3)
{
    float3 eyePosW;
    float gSpecularPower = 30.0f;
    float4 Specular_color = { 0.1f, 0.1f, 0.1f, 1.0f };
};

struct PS_IN
{
    float4 posH : SV_Position;
    float4 posW : POSITION0;
    float4 normalW : NORMAL0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

Texture2D tex0 : register(t0);
Texture2D tex1 : register(t1);

SamplerState samp;

float4 main(PS_IN pi) : SV_TARGET
{    
    //UVâ¡çH
    float2 uv;
    float angle = 3.14159f * 45 / 180.0f;
    uv.x = pi.uv.x * cos(angle) + pi.uv.y * sin(angle);
    uv.y = -pi.uv.x * sin(angle) + pi.uv.y * cos(angle);
    
    float4 tex_color = 
    tex0.Sample(samp, pi.uv) * pi.color.g + 
    tex1.Sample(samp, pi.uv) * pi.color.r;
    
    //çﬁéøË¯êF
    float3 material_color = tex_color.rgb * diffuse_color.rgb;
    
    //ïΩçsåı
    float4 normalW = normalize(pi.normalW);
    float dl = max(0.0f, dot(-directional_world_vector, normalW));
    float3 diffuse = material_color * directional_color.rgb * dl;
    
    //ä¬ã´åı
    float3 ambient = material_color * ambient_color.rgb;
    
    //specular
    float3 toEye = normalize(eyePosW - pi.posW.xyz);
    float3 r = reflect(directional_world_vector, normalW).xyz;
    float t = pow(max(dot(r, toEye), 0.0f), gSpecularPower);
    float3 specular = Specular_color.rgb * t;
    
    
    
    
    
    //float alpha = tex.Sample(samp, pi.uv).a * pi.color.a * diffuse_color.a;
    float3 lcolor = ambient + diffuse + specular; 
    
    return float4(lcolor, 1.0f);
}

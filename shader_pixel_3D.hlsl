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
    float4 directional_color = { 1.0f, 1.0f, 1.0f, 1.0f};
};
cbuffer PS_CONSTANT_BUFFER : register(b3)
{
    float3 eyePosW;
    float gSpecularPower;
    float4 Specular_color;
};

struct PointLight
{    
    float3 pointlight_posW ;
    float pointlight_range ;
    float4 pointlight_color;
    //float  SpecularPower = 30.0f;
    //float4 Specular_color = { 0.1f, 0.1f, 0.1f, 1.0f };
};
cbuffer PS_CONSTANT_BUFFER : register(b4)
{
    PointLight point_light[4];
    int count;
    float3 point_light_dummy;
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
    //çﬁéøË¯êF
    float3 material_color = tex.Sample(samp, pi.uv).rgb * pi.color.rgb * diffuse_color.rgb;
    
    //ïΩçsåı
    float4 normalW = normalize(pi.normalW);
   // float dl = max(0.0f, dot(-directional_world_vector, normalW));
    float dl = (dot(-directional_world_vector, normalW) + 1.0f) * 0.5f;
    float3 diffuse = material_color * directional_color.rgb * dl;
    
    //ä¬ã´åı
    float3 ambient = material_color * ambient_color.rgb;
    
    //specular
    float3 toEye = normalize(eyePosW - pi.posW.xyz);
    float3 r = reflect(directional_world_vector, normalW).xyz;
    float t = pow(max(dot(r, toEye), 0.0f), gSpecularPower);
    float3 specular = Specular_color.rgb * t;
    
    
    
    float alpha = tex.Sample(samp, pi.uv).a * pi.color.a * diffuse_color.a ;
    float3 lcolor = ambient + diffuse + specular;
    
    float lim =1.0- max(dot(normalW.xyz, toEye), 0.0f);
    lim = pow(lim, 7.5f);
    lcolor += float3(lim, lim, lim);
    
    //ñ 
    for (int i = 0; i < count;i++)
    {
        float3 lightToPixel = pi.posW.xyz - point_light[i].pointlight_posW;
        
        float d = length(lightToPixel);
    
       // float a = pow(max(1.0f - 1.0f / point_light[i].pointlight_range * d, 0.0f), 2.0f);
        float a = 1.0 / (1.0 + d * d / point_light[i].pointlight_range * point_light[i].pointlight_range);
        
        float dl = max(0.0f, dot(-normalize(lightToPixel), normalW.xyz));
        
        lcolor += material_color *  point_light[i].pointlight_color.rgb * a *dl;
        
        //ÍyåıìIspecular
        float3 r = reflect(normalize(lightToPixel), normalW.xyz);
        float t = pow(max(dot(r, toEye), 0.0f), gSpecularPower);
        float3 pointLight_specular = point_light[i].pointlight_color.rgb * t * a;
        lcolor += pointLight_specular;

    }
    return float4(lcolor, alpha);
    
}

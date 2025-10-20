/*==============================================================================

   3D描画用頂点シェーダー [shader_vertex_Field.hlsl]
														 Author : Youhei Sato
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/

// 定数バッファ
cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    float4x4 world;
};
cbuffer VS_CONSTANT_BUFFER : register(b1)
{
    float4x4 view;
};
cbuffer VS_CONSTANT_BUFFER : register(b3)
{
    float4x4 projection;
};
cbuffer VS_CONSTANT_BUFFER : register(b4)
{
    float4 ambient_color;
};
cbuffer VS_CONSTANT_BUFFER : register(b5)
{
    float4 directional_world_vector;
    float4 directional_color;
};

struct VS_OUT
{
    float4 posH : SV_Position;
    float4 color : COLOR0;
    float4 light_color : COLOR1;
    float4 ambient : COLOR2;
    float2 uv : TEXCOORD0;
};
struct VS_IN
{
    float4 posL : POSITION0;
    float4 normalL : NORMAL0;
    float4 color : COLOR0;   
    float2 uv : TEXCOORD0;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
VS_OUT main(VS_IN vi)
{
    VS_OUT vo;
    //float4 posW = mul(vi.posL, world);
    //float4 posWV = mul(posW, view);
    //vo.posH = mul(posWV, projection); //座標變換
    float4x4 mtxWV = mul(world, view);
    float4x4 mtxWVP = mul(mtxWV, projection);
    vo.posH = mul(vi.posL, mtxWVP);
    
    
    float normalW = mul(float4(vi.normalL.xyz, 0.0f), world);
    normalW = normalize(normalW);
    float d1 = max(0, dot(-directional_world_vector, normalW));
    
    vo.color = vi.color;
   
   
    vo.light_color = float4(directional_color.rgb * d1, 1.0f);
    vo.ambient = float4(ambient_color.rgb, 1.0f);
    vo.uv = vi.uv;
    
    return vo;
}

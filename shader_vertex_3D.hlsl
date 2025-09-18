/*==============================================================================

   3D描画用頂点シェーダー [shader_vertex_3d.hlsl]
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

struct VS_OUT
{
    float4 posH : SV_Position;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};
struct VS_IN
{
    float4 posL : POSITION0;
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
    
    
    vo.color = vi.color;
    vo.uv = vi.uv;
    
    return vo;
}

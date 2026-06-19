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
// ----------------------------
// 新規追加：ボーン行列
// CPU側で「最終変換済み」行列 (offset * global) を転送してください
// 128 は必要に応じて変更
// ----------------------------
cbuffer SKINNING_BUFFER : register(b4)
{
    float4x4 gBones[128];
};
struct VS_OUT
{
    float4 posH : SV_Position;
    float4 posW : POSITION0;
    float4 normalW : NORMAL0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};
struct VS_IN
{
    float4 posL : POSITION0;
    float4 normalL : NORMAL0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
    
    uint4 boneIdx : BONEINDEX; // 頂点に振られたボーンの index
    float4 weight : BONEWEIGHT; // それぞれの重み
};


VS_OUT main(VS_IN vi)
{
    VS_OUT vo;

    //---------------------------------------------------------------------
    // ★ 1. スキニング：ローカル → ボーン変換後の頂点へ
    //---------------------------------------------------------------------
    float4 localPos = vi.posL;
    float4 localNormal = float4(vi.normalL.xyz, 0);

    // 頂点位置
    float4 skinnedPos = 0;
    skinnedPos += mul(localPos, gBones[vi.boneIdx.x]) * vi.weight.x;
    skinnedPos += mul(localPos, gBones[vi.boneIdx.y]) * vi.weight.y;
    skinnedPos += mul(localPos, gBones[vi.boneIdx.z]) * vi.weight.z;
    skinnedPos += mul(localPos, gBones[vi.boneIdx.w]) * vi.weight.w;

    // 法線（w=0）
    float4 skinnedNormal = 0;
    skinnedNormal += mul(localNormal, gBones[vi.boneIdx.x]) * vi.weight.x;
    skinnedNormal += mul(localNormal, gBones[vi.boneIdx.y]) * vi.weight.y;
    skinnedNormal += mul(localNormal, gBones[vi.boneIdx.z]) * vi.weight.z;
    skinnedNormal += mul(localNormal, gBones[vi.boneIdx.w]) * vi.weight.w;

    //---------------------------------------------------------------------
    // ★ 2. あなたが使っていた World / View / Projection
    //---------------------------------------------------------------------
    float4x4 mtxWV = mul(world, view);
    float4x4 mtxWVP = mul(mtxWV, projection);

    vo.posH = mul(skinnedPos, mtxWVP);

    // posW
    vo.posW = mul(skinnedPos, world);

    // 法線をワールド空間へ
    vo.normalW = normalize(mul(skinnedNormal, world));

    // 既存処理
    vo.color = vi.color;
    vo.uv = vi.uv;

    return vo;
}

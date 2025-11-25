#include "AnimationSystem.h"
#include <algorithm> // std::min, std::max
#include "model.h"
using namespace DirectX;

// ========================
// 工具函式
// ========================

static double WrapAnimationTime(double time, const Animation& anim, bool loop)
{
    double duration = anim.duration;
    if (duration <= 0.0) return 0.0;

    if (loop)
    {
        // 讓 time 落在 [0, duration)
        double t = fmod(time, duration);
        if (t < 0.0) t += duration;
        return t;
    }
    else
    {
        // Clamp 在結尾
        return std::min(time, duration);
    }
}

// ========================
// 插值 Position
// ========================
XMFLOAT3 SamplePosition(const AnimationChannel& c, double time)
{
    if (c.positions.empty())
        return XMFLOAT3(0, 0, 0);

    if (c.positions.size() == 1)
        return c.positions[0].second;

    // 找到 time 落在哪兩個 key 之間
    for (size_t i = 0; i < c.positions.size() - 1; ++i)
    {
        double t0 = c.positions[i].first;
        double t1 = c.positions[i + 1].first;
        if (time >= t0 && time <= t1)
        {
            float factor = (t1 == t0) ? 0.0f : static_cast<float>((time - t0) / (t1 - t0));

            XMVECTOR p0 = XMLoadFloat3(&c.positions[i].second);
            XMVECTOR p1 = XMLoadFloat3(&c.positions[i + 1].second);
            XMVECTOR r = XMVectorLerp(p0, p1, factor);

            XMFLOAT3 out;
            XMStoreFloat3(&out, r);
            return out;
        }
    }

    // time 超過最後一個 key，就給最後一個
    return c.positions.back().second;
}

// ========================
// 插值 Rotation（Slerp）
// ========================
XMFLOAT4 SampleRotation(const AnimationChannel& c, double time)
{
    if (c.rotations.empty())
        return XMFLOAT4(0, 0, 0, 1);

    if (c.rotations.size() == 1)
        return c.rotations[0].second;

    for (size_t i = 0; i < c.rotations.size() - 1; ++i)
    {
        double t0 = c.rotations[i].first;
        double t1 = c.rotations[i + 1].first;
        if (time >= t0 && time <= t1)
        {
            float factor = (t1 == t0) ? 0.0f : static_cast<float>((time - t0) / (t1 - t0));

            XMVECTOR r0 = XMLoadFloat4(&c.rotations[i].second);
            XMVECTOR r1 = XMLoadFloat4(&c.rotations[i + 1].second);

            XMVECTOR r = XMQuaternionSlerp(r1, r0, factor);
            r = XMQuaternionNormalize(r);

            XMFLOAT4 out;
            XMStoreFloat4(&out, r);
            return out;
        }
    }

    return c.rotations.back().second;
}

// ========================
// 插值 Scale
// ========================
XMFLOAT3 SampleScale(const AnimationChannel& c, double time)
{
    if (c.scales.empty())
        return XMFLOAT3(1, 1, 1);

    if (c.scales.size() == 1)
        return c.scales[0].second;

    for (size_t i = 0; i < c.scales.size() - 1; ++i)
    {
        double t0 = c.scales[i].first;
        double t1 = c.scales[i + 1].first;
        if (time >= t0 && time <= t1)
        {
            float factor = (t1 == t0) ? 0.0f : static_cast<float>((time - t0) / (t1 - t0));

            XMVECTOR s0 = XMLoadFloat3(&c.scales[i].second);
            XMVECTOR s1 = XMLoadFloat3(&c.scales[i + 1].second);
            XMVECTOR r = XMVectorLerp(s0, s1, factor);

            XMFLOAT3 out;
            XMStoreFloat3(&out, r);
            return out;
        }
    }

    return c.scales.back().second;
}

// ========================
// 建立 Local Matrix
// ========================
static XMMATRIX MakeLocalMatrix(const XMFLOAT3& pos, const XMFLOAT4& rot, const XMFLOAT3& scale)
{
    //XMFLOAT4 test = { 0,0,0,1 };
    XMMATRIX T = XMMatrixTranslation(pos.x, pos.y, pos.z);
    //XMMATRIX T = XMMatrixTranslation(0, 0,0);
    XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&rot));
   // XMMATRIX S = XMMatrixScaling(1,1,1);
    XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);

    // 一般是 S * R * T
    return S * R * T;
}

// ========================
// 更新整個 skeleton（bones 的 finalTransform）
// animTimeInTicks：已經轉成 “動畫的 tick”
// ========================
void UpdateSkeleton(MODEL& model, double animTimeInTicks)
{
    if (!model.AiScene || model.bones.empty())
        return;

    XMMATRIX identity = XMMatrixIdentity();
    ReadNodeHierarchy(model, animTimeInTicks,
        model.AiScene->mRootNode,\
        identity);
}

// ========================
// 組合成丟去 shader 用的 matrix 陣列
// ========================
void BuildSkinMatrices(const MODEL& model, std::vector<XMMATRIX>& outSkinMatrices)
{
    size_t count = model.bones.size();
    outSkinMatrices.resize(count);

    for (size_t i = 0; i < count; ++i)
    {
        // DirectX / HLSL 常見做法是丟 row_major 或 Transpose 後的 matrix
        outSkinMatrices[i] = XMMatrixTranspose(model.bones[i].finalTransform);
    }
}

// ========================
// AnimationPlayer::Update
// ========================
void AnimationPlayer::Update(MODEL& model, double deltaTime)
{
    const Animation& anim = model.animation;

    // 把 deltaTime(秒) 轉成 ticks
    currentTime += deltaTime;

    double ticks = currentTime * anim.ticksPerSecond/2;
    double duration = anim.duration; // duration in ticks

    double timeInTicks;
    timeInTicks = fmod(ticks, duration);

    UpdateSkeleton(model, timeInTicks);
}

void ReadNodeHierarchy(MODEL& model,
    double animTimeInTicks,
    const aiNode* node,
    const XMMATRIX& parentTransform)
{
    std::string nodeName = node->mName.C_Str();

    // --- 1) bind pose local transform ---
    XMMATRIX nodeTransform = AiToXMMATRIX(node->mTransformation);

    // --- 2) 如果有動畫通道，用動畫覆蓋 ---
    auto it = model.animation.channels.find(nodeName);
    bool hasAnimation = (it != model.animation.channels.end());
    bool isSkinBone = (model.boneIndex.count(nodeName) > 0);
    XMMATRIX bindLocal = AiToXMMATRIX(node->mTransformation);

    // 預設 = bind pose
    nodeTransform = bindLocal;

    // 如果這個 bone 真的是 skin bone → 才套動畫
    if (isSkinBone && hasAnimation)
    {
        const AnimationChannel& ch = it->second;

        XMFLOAT3 pos = SamplePosition(ch, animTimeInTicks);
        XMFLOAT4 rot = SampleRotation(ch, animTimeInTicks);
        XMFLOAT3 scale = SampleScale(ch, animTimeInTicks);

        nodeTransform =  MakeLocalMatrix(pos, rot, scale);
    }
    // --- 3) global = parent * local ---
    XMMATRIX globalTransform =  nodeTransform * parentTransform;

    // --- 4) 如果這是 bone，更新 finalTransform ---
    auto boneIt = model.boneIndex.find(nodeName);
    if (boneIt != model.boneIndex.end())
    {
        int boneIndex = boneIt->second;
        Bone& bone = model.bones[boneIndex];

        bone.finalTransform =  bone.offsetMatrix * globalTransform;
    }

    // --- 5) 遞迴子節點 ---
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ReadNodeHierarchy(model, animTimeInTicks,
            node->mChildren[i], globalTransform);
    }
}

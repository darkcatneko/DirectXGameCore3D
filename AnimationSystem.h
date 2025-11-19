#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "model.h"

// 簡單的動畫播放器（每個 model 可以有一個）
struct AnimationPlayer
{
    double currentTime = 0.0;  // 以 Animation 的 tick 為單位
    bool loop = true;

    // 每幀呼叫一次：deltaTime = 秒數
    void Update(MODEL& model, double deltaTime);
};

// =========================================
// 內部輔助函式（你也可以放在 cpp 裡當 static）
// =========================================
DirectX::XMFLOAT3 SamplePosition(const AnimationChannel& c, double time);
DirectX::XMFLOAT4 SampleRotation(const AnimationChannel& c, double time);
DirectX::XMFLOAT3 SampleScale(const AnimationChannel& c, double time);

// 直接用動畫時間（tick）更新整個骨架
void UpdateSkeleton(MODEL& model, double animTimeInTicks);

// 把 bones 裡的 finalTransform 整理成陣列，丟去 shader 用
// outSkinMatrices.size() 會被 resize 成 bones.size()
void BuildSkinMatrices(const MODEL& model, std::vector<DirectX::XMMATRIX>& outSkinMatrices);
void ReadNodeHierarchy(MODEL& model,
    double animTimeInTicks,
    const aiNode* node,
    const DirectX::XMMATRIX& parentTransform);
#endif

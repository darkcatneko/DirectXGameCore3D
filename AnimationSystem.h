#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "model.h"

struct AnimClip
{
    Animation* anim;     // 指向 MODEL.animation 或多個 Animation
    bool loop = true;
    double speed = 1.0;  // 播放速度
};
struct AnimState
{
    std::string name;    // "Idle", "Walk", ...
    AnimClip clip;
};
struct Animator
{
    std::unordered_map<std::string, AnimState> states;

    std::string current;    // active state
    std::string next;       // crossfade target
	bool zeroTransition = false;

    double timeCurrent = 0.0;
    double timeNext = 0.0;

    float blend = 0.0f;     // 0→1
    float blendSpeed = 5.0f;
    void Register(const std::string& name, Animation* anim, bool loop = true, double speed = 1.0);
	void Initialize(const std::string& defaultState);
    void CrossFade(MODEL& model,Animator& animator, const std::string& target, float fadeTime);
    void CrossFadeToZero(MODEL& model, Animator& animator, const std::string& target, float fadeTime);
    void Update(Animator& animator, MODEL& model, double deltaTime);
};

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
void UpdateSkeletonBlended(MODEL& model, double timeA, double timeB, double alpha);
// 把 bones 裡的 finalTransform 整理成陣列，丟去 shader 用
// outSkinMatrices.size() 會被 resize 成 bones.size()
void BuildSkinMatrices(const MODEL& model, std::vector<DirectX::XMMATRIX>& outSkinMatrices);
void ReadNodeHierarchy(MODEL& model,
    double animTimeInTicks,
    const aiNode* node,
    const DirectX::XMMATRIX& parentTransform);
void ReadNodeHierarchyBlend(MODEL& model,
    double animA,
	double animB,
	double alpha,
    const aiNode* node,
    const DirectX::XMMATRIX& parentTransform);
#endif

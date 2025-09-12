#ifndef EFFECT_H
#define EFFECT_H
#include "DirectXMath.h"

static constexpr int EFFECT_MAX = 256;

void  Effect_Initialize();
void  Effect_Finitialize();

void  EffectUpdate(float time);
void  EffectDraw();
void  EffectDrawUP();
void  Effect_Creater(DirectX::XMFLOAT2& position, int Effect_ID);//主要改造點

#endif // !EFFECT_H


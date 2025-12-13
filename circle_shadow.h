#ifndef CIRCLE_SHADOW_H
#define	CIRCLE_SHADOW_H

#include <d3d11.h>
#include <DirectXMath.h>

void CircleShadow_Initialize();
void CircleShadow_Finalize();
void CircleShadow_Draw(DirectX::XMFLOAT3 position);


#endif // !CIRCLE_SHADOW_H


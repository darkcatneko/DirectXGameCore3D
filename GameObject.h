#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include "DirectXMath.h"
class Transform
{
public:
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Rotation;
	DirectX::XMFLOAT4 RotationQ;
	DirectX::XMFLOAT3 Scale;
	Transform();
	Transform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 scale) ;
	Transform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotateQ);
	~Transform();

private:

};

class GameObject
{
public:
	Transform transform;
	GameObject();
	GameObject(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 scale);
	GameObject(DirectX::XMFLOAT3 pos);
	GameObject(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotate);
	GameObject(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 rotate);
	~GameObject();

private:

};





#endif

#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::GameObject(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 scale)
:transform(pos, rotate, scale)
{
}

GameObject::GameObject(DirectX::XMFLOAT3 pos):transform(pos, {0,0,0}, { 1,1,1 })
{
}

GameObject::GameObject(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotate) :transform(pos, rotate, { 1,1,1 })
{
}

GameObject::GameObject(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 rotate)
{
	transform.Position = pos;
	transform.Rotation = { 0,0,0 };
	transform.Scale = { 1,1,1 };
	transform.RotationQ = rotate;
}

GameObject::~GameObject()
{
}

Transform::Transform()
{
}

Transform::Transform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 scale)
	:Position(pos), Rotation(rotate), Scale(scale)
{
}

Transform::Transform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotateQ)
{
	Position = pos;
	Rotation = rotate;
	Scale = scale;
	RotationQ = rotateQ;
}

Transform::~Transform()
{
}
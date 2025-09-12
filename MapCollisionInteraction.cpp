#include "MapCollisionInteraction.h"

bool OnGameObjectCircleCollisionEnter(const Circle& a, const GameobjectColliderCircle& b)
{
	float x1 = b.center.x - a.center.x;
	float y1 = b.center.y - a.center.y;



	return (a.radius * a.radius + b.radius * b.radius) > (x1 * x1 + y1 * y1);
}

bool OnGameObjectCollisionEnter(const Box& a, const GameobjectColliderBox& b)
{
	float at = a.center.y - a.half_height;//a top
	float ab = a.center.y + a.half_height;//a bottom
	float al = a.center.x - a.half_width; //a left
	float ar = a.center.x + a.half_width; //a right
	float bt = b.center.y - b.half_height;//b top
	float bb = b.center.y + b.half_height;//b bottom
	float bl = b.center.x - b.half_width; //b left
	float br = b.center.x + b.half_width; //b right
	return al<br && ar>bl && at<bb && ab>bt;
}

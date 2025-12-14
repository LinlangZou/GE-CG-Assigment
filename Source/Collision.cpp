#include "Collision.h"
#include <algorithm>

CollisionBox::CollisionBox()
	: center(0, 0, 0), halfSize(1, 1, 1), hitTime(0.0f)
{
}

CollisionBox::CollisionBox(const Vec3& center, const Vec3& halfSize)
	: center(center), halfSize(halfSize), hitTime(0.0f)
{
}

void CollisionBox::setPosition(const Vec3& pos)
{
	center = pos;
}

void CollisionBox::setSize(const Vec3& size)
{
	halfSize = size;
}

bool CollisionBox::pointInAABB(const Vec3& point, const Vec3& center, const Vec3& halfSize) const
{
	return (point.x >= center.x - halfSize.x && point.x <= center.x + halfSize.x &&
			point.y >= center.y - halfSize.y && point.y <= center.y + halfSize.y &&
			point.z >= center.z - halfSize.z && point.z <= center.z + halfSize.z);
}

bool CollisionBox::checkCollision(const Vec3& point) const
{
	return pointInAABB(point, center, halfSize);
}

bool CollisionBox::rayIntersect(const Vec3& rayOrigin, const Vec3& rayDirection) const
{
	Vec3 min = center - halfSize;
	Vec3 max = center + halfSize;
	
	float tmin = 0.0f;
	float tmax = 1000000.0f;
	
	for (int i = 0; i < 3; i++)
	{
		if (fabsf(rayDirection.coords[i]) < 0.0001f)
		{
			if (rayOrigin.coords[i] < min.coords[i] || rayOrigin.coords[i] > max.coords[i])
				return false;
		}
		else
		{
			float invD = 1.0f / rayDirection.coords[i];
			float t0 = (min.coords[i] - rayOrigin.coords[i]) * invD;
			float t1 = (max.coords[i] - rayOrigin.coords[i]) * invD;
			
			if (invD < 0.0f)
				std::swap(t0, t1);
			
			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;
			
			if (tmax < tmin)
				return false;
		}
	}
	
	return tmax > 0.0f;
}

void CollisionBox::updateHitTime(const Vec3& rayOrigin, const Vec3& rayDirection, bool isShooting, float dt)
{
	if (isShooting && rayIntersect(rayOrigin, rayDirection))
	{
		hitTime += dt;
	}
}



#pragma once

#include "Maths.h"

// ==================== 碰撞盒类 ====================

class CollisionBox
{
public:
	Vec3 center;
	Vec3 halfSize;
	float hitTime;
	
	CollisionBox();
	CollisionBox(const Vec3& center, const Vec3& halfSize);
	
	void setPosition(const Vec3& pos);
	void setSize(const Vec3& size);
	
	bool checkCollision(const Vec3& point) const;
	
	bool rayIntersect(const Vec3& rayOrigin, const Vec3& rayDirection) const;
	
	void updateHitTime(const Vec3& rayOrigin, const Vec3& rayDirection, bool isShooting, float dt);
	
	float getHitTime() const { return hitTime; }
	
	void resetHitTime() { hitTime = 0.0f; }

private:
	bool pointInAABB(const Vec3& point, const Vec3& center, const Vec3& halfSize) const;
};
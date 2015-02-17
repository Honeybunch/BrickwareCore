#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include "BrickwareCoreDLL.h"

#include "Collider.h"

class BRICKWARE_CORE_API BoxCollider : public Collider
{
public:
	BoxCollider();
	~BoxCollider();

	Vector3 getCenter();
	Vector3 getSize();

	virtual Component* Clone();
	virtual void Start();

private:
	Vector3 center;
	Vector3 size; //Each element of this vector represents width on one axis
	Vector3 halfSize;

	bool isCollidingWithSphere(SphereCollider* other);
	bool isCollidingWithBox(BoxCollider* other);
	bool isCollidingWithFrustum(FrustumCollider* other);
	bool isCollidingWithBounds(Bounds other);
};

#endif
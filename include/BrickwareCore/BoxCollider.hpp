#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

//DLL Headers
#include "BrickwareCore/BrickwareCoreDLL.hpp"

//System Level Headers
#include <iostream>
#include <limits>

//Project Headers
#include "BrickwareCore/Collider.hpp"
#include "BrickwareCore/Camera.hpp"

namespace Brickware
{
	namespace Core
	{
		class BRICKWARE_CORE_API BoxCollider : public Collider
		{
		public:
			BoxCollider();
			BoxCollider(BoxCollider& other);
			~BoxCollider();

			Math::Vector3 getCenter();
			Math::Vector3 getSize();

			virtual Component* Clone();
			virtual void Start();

#ifdef BRICKWARE_DEBUG
			virtual void DebugDraw() override;
#endif

		private:
			Math::Vector3 size; //Each element of this vector represents width on one axis
			Math::Vector3 halfSize;

			bool isCollidingWithSphere(SphereCollider* other);
			bool isCollidingWithBox(BoxCollider* other, Collision* collision);
			bool isCollidingWithFrustum(FrustumCollider* other);
			bool isCollidingWithBounds(Math::Bounds other);
			bool isCollidingWithRay(Math::Ray other, Math::Vector3* pointOfCollision);
			bool isCollidingWithPoint(Math::Vector3 point);

			//Box specific methods
			bool isRayIntersectingSlab(Math::Vector3 projectionAxis, Math::Vector3 rayDirection, Math::Vector3 localizedCenter, float* min, float* max);
		};
	}
}
#endif
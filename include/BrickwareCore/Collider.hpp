#ifndef COLLIDER_H
#define COLLIDER_H

//Pre-Include Defines
#define NOMINMAX

//DLL Headers
#include "BrickwareCore/BrickwareCoreDLL.hpp"

//Other Brickware Project Headers
#include "BrickwareMath/Matrix3.hpp"
#include "BrickwareMath/Quaternion.hpp"
#include "BrickwareMath/Vector3.hpp"
#include "BrickwareMath/Bounds.hpp"
#include "BrickwareMath/Ray.hpp"

#include "BrickwareGraphics/Primitive.hpp"

//System Level Headers
#include <vector>
#include <limits>

//Project Headers
#include "BrickwareCore/Component.hpp"

namespace Brickware
{
	namespace Core
	{
		class SphereCollider;
		class BoxCollider;
		class FrustumCollider;

#ifdef _WIN32
		//Used to avoid warnings about exporting std::vectors
		template class __declspec(dllexport) std::vector < Math::Vector3 >;
#endif

		class BRICKWARE_CORE_API Collider : public Component
		{
		public:
			Collider();

			bool isColliding(Collider* collider, Collision* collsion);
			bool isColliding(Math::Bounds bounds);
			bool isColliding(Math::Ray ray, Math::Vector3* pointOfCollision);
			bool isColliding(Math::Vector3 point);

			virtual ~Collider();

		protected:
			bool enabled;
			bool isTrigger;

			Math::Vector3 center;
			std::vector<Math::Vector3> normals;
			std::vector<Math::Vector3> worldNormals;

			bool TestSAT(Collider* other, Collision* collision);

			void calculateWorldData();

		private:
			virtual bool isCollidingWithSphere(SphereCollider* other) = 0;
			virtual bool isCollidingWithBox(BoxCollider* other, Collision* collision) = 0;
			virtual bool isCollidingWithFrustum(FrustumCollider* other) = 0;
			virtual bool isCollidingWithBounds(Math::Bounds other) = 0;
			virtual bool isCollidingWithRay(Math::Ray other, Math::Vector3* pointOfCollision) = 0;
			virtual bool isCollidingWithPoint(Math::Vector3 point) = 0;

			Math::Vector2 getProjectedBounds(Math::Vector3 axis, Math::Vector3* worldPoints);
		};
	}
}

#endif

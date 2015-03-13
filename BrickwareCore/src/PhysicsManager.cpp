#define BRICKWARE_CORE_EXPORTS

#include "PhysicsManager.h"

//Statics
std::unordered_map<Rigidbody*, int> PhysicsManager::rigidbodies;
std::unordered_map<Collider*, int> PhysicsManager::colliders;
float PhysicsManager::gravity;

float PhysicsManager::GetGravity(){ return gravity; }
void PhysicsManager::SetGravity(float gravity){ PhysicsManager::gravity = gravity; }

void PhysicsManager::Initialize()
{
	gravity = -.0004905f;
}

void PhysicsManager::Update()
{
	//Update rigidbodies
	for (std::pair<Rigidbody*, int> pair : rigidbodies)
	{		
		Rigidbody* rigidbody = pair.first;
		rigidbody->FixedUpdate();
	}	

	//Detect collisions
	for (auto it1 = colliders.begin(); it1 != colliders.end(); it1++)
	{
		Collider* test = it1->first;

		for (auto it2 = colliders.begin(); it2 != colliders.end(); it2++)
		{
			Collider* other = it2->first;

			std::vector<Vector3> pointsOfContact;

			bool colliding = false;
			if (other != test)
				colliding = test->isColliding(other, pointsOfContact);
			
#ifdef _DEBUG
			if (Debug::Debugging)
			{
				Primitive::SetColor(Vector4(1, 0, .8f, 1));
				Primitive::SetPointSize(10.0f);

				for (unsigned int i = 0; i < pointsOfContact.size(); i++)
					Primitive::DrawPoint(pointsOfContact[i]);
			}
#endif

			if (other != test && colliding)
			{
				//Dispatch collisions
				GameObject* testObj = test->getGameObject();
				GameObject* otherObj = other->getGameObject();

				Collision* testCollision = new Collision(otherObj->getComponent<Rigidbody>(), other, std::vector<Vector3>());
				Collision* otherCollision = new Collision(testObj->getComponent<Rigidbody>(), test, std::vector<Vector3>());

				testObj->OnCollisionEnter(testCollision);
				otherObj->OnCollisionEnter(otherCollision);
			}
		}
	}
}

void PhysicsManager::Destroy()
{

}

void PhysicsManager::AddRigidbody(Rigidbody* rigidbody)
{
	rigidbodies[rigidbody] = 0;
}
void PhysicsManager::AddCollider(Collider* collider)
{
	colliders[collider] = 0;
}

void PhysicsManager::RemoveRigidbody(Rigidbody* rigidbody)
{
	rigidbodies.erase(rigidbody);
}
void PhysicsManager::RemoveCollider(Collider* collider)
{
	colliders.erase(collider);
}

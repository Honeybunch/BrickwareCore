#define BRICKWARE_CORE_EXPORTS

#include "BrickwareCore\Rigidbody.hpp"
#include "BrickwareCore\PhysicsManager.hpp"

using namespace Brickware;
using namespace Core;
using namespace Math;

Rigidbody::Rigidbody()
{
	velocity = Vector3();
	sleepVelocity = Vector3(.1f, .1f, .1f);

	angularVelocity = Vector3();
	angularSleepVelocity = Vector3(.1f, .1f, .1f);

	centerOfMass = Vector3();
	worldCenterOfMass = Vector3();

	mass = 1.0f;
	inverseMass = 1.0f;

	collisionIterations = 3;

	drag = .999f;
	angularDrag = .999f;

	detectCollisions = true;
	isKinematic = true;
	useGravity = true;
}

void Rigidbody::Start()
{
	transform = getGameObject()->getTransform();

	//We should probably get an inertia tensor from a collider or mesh but we'll just assume it's a box
	Vector3 scale = getGameObject()->getTransform()->getScale();
	float inertiaTensorX = (mass * (powf(scale[1] * 2, 2) + powf(scale[2] * 2, 2))) / 12;
	float inertiaTensorY = (mass * (powf(scale[0] * 2, 2) + powf(scale[2] * 2, 2))) / 12;
	float inertiaTensorZ = (mass * (powf(scale[0] * 2, 2) + powf(scale[1] * 2, 2))) / 12;

	inertiaTensor = Vector3(inertiaTensorX, inertiaTensorY, inertiaTensorZ);

	//Register into the physics system
	PhysicsManager::AddRigidbody(this);
}

//Accessors
Vector3 Rigidbody::getVelocity(){ return velocity; }
Vector3 Rigidbody::getSleepVelocity(){ return sleepVelocity; }

Vector3 Rigidbody::getAngularVelocity(){ return angularVelocity; }
Vector3 Rigidbody::getAngluarSleepVelocity(){ return angularSleepVelocity; }

Vector3 Rigidbody::getCenterOfMass(){ return centerOfMass; }
Vector3 Rigidbody::getWorldCenterOfMass(){ return worldCenterOfMass; }

//Mutators
void Rigidbody::setMass(float mass){ this->mass = mass; this->inverseMass = 1 / mass; }
void Rigidbody::setIsKinematic(bool isKinematic){ this->isKinematic = isKinematic; }
void Rigidbody::setUseGravity(bool useGravity){ this->useGravity = useGravity; }

//Functions to manipulate rigidbody
void Rigidbody::addForce(Vector3 force, Vector3 point)
{
	frameForce += force;

	Vector3 torque = Vector3::Cross(point, force);
	frameTorque += torque;
}

void Rigidbody::addImpulse(Vector3 impulse, Vector3 point)
{
	this->impulse += impulse;

	Vector3 instantTorque = Vector3::Cross(point, impulse);
	frameInstantTorque += instantTorque;
}

//void Rigidbody::addForceAtPosition(Vector3 force, Vector3 position){}
//void Rigidbody::addExplosionForce(Vector3 force, Vector3 position, float radius){}

void Rigidbody::addTorque(Vector3 torque)
{
	frameTorque += torque;
}

void Rigidbody::addInstantaneousTorque(Vector3 instantTorque)
{
	frameInstantTorque += instantTorque;
}

Matrix3 Rigidbody::momentOfInertia()
{
	Matrix3 rotationMatrix = getGameObject()->getTransform()->getRotation().getRotationMatrix();

	Matrix3 worldInertia = Matrix3(inertiaTensor[0], 0, 0,
		0, inertiaTensor[1], 0,
		0, 0, inertiaTensor[2]);
	Matrix3 inverseWorldInertia = worldInertia.getInverse();
	Matrix3 transposedRotationMatrix = rotationMatrix.getTranspose();
	worldInertia = rotationMatrix * inverseWorldInertia * transposedRotationMatrix;

	return worldInertia;
}

//Called on a fixed timestep for physics calculations
void Rigidbody::FixedUpdate()
{
	float deltaTime = GameTime::GetDeltaTime();

	//Calculate acceleration from applied forces
	Vector3 frameAcceleration = (frameForce * inverseMass) * deltaTime;			//A = 1/mass * Force
	angularAcceleration = momentOfInertia() * frameTorque;						//AA = 1/Inertia * Torque
	
	//Calcuate net impluse for later
	Vector3 netImpulse = impulse * inverseMass;									//J = J * 1/mass
	frameInstantTorque = momentOfInertia() * frameInstantTorque;

	//Calculate velocities
	Vector3 startVelocity = velocity * deltaTime;								//VT = V0 * deltaTime
	angularVelocity += angularAcceleration * deltaTime;

	Vector3 VAT = startVelocity + (acceleration * 0.5f * powf(deltaTime, 2));	//VAT = VT + (A1 * 1/2 * deltaTime^2)
	Vector3 AngularVT = angularVelocity * deltaTime;							//AngularVT = AV0 + AA * deltaTime^2

	//Apply constant forces
	if (useGravity)
		acceleration[1] += PhysicsManager::gravity;
	
	//Integrate velocity into position
	Vector3 position = transform->getPosition();
	position += VAT;
	transform->setPosition(position);

	//Integrate angular velocity into rotation
	Vector3 eulerRotation = transform->getEulerRotation();
	eulerRotation += AngularVT;
	transform->setEulerRotation(eulerRotation);

	//Integrate acceleration into velocity
	velocity += frameAcceleration;
	angularVelocity += angularAcceleration;

	//Apply impulse
	velocity += netImpulse;
	angularVelocity += frameInstantTorque;

	impulse = Vector3();
	frameInstantTorque = Vector3();

	frameForce = Vector3();
	frameTorque = Vector3();
}
void Rigidbody::OnCollision(Collision* collision)
{
	if (!isKinematic)
		return;

	Rigidbody* otherRigidbody = collision->getOtherRigidbody();
	Collider* otherCollider = collision->getOtherCollider();

	if (otherRigidbody == nullptr)
		return;

	Vector3 MTV = collision->getMTV();
	Vector3 normal = Vector3::Normalize(MTV);

	//Reposition rigidbody's game object back to where the collision happened so that it no longer intersects
	transform->setPosition(transform->getPosition() + MTV);

	//Calculate new forces
	bool    otherIsKinematic = otherRigidbody->isKinematic;
	float   otherMass = otherRigidbody->mass;
	Vector3 otherVelocity = otherRigidbody->velocity;
	Vector3 otherAngularVelocity = otherRigidbody->angularVelocity;
	Vector3 otherCenterOfMass = otherRigidbody->centerOfMass;
	Vector3 otherInertiaTensor = otherRigidbody->inertiaTensor;
	Matrix3 otherRotationMatrix = otherRigidbody->getGameObject()->getTransform()->getRotation().getRotationMatrix();

	Matrix3 otherMomentOfInertia = otherRigidbody->momentOfInertia();
	
	//Calculate which point we're going to use as the "Point of collision"
	std::vector<Vector3> pointsOfCollision = collision->getPointsOfCollision();
	Vector3 pointOfCollision;

	if (pointsOfCollision.size() == 0)
		return;

	unsigned int bestPointIndex = 0;
	float bestPointDot = 0;

	if (pointsOfCollision.size() > 1)
	{
		for (unsigned int i = 0; i < pointsOfCollision.size(); i++)
		{
			float dot = Vector3::Dot(pointsOfCollision[i], MTV);
			if (fabsf(dot) > fabsf(bestPointDot))
			{
				bestPointDot = dot;
				bestPointIndex = i;
			}
		}
	}

	pointOfCollision = pointsOfCollision[bestPointIndex];

	//Get radii
	Vector3 radius = pointOfCollision - (centerOfMass);
	Vector3 otherRadius = pointOfCollision - (otherCollider->getGameObject()->getTransform()->getPosition() + otherCenterOfMass);

	Matrix3 momentOfInertia = this->momentOfInertia();

	//Determine the impulse based on Chris Hecker's formula
	float e = 0.5f;
	Vector3 relativeVelocity;

	//Calculate the numerator of the impulse calculation
	Vector3 totalVelocity1 = Vector3::Cross(angularVelocity, radius);
	totalVelocity1 += velocity;

	Vector3 totalVelocity2;

	if (otherIsKinematic)
	{
		totalVelocity2 = Vector3::Cross(otherAngularVelocity, otherRadius);
		totalVelocity2 += otherVelocity;
	}

	relativeVelocity = totalVelocity1 - totalVelocity2;

	float relativeNormalVelocity = Vector3::Dot(relativeVelocity, normal);
	float numerator = (-1 - e) * relativeNormalVelocity;
	
	//Calculate the denominator

	Vector3 torque1 = Vector3::Cross(radius, MTV);
	Vector3 torque2 = Vector3::Cross(otherRadius, MTV);

	Vector3 velFromTorque1 = momentOfInertia * torque1;
	velFromTorque1 = Vector3::Cross(velFromTorque1, radius);

	Vector3 velFromTorque2;
	if (otherIsKinematic)
	{
		velFromTorque2 = otherMomentOfInertia * torque2;
		velFromTorque2 = Vector3::Cross(velFromTorque2, otherRadius);
	}

	Vector3 velFromTorques = velFromTorque1;
	velFromTorques += velFromTorque2;

	float inverseMassSum = inverseMass;
	if (otherIsKinematic)
		inverseMassSum += otherRigidbody->inverseMass;

	float denominator = inverseMassSum + Vector3::Dot(velFromTorques, MTV);

	//Finally calculate impulse
	float impulse = numerator / denominator;
	Vector3 impulseVec = normal * impulse;
 	addImpulse(impulseVec, pointOfCollision);
}

Rigidbody::~Rigidbody()
{
	PhysicsManager::RemoveRigidbody(this);
}

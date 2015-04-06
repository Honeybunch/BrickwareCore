#define BRICKWARE_CORE_EXPORTS

#include "Rigidbody.h"
#include "PhysicsManager.h"

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

	drag = .999f;
	angularDrag = .999f;

	detectCollisions = true;
	isKinematic = true;
	useGravity = true;
}

void Rigidbody::Start()
{
	//Apply gravity
	if (useGravity)
		acceleration = Vector3(0, PhysicsManager::gravity, 0);

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
void Rigidbody::setMass(float mass){ this->mass = mass; }
void Rigidbody::setIsKinematic(bool isKinematic){ this->isKinematic = isKinematic; }
void Rigidbody::setUseGravity(bool useGravity){ this->useGravity = useGravity; }

//Functions to manipulate rigidbody
void Rigidbody::addForce(Vector3 force)
{
	//Derive acceleration from the applied force divided by the object's mass
	acceleration += force / mass;
}
void Rigidbody::addForceAtPosition(Vector3 force, Vector3 position){}
void Rigidbody::addExplosionForce(Vector3 force, Vector3 position, float radius){}

void Rigidbody::addTorque(Vector3 torque)
{
	angularAcceleration += momentOfInertia() * torque;
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
	Transform* transform = getGameObject()->getTransform();

	//Apply velocity to position
	Vector3 position = transform->getPosition();
	position += velocity;
	transform->setPosition(position);

	//Apply angular velocity to rotation
	Vector3 eulerRotation = transform->getEulerRotation();
	eulerRotation += angularVelocity;
	transform->setEulerRotation(eulerRotation);
	
	//Derive velocity from acceleration 
	velocity += acceleration;
	angularVelocity += angularAcceleration;

	//Apply drag
	velocity *= drag;
	angularVelocity *= angularDrag;

	acceleration = Vector3();
	angularAcceleration = Vector3();

	//Add gravity
	if (useGravity)
		acceleration[1] += PhysicsManager::gravity;
}
void Rigidbody::OnCollision(Collision* collision)
{
	if (!isKinematic)
		return;

	Rigidbody* otherRigidbody = collision->getOtherRigidbody();
	Collider* otherCollider = collision->getOtherCollider();

	if (otherRigidbody == NULL)
		return;

	//Reposition rigidbody's game object back to where the collision happened so that it no longer intersects
	Vector3 MTV = collision->getMTV();
	Vector3 normal = Vector3::Normalize(MTV);

	getGameObject()->getTransform()->setPosition(getGameObject()->getTransform()->getPosition() + MTV);

	//Calculate new forces
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
	float bestPointDot = std::numeric_limits<float>::min();

	if (pointsOfCollision.size() > 1)
	{
		for (unsigned int i = 0; i < pointsOfCollision.size(); i++)
		{
			float dot = Vector3::Dot(pointsOfCollision[i], MTV);
			if (dot > bestPointDot)
			{
				bestPointDot = dot;
				bestPointIndex = i;
			}
		}
	}

	pointOfCollision = pointsOfCollision[bestPointIndex];

	//Get radii
	Vector3 radius = pointOfCollision - (getGameObject()->getTransform()->getPosition() + centerOfMass);
	Vector3 otherRadius = (otherCollider->getGameObject()->getTransform()->getPosition() + otherCenterOfMass) - pointOfCollision;

	Matrix3 momentOfInertia = this->momentOfInertia();

	//Determine the impulse based on Chris Hecker's formula
	float e = 0.9f;
	Vector3 relativeVelocity;

	//Calculate the numerator of the impulse calculation
	Vector3 totalVelocity1 = Vector3::Cross(angularVelocity, radius);
	totalVelocity1 += velocity;

	Vector3 totalVelocity2 = Vector3::Cross(otherAngularVelocity, otherRadius);
	totalVelocity2 += otherVelocity;

	relativeVelocity = totalVelocity1 - totalVelocity2;

	float relativeNormalVelocity = Vector3::Dot(relativeVelocity, normal);
	float numerator = (-1 - e) * relativeNormalVelocity;
	
	//Calculate the denominator

	Vector3 torque1 = Vector3::Cross(radius, normal);
	Vector3 torque2 = Vector3::Cross(otherRadius, normal);

	Vector3 velFromTorque1 = momentOfInertia * torque1;
	Vector3 velFromTorque2 = otherMomentOfInertia * torque2;

	velFromTorque1 = Vector3::Cross(velFromTorque1, radius);
	velFromTorque2 = Vector3::Cross(velFromTorque2, otherRadius);

	Vector3 velFromTorques = velFromTorque1;
	if (otherRigidbody->isKinematic)
		velFromTorques += velFromTorque2;

	float inverseMassSum = (1 / mass);
	if (otherRigidbody->isKinematic)
		inverseMassSum += (1 / otherMass);

	float denominator = inverseMassSum + Vector3::Dot(velFromTorques, normal);

	//Finally calculate impulse
	float impulse = numerator / denominator;

	//Determine resultant angularVelocity
	angularVelocity += momentOfInertia * Vector3::Cross(radius, normal * impulse);

	//Determine resulting velocity
	velocity += (normal * (impulse/mass));
}

Rigidbody::~Rigidbody()
{
	PhysicsManager::RemoveRigidbody(this);
}

#define BRICKWARE_CORE_EXPORTS

#include "BrickwareCore/Octree.hpp"
#include "BrickwareCore/Camera.hpp"
#include "BrickwareCore/Settings.hpp"
#include "BrickwareCore/Game.hpp"
#include "BrickwareCore/GameObject.hpp"

using namespace Brickware;
using namespace Core;
using namespace Math;
using namespace Graphics;

//Statics
void GameObject::Destroy(GameObject* gameObject)
{
	gameObject->toDestroy = true;
}

GameObject::GameObject()
{
	transform = new Transform();

	addComponent(transform);

	Game::gameObjects.push_back(this);

	toDestroy = false;
}

GameObject::GameObject(Transform* transform)
{
	GameObject::transform = transform;

	addComponent(GameObject::transform);

	Game::gameObjects.push_back(this);

	toDestroy = false;
}

GameObject::GameObject(GameObject& other)
{
	components = std::vector<Component*>();

	//Copy components that are able to be copied over
	const size_t componentCount = other.components.size();

	for (unsigned int i = 0; i < componentCount; i++)
	{
		Component* copiedComponent = other.components[i]->Clone();
		if (copiedComponent)
		{
			components.push_back(copiedComponent);
			copiedComponent->setGameObject(this);
		}
	}

	//Determine which of the copied components is the Transform
	for (unsigned int i = 0; i < componentCount; i++)
	{
		Transform* newTransform = dynamic_cast<Transform*>(components[i]);
		if (newTransform)
		{
			transform = newTransform;
			break;
		}
	}

	Game::gameObjects.push_back(this);

	toDestroy = false;
}

std::vector<GameObject*> GameObject::getGameObjects(){ return Game::gameObjects; }

//Accessors
Transform* GameObject::getTransform(){ return transform; }

std::vector<Component*>& GameObject::getComponents(){ return components; }

//Mutators
void GameObject::addComponent(Component* newComponent)
{
	newComponent->setGameObject(this);
	components.push_back(newComponent);
}

void GameObject::Start()
{
	for (unsigned int i = 0; i < components.size(); i++)
		components[i]->Start();
}

void GameObject::Update()
{
	for (unsigned int i = 0; i < components.size(); i++)
		components[i]->Update();
}

void GameObject::FixedUpdate()
{
	for (unsigned int i = 0; i < components.size(); i++)
		components[i]->FixedUpdate();
}

void GameObject::OnCollisionEnter(Collision* collision)
{
	for (unsigned int i = 0; i < components.size(); i++)
		components[i]->OnCollision(collision);
}

void GameObject::OnMouseOver()
{
	for (unsigned int i = 0; i < components.size(); i++)
		components[i]->OnMouseOver();
}

void GameObject::OnMouseStay()
{
	for (unsigned int i = 0; i < components.size(); i++)
		components[i]->OnMouseStay();
}

void GameObject::OnMouseExit()
{
	for (unsigned int i = 0; i < components.size(); i++)
		components[i]->OnMouseExit();
}

void GameObject::OnRender()
{
	for (unsigned int i = 0; i < components.size(); i++)
	{
		components[i]->Render();
#ifdef BRICKWARE_DEBUG
		if (Debug::Debugging)
			components[i]->DebugDraw();
#endif
	}
}

GameObject::~GameObject()
{
	for (unsigned int i = 0; i < components.size(); i++)
		delete components[i];

	components.clear();

	//Before the GameObject is deleted make sure to remove it from the collection
	for (unsigned int i = 0; i < Game::gameObjects.size(); i++)
	{
		if (Game::gameObjects[i] == this)
 			Game::gameObjects.erase(Game::gameObjects.begin() + i, Game::gameObjects.begin() + 1 + i);
	}
}

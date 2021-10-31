#pragma once
#include "headers.h"
#include "Renderer3D/Model3D.h"
#include "box2d/box2d.h"

struct Window
{
  Window(GLFWwindow* _window) : window(_window) {}

  GLFWwindow* window;
};

struct Color
{
  Color(GLfloat _red, GLfloat _green, GLfloat _blue, GLfloat _alpha) : red(_red), green(_green), blue(_blue), alpha(_alpha) {}

  GLfloat red, green, blue, alpha;
};

struct Transform
{
	glm::vec3 position, scale;
	//first 3 values are axis of rotation, last value is rotation degrees
	glm::vec4 rotation;

	Transform(glm::vec3 _position, glm::vec4 _rotation, glm::vec3 _scale) : position(_position), rotation(_rotation), scale(_scale) {}
};

struct Rigidbody
{
public:
	enum class BodyShape { BOX, CIRCLE };
	enum class ColliderType { PLAYER, PLAYER_ATTACK, ENEMY, ENEMY_ATTACK, COLLECTIBLE, WALL };

	Rigidbody(BodyShape _bs, ColliderType _ct, float _size, glm::vec2 _pos) : shape(_bs), type(_ct), position(_pos), size(_size) {}

	glm::vec2 GetPosition()
	{
		b2Vec2 bodyPosition = body->GetPosition();
		return glm::vec2(bodyPosition.x, bodyPosition.y);
	}

	void SetPosition(glm::vec2 p)
	{
		body->SetTransform(b2Vec2(p.x, p.y), 0);
	}

	glm::vec2 GetVelocity()
	{
		b2Vec2 bodyVelocity = body->GetLinearVelocity();
		return glm::vec2(bodyVelocity.x, bodyVelocity.y);
	}

	void SetVelocity(glm::vec2 v)
	{
		body->SetLinearVelocity(b2Vec2(v.x, v.y));
	}

	void ApplyForce(glm::vec2 f)
	{
		body->ApplyForce(b2Vec2(f.x, f.y), body->GetLocalCenter(), true);
	}

	void DeleteBody()
	{
		toDelete = true;
	}

	float size;
	glm::vec2 position;
	BodyShape shape;
	ColliderType type;

	b2Body* body = nullptr;
	bool isCreated = false;
	bool toDelete = false;

	Entity* parentEntity;
};

struct Collision
{
	Entity* a;
	Entity* b;

	Collision(Entity* _a, Entity* _b) : a(_a), b(_b) {}
};

struct GameObject
{
	string name;
	GameObject(string _name) : name(_name) {}
};


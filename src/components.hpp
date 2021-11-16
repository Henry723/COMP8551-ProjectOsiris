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

//Collider struct for creating fixtures on a rigidbody.
struct Collider
{
	enum class Shape { BOX, CIRCLE }; //Available shapes for fixtures
	bool isSensor; //Is the fixture a sensor? Sensors do not have physical collision
	Shape shape; //Shape of the fixture
	int size; //Size of the fixture
	string type; //Optional type to name a fixture, helpful for custom hit detection
	glm::vec2 position; //Position of the fixture relative to the body

	//Basic constructor
	Collider(Shape _shape, glm::vec2 _pos, bool sensor, int _size, string _type) : shape(_shape), position(_pos), isSensor(sensor), size(_size), type(_type) {};
};

//Rigidbody to handle movement and collision of bodies.
struct Rigidbody
{
	//Types of colliders to determine body types.
	//Players and enemies will be dynamic. Attacks are kinematic. Walls and collectibles are static.
	enum class ColliderType { PLAYER, PLAYER_ATTACK, ENEMY, ENEMY_ATTACK, COLLECTIBLE, WALL };

	//Passthrough Box2D function for getting a body position.
	glm::vec2 GetPosition()
	{
		b2Vec2 bodyPosition = body->GetPosition();
		return glm::vec2(bodyPosition.x, bodyPosition.y);
	}

	//Passthrough Box2D function for setting a body position.
	void SetPosition(glm::vec2 p)
	{
		body->SetTransform(b2Vec2(p.x, p.y), 0);
	}

	//Passthrough Box2D function for getting a body velocity.
	glm::vec2 GetVelocity()
	{
		b2Vec2 bodyVelocity = body->GetLinearVelocity();
		return glm::vec2(bodyVelocity.x, bodyVelocity.y);
	}

	//Passthrough Box2D function for setting a body velocity.
	void SetVelocity(glm::vec2 v)
	{
		body->SetLinearVelocity(b2Vec2(v.x, v.y));
	}

	//Passthrough Box2D function for applying force to a body
	void ApplyForce(glm::vec2 f)
	{
		body->ApplyForce(b2Vec2(f.x, f.y), body->GetLocalCenter(), true);
	}

	//Passthrough Box2D function for applying impulse to a body
	void ApplyImpulse(glm::vec2 i)
	{
		body->ApplyForce(b2Vec2(i.x, i.y), body->GetPosition(), true);
	}

	//Movement function to essentially SetPosition() but compatible with collision
	void MoveToPosition(glm::vec2 newPos, float speed)
	{
		if (!moveBody)
		{
			moveBody = true; //Checked on update
			dest = newPos; //Set destination
			glm::vec2 a = this->GetPosition();
			glm::vec2 b = newPos;
			this->SetVelocity(speed * (b - a)); //Set a velocity vector in the direction to go
		}
	}

	//Set a body's deletion status to true
	void DeleteBody()
	{
		toDelete = true;
	}

	b2Body* body = nullptr; //Box2D body reference
	ColliderType type; //Type of collider
	glm::vec2 position; //Starting position, only used on creation
	glm::vec2 dest; //Destination if moving
	bool moveBody = false; //Is the body moving?
	bool isCreated = false; //Is the body created yet?
	bool toDelete = false; //Should the body be deleted?
	vector<Collider> colliders; //Colliders to add as fixtures

	//Basic constructor
	Rigidbody(vector<Collider> _colliders, ColliderType _ct, glm::vec2 _pos) : colliders(_colliders), type(_ct), position(_pos), dest(_pos) {}
};

//Collision struct to be created for collision events
struct Collision
{
	Entity* a; //Left colliding element
	Entity* b; //Right colliding element

	string fA = ""; //Fixture name for left element
	string fB = ""; //Fixture name for right element

	//Regular collider, no fixture data
	Collision(Entity* _a, Entity* _b) : a(_a), b(_b) {}

	//Collider including fixture names
	Collision(Entity* _a, Entity* _b, string _fA, string _fB) : a(_a), b(_b), fA(_fA), fB(_fB) {}
};

//Simple GameObject struct for tracking generic info, only includes name for now.
struct GameObject
{
	string name;
	GameObject(string _name) : name(_name) {}
};


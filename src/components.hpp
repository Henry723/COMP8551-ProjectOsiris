#pragma once
#include "headers.h"
#include "Renderer3D/Model3D.h"
#include "box2d/box2d.h"
#include <stack>

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
	b2Fixture* fixture;

	//Basic constructor
	Collider(Shape _shape, glm::vec2 _pos, bool sensor, int _size, string _type) : shape(_shape), position(_pos), isSensor(sensor), size(_size), type(_type) {};
};

//Struct for b2Body user data
struct CollisionData
{
	Entity e; //Parent entity of the body
};

//Struct for b2Fixture user data
struct FixtureData
{
	string name; //Name given to the collider, used for sensors
};

//Simple GameObject struct for tracking generic info, only includes name for now.
struct GameObject
{
	string name;
	GameObject(string _name) : name(_name) {}
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

	bool IsColliding(string fixtureName)
	{
		for (b2ContactEdge* edge = body->GetContactList(); edge; edge = edge->next)
		{
			b2Fixture* fixA = edge->contact->GetFixtureA();
			b2Fixture* fixB = edge->contact->GetFixtureB();

			FixtureData* dataA = (FixtureData * )fixA->GetUserData().pointer;
			FixtureData* dataB = (FixtureData*)fixB->GetUserData().pointer;

			if (dataA->name == fixtureName && fixA->GetBody() == body) return true;
			if (dataB->name == fixtureName && fixB->GetBody() == body) return true;
		}
		return false;
	}

	bool IsCollidingWithBody(string fixtureName)
	{
		vector<b2Fixture*> collisions;
		for (b2ContactEdge* edge = body->GetContactList(); edge; edge = edge->next)
		{
			b2Fixture* fixA = edge->contact->GetFixtureA();
			b2Fixture* fixB = edge->contact->GetFixtureB();

			FixtureData* dataA = (FixtureData*)fixA->GetUserData().pointer;
			FixtureData* dataB = (FixtureData*)fixB->GetUserData().pointer;
			
			if (dataA->name == fixtureName && fixA->GetBody() == body) collisions.push_back(fixB);
			if (dataB->name == fixtureName && fixB->GetBody() == body) collisions.push_back(fixA);
		}
		for (b2Fixture* fixture : collisions) if (!fixture->IsSensor()) return true;
		return false;
	}

	bool IsCollidingWithSensor(string fixtureName)
	{
		vector<b2Fixture*> collisions;
		for (b2ContactEdge* edge = body->GetContactList(); edge; edge = edge->next)
		{
			b2Fixture* fixA = edge->contact->GetFixtureA();
			b2Fixture* fixB = edge->contact->GetFixtureB();

			FixtureData* dataA = (FixtureData*)fixA->GetUserData().pointer;
			FixtureData* dataB = (FixtureData*)fixB->GetUserData().pointer;

			if (dataA->name == fixtureName && fixA->GetBody() == body) collisions.push_back(fixB);
			if (dataB->name == fixtureName && fixB->GetBody() == body) collisions.push_back(fixA);
		}
		for (b2Fixture* fixture : collisions) if (fixture->IsSensor()) return true;
		return false;
	}

	vector<Collider> GetColliders()
	{
		return colliders;
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

//Collision struct to be created for collision events
struct EndCollision
{
	Entity* a; //Left colliding element
	Entity* b; //Right colliding element

	string fA = ""; //Fixture name for left element
	string fB = ""; //Fixture name for right element

	//Regular collider, no fixture data
	EndCollision(Entity* _a, Entity* _b) : a(_a), b(_b) {}

	//Collider including fixture names
	EndCollision(Entity* _a, Entity* _b, string _fA, string _fB) : a(_a), b(_b), fA(_fA), fB(_fB) {}
};

struct PlayerMoveStart {};
struct PlayerMoveFinish {};
struct PlayerTurnEnd {};
struct PlayerTurnStart {};
struct EnemyTurnStart {};
struct EnemyTurnEnd {};
struct EnemyMoveStart {};
struct EnemyMoveEnd {};
struct TimerEnd {};

struct CommandFlags
{
	enum EnemyCommand {
		MOVE_UP,
		MOVE_DOWN,
		MOVE_LEFT,
		MOVE_RIGHT,
		WAITING
	};

	int nextMoveDir = -1;
	int moveTurn = 0;
	bool move_on_evens() { return moveTurn % 2 == 0; }

	// Enemy Command input flags
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	bool isMoving = false;
	bool moveComplete = false;

	EnemyCommand nextCommand = WAITING;

	//Attack input flags
	bool attackLeft = false;
	bool attackRight = false;
	bool attackUp = false;
	bool attackDown = false;

	//Collision flags for available movement.
	//		These will only be false if obstacles are detected.
	bool canMoveUp = true;
	bool canMoveDown = true;
	bool canMoveLeft = true;
	bool canMoveRight = true;

	string destination = "";

	//Directional entities relative to body
	Entity* leftEntity = nullptr;
	Entity* rightEntity = nullptr;
	Entity* upEntity = nullptr;
	Entity* downEntity = nullptr;

	Entity* playerEntity = nullptr;

	stack<Entity*> leftContacts;
	stack<Entity*> rightContacts;
	stack<Entity*> upContacts;
	stack<Entity*> downContacts;

	CommandFlags()
	{
		srand(time(NULL));
		/* generate random number between 1 and 2: */
		moveTurn = (rand() % 2) + 1;
	}
};



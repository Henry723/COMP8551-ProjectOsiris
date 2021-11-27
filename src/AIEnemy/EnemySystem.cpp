#include "EnemySystem.h"

void EnemySystem::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	
	if (gameState != RUNNING) return;
	if (enemyTurn)
	{
		ComponentHandle<GameObject> gameObject;
		for (Entity entity : es.entities_with_components(gameObject))
		{
			ComponentHandle<GameObject> object = entity.component<GameObject>();
			if (object && object->name == "enemy")
			{
				ComponentHandle<Rigidbody> rigidbody = entity.component<Rigidbody>();
				ComponentHandle<Transform> transform = entity.component<Transform>();
				ComponentHandle<CommandFlags> commands = entity.component<CommandFlags>();

				if (commands->playerEntity)
				{
					cout << "ENEMY SYSTEM: attack here" << endl;
				}
				else
				{
				

					int direction = rand() % 4;
					glm::vec2 position = rigidbody->GetPosition();
					switch(direction)
					{
						case 0:
							if (commands->leftContacts.size() == 0)
							{
								cout << "ENEMY SYSTEM: moving left" << endl;
								rigidbody->MoveToPosition(glm::vec2(position.x - 2, position.y), 0.5);
								transform->rotation = glm::vec4(0, 1, 0, 20.5f);
								break;
							}
						case 1:
							if (commands->upContacts.size() == 0)
							{
								cout << "ENEMY SYSTEM: moving up" << endl;
								rigidbody->MoveToPosition(glm::vec2(position.x, position.y - 2), 0.5);
								transform->rotation = glm::vec4(0, 1, 0, 0);
								break;
							}
						case 2:
							if (commands->rightContacts.size() == 0)
							{
								cout << "ENEMY SYSTEM: moving right" << endl;
								rigidbody->MoveToPosition(glm::vec2(position.x + 2, position.y), 0.5);
								transform->rotation = glm::vec4(0, -1, 0, 20.5f);
								break;
							}
						case 3:
							if (commands->downContacts.size() == 0)
							{
								cout << "ENEMY SYSTEM: moving down" << endl;
								rigidbody->MoveToPosition(glm::vec2(position.x, position.y + 2), 0.5);
								transform->rotation = glm::vec4(0, 1, 0, 9.5f);
								break;
							}
					}
				}

				// === THIS Block checks the enemy's current state, and sets according flags.

				// If the enemy moves on this turn, perform the action in the saved direction.
				/*
				if (commands->playerEntity)
				{
					cout << endl << "should attack" << endl << endl;
				}
				else if (currentTurnCounter % 2 == 0)
				{
					switch (commands->nextMoveDir)
					{
					case 1:
						commands->nextMoveDir = -1;
						if (commands->leftContacts.size() == 0) {
							commands->left = true;
							commands->destination = "enemy_left";
						}
						
						break;
					case 2:
						commands->nextMoveDir = -1;
						if (commands->rightContacts.size() == 0) {
							commands->right = true;
							commands->destination = "enemy_right";
						}
						
						break;
					case 3:
						commands->nextMoveDir = -1;
						if (commands->upContacts.size() == 0) {
							commands->up = true;
							commands->destination = "enemy_up";
						}
						
						break;
					case 4:
						commands->nextMoveDir = -1;
						if (commands->downContacts.size() == 0) {
							commands->down = true;
							commands->destination = "enemy_bottom";
						}
						
						break;

					default:
						cout << "Random number generator failed - received " << randy << endl;
					}
				}

				// If the enemy doesn't move on this turn, instead signal it's intended direction.
				else
				{
					// 1. Determine available moves.


					randy = (rand() % 4) + 1;
					commands->nextMoveDir = randy;
				}

				// === 


				// ===  This block acts on the required flag.
				
				//First get current position
				glm::vec2 position = rigidbody->GetPosition();
				//For each direction, check if the movement flag is set and if you can move in that direction
				//First reset move flags before moving so future collisions update them correctly.
				//Then move the Rigidbody which will update the Transform as well
				//Velocity probably needs to be better scaled in-engine
				//Spaces between centers of tiles is 2.
				if (commands->left )
				{
					rigidbody->MoveToPosition(glm::vec2(position.x - 2, position.y), 0.5);
					transform->rotation = glm::vec4(0, 1, 0, 20.5f);
				}
				if (commands->right )
				{
					rigidbody->MoveToPosition(glm::vec2(position.x + 2, position.y), 0.5);
					transform->rotation = glm::vec4(0, -1, 0, 20.5f);
				}
				if (commands->up )
				{
					rigidbody->MoveToPosition(glm::vec2(position.x, position.y - 2), 0.5);
					transform->rotation = glm::vec4(0, 1, 0, 0);
				}
				if (commands->down )
				{
					rigidbody->MoveToPosition(glm::vec2(position.x, position.y + 2), 0.5);
					transform->rotation = glm::vec4(0, 1, 0, 9.5f);
					
				}
				*/


				// Reset input flags, needed here in case the movement didn't fire (moves would stack otherwise)
				commands->right = false;
				commands->up = false;
				commands->down = false;
				commands->left = false;

				commands->attackRight = false;
				commands->attackUp = false;
				commands->attackDown = false;
				commands->attackLeft = false;

			}
		}

		enemyTurn = false;
	}
	
}

void EnemySystem::configure(EventManager& em) {
	
	em.subscribe<Collision>(*this);
	em.subscribe<EndCollision>(*this);
	em.subscribe<EnemyDebugInput>(*this);
}

//Receive function just sets boolean flags to be picked up by update loop
void EnemySystem::receive(const EnemyDebugInput& event) {
	enemyTurn = true;
}

void EnemySystem::receive(const Collision& event)
{
	//Check for valid events
	if (event.a->valid() && event.b->valid())
	{
		//Only game objects should be involved in collisions, so should be safe to grab these components.
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();
		if (!objectA || !objectB) return; //Invalid collision, objects dont exist
		if (objectA->name == "enemy" || objectB->name == "enemy") //Single Enemy collision
		{
			//Grab enemy data
			Entity* enemy = objectA->name == "enemy" ? event.a : event.b;
			string enemyCollider = objectA->name == "enemy" ? event.fA : event.fB;
			ComponentHandle<CommandFlags> flags = enemy->component<CommandFlags>();
			//Grab other entity data
			Entity* other = objectA->name == "enemy" ? event.b : event.a;
			string otherName = objectA->name == "enemy" ? objectB->name : objectA->name;
			string otherType = objectA->name == "enemy" ? event.fB : event.fA;

			//Need to limit the enemy's movement here.
			//When can an enemy not move?
			//When the sensor detects a wall, player, or enemy.
			//AND when the sensor detects an enemy moving into an adjacent space.



			//If we hit a player, wall, or enemy and their body collider with our sensor...
			if ((otherName == "wall" || otherName == "player" || otherName == "enemy") 
				&& (otherType == "body" || otherType == "enemy_body")
				&& enemyCollider != "enemy_body")
			{
				//Set movement flags
				if (enemyCollider == "enemy_left")
				{
					cout << "ENEMY SYSTEM: cannot move left" << endl;
					flags->canMoveLeft = false;
					flags->leftContacts.push(other);
				}
				if (enemyCollider == "enemy_right")
				{
					cout << "ENEMY SYSTEM: cannot move right" << endl;
					flags->canMoveRight = false;
					flags->rightContacts.push(other);
				}
				if (enemyCollider == "enemy_top")
				{
					cout << "ENEMY SYSTEM: cannot move up" << endl;
					flags->canMoveUp = false;
					flags->upContacts.push(other);
				}
				if (enemyCollider == "enemy_bottom") {
					cout << "ENEMY SYSTEM: cannot move down" << endl;
					flags->canMoveDown = false;
					flags->downContacts.push(other);
				}
			}
			if (otherName == "player" && otherType == "body" && enemyCollider != "body")
			{
				flags->playerEntity = other;
				if (otherType == "left") flags->attackLeft = true;
				else if (otherType == "right") flags->attackRight = true;
				else if (otherType == "top") flags->attackUp = true;
				else if (otherType == "bottom") flags->attackDown = true;
			}
		}
	}
}

void EnemySystem::receive(const EndCollision& event)
{
	//Only game objects should be involved in collisions, so should be safe to grab these components.
	if (event.a->valid() && event.b->valid())
	{
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();
		if (!objectA || !objectB) return; //Invalid collision, objects dont exist
		if (objectA->name == "enemy" || objectB->name == "enemy") //Enemy collision
		{
			//Enemy data
			Entity* enemy = objectA->name == "enemy" ? event.a : event.b;
			string enemyCollider = objectA->name == "enemy" ? event.fA : event.fB;
			ComponentHandle<CommandFlags> flags = enemy->component<CommandFlags>();

			//Other entity data
			Entity* other = objectA->name == "enemy" ? event.b : event.a;
			string otherName = objectA->name == "enemy" ? objectB->name : objectA->name;
			string otherType = objectA->name == "enemy" ? event.fB : event.fA;

			//If we've ended collision with a body.
			if (otherType == "body" || otherType == "enemy_body")
			{
				//Unset the directional data if one of our sensors ended the collision
				if (enemyCollider == "enemy_left" && flags->leftContacts.size() > 0) { flags->leftContacts.pop(); cout << "ENEMY SYSTEM: can move left" << endl; }
				else if (enemyCollider == "enemy_right" && flags->rightContacts.size() > 0) { flags->rightContacts.pop(); cout << "ENEMY SYSTEM: can move right" << endl;
				}
				else if (enemyCollider == "enemy_top" && flags->upContacts.size() > 0) { flags->upContacts.pop(); cout << "ENEMY SYSTEM: can move up" << endl;
				}
				else if (enemyCollider == "enemy_bottom" && flags->downContacts.size() > 0) { flags->downContacts.pop(); cout << "ENEMY SYSTEM: can move down" << endl;
				}
				if (otherName == "player") flags->playerEntity = nullptr;
			}
		}
	}
}



#include "EnemySystem.h"

void EnemySystem::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	ComponentHandle<Rigidbody> rigidbody;

	if (enemyTurn)
	
	{
		++currentTurnCounter;
		cout << "Resolving enemy turn " << currentTurnCounter << endl;

		/* initialize random seed: */
		srand(time(NULL));
		int randy;

		for (Entity entity : es.entities_with_components(rigidbody))
		{

			ComponentHandle<Rigidbody> rigidbody = entity.component<Rigidbody>();
			ComponentHandle<Transform> transform = entity.component<Transform>();
			ComponentHandle<GameObject> object = entity.component<GameObject>();
			ComponentHandle<CommandFlags> commands = entity.component<CommandFlags>();
			
			// Iterate over enemies.
			if (object && object->name == "enemy")
			{
				// === THIS Block checks the enemy's current state, and sets according flags.

				// If the enemy moves on this turn, perform the action in the saved direction.
				if (currentTurnCounter % 2 == 0)
				{
					switch (commands->nextMoveDir)
					{
					case 1:
						commands->canMoveLeft = true;
						commands->left = true;
						break;
					case 2:
						commands->canMoveUp = true;
						commands->up = true;
						break;
					case 3:
						commands->canMoveRight = true;
						commands->right = true;
						break;
					case 4:
						commands->canMoveDown = true;
						commands->down = true;
						break;

					default:
						cout << "Random number generator failed - received " << randy << endl;
					}
				}

				// If the enemy doesn't move on this turn, instead signal it's intended direction.
				else
				{
					randy = (rand() % 4) + 1;
					commands->nextMoveDir = randy;

					switch (commands->nextMoveDir)
					{
					// LEFT
					case 1:
						transform->rotation = glm::vec4(0, 1, 0, 20.5f);
						break;

					// UP
					case 2:
						transform->rotation = glm::vec4(0, 1, 0, 0);
						break;

					// RIGHT
					case 3:
						transform->rotation = glm::vec4(0, -1, 0, 20.5f);
						break;

					// DOWN
					case 4:
						transform->rotation = glm::vec4(0, 1, 0, 9.5f);
						break;

					default:
						cout << "Random number generator failed - received " << randy << endl;
					}
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
				if (commands->left && commands->canMoveLeft)
				{
					ResetCollisionEntities();
					rigidbody->MoveToPosition(glm::vec2(position.x - 2, position.y), 0.5);
					transform->rotation = glm::vec4(0, 1, 0, 20.5f);
				}
				if (commands->right && commands->canMoveRight)
				{
					ResetCollisionEntities();
					rigidbody->MoveToPosition(glm::vec2(position.x + 2, position.y), 0.5);
					transform->rotation = glm::vec4(0, -1, 0, 20.5f);
				}
				if (commands->up && commands->canMoveUp)
				{
					ResetCollisionEntities();
					rigidbody->MoveToPosition(glm::vec2(position.x, position.y - 2), 0.5);
					transform->rotation = glm::vec4(0, 1, 0, 0);
				}
				if (commands->down && commands->canMoveDown)
				{
					ResetCollisionEntities();
					rigidbody->MoveToPosition(glm::vec2(position.x, position.y + 2), 0.5);
					transform->rotation = glm::vec4(0, 1, 0, 9.5f);
					
				}

				if (commands->attackLeft && leftEntity)
				{
					ComponentHandle<GameObject> targetObj = leftEntity->component<GameObject>();
					cout << "attacking " << targetObj->name << " to the left" << endl;

					ComponentHandle<Rigidbody> targetRb = leftEntity->component<Rigidbody>();
					targetRb->DeleteBody();
					leftEntity->destroy();
					commands->canMoveLeft = true;
					this->leftEntity = nullptr;
				}
				if (commands->attackRight && rightEntity)
				{
					ComponentHandle<GameObject> targetObj = rightEntity->component<GameObject>();
					cout << "attacking " << targetObj->name << " to the right" << endl;

					ComponentHandle<Rigidbody> targetRb = rightEntity->component<Rigidbody>();
					targetRb->DeleteBody();
					rightEntity->destroy();
					commands->canMoveRight = true;
					this->rightEntity = nullptr;
				}
				if (commands->attackUp && upEntity)
				{
					ComponentHandle<GameObject> targetObj = upEntity->component<GameObject>();
					cout << "attacking " << targetObj->name << " to the top" << endl;

					ComponentHandle<Rigidbody> targetRb = upEntity->component<Rigidbody>();
					targetRb->DeleteBody();
					upEntity->destroy();
					commands->canMoveUp = true;
					this->upEntity = nullptr;
				}
				if (commands->attackDown && downEntity)
				{
					ComponentHandle<GameObject> targetObj = downEntity->component<GameObject>();
					cout << "attacking " << targetObj->name << " to the bottom" << endl;

					ComponentHandle<Rigidbody> targetRb = downEntity->component<Rigidbody>();
					targetRb->DeleteBody();
					downEntity->destroy();
					commands->canMoveDown = true;
					this->downEntity = nullptr;
				}


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

//Convenience function for resetting detected entities
void EnemySystem::ResetCollisionEntities()
{
	this->leftEntity = nullptr;
	this->rightEntity = nullptr;
	this->upEntity = nullptr;
	this->downEntity = nullptr;
}

void EnemySystem::configure(EventManager& em) {
	
	em.subscribe<Collision>(*this);
	em.subscribe<EnemyDebugInput>(*this);
	//em.subscribe<AttackInput>(*this);
}

//Receive function just sets boolean flags to be picked up by update loop
void EnemySystem::receive(const EnemyDebugInput& event) {
	// This event will be replaced with the "OnPlayerAction" Event.
	// It will be responsible for

	//	1. Get the next command from the AI component.
	//	2. Determine if the command should be a move or attack flag
	// 
	enemyTurn = true;

	//cout << "Enemy Input Fired" << endl;
	//EnemyDebugInput::InpDir dir = event.dir; // This will be the "Command"

	//switch (dir) {
	//	case EnemyDebugInput::UP:
	//		up = true;
	//		break;
	//	case EnemyDebugInput::LEFT:
	//		left = true;
	//		break;
	//	case EnemyDebugInput::DOWN:
	//		down = true;
	//		break;
	//	case EnemyDebugInput::RIGHT:
	//		right = true;
	//		break;
	//}
}

void EnemySystem::receive(const Collision& event)
{
	cout << "collision event received for " << event.fA << " and " << event.fB << endl;

	//Direct body collision
	//if (event.fA == "body" && event.fB == "body")
	//{
	//	//So far we only have treasure entities so we can delete that
	//	ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
	//	ComponentHandle<GameObject> objectB = event.b->component<GameObject>();
	//	if (objectA->name == "treasure") event.a->destroy();
	//	else if (objectB->name == "treasure") event.b->destroy();
	//}
	
	//One collider was a named sensor

	/*if (event.fA == "left" || event.fB == "left")
	{
		this->leftEntity = event.fA == "left" ? event.b : event.a;
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();
		if (objectA->name == "player") this->canMoveLeft = false;
		else if (objectB->name == "player") this->canMoveLeft = false;
	}
	else if (event.fA == "right" || event.fB == "right")
	{
		this->rightEntity = event.fA == "right" ? event.b : event.a;
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();
		if (objectA->name == "player") this->canMoveRight = false;
		else if (objectB->name == "player") this->canMoveRight = false;
	}
	else if (event.fA == "top" || event.fB == "top")
	{
		this->upEntity = event.fA == "top" ? event.b : event.a;
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();
		if (objectA->name == "player") this->canMoveUp = false;
		else if (objectB->name == "player") this->canMoveUp = false;
	}
	else if (event.fA == "bottom" || event.fB == "bottom")
	{
		this->downEntity = event.fA == "bottom" ? event.b : event.a;
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();
		if (objectA->name == "player") this->canMoveDown = false;
		else if (objectB->name == "player") this->canMoveDown = false;
	}*/
}



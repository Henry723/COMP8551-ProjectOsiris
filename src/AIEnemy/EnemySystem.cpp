#include "EnemySystem.h"

void EnemySystem::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	ComponentHandle<Rigidbody> rigidbody;

	if (enemyTurn)
	
	{
		++currentTurnCounter;
		//cout << "Resolving enemy turn " << currentTurnCounter << endl;

		/* initialize random seed: */
		srand(time(NULL));
		int randy;

		for (Entity entity : es.entities_with_components(rigidbody))
		{
			ComponentHandle<GameObject> object = entity.component<GameObject>();
			
			// Iterate over enemies.
			if (object && object->name == "enemy")
			{

				ComponentHandle<Rigidbody> rigidbody = entity.component<Rigidbody>();
				ComponentHandle<Transform> transform = entity.component<Transform>();
				ComponentHandle<CommandFlags> commands = entity.component<CommandFlags>();

				// === THIS Block checks the enemy's current state, and sets according flags.

				// If the enemy moves on this turn, perform the action in the saved direction.
				if (currentTurnCounter % 2 == 0)
				{
					switch (commands->nextMoveDir)
					{
					case 1:
						commands->nextMoveDir = -1;
						if (commands->canMoveLeft) {
							commands->left = true;
						}
						else {
							commands->attackLeft = true;
						}
						
						break;
					case 2:
						commands->nextMoveDir = -1;
						if (commands->canMoveUp) {
							commands->up = true;
						}
						else {
							commands->attackUp = true;
						}
						
						break;
					case 3:
						commands->nextMoveDir = -1;
						if (commands->canMoveRight) {
							commands->right = true;
						}
						else {
							commands->attackRight = true;
						}
						
						break;
					case 4:
						commands->nextMoveDir = -1;
						if (commands->canMoveDown) {
							commands->down = true;
						}
						else {
							commands->attackDown = true;
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

					
				}
				if (commands->attackRight && rightEntity)
				{
					ComponentHandle<GameObject> targetObj = rightEntity->component<GameObject>();
					cout << "attacking " << targetObj->name << " to the right" << endl;

					
				}
				if (commands->attackUp && upEntity)
				{
					ComponentHandle<GameObject> targetObj = upEntity->component<GameObject>();
					cout << "attacking " << targetObj->name << " to the top" << endl;

					
				}
				if (commands->attackDown && downEntity)
				{
					ComponentHandle<GameObject> targetObj = downEntity->component<GameObject>();
					cout << "attacking " << targetObj->name << " to the bottom" << endl;

					
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

void EnemySystem::ResetCommandFlags(CommandFlags* flags) {

}

//Convenience function for resetting detected entities
void EnemySystem::ResetCollisionEntities()
{
	/*flags->leftEntity = nullptr;
	flags->rightEntity = nullptr;
	flags->upEntity = nullptr;
	flags->downEntity = nullptr;*/

	this->leftEntity = nullptr;
	this->rightEntity = nullptr;
	this->upEntity = nullptr;
	this->downEntity = nullptr;
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
	
	if (event.a->valid() && event.b->valid()) {
		cout << "ENEMY - valid colllision detected " << endl;
		
		//Only game objects should be involved in collisions, so should be safe to grab these components.
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();

		if (event.fA == "body" || event.fB == "body" || event.fA == "enemy_body" || event.fB == "enemy_body")
		{
			//Colliding with a enemy sensor for left Object
			if (objectA->name == "enemy" && event.fA != "enemy_body")
			{
				//Sensor colliding with an player body
				if ( ( (objectB->name == "player" || objectB->name == "wall") && event.fB == "body") || (objectB->name == "enemy" && event.fB == "enemy_body"))
				{
					ComponentHandle<CommandFlags> flags = event.a->component<CommandFlags>();

					if (event.fA == "enemy_left")
					{
						flags->canMoveLeft = false;
						flags->leftEntity = event.b;
					}
					else if (event.fA == "enemy_right")
					{
						flags->canMoveRight = false;
						flags->rightEntity = event.b;
					}
					else if (event.fA == "enemy_top")
					{
						flags->canMoveUp = false;
						flags->upEntity = event.b;
					}
					else if (event.fA == "enemy_bottom")
					{
						flags->canMoveDown = false;
						flags->downEntity = event.b;
					}
				}
			}
			else if (objectB->name == "enemy" && event.fB != "enemy_body")
			{
				//Sensor colliding with an player body
				if ( ((objectA->name == "player" || objectA->name == "wall") && event.fA == "body" ) || (objectA->name == "enemy" && event.fA == "enemy_body") )
				{
					ComponentHandle<CommandFlags> flags = event.b->component<CommandFlags>();

					if (event.fB == "enemy_left")
					{
						flags->canMoveLeft = false;
						flags->leftEntity = event.a;
					}
					else if (event.fB == "enemy_right")
					{
						flags->canMoveRight = false;
						flags->rightEntity = event.a;
					}
					else if (event.fB == "enemy_top")
					{
						flags->canMoveUp = false;
						flags->upEntity = event.a;
					}
					else if (event.fB == "enemy_bottom")
					{
						flags->canMoveDown = false;
						flags->downEntity = event.a;
					}
				}
			}
		}
	}
	
}

void EnemySystem::receive(const EndCollision& event)
{
	//Only game objects should be involved in collisions, so should be safe to grab these components.
	if (event.a->valid())
	{
		ComponentHandle<GameObject> objectA = event.a->component<GameObject>();
		if (objectA->name == "enemy" && event.fA != "enemy_body")
		{
			ComponentHandle<CommandFlags> flags = event.a->component<CommandFlags>();
			if (event.fA == "enemy_left")
			{
				flags->canMoveLeft = true;
				flags->leftEntity = nullptr;
			}
			else if (event.fA == "enemy_right")
			{
				flags->canMoveRight = true;
				flags->rightEntity = nullptr;
			}
			else if (event.fA == "enemy_top")
			{
				flags->canMoveUp = true;
				flags->upEntity = nullptr;
			}
			else if (event.fA == "enemy_bottom")
			{
				flags->canMoveDown = true;
				flags->downEntity = nullptr;
			}
		}
	}
	if (event.b->valid())
	{
		ComponentHandle<GameObject> objectB = event.b->component<GameObject>();
		if (objectB->name == "enemy" && event.fB != "enemy_body")
		{

			ComponentHandle<CommandFlags> flags = event.b->component<CommandFlags>();
			if (event.fB == "enemy_left")
			{
				flags->canMoveLeft = true;
				flags->leftEntity = nullptr;
			}
			else if (event.fB == "enemy_right")
			{
				flags->canMoveRight = true;
				flags->rightEntity = nullptr;
			}
			else if (event.fB == "enemy_top")
			{
				flags->canMoveUp = true;
				flags->upEntity = nullptr;
			}
			else if (event.fB == "enemy_bottom")
			{
				flags->canMoveDown = true;
				flags->downEntity = nullptr;
			}
		}
	}
}



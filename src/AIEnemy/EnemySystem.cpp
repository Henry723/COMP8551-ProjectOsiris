#include "EnemySystem.h"

void EnemySystem::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	if (gameState != RUNNING) return; //Make sure game is running before applying update.
	if (enemyTurn) //Check if enemy turn is active, based on TurnEvents
	{
		ComponentHandle<GameObject> gameObject; //Game object component handle
		for (Entity entity : es.entities_with_components(gameObject)) //Iterate over game objects
		{
			ComponentHandle<GameObject> object = entity.component<GameObject>(); //Get the component
			if (object && object->name == "enemy") //If we've found an enemy...
			{
				//Get the rigidbody, transform, and commands from the entity components.
				ComponentHandle<Rigidbody> rigidbody = entity.component<Rigidbody>();
				ComponentHandle<Transform> transform = entity.component<Transform>();
				ComponentHandle<CommandFlags> commands = entity.component<CommandFlags>();
				//cout << "enemy start" << endl;
				if (commands->isMoving && !rigidbody->moveBody)
				{
					commands->isMoving = false;
					movingEnemies--;
					
				}

				//If the playerEntity is set, then it's time to attack.
				if (commands->playerEntity)
				{
					if (commands->attackLeft) //Player to the left.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == 1 && transform->rotation.w == 20.5)
							cout << "ENEMY SYSTEM: attacking left enemy" << endl;
						//If not, rotate towards them
						else
							transform->rotation = glm::vec4(0, 1, 0, 20.5f);
					}
					else if (commands->attackUp) //Player above.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == 1 && transform->rotation.w == 0)
							cout << "ENEMY SYSTEM: attacking up enemy" << endl;
						//If not, rotate towards them
						else
							transform->rotation = glm::vec4(0, 1, 0, 0);
					}
					else if (commands->attackRight) //Player to the right.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == -1 && transform->rotation.w == 20.5)
							cout << "ENEMY SYSTEM: attacking right enemy" << endl;
						//If not, rotate towards them
						else
							transform->rotation = glm::vec4(0, -1, 0, 20.5f);
					}
					else if (commands->attackDown) //Player below.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == 1 && transform->rotation.w == 9.5)
							cout << "ENEMY SYSTEM: attacking down enemy" << endl;
						//If not, rotate towards them
						else
							transform->rotation = glm::vec4(0, 1, 0, 9.5f);
					}
				}
				else if(!commands->isMoving && !commands->moveComplete)
				{
					
					vector<CommandFlags::EnemyCommand> possibleMoves;
					if (!rigidbody->IsCollidingWithBody("enemy_left")) {
						possibleMoves.push_back(CommandFlags::MOVE_LEFT);
						cout << "do a move left" << endl;
					}
					else if (commands->nextCommand == CommandFlags::MOVE_LEFT)
					{
						commands->nextCommand = CommandFlags::WAITING;
					}

					if (!rigidbody->IsCollidingWithBody("enemy_top"))
					{
						possibleMoves.push_back(CommandFlags::MOVE_UP);
						cout << "do a move up" << endl;
					}
					else if (commands->nextCommand == CommandFlags::MOVE_UP)
					{
						commands->nextCommand = CommandFlags::WAITING;
					}
					
					if (!rigidbody->IsCollidingWithBody("enemy_right"))
					{ 
						possibleMoves.push_back(CommandFlags::MOVE_RIGHT);
						cout << "do a move right" << endl;
					}
					else if (commands->nextCommand == CommandFlags::MOVE_RIGHT)
					{
						commands->nextCommand = CommandFlags::WAITING;
					}

					if (!rigidbody->IsCollidingWithBody("enemy_bottom")) 
					{ 
						possibleMoves.push_back(CommandFlags::MOVE_DOWN);
						cout << "do a move down" << endl;
					}
					else if (commands->nextCommand == CommandFlags::MOVE_DOWN)
					{
						commands->nextCommand = CommandFlags::WAITING;
					}


					glm::vec2 position = rigidbody->GetPosition();
					CommandFlags::EnemyCommand nextMove = commands->nextCommand;
					if (nextMove == CommandFlags::WAITING)
					{
						nextMove = possibleMoves[rand() % possibleMoves.size()];
						commands->nextCommand = nextMove;
					}
					switch (nextMove)
					{
					case CommandFlags::MOVE_LEFT:
						if (transform->rotation.y == 1 && transform->rotation.w == 20.5)
						{
							rigidbody->MoveToPosition(glm::vec2(position.x - 2, position.y), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true;
							events.emit<EnemyMoveStart>();
							movingEnemies++;
							commands->moveComplete = true;

						}
						else
						{
							transform->rotation = glm::vec4(0, 1, 0, 20.5f);
							commands->moveComplete = true;
						}
						break;
					case CommandFlags::MOVE_UP:
						if (transform->rotation.y == 1 && transform->rotation.w == 0)
						{
							rigidbody->MoveToPosition(glm::vec2(position.x, position.y - 2), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true;
							events.emit<EnemyMoveStart>();
							movingEnemies++;
							commands->moveComplete = true;
						}
						else
						{
							transform->rotation = glm::vec4(0, 1, 0, 0);
							commands->moveComplete = true;
						}
						break;
					case CommandFlags::MOVE_RIGHT:
						if (transform->rotation.y == -1 && transform->rotation.w == 20.5)
						{
							rigidbody->MoveToPosition(glm::vec2(position.x + 2, position.y), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true;
							events.emit<EnemyMoveStart>();
							movingEnemies++;
							commands->moveComplete = true;
							commands->moveComplete = true;

						}
						else
						{
							transform->rotation = glm::vec4(0, -1, 0, 20.5f);
						}
						break;
					case CommandFlags::MOVE_DOWN:
						if (transform->rotation.y == 1 && transform->rotation.w == 9.5)
						{
							rigidbody->MoveToPosition(glm::vec2(position.x, position.y + 2), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true;
							events.emit<EnemyMoveStart>();
							movingEnemies++;
							commands->moveComplete = true;

						}
						else
						{
							transform->rotation = glm::vec4(0, 1, 0, 9.5f);
							commands->moveComplete = true;
						}
						break;
					}

				}


				// Reset input flags, needed here in case the movement didn't fire (moves would stack otherwise)
				commands->right = false;
				commands->up = false;
				commands->down = false;
				commands->left = false;


			}
		}
		if (movingEnemies == 0)
		{
			ComponentHandle<GameObject> gameObject; //Game object component handle
			for (Entity entity : es.entities_with_components(gameObject)) //Iterate over game objects
			{
				ComponentHandle<GameObject> object = entity.component<GameObject>(); //Get the component
				if (object && object->name == "enemy") //If we've found an enemy...
				{
					ComponentHandle<CommandFlags> commands = entity.component<CommandFlags>();
					commands->moveComplete = false;
				}
			}
			cout << "end enemy turn" << endl;
			enemyTurn = false; //Reset enemy turn flag.
			events.emit<EnemyTurnEnd>();
		}
		
	}
}

void EnemySystem::configure(EventManager& em) {

	em.subscribe<Collision>(*this);
	em.subscribe<EndCollision>(*this);
	em.subscribe<PlayerTurnEnd>(*this);

}

//Receive function just sets boolean flags to be picked up by update loop
void EnemySystem::receive(const PlayerTurnEnd& event) {
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
					flags->canMoveLeft = false;
					flags->leftContacts.push(other);
				}
				if (enemyCollider == "enemy_right")
				{
					flags->canMoveRight = false;
					flags->rightContacts.push(other);
				}
				if (enemyCollider == "enemy_top")
				{
					flags->canMoveUp = false;
					flags->upContacts.push(other);
				}
				if (enemyCollider == "enemy_bottom") {
					flags->canMoveDown = false;
					flags->downContacts.push(other);
				}
			}
			if (otherName == "player" && otherType == "body" && enemyCollider != "body")
			{
				flags->playerEntity = other;
				if (enemyCollider == "enemy_left") { flags->attackLeft = true; }
				else if (enemyCollider == "enemy_right") { flags->attackRight = true; }
				else if (enemyCollider == "enemy_top") { flags->attackUp = true; }
				else if (enemyCollider == "enemy_bottom") { flags->attackDown = true; }
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
				if (enemyCollider == "enemy_left" && flags->leftContacts.size() > 0) { flags->leftContacts.pop(); }
				else if (enemyCollider == "enemy_right" && flags->rightContacts.size() > 0) {
					flags->rightContacts.pop(); 
				}
				else if (enemyCollider == "enemy_top" && flags->upContacts.size() > 0) {
					flags->upContacts.pop(); 
				}
				else if (enemyCollider == "enemy_bottom" && flags->downContacts.size() > 0) {
					flags->downContacts.pop(); 
				}
				if (otherName == "player")
				{
					flags->playerEntity = nullptr;
					flags->attackDown = false;
					flags->attackUp = false;
					flags->attackRight = false;
					flags->attackLeft = false;
				}
			}
		}
	}
}



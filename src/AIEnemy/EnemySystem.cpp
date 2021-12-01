#include "EnemySystem.h"

void EnemySystem::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	if (gameState != RUNNING) return; //Make sure game is running before applying update.
	if (enemyTurn) //Check if enemy turn is active, based on TurnEvents
	{
		int enemyTurns = 0;
		ComponentHandle<GameObject> gameObject; //Game object component handle
		for (Entity entity : es.entities_with_components(gameObject)) //Iterate over game objects
		{
			ComponentHandle<GameObject> object = entity.component<GameObject>(); //Get the component
			if (object && object->name == "enemy") //If we've found an enemy...
			{
				enemyTurns++;
				//Get the rigidbody, transform, and commands from the entity components.
				ComponentHandle<Rigidbody> rigidbody = entity.component<Rigidbody>();
				ComponentHandle<Transform> transform = entity.component<Transform>();
				ComponentHandle<CommandFlags> commands = entity.component<CommandFlags>();
				if (commands->isMoving && !rigidbody->moveBody)
				{
					commands->isMoving = false;
					movingEnemies--;
				}

				CheckForPlayer(rigidbody.get(), commands.get());
				//If the playerEntity is set, then it's time to attack.
				if (commands->playerEntity && !commands->moveComplete)
				{
					cout << enemyTurns << "found player" << endl;
					
					commands->moveComplete = true;
					if (commands->attackLeft) //Player to the left.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == 1 && transform->rotation.w == 20.5)
						{
							events.emit<EnemyAttack>();
						}
						//If not, rotate towards them
						else{
							cout << enemyTurns << "attaack rotate" << endl;
							transform->rotation = glm::vec4(0, 1, 0, 20.5f);
					}
					}
					else if (commands->attackUp) //Player above.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == 1 && transform->rotation.w == 0)
						{
							events.emit<EnemyAttack>();
						}
						//If not, rotate towards them
						else{
							cout << enemyTurns << "attaack rotate" << endl;
							transform->rotation = glm::vec4(0, 1, 0, 0);
					}
					}
					else if (commands->attackRight) //Player to the right.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == -1 && transform->rotation.w == 20.5)
						{
							events.emit<EnemyAttack>();
						}
						//If not, rotate towards them
						else{
							cout << enemyTurns << "attaack rotate" << endl;
							transform->rotation = glm::vec4(0, -1, 0, 20.5f);
					}
					}
					else if (commands->attackDown) //Player below.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == 1 && transform->rotation.w == 9.5)
						{
							events.emit<EnemyAttack>();
						}
						//If not, rotate towards them
						else {
							cout << enemyTurns << "attaack rotate" << endl;
							transform->rotation = glm::vec4(0, 1, 0, 9.5f);
						}
					}
				}
				else if (!commands->isMoving && !commands->moveComplete)
				{
					vector<CommandFlags::EnemyCommand> possibleMoves = AvailableMoves(rigidbody.get(), commands.get());


					glm::vec2 position = rigidbody->GetPosition();
					CommandFlags::EnemyCommand nextMove = commands->nextCommand;
					if (nextMove == CommandFlags::WAITING && possibleMoves.size() > 0)
					{
						nextMove = possibleMoves[rand() % possibleMoves.size()];
						commands->nextCommand = nextMove;
					}
					switch (nextMove)
					{
					case CommandFlags::MOVE_LEFT:
						if (transform->rotation.y == 1 && transform->rotation.w == 20.5)
						{
							commands->destination = glm::vec2(position.x - 4, position.y);
							rigidbody->MoveToPosition(glm::vec2(position.x - 2, position.y), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true;
							movingEnemies++;
							commands->moveComplete = true;

						}
						else
						{
							cout << "move rotate" << endl;
							transform->rotation = glm::vec4(0, 1, 0, 20.5f);
							commands->moveComplete = true;
							commands->destination = glm::vec2(position.x - 2, position.y);
						}
						break;
					case CommandFlags::MOVE_UP:
						if (transform->rotation.y == 1 && transform->rotation.w == 0)
						{
							commands->destination = glm::vec2(position.x, position.y - 4);
							rigidbody->MoveToPosition(glm::vec2(position.x, position.y - 2), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true;
							movingEnemies++;
							commands->moveComplete = true;
						}
						else
						{
							cout << "move rotate" << endl;
							transform->rotation = glm::vec4(0, 1, 0, 0);
							commands->moveComplete = true;
							commands->destination = glm::vec2(position.x, position.y - 2);
						}
						break;
					case CommandFlags::MOVE_RIGHT:
						if (transform->rotation.y == -1 && transform->rotation.w == 20.5)
						{
							commands->destination = glm::vec2(position.x + 4, position.y);
							rigidbody->MoveToPosition(glm::vec2(position.x + 2, position.y), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true;
							movingEnemies++;
							commands->moveComplete = true;

						}
						else
						{
							cout << "move rotate" << endl;
							transform->rotation = glm::vec4(0, -1, 0, 20.5f);
							commands->moveComplete = true;
							commands->destination = glm::vec2(position.x + 2, position.y);
						}
						break;
					case CommandFlags::MOVE_DOWN:
						if (transform->rotation.y == 1 && transform->rotation.w == 9.5)
						{

							commands->destination = glm::vec2(position.x, position.y + 4);
							rigidbody->MoveToPosition(glm::vec2(position.x, position.y + 2), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true;
							movingEnemies++;
							commands->moveComplete = true;

						}
						else
						{

							cout << "move rotate" << endl;
							transform->rotation = glm::vec4(0, 1, 0, 9.5f);
							commands->moveComplete = true;
							commands->destination = glm::vec2(position.x, position.y + 2);
						}
						break;
					}
				}
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
			enemyTurn = false; //Reset enemy turn flag.
			events.emit<EnemyTurnEnd>(); //Let other systems know the enemies are done.
		}
	}
}

//Function to check for adjacent player on update
void EnemySystem::CheckForPlayer(Rigidbody* rigidbody, CommandFlags* commands)
{
	commands->attackLeft = false;
	commands->attackDown = false;
	commands->attackRight = false;
	commands->attackUp = false;

	//Create vectors with the fixture names and corresponding command names.
	vector<string> ColliderNames = { "enemy_left", "enemy_right", "enemy_bottom", "enemy_top" };
	//Loop over the name vectors.
	for (int i = 0; i < ColliderNames.size(); i++)
	{
		//If the enemy's fixtures are colliding with other bodies
		vector<Entity> collisions = rigidbody->CollidingBodies(ColliderNames[i]);
		//For each colliding body...
		for (Entity e : collisions)
		{
			ComponentHandle<GameObject> otherObject = e.component<GameObject>();
			if (otherObject && otherObject->name == "player")
			{
				if (i == 0) commands->attackLeft = true;
				else if (i == 1) commands->attackRight = true;
				else if (i == 2) commands->attackDown = true;
				else commands->attackUp = true;
				commands->playerEntity = &e;
				return;
			}
		}
	}
	commands->playerEntity = nullptr;
}

//Function to determine a list of available directions the enemy can move.
vector<CommandFlags::EnemyCommand> EnemySystem::AvailableMoves(Rigidbody* rigidbody, CommandFlags* commands)
{
	//Create a vector to store potential moves.
	vector<CommandFlags::EnemyCommand> possibleMoves;

	//Create vectors with the fixture names and corresponding command names.
	vector<string> ColliderNames = { "enemy_left", "enemy_right", "enemy_bottom", "enemy_top" };
	vector<CommandFlags::EnemyCommand> CommandNames = { CommandFlags::MOVE_LEFT, CommandFlags::MOVE_RIGHT,
														CommandFlags::MOVE_DOWN, CommandFlags::MOVE_UP };
	//Loop over the name vectors.
	for (int i = 0; i < ColliderNames.size(); i++)
	{
		bool SensorSafe = true; //Assume the sensor check won't cause issues.
		//If the enemy's fixtures are colliding with other sensors
		vector<Entity> collisions = rigidbody->CollidingSensors(ColliderNames[i]);
		//For each colliding sensor...
		for (Entity e : collisions)
		{
			//Get the entity's command flags
			ComponentHandle<CommandFlags> otherFlags = e.component<CommandFlags>();
			if (!otherFlags) continue; //If the flags don't exist, continue.
			//If the flags are there, make sure the enemies don't share a destination.
			//If the sensors do share a destination, this direction is not safe.
			if (otherFlags->destination.x == commands->destination.x
				&& otherFlags->destination.y == commands->destination.y) SensorSafe = false;
		}

		//If the sensor is not colliding with a body or any unsafe sensors, push it to the vector.
		if (!rigidbody->IsCollidingWithBody(ColliderNames[i]) && SensorSafe)
			possibleMoves.push_back(CommandNames[i]);
		//If the direction was not deemed safe but is the next command for the enemy, set enemy to waiting.
		else if (commands->nextCommand == CommandNames[i])
			commands->nextCommand = CommandFlags::WAITING;
	}
	//Return list of potential moves.
	return possibleMoves;
}

/***********************************************
*****************EVENT HANDLERS*****************
************************************************/

//Configure function that sets event subscriptions.
void EnemySystem::configure(EventManager& em)
{
	em.subscribe<PlayerTurnEnd>(*this);
}

//Receiver for player's end of turn, set's enemy turn to true.
void EnemySystem::receive(const PlayerTurnEnd& event)
{
	enemyTurn = true;
}
#include "EnemySystem.h"

void EnemySystem::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	if (gameState != GameState::RUNNING) return; //Make sure game is running before applying update.
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

				//Check if the rigidbody stopped moving and command flag needs updating
				if (commands->isMoving && !rigidbody->moveBody) 
				{
					commands->isMoving = false; //Rigidbody stopped, update flag
					movingEnemies--; //Decrement moving enemies tracker
				}

				CheckForPlayer(rigidbody.get(), commands.get()); //Check for player collisions

				//If the playerEntity is set, then it's time to attack.
				if (commands->playerEntity && !commands->moveComplete)
				{
					commands->moveComplete = true; //Whether attacking or turning, counts as a move
					if (commands->attackLeft) //Player to the left.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == 1 && transform->rotation.w == 20.5)
							AttackPlayer(events, commands->playerEntity);
						//If not, rotate towards them
						else transform->rotation = glm::vec4(0, 1, 0, 20.5f);
					}
					else if (commands->attackUp) //Player above.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == 1 && transform->rotation.w == 0)
							AttackPlayer(events, commands->playerEntity);
						//If not, rotate towards them
						else transform->rotation = glm::vec4(0, 1, 0, 0);
					}
					else if (commands->attackRight) //Player to the right.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == -1 && transform->rotation.w == 20.5)
							AttackPlayer(events, commands->playerEntity);
						//If not, rotate towards them
						else transform->rotation = glm::vec4(0, -1, 0, 20.5f);
					}
					else if (commands->attackDown) //Player below.
					{
						//If we're facing the enemy, attack.
						if (transform->rotation.y == 1 && transform->rotation.w == 9.5)
							AttackPlayer(events, commands->playerEntity);
						//If not, rotate towards them
						else transform->rotation = glm::vec4(0, 1, 0, 9.5f);
					}
				}
				// No player, so check if we need to move or rotate.
				else if (!commands->isMoving && !commands->moveComplete)
				{
					//Get a list of possible moves for the enemy to make.
					vector<CommandFlags::EnemyCommand> possibleMoves = AvailableMoves(rigidbody.get(), commands.get());

					//Get position component to update.
					glm::vec2 position = rigidbody->GetPosition();

					//Next move for the enemy, defaults to WAITING
					CommandFlags::EnemyCommand nextMove = commands->nextCommand;
					if (nextMove == CommandFlags::WAITING && possibleMoves.size() > 0)
					{
						//Get a random next move and update command flags
						nextMove = possibleMoves[rand() % possibleMoves.size()];
						commands->nextCommand = nextMove;
					}

					switch (nextMove) //Switch statement for enemy's next move.
					{
					//Each case for movements is the same, just in regards to their own direction.
					case CommandFlags::MOVE_LEFT: //Left movement
						commands->moveComplete = true; //Moving or rotating, flag should be set
						//Make sure enemy is facing the right direction
						if (transform->rotation.y == 1 && transform->rotation.w == 20.5)
						{
							//Set destination to inform other enemies.
							//Set to one tile past total movement to account for continued movement in same direction.
							commands->destination = glm::vec2(position.x - 4, position.y);
							//Start enemy movement
							rigidbody->MoveToPosition(glm::vec2(position.x - 2, position.y), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true; //Set moving flag
							movingEnemies++; //Increment number of moving enemies
						}
						else //Enemy isn't facing the right direction, rotate it.
						{
							//Update transform
							transform->rotation = glm::vec4(0, 1, 0, 20.5f);
							//Set destination
							commands->destination = glm::vec2(position.x - 2, position.y);
						}
						break;
					case CommandFlags::MOVE_UP:
						commands->moveComplete = true;
						if (transform->rotation.y == 1 && transform->rotation.w == 0)
						{
							commands->destination = glm::vec2(position.x, position.y - 4);
							rigidbody->MoveToPosition(glm::vec2(position.x, position.y - 2), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true;
							movingEnemies++;
						}
						else
						{
							transform->rotation = glm::vec4(0, 1, 0, 0);
							commands->destination = glm::vec2(position.x, position.y - 2);
						}
						break;
					case CommandFlags::MOVE_RIGHT:
						commands->moveComplete = true;
						if (transform->rotation.y == -1 && transform->rotation.w == 20.5)
						{
							commands->destination = glm::vec2(position.x + 4, position.y);
							rigidbody->MoveToPosition(glm::vec2(position.x + 2, position.y), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true;
							movingEnemies++;
						}
						else
						{
							transform->rotation = glm::vec4(0, -1, 0, 20.5f);
							commands->destination = glm::vec2(position.x + 2, position.y);
						}
						break;
					case CommandFlags::MOVE_DOWN:
						commands->moveComplete = true;
						if (transform->rotation.y == 1 && transform->rotation.w == 9.5)
						{
							commands->destination = glm::vec2(position.x, position.y + 4);
							rigidbody->MoveToPosition(glm::vec2(position.x, position.y + 2), enemySpeed);
							commands->nextCommand = CommandFlags::EnemyCommand::WAITING;
							commands->isMoving = true;
							movingEnemies++;
						}
						else
						{
							transform->rotation = glm::vec4(0, 1, 0, 9.5f);
							commands->destination = glm::vec2(position.x, position.y + 2);
						}
						break;
					}
				}
			}
		}
		if (movingEnemies == 0) //No moving enemies and loop has exited, can end turn.
		{
			ComponentHandle<CommandFlags> commandFlags; //Command flag component handle
			for (Entity entity : es.entities_with_components(commandFlags)) //Iterate over flags
				commandFlags->moveComplete = false; //Reset all move flags
			enemyTurn = false; //Reset enemy turn flag.
			events.emit<EnemyTurnEnd>(); //Let other systems know the enemies are done.
		}
	}
}

//Function to check for adjacent player on update
void EnemySystem::CheckForPlayer(Rigidbody* rigidbody, CommandFlags* commands)
{
	//Reset flags since each turn should check independently in case player moved.
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
		vector<Entity*> collisions = rigidbody->CollidingBodies(ColliderNames[i]);
		//For each colliding body...
		for (Entity* e : collisions)
		{
			ComponentHandle<GameObject> otherObject = e->component<GameObject>();
			if (otherObject && otherObject->name == "player") //Check for a player collision
			{
				//Depending on what index we're at, set attack flag
				if (i == 0) commands->attackLeft = true;
				else if (i == 1) commands->attackRight = true;
				else if (i == 2) commands->attackDown = true;
				else commands->attackUp = true;
				commands->playerEntity = e; //Set player entity flag
				return; //Only one player, safe to return.
			}
		}
	}
	commands->playerEntity = nullptr; //No player found, set pointer to null in case it was set on a previous turn.
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

void EnemySystem::AttackPlayer(EventManager& events, Entity* player)
{
	//Get the player's health component
	ComponentHandle<Health> playerHealth = player->component<Health>();
	//Decrement and check for 0, Game Over event thrown here.
	if (--playerHealth->curHealth <= 0) 
		events.emit<GameOver>();
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
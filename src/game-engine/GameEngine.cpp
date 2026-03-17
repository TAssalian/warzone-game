#include "GameEngine.h"
#include "CommandProcessing.h"

#include <iostream>
#include <sstream>
#include <vector>
#include "../map/Map.h"
#include "../orders/Orders.h"
#include "../player/Player.h"
#include "../cards/Cards.h"
#include <random>
#include <algorithm>

using std::cout;
using std::endl;
using std::string;

namespace {

string extractCommandName(const string &input) {
  std::istringstream stream(input);
  string token;
  stream >> token;
  return token;
}

std::vector<string> extractCommandTokens(const string &input) {
  std::istringstream stream(input);
  std::vector<string> tokens;
  string token;

  while (stream >> token) {
    tokens.push_back(token);
  }

  return tokens;
}

bool isCommand(const string &input, const string &name) {
  return extractCommandName(input) == name;
}

bool hasRequiredParameter(const string &input) {
  return extractCommandTokens(input).size() >= 2;
}

} // namespace

// mapping of gameStates
GameState stringToGameState(const std::string &str) {
  if (isCommand(str, "loadmap"))
    return MAP_LOADED;
  if (isCommand(str, "validatemap"))
    return MAP_VALIDATED;
  if (isCommand(str, "addplayer"))
    return PLAYERS_ADDED;
  if (isCommand(str, "gamestart"))
    return ASSIGN_REINFORCEMENT;
  if (isCommand(str, "issueorders"))
    return ISSUE_ORDERS;
  if (isCommand(str, "endissueorders"))
    return EXECUTE_ORDERS;
  if (isCommand(str, "execorder"))
    return EXECUTE_ORDERS;
  if (isCommand(str, "win"))
    return WIN;
  if (isCommand(str, "replay"))
    return START;
  if (isCommand(str, "quit"))
    return END;

  return START;
}

// default
GameEngine::GameEngine() {
  currentState = new GameState(GameState::START);
  command = new string("");
  deck = new Deck();
}
// copy
GameEngine::GameEngine(const GameEngine &other) {
  this->currentState = new GameState(*other.currentState);
  this->command = new string(*other.command);
}
// overload =
GameEngine &GameEngine::operator=(const GameEngine &other) {
  if (this != &other) {
    delete this->currentState;
    delete this->command;

    currentState = new GameState(*other.currentState);
    this->command = new string(*other.command);
  }
  return *this;
}
// destructor
GameEngine::~GameEngine() {
  for (int i = 0; i < getPlayerCount(); i++) {
    delete players[i];
  }
  delete [] players;
  delete currentState;
  delete command;
  delete mapLoader;
  delete deck;
}

int GameEngine::getPlayerCount() const {
    int count = 0;
    for (Player* i : players) {
        if (i == nullptr) {
            break;
        }
        else {
            count++;
        }
    }
    return count;
}

Player** GameEngine::getPlayers()
{
    return players;
}

MapLoader* GameEngine::getMapLoader()
{
    return mapLoader;
}

void GameEngine::addPlayer(std::string name)
{
    int playerCount = getPlayerCount();
    if (playerCount < 6) {
        players[playerCount] = new Player(name, deck);
    }
}

// getter state number
GameState GameEngine::getCurrentState() const { return *currentState; }
// getter state name
string GameEngine::getCurrentStateName() const {
  return GameStateNames[(*currentState)];
}
// setter to update current state

void GameEngine::setCurrentState(string nextState) {
  delete currentState;
  currentState = new GameState(stringToGameState(nextState));
}

// getter for command
string GameEngine::getCommand() const { return *command; }

// setter for command
void GameEngine::setCommand(string nextCommand) {
  delete command;
  command = new string(nextCommand);
}

// validate command for current state
bool GameEngine::validateCommand(string &command) {
  GameState state = *currentState;

  switch (state) {
  case START:
    return isCommand(command, "loadmap") && hasRequiredParameter(command);
  case MAP_LOADED:
    return (isCommand(command, "loadmap") && hasRequiredParameter(command)) ||
           isCommand(command, "validatemap");
  case MAP_VALIDATED:
    return isCommand(command, "addplayer") && hasRequiredParameter(command);
  case PLAYERS_ADDED: {
      // Ensure the user adds at least two players
      if (getPlayerCount() < 2) {
          return (isCommand(command, "addplayer") && hasRequiredParameter(command));
      }

      // Ensure the player can't add more than 6 players, at this point they can only start the game
      if (getPlayerCount() == 6) {
          return isCommand(command, "gamestart");
      }

      // Where the player count is at least two but less than or equal to 6
      return (isCommand(command, "addplayer") && hasRequiredParameter(command)) ||
          isCommand(command, "gamestart");
  }
  case ASSIGN_REINFORCEMENT:
    return isCommand(command, "issueorders");
  case ISSUE_ORDERS:
    return true;
    // return isCommand(command, "issueorder") || isCommand(command, "endissueorders");
  case EXECUTE_ORDERS:
    return isCommand(command, "execorder") || isCommand(command, "win");
  case WIN:
    return isCommand(command, "replay") || isCommand(command, "quit");
  default:
    return false;
  }
}

// next valid command for current state (with numbers)
string GameEngine::getNextValidCommand() const {
  GameState state = *currentState;

  switch (state) {
  case START:
    return "Start -> loadmap <mapfile>";
  case MAP_LOADED:
    return "Map Loaded -> loadmap <mapfile> | validatemap";
  case MAP_VALIDATED:
    return "Map Validated -> addplayer <playername>";
  case PLAYERS_ADDED: {
      if (getPlayerCount() < 2) {
        return "Players Added -> addplayer <playername>";
      }

      if (getPlayerCount() == 6) {
          return "Players Added -> gamestart";
      }

      return "Players Added -> addplayer <playername> | gamestart";
  }
  case ASSIGN_REINFORCEMENT:
    return "See the instructions below";
  case ISSUE_ORDERS:
    return "Issue Orders -> issueorder | endissueorders";
  case EXECUTE_ORDERS:
    return "Execute Orders -> execorder | win";
  case WIN:
    return "Win -> replay | quit";
  default:
    return "";
  }
}

/**
 * Validates the command and, if valid, applies the state transition.
 * Returns true on success, false if the command was invalid.
 */
bool GameEngine::transition(const string &cmd) {
  string mutableCmd = cmd;
  if (!validateCommand(mutableCmd)) {
    return false;
  }
  setCurrentState(mutableCmd);
  return true;
}

bool GameEngine::transition(Command *cmd) {
  if (cmd == nullptr) {
    return false;
  }

  string text = cmd->getCommand();
  string mutableCmd = text;
  if (!validateCommand(mutableCmd)) {
    if (cmd->getEffect().empty()) {
      cmd->saveEffect("ERROR: \"" + text + "\" is not a valid command in state " +
                      getCurrentStateName() + ".");
    }
    return false;
  }

  const string previousState = getCurrentStateName();
  const string commandName = extractCommandName(text);

  if (commandName == "gamestart") {

      Map* map = mapLoader->map;
      std::vector<Territory*> territories(*map->territories);
      
      int territoryCount = map->territories->size();
      int playerCount = getPlayerCount();

      if (playerCount <= 0) return false;

      // --- Step 1: Determine randomly the order of player for players in the game. ---
      // We can do this by shuffling the player array and whoever's in position 0 will go first
      std::random_device rd;
      std::mt19937 g(rd());
      std::shuffle(std::begin(players), std::begin(players) + playerCount, g);

      // shuffle territories too
      std::shuffle(std::begin(territories), std::end(territories), g);


      // --- Step 2: Distribute territories evenly to players. ---
      // Distribute them to each player one at a time
      for (int i = 0; i < territories.size(); ++i) {
          int playerIndex = i % playerCount;
          players[playerIndex]->setId(playerIndex);
          players[playerIndex]->addTerritory(territories.at(i));
          *territories.at(i)->playerId = playerIndex;
      }

      // --- Step 3: Distribute 50 initial armies, placed in reinforcement pool ---
      //                        AND
      // --- Step 4: Let each player draw 2 initial cards from the deck
      for (Player* player : players) {
          if (player == nullptr) continue;
          player->setReinforcementPool(50);
          player->getHand()->drawFromDeckMultiple(2, deck);
      }

      // --- Step 5: Switch game to play phase ---
      setCurrentState(text);
      cmd->saveEffect("game started; entering reinforcement phase");
      return true;
  }

  // if (commandName == "issueorders") {
  //   setCurrentState(text);
  //   cmd->saveEffect("reinforcements assigned; entering issue orders phase");
  //   return true;
  // }

  if (commandName == "issueorder") {
    // setCurrentState(text);
    cmd->saveEffect("order issued");
    return true;
  }

  // if (commandName == "endissueorders") {
  //   cmd->saveEffect("all orders issued; entering execute orders phase");
  //   return true;
  // }

  if (commandName == "execorder") {
    setCurrentState(text);
    cmd->saveEffect("order executed");
    return true;
  }

  if (commandName == "win") {
    setCurrentState(text);
    cmd->saveEffect("game won");
    return true;
  }

  if (commandName == "replay") {
    setCurrentState(text);
    cmd->saveEffect("replay selected");
    return true;
  }

  if (commandName == "quit") {
    setCurrentState(text);
    cmd->saveEffect("quit selected");
    return true;
  }

  if (commandName == "addplayer") {
      string name = extractCommandTokens(text)[1];
      addPlayer(name);
      setCurrentState(text);
      cmd->saveEffect("added player " + name);
      return true;
  }

  if (commandName == "loadmap") {
      // Make sure map name is in format without the .map extension
      string mapName = extractCommandTokens(text)[1];
      int dotPos = mapName.find(".map");
      if (dotPos) {
          mapName = mapName.substr(0, dotPos);
      }
      string file = "./data/maps/" + mapName + ".map";
      
      if (mapLoader != nullptr){
          delete mapLoader;
      }
      mapLoader = new MapLoader(file);

      setCurrentState(text);
      cmd->saveEffect("loaded file " + file);
      return true;
  }

  if (commandName == "validatemap") {

      if (mapLoader == nullptr) {
          return false;
      }

      if (mapLoader->isValid()) {
          setCurrentState(text);
          cmd->saveEffect("The map is valid.");
      }
      else {
          setCurrentState("start");
          cmd->saveEffect("The map could not be validated, please use a different map. Error: " + string(mapLoader->getErrMsg()));
      }

      return true;
  }
}

static void printBanner(const string& title) {
    cout << "\n--- " << title << " ---" << endl;
}

int GameEngine::startupPhase(int argc, char* argv[])
{
    const std::string cmd = GameEngine::getCommand();
    string mode(argv[1]);
    CommandProcessor* processor = nullptr;

    if (mode == "-console") {
        cout << "\n[Mode] Console — type commands manually." << endl;
        cout << "Commands:\n"
            << "  loadmap <mapfile>, validatemap, addplayer <playername>,\n"
            << "  gamestart, issueorders, issueorder, endissueorders,\n"
            << "  execorder, win, replay, quit\n"
            << endl;

        processor = new CommandProcessor(this);

    }
    else if (mode == "-file") {
        if (argc < 3) {
            cout << "ERROR: -file mode requires a filename argument." << endl;
            return 1;
        }

        string filename(argv[2]);
        cout << "\n[Mode] File — reading commands from: " << filename << endl;

        processor = new FileCommandProcessorAdapter(this, filename);

    }
    else {
        cout << "ERROR: Unknown mode \"" << mode << "\"." << endl;
        cout << "Use -console or -file <filename>." << endl;
        return 1;
    }


    // Main command loop
    printBanner("Begin startup phase.");
    cout << *this << endl; // Prints current state of engine
    cout << "Valid commands: " << this->getNextValidCommand() << "\n" << endl; // Shows the user the current valid command for this state

    while (true) {
        // getCommand() is the public entry point required by the assignment.
        Command* cmd = processor->getCommand();

        // If EOF break the loop
        if (cmd == nullptr) {
            cout << "\n[Driver] No more commands from source. Stopping." << endl;
            break;
        }

        // getCommand() stores invalid commands too, and transition writes the real execution effect only for commands that can be processed.
        this->transition(cmd);

        // Print the command and its effect
        cout << *cmd << endl; // Raw command text from the user
        cout << *this << endl; // Print state of the engine
        cout << "Valid commands: " << this->getNextValidCommand() << "\n" << endl; // Print current possible commands

        // main game loop
        if (this->getCurrentState() == GameState::ASSIGN_REINFORCEMENT) {
          mainGameLoop(processor);
        }

        // Stop if the game has ended
        if (this->getCurrentState() == END) {
            cout << "[Driver] Game ended (quit received)." << endl;
            break;
        }
    }

    cout << *processor << endl;

    delete processor;
    return 0;
}

void GameEngine::printGameStats() {
  // print continents
  cout << "\nContinents: <id> <name> <bonus value>\n";
  for (int i = 0; i < mapLoader->getContinentsNum(); i++) {
    cout << i << " " << *mapLoader->getContinentName(i) << " " << mapLoader->getContinentBonusValue(i) << "\n";
  }
  
  // print territories
  cout << "\nTerritories: <id> <name> <continent> <owner> <armies> \n";
  for (int i = 0; i < mapLoader->getTerritoriesNum(); i++) {
    int continentId = mapLoader->getTerritoryContinentId(i);
    int playerId = mapLoader->getTerritoryPlayerId(i);

    cout << i << " " << *mapLoader->getTerritoryName(i) << " "
      << continentId << "-" << *mapLoader->getContinentName(continentId) << " ";   
      if (playerId != -1) {
        cout << playerId << "-" << players[playerId]->getName();
      } else {
        cout << "neutral";
      }
      cout << " " << mapLoader->getTerritoryArmiesNum(i) << "\n";
  }

  // print borders
  cout << "\nBorders: <territoryId> <neighboarsIdsLists>\n";
  for (int i = 0; i < mapLoader->getTerritoriesNum(); i++) {
    cout << i << ":";
    for (auto neighborId : *mapLoader->getTerritoryNeighborsIds(i)) {
      cout << " " << *neighborId;
    }
    cout << "\n";
  }

  // print players
  cout << "\nPlayers: <id> <name> <reinforcement> <cards list>\n";
  for (int i = 0; i < getPlayerCount(); i++) {
    // skip if not active
    if (players[i]->getTerritories().empty()) continue;

    cout << i << " " << players[i]->getName() << " " << players[i]->getReinforcementPool() << " ";

    cout << "[";
    bool first = true;
    for (auto card : *players[i]->getHand()->cards) {
      if (!first) cout << ", ";
      first = false;
      cout << cardTypeName(card->type);
    }
    cout << "]\n";
  }
}

void GameEngine::mainGameLoop(CommandProcessor* processor) {
  cout << "---------------------Welcome to the game-----------------------\n";
  cout << "Initital Stats\n";
  printGameStats();

  Player::mapLoader = mapLoader;
  Player::gameEngine = this;
  Order::gameEngine = this;

  while (getCurrentState() != GameState::WIN && getCurrentState() != GameState::END) {
    reinforcementPhase();

    // print new turn stats after this turn reinforcement phase
    cout << "\n\n-------New turn [after reinforcement phase]--------\n";
    printGameStats();

    issueOrdersPhase(processor);
    executeOrdersPhase();

    // check if there is one person owns all the territories and he wins
    for (int i = 0; i < getPlayerCount(); i++) {
      if (players[i]->getTerritories().size() == mapLoader->getTerritoriesNum()) {
        setCurrentState("win");

        cout << "-------- Congratulations: player " << i << "-" << players[i]->getName() << " won!!! ----------\n\n";
      }
    }
  }
}

void GameEngine::reinforcementPhase() {
  // add pool inforcement troops for territories
  for (int i = 0; i < getPlayerCount(); i++) {
    players[i]->setReinforcementPool(
      static_cast<int>(players[i]->getTerritories().size()) / 3 
      + players[i]->getReinforcementPool());
  }

  // add pool inforcement troops for continents bonus value
  for (int i = 0; i < mapLoader->getContinentsNum(); i++) {
    int playerId = -2;

    // check if the continent i belongs to one single player
    for (int j = 0; j < mapLoader->getTerritoriesNum(); j++) {
      if (mapLoader->getTerritoryContinentId(j) != i) continue; // skip

      if (playerId == -2) {
        playerId = mapLoader->getTerritoryPlayerId(j);
      } else if (playerId != mapLoader->getTerritoryPlayerId(j)) {
        playerId = -1;
        break;
      }
    }

    if (playerId >= 0) {
      players[playerId]->setReinforcementPool(players[playerId]->getReinforcementPool() + mapLoader->getContinentBonusValue(i));
    }
  }

  // minimum 3 armies should go to reinforcement pools of active players
  for (int i = 0; i < getPlayerCount(); i++) {
    if (!players[i]->getTerritories().empty()) {
      players[i]->setReinforcementPool(std::max(3, players[i]->getReinforcementPool()));
    }
  }
}

void GameEngine::issueOrdersPhase(CommandProcessor* processor) {

  setCurrentState("issueorders");

  int activePlayersCount = 0;
  for (int i = 0; i < getPlayerCount(); i++) {
    if (players[i]->getTerritories().size() > 0) activePlayersCount++;
  }

  vector<bool> isPlayerFinishedOrdering(getPlayerCount(), false);
  int finishedOrderingPlayerCount = 0;

  while (finishedOrderingPlayerCount != activePlayersCount) {
    for (int i = 0; i < getPlayerCount(); i++) {
      // if the player is not active or is finished ordering skip
      if (players[i]->getTerritories().size() == 0 || isPlayerFinishedOrdering[i]) {
        continue;
      }

      // -- display the instrunctions for ordering for player i
      cout << "\n--------------\n";
      cout << "Ordering instructions for player " << i << "-" << players[i]->getName()
        << " with reinforcement " << players[i]->getReinforcementPool() << "\n";
      cout << "--------------\n";
      cout << "deploy <armNum> <territoryId>\n";
      cout << "advance <armNum> <sourceTerritoryId> <targetTerritoryId>\n";
      cout << "airlift <armNum> <sourceTerritoryId> <targetTerritoryId>\n";
      cout << "bomb <targetTerritoryId>\n";
      cout << "blockade <targetTerritoryId>\n";
      cout << "negotiate <opponentPlayerId>\n";
      cout << "endissueorder\n";
      cout << "\n";
      // --

      // getCommand() is the public entry point required by the assignment.
      Command* cmd = processor->getCommand();

      // If EOF break the loop
      if (cmd == nullptr) {
          cout << "\n[Driver] No more commands from source. Stopping." << endl;
          setCurrentState("quit");
          break;
      }


      // check endissueorder command
      if (extractCommandName(cmd->getCommand()) == "endissueorder") {
        if (players[i]->getReinforcementPool() > 0) {
          cout << "Error: you can't stop issueing orders, you haven't deployed you reinforcement pool yet\n";
        } else {
          finishedOrderingPlayerCount++;
          isPlayerFinishedOrdering[i] = true;
        }
      } else {
        // survey the command and issue the correct order
        players[i]->issueOrder(cmd->getCommand());
      }
      
    }
  }
}

void GameEngine::executeOrdersPhase() {
  cout << "\n\n----------------Execution phase-----------------\n";
  

  int emptyOrderListPlayersCount = 0;
  while (emptyOrderListPlayersCount != getPlayerCount()) {
    for (int i = 0; i < getPlayerCount(); i++) {
      if (!players[i]->getOrders()->getOrders()->empty()) {
        // execute the first order
        Order* order = players[i]->getOrders()->getOrders()->front();
        order->execute();
        
        // print the execution effect
        cout << players[i]->getId() << "-" << players[i]->getName() << ": " 
          << order->getOrderType() << " -> " << order->getOrderEffect() << "\n";

        players[i]->getOrders()->remove(0);
      }
    }

    // find the number of players whose orders lists are empty
    emptyOrderListPlayersCount = 0;
    for (int i = 0; i < getPlayerCount(); i++) {
      if (players[i]->getOrders()->getOrders()->empty()) {
        emptyOrderListPlayersCount++;
      }
    }
  }

  for (int i = 0; i < getPlayerCount(); i++) {
    // reward cards to player who has captured at least one territory
    if (players[i]->getConqueredThisTurn() && deck->size() > 0) {
      players[i]->getHand()->cards->push_back(&players[i]->getDeck()->draw());
    }

    // reset the attributes of this turn for player
    players[i]->setConqueredThisTurn(false);
    players[i]->clearNegotiatedPlayers();
  }
  
}


// overloading << to print the current state
ostream &operator<<(ostream &os, const GameEngine &gameEngine) {
  os << "Current state: " << gameEngine.getCurrentStateName();
  return os;
}

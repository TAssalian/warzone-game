#include "GameEngine.h"
#include "CommandProcessing.h"
#include "../Observer/LoggingObserver.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <fstream>
#include "../map/Map.h"
#include "../orders/Orders.h"
#include "../player/Player.h"
#include "../player/PlayerStrategies.h"
#include "../cards/Cards.h"
#include <random>
#include <algorithm>
#include <unordered_map>

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

std::vector<string> buildTournamentPlayerLabels(const std::vector<string> &strategies) {
  std::unordered_map<string, int> strategyCounts;
  std::unordered_map<string, int> strategyOrdinals;
  std::vector<string> labels;
  labels.reserve(strategies.size());

  for (const string &strategy : strategies) {
    strategyCounts[strategy]++;
  }

  for (const string &strategy : strategies) {
    if (strategyCounts[strategy] == 1) {
      labels.push_back(strategy);
      continue;
    }

    const int ordinal = ++strategyOrdinals[strategy];
    labels.push_back(strategy + " " + std::to_string(ordinal));
  }

  return labels;
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
  mapLoader = nullptr;

  for (int i = 0; i < 6; i++) {
    players[i] = nullptr;
  }
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
  // Note: players is a fixed-size array, not dynamically allocated
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
    return (isCommand(command, "loadmap") && hasRequiredParameter(command)) ||
           isCommand(command, "tournament");
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
    return "Start -> loadmap <mapfile> | tournament -M <maps> -P <strategies> -G <games> -D <turns>";
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
  notify(this); // Notify observers of the state change
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

  if (commandName == "tournament") {
    CommandProcessor processor(this);
    TournamentParams params;
    string errorMsg;

    if (!processor.parseTournamentCommand(text, params, errorMsg)) {
      cmd->saveEffect("ERROR: " + errorMsg);
      return false;
    }

    TournamentResults results = runTournament(params);
    printTournamentResults(results);
    logTournamentResults(results);
    resetGameState();

    cmd->saveEffect("tournament completed");
    return true;
  }

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
    notify(this); // Notify observers of the state change
    cmd->saveEffect("game started; entering reinforcement phase");
    return true;
  }

  // if (commandName == "issueorders") {
  //   setCurrentState(text);
  //   notify(this); // Notify observers of the state change
  //   cmd->saveEffect("reinforcements assigned; entering issue orders phase");
  //   return true;
  // }

  if (commandName == "issueorder") {
    // setCurrentState(text);
    cmd->saveEffect("order issued");
    return true;
  }

  // if (commandName == "endissueorders") {
  //   notify(this); // Notify observers of the state change
  //   cmd->saveEffect("all orders issued; entering execute orders phase");
  //   return true;
  // }

  if (commandName == "execorder") {
    setCurrentState(text);
    notify(this); // Notify observers of the state change
    cmd->saveEffect("order executed");
    return true;
  }

  if (commandName == "win") {
    setCurrentState(text);
    notify(this); // Notify observers of the state change
    cmd->saveEffect("game won");
    return true;
  }

  if (commandName == "replay") {
    setCurrentState(text);
    notify(this); // Notify observers of the state change
    cmd->saveEffect("replay selected");
    return true;
  }

  if (commandName == "quit") {
    setCurrentState(text);
    notify(this); // Notify observers of the state change
    cmd->saveEffect("quit selected");
    return true;
  }

  if (commandName == "addplayer") {
    string name = extractCommandTokens(text)[1];
    addPlayer(name);
    setCurrentState(text);
    notify(this); // Notify observers of the state change
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
    notify(this); // Notify observers of the state change
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

    notify(this); // Notify observers of the state change
    return true;
  }
  
  return false; // Default case for unknown commands
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

        cout << "\n\n-------- Congratulations: player " << i << "-" << players[i]->getName() << " won!!! ----------\n\n";
        
        // show the valid commands
        cout << "Valid commands: Win -> replay - quit\n";

        // reset the gameengine
        delete command;
        delete deck;
        delete mapLoader;
        command = new string("");
        deck = new Deck();
        mapLoader = nullptr;

        for (int i = 0; i < 6; i++) {
          if (players[i] != nullptr) delete players[i];
          players[i] = nullptr;
        }
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

//logging string for GameEngine
std::string GameEngine::stringToLog() const {
    return "GameEngine::transition() [" + getCurrentStateName() + "]";
}

// Reset game state for starting a new game (used between tournament games)
void GameEngine::resetGameState() {
  // Delete and reset players
  for (int i = 0; i < 6; i++) {
    if (players[i] != nullptr) {
      delete players[i];
      players[i] = nullptr;
    }
  }
  
  // Reset other state
  delete command;
  delete mapLoader;
  delete deck;
  
  command = new string("");
  deck = new Deck();
  mapLoader = nullptr;
  
  delete currentState;
  currentState = new GameState(GameState::START);
}

// Run a single game and return the winner's strategy name or Draw
string GameEngine::runSingleGame(const string &mapFile, const vector<string> &strategies, int maxTurns) {
  // Reset game state
  resetGameState();
  
  // Load map
  string file = "./data/maps/" + mapFile;
  // Remove .map extension if already present
  int dotPos = file.find(".map");
  if (dotPos == string::npos) {
    file += ".map";
  }
  
  if (mapLoader != nullptr) {
    delete mapLoader;
  }
  mapLoader = new MapLoader(file);
  
  if (!mapLoader->isValid()) {
    cout << "[Tournament] Error: Map " << mapFile
         << " could not be loaded or is invalid: "
         << mapLoader->getErrMsg() << endl;
    return "Error";
  }
  
  const std::vector<string> playerLabels = buildTournamentPlayerLabels(strategies);

  // Create players with strategies
  for (size_t i = 0; i < strategies.size(); i++) {
    const string &playerName = playerLabels[i];
    addPlayer(playerName);
    
    // Set the strategy for the player
    PlayerStrategy* strategy = nullptr;
    string stratLower = strategies[i];
    std::transform(stratLower.begin(), stratLower.end(), stratLower.begin(), ::tolower);
    
    if (stratLower == "aggressive") {
      strategy = new AggressivePlayerStrategy();
    } else if (stratLower == "benevolent") {
      strategy = new BenevolentPlayerStrategy();
    } else if (stratLower == "neutral") {
      strategy = new NeutralPlayerStrategy();
    } else if (stratLower == "cheater") {
      strategy = new CheaterPlayerStrategy();
    }
    
    players[i]->setStrategy(strategy);
  }
  
  // Distribute territories and set up game (similar to gamestart command)
  Map* map = mapLoader->map;
  std::vector<Territory*> territories(*map->territories);
  int playerCount = getPlayerCount();
  
  if (playerCount <= 0) {
    return "Error";
  }
  
  // Shuffle players and territories
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(std::begin(players), std::begin(players) + playerCount, g);
  std::shuffle(std::begin(territories), std::end(territories), g);
  
  // Distribute territories evenly
  for (size_t i = 0; i < territories.size(); ++i) {
    int playerIndex = i % playerCount;
    players[playerIndex]->setId(playerIndex);
    players[playerIndex]->addTerritory(territories.at(i));
    *territories.at(i)->playerId = playerIndex;
  }
  
  // Distribute initial armies and cards
  for (int i = 0; i < playerCount; i++) {
    if (players[i] == nullptr) continue;
    players[i]->setReinforcementPool(50);
    players[i]->getHand()->drawFromDeckMultiple(2, deck);
  }
  
  // Set static references
  Player::mapLoader = mapLoader;
  Player::gameEngine = this;
  Order::gameEngine = this;
  
  setCurrentState("gamestart");
  
  // Run the game loop for up to maxTurns
  int turnCount = 0;
  string winner = "Draw";
  
  while (turnCount < maxTurns) {
    turnCount++;
    
    // Reinforcement phase
    reinforcementPhase();
    
    // Issue orders phase (automated for computer players)
    for (int i = 0; i < playerCount; i++) {
      if (players[i] == nullptr || players[i]->getTerritories().empty()) continue;
      players[i]->getStrategy()->issueOrder(players[i]);
    }
    
    // Execute orders phase
    for (int i = 0; i < playerCount; i++) {
      if (players[i] == nullptr) continue;
      OrderList* orderList = players[i]->getOrders();
      while (!orderList->getOrders()->empty()) {
        Order* order = orderList->getOrders()->front();
        order->execute();
        orderList->remove(0);
      }
    }
    
    // Post-turn cleanup
    for (int i = 0; i < playerCount; i++) {
      if (players[i] == nullptr) continue;
      
      // Reward cards
      if (players[i]->getConqueredThisTurn() && deck->size() > 0) {
        players[i]->getHand()->cards->push_back(&players[i]->getDeck()->draw());
      }
      
      // Reset turn-specific attributes
      players[i]->setConqueredThisTurn(false);
      players[i]->clearNegotiatedPlayers();
    }
    
    // Check for a winner (player owns all territories)
    for (int i = 0; i < playerCount; i++) {
      if (players[i] == nullptr) continue;
      if (players[i]->getTerritories().size() == mapLoader->getTerritoriesNum()) {
        winner = players[i]->getName();
        return winner;
      }
    }
    
    // Check if only one player remains with territories
    int activePlayers = 0;
    int lastActivePlayer = -1;
    for (int i = 0; i < playerCount; i++) {
      if (players[i] != nullptr && !players[i]->getTerritories().empty()) {
        activePlayers++;
        lastActivePlayer = i;
      }
    }
    
    if (activePlayers == 1 && lastActivePlayer >= 0) {
      winner = players[lastActivePlayer]->getName();
      return winner;
    }
    
    if (activePlayers == 0) {
      return "Draw";
    }
  }
  
  return winner; // "Draw" if no winner after maxTurns
}

// Run the full tournament
TournamentResults GameEngine::runTournament(const TournamentParams &params) {
  TournamentResults results;
  results.mapFiles = params.mapFiles;
  results.playerStrategies = params.playerStrategies;
  results.numberOfGames = params.numberOfGames;
  results.maxTurns = params.maxTurns;
  
  // Initialize results table
  results.results.resize(params.mapFiles.size());
  for (size_t i = 0; i < params.mapFiles.size(); i++) {
    results.results[i].resize(params.numberOfGames);
  }
  
  cout << "\n========== TOURNAMENT MODE ==========\n" << endl;
  cout << "Maps: ";
  for (const auto &m : params.mapFiles) cout << m << " ";
  cout << "\nStrategies: ";
  for (const auto &s : params.playerStrategies) cout << s << " ";
  cout << "\nGames per map: " << params.numberOfGames << endl;
  cout << "Max turns: " << params.maxTurns << endl;
  
  // Play all games
  for (size_t mapIdx = 0; mapIdx < params.mapFiles.size(); mapIdx++) {
    const string &mapFile = params.mapFiles[mapIdx];
    
    for (int gameNum = 0; gameNum < params.numberOfGames; gameNum++) {
      string winner = runSingleGame(mapFile, params.playerStrategies, params.maxTurns);
      results.results[mapIdx][gameNum] = winner;
    }
  }
  
  return results;
}

// Print tournament results to console
void GameEngine::printTournamentResults(const TournamentResults &results) {
  cout << "\n========== TOURNAMENT RESULTS ==========\n" << endl;
  
  // Print table header
  cout << std::setw(15) << " ";
  for (int g = 1; g <= results.numberOfGames; g++) {
    cout << std::setw(15) << ("Game " + std::to_string(g));
  }
  cout << endl;
  
  // Print table rows
  for (size_t m = 0; m < results.mapFiles.size(); m++) {
    cout << std::setw(15) << results.mapFiles[m];
    for (int g = 0; g < results.numberOfGames; g++) {
      cout << std::setw(15) << results.results[m][g];
    }
    cout << endl;
  }
  
  cout << "\n=========================================\n" << endl;
}

// Log tournament results to the log file
void GameEngine::logTournamentResults(const TournamentResults &results) {
  std::ostringstream oss;
  oss << "\nTournament mode:\n";
  oss << "M: ";
  for (size_t i = 0; i < results.mapFiles.size(); i++) {
    if (i > 0) oss << ", ";
    oss << results.mapFiles[i];
  }
  oss << "\n";
  
  oss << "P: ";
  for (size_t i = 0; i < results.playerStrategies.size(); i++) {
    if (i > 0) oss << ", ";
    oss << results.playerStrategies[i];
  }
  oss << "\n";
  
  oss << "G: " << results.numberOfGames << "\n";
  oss << "D: " << results.maxTurns << "\n";
  
  oss << "\nResults:\n";
  
  // Table header
  oss << std::setw(15) << " ";
  for (int g = 1; g <= results.numberOfGames; g++) {
    oss << std::setw(15) << ("Game " + std::to_string(g));
  }
  oss << "\n";
  
  // Table rows
  for (size_t m = 0; m < results.mapFiles.size(); m++) {
    oss << std::setw(15) << results.mapFiles[m];
    for (int g = 0; g < results.numberOfGames; g++) {
      oss << std::setw(15) << results.results[m][g];
    }
    oss << "\n";
  }
  
  // Write to log file only (don't print to console)
  std::ofstream logFile("gamelog.txt", std::ios::app);
  if (logFile.is_open()) {
    logFile << oss.str();
    logFile.close();
  }
}

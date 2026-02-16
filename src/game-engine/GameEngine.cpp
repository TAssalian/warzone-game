#include "GameEngine.h"

using std::string;

// mapping of gameStates
GameState stringToGameState(const std::string &str) {
  if (str == "load-map" || str == "1")
    return MAP_LOADED;
  if (str == "validate-map" || str == "2")
    return MAP_VALIDATED;
  if (str == "add-player" || str == "3")
    return PLAYERS_ADDED;
  if (str == "assign-countries" || str == "4")
    return ASSIGN_REINFORCEMENT;

  if (str == "issue-order" || str == "5")
    return ISSUE_ORDERS;
  if (str == "end-issue-orders" || str == "6")
    return EXECUTE_ORDERS;
  if (str == "exec-order" || str == "7")
    return EXECUTE_ORDERS;
  if (str == "end-exec-orders" || str == "8")
    return ASSIGN_REINFORCEMENT;

  if (str == "win" || str == "9")
    return WIN;
  if (str == "play" || str == "10")
    return START;
  if (str == "end" || str == "11")
    return END;

  return START;
}

// default
GameEngine::GameEngine() {
  currentState = new GameState(GameState::START);
  command = new string("");
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
  delete currentState;
  delete command;
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
    return command == "load-map" || command == "1";
  case MAP_LOADED:
    return command == "load-map" || command == "1" ||
           command == "validate-map" || command == "2";
  case MAP_VALIDATED:
    return command == "add-player" || command == "3";
  case PLAYERS_ADDED:
    return command == "add-player" || command == "3" ||
           command == "assign-countries" || command == "4";
  case ASSIGN_REINFORCEMENT:
    return command == "issue-order" || command == "5";
  case ISSUE_ORDERS:
    return command == "issue-order" || command == "5" ||
           command == "end-issue-orders" || command == "6";
  case EXECUTE_ORDERS:
    return command == "exec-order" || command == "7" ||
           command == "end-exec-orders" || command == "8" || command == "win" ||
           command == "9";
  case WIN:
    return command == "play" || command == "10" || command == "end" ||
           command == "11";
  default:
    return false;
  }
}

// next valid command for current state (with numbers)
string GameEngine::getNextValidCommand() const {
  GameState state = *currentState;

  switch (state) {
  case START:
    return "Start -> load-map(1)";
  case MAP_LOADED:
    return "Map Loaded -> load-map(1) | validate-map(2)";
  case MAP_VALIDATED:
    return "Map Validated -> add-player(3)";
  case PLAYERS_ADDED:
    return "Players Added -> add-player(3) | assign-countries(4)";
  case ASSIGN_REINFORCEMENT:
    return "Assign Reinforce -> issue-order(5)";
  case ISSUE_ORDERS:
    return "Issue Orders -> issue-order(5) | end-issue-orders(6)";
  case EXECUTE_ORDERS:
    return "Execute Orders -> exec-order(7) | end-exec-orders(8) | win(9)";
  case WIN:
    return "Win -> play(10) | end(11)";
  default:
    return "";
  }
}

// overloading << to print the current state
ostream &operator<<(ostream &os, const GameEngine &gameEngine) {
  os << "Current state: " << gameEngine.getCurrentStateName();
  return os;
}

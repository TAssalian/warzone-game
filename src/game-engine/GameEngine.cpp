#include "GameEngine.h"
#include "CommandProcessing.h"

#include <sstream>
#include <vector>

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
    return isCommand(command, "loadmap") && hasRequiredParameter(command);
  case MAP_LOADED:
    return (isCommand(command, "loadmap") && hasRequiredParameter(command)) ||
           isCommand(command, "validatemap");
  case MAP_VALIDATED:
    return isCommand(command, "addplayer") && hasRequiredParameter(command);
  case PLAYERS_ADDED:
    return (isCommand(command, "addplayer") && hasRequiredParameter(command)) ||
           isCommand(command, "gamestart");
  case ASSIGN_REINFORCEMENT:
    return isCommand(command, "issueorders");
  case ISSUE_ORDERS:
    return isCommand(command, "issueorder") || isCommand(command, "endissueorders");
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
  case PLAYERS_ADDED:
    return "Players Added -> addplayer <playername> | gamestart";
  case ASSIGN_REINFORCEMENT:
    return "Assign Reinforcement -> issueorders";
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
    setCurrentState(text);
    cmd->saveEffect("game started; entering reinforcement phase");
    return true;
  }

  if (commandName == "issueorders") {
    setCurrentState(text);
    cmd->saveEffect("reinforcements assigned; entering issue orders phase");
    return true;
  }

  if (commandName == "issueorder") {
    cmd->saveEffect("order issued");
    return true;
  }

  if (commandName == "endissueorders") {
    setCurrentState(text);
    cmd->saveEffect("all orders issued; entering execute orders phase");
    return true;
  }

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

  // loadmap, validatemap and addplayer done here because state stays the same
  setCurrentState(text);
  cmd->saveEffect("state changed from " + previousState + " to " +
                  getCurrentStateName());
  return true;
}

// overloading << to print the current state
ostream &operator<<(ostream &os, const GameEngine &gameEngine) {
  os << "Current state: " << gameEngine.getCurrentStateName();
  return os;
}

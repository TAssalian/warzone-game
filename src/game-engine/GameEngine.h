#pragma once
#include "../Observer/LoggingObserver.h"
#include "CommandProcessing.h"
#include <iostream>
#include <string>
#include <vector>
#include "../player/Player.h"
#include "../map/Map.h"
#include "../cards/Cards.h"

class Command;
class CommandProcessor;
struct TournamentParams;
struct TournamentResults;

using std::string, std::ostream;

enum GameState {
  START,
  MAP_LOADED,
  MAP_VALIDATED,
  PLAYERS_ADDED,
  ASSIGN_REINFORCEMENT,
  ISSUE_ORDERS,
  EXECUTE_ORDERS,
  WIN,
  END,

};
static const string GameStateNames[] = {
    "START",
    "MAP_LOADED",
    "MAP_VALIDATED",
    "PLAYERS_ADDED",
    "ASSIGN_REINFORCEMENT",
    "ISSUE_ORDERS",
    "EXECUTE_ORDERS",
    "WIN",
    "END",
};

GameState stringToGameState(const std::string &str);

class GameEngine: public ILoggable, public Subject {

private:
  GameState *currentState;
  string *command;
  Player *players[6];
  MapLoader* mapLoader;
  Deck* deck;

public:
  GameEngine();
  GameEngine(const GameEngine &other);
  GameEngine &operator=(const GameEngine &other);
  ~GameEngine();

  void addPlayer(std::string name);
  int getPlayerCount() const;

  Player** getPlayers();
  MapLoader* getMapLoader();

  // getter for state and state name
  GameState getCurrentState() const;
  string getCurrentStateName() const;

  // Setter for currentstate
  void setCurrentState(string nextState);

  // getter for command
  string getCommand() const;

  // setter for command
  void setCommand(string nextCommand);

  // validate command for current state
  bool validateCommand(string &command);

  // next valid command for current state
  string getNextValidCommand() const;

  // validate and apply state transition in one call
  bool transition(const std::string &cmd);
  bool transition(Command *cmd);

  // a command-based user interaction to start the game
  int startupPhase(int argc, char* argv[]);

  // main game methods
  void mainGameLoop(CommandProcessor* processor);
  void reinforcementPhase();
  void issueOrdersPhase(CommandProcessor* processor);
  void executeOrdersPhase();

  // Tournament mode methods
  TournamentResults runTournament(const TournamentParams &params);
  std::string runSingleGame(const std::string &mapFile, const std::vector<std::string> &strategies, int maxTurns);
  void logTournamentResults(const TournamentResults &results);
  void printTournamentResults(const TournamentResults &results);
  void resetGameState();

  // details of the game (such as players, territories, ...)
  void printGameStats();

  string stringToLog() const override;

  friend ostream &operator<<(ostream &os, const GameEngine &gameEngine);
};

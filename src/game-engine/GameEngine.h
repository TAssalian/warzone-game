#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "../player/Player.h"
#include "../map/Map.h"
#include "../cards/Cards.h"

class Command;
class CommandProcessor;

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

class GameEngine {

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

  // details of the game (such as players, territories, ...)
  void printGameStats();


  friend ostream &operator<<(ostream &os, const GameEngine &gameEngine);
};

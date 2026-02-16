#pragma once

#include <iostream>
#include <string>
#include <vector>

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

public:
  GameEngine();
  GameEngine(const GameEngine &other);
  GameEngine &operator=(const GameEngine &other);
  ~GameEngine();

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

  friend ostream &operator<<(ostream &os, const GameEngine &gameEngine);
};
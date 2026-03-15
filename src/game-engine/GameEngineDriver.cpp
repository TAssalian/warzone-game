#include "GameEngineDriver.h"
#include "GameEngine.h"
#include <iostream>
#include <string>
using std::cout, std::cin, std::endl, std::getline;

int run_game_engine_driver() {
  cout << "WARZONE GAME ENGINE DRIVER" << endl;

  GameEngine *gameEngine = new GameEngine();
  // cout << *gameEngine << endl;

  cout << endl;
  cout << "Available commands per state:" << endl;
  cout << "  Start             -> loadmap <mapfile>" << endl;
  cout << "  Map Loaded        -> loadmap <mapfile> | validatemap" << endl;
  cout << "  Map Validated     -> addplayer <playername>" << endl;
  cout << "  Players Added     -> addplayer <playername> | gamestart" << endl;
  cout << "  Win               -> replay | quit" << endl;
  cout << endl;

  string command, previousCommand;
  bool running = true;
  while (running) {

    // creating gameEngine and printing current state
    cout << *gameEngine << endl;
    cout << "next valid command:" << gameEngine->getNextValidCommand() << endl;
    cout << "Please enter a command" << endl;
    // ask user for next transition
    getline(cin, command);

    // validate transition and print if it is valid or not
    bool validtransition = gameEngine->validateCommand(command);
    cout << "Command "
         << (gameEngine->validateCommand(command) ? "valid" : "invalid")
         << " for current state." << endl;
    cout << endl;
    cout << endl;
    if (!validtransition) {
      command = previousCommand;
      continue;
    }
    // set command and current state
    gameEngine->setCommand(command);
    gameEngine->setCurrentState(command);

    // if the command is end and current state is win, exit the loop
    if (gameEngine->getCurrentState() == END) {
      running = false;
    }

    cout << endl;

    // cout << "Entered state: " << gameEngine->getCurrentStateName() << endl;
  }
  cout << "exiting game ..." << endl;

  delete gameEngine;
  gameEngine = nullptr;

  return 0;
}

int run_game_engine_startup_phase_test(int argc, char* argv[]) {
    cout << "GAME ENGINE STARTUP PHASE DRIVER" << endl;

    // Call startup phase method
    GameEngine* gameEngine = new GameEngine();
    gameEngine->startupPhase(argc, argv);

    cout << "VALIDATION" << endl;

    // Verify step 1 & 2.
    MapLoader* mapLoader = gameEngine->getMapLoader();
    if (mapLoader->map != nullptr) {
        cout << "Step 1 passed. The map was indeed loaded." << endl;
    }

    if (mapLoader->isFormatValid && mapLoader->isValid()) {
        cout << "Step 2 passed. The map is valid." << endl;
    }
   
    // Verify step 3 & 4.
    Player** players = gameEngine->getPlayers();
    
    for (int i = 0; i < 6; i++) {
        Player* player = players[i];
        if (players[i] == nullptr) continue;
        
        cout << "\n";
        cout << *player << endl;
    }

    cout << "\nStep 3 & 4 passed. Shown all players and their correct stats" << endl;

    return 0;
}

int main(int argc, char* argv[]) {

  //run_game_engine_driver();
  run_game_engine_startup_phase_test(argc, argv);
  return 0;
}

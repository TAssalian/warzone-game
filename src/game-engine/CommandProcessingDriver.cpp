#include "CommandProcessing.h"
#include "GameEngine.h"

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

static void printBanner(const string &title) {
  cout << "\n--- " << title << " ---" << endl;
}

//Parses commands and runs the demo:
//-console            : reads commands from console
//-file <filename>    : reads commands from text file
int run_command_processing_driver(int argc, char *argv[]) {
  cout << "========================================" << endl;
  cout << "  WARZONE — Command Processing Driver   " << endl;
  cout << "========================================" << endl;

  if (argc < 2) {
    cout << "\nUsage:\n"
         << "  " << argv[0] << " -console\n"
         << "  " << argv[0] << " -file <commands_file>\n"
         << endl;
    return 1;
  }

  string mode(argv[1]);
  GameEngine *engine = new GameEngine();
  CommandProcessor *processor = nullptr;

  if (mode == "-console") {
    cout << "\n[Mode] Console — type commands manually." << endl;
    cout << "Commands:\n"
         << "  loadmap <mapfile>, tournament -M <maps> -P <strategies> -G <games> -D <turns>,\n"
         << "  validatemap, addplayer <playername>,\n"
         << "  gamestart, issueorders, issueorder, endissueorders,\n"
         << "  execorder, win, replay, quit\n"
         << endl;

    processor = new CommandProcessor(engine);

  } else if (mode == "-file") {
    if (argc < 3) {
      cout << "ERROR: -file mode requires a filename argument." << endl;
      delete engine;
      return 1;
    }

    string filename(argv[2]);
    cout << "\n[Mode] File — reading commands from: " << filename << endl;

    processor = new FileCommandProcessorAdapter(engine, filename);

  }
  else {
    cout << "ERROR: Unknown mode \"" << mode << "\"." << endl;
    cout << "Use -console or -file <filename>." << endl;
    delete engine;
    return 1;
  }



  // Main command loop
  printBanner("Starting Command Processing Loop");
  cout << *engine << endl; // Prints current state of engine
  cout << "Valid commands: " << engine->getNextValidCommand() << "\n" << endl; // Shows the user the current valid command for this state

  while (true) {
    // getCommand() is the public entry point required by the assignment.
    Command *cmd = processor->getCommand();

    // If EOF break the loop
    if (cmd == nullptr) {
      cout << "\n[Driver] No more commands from source. Stopping." << endl;
      break;
    }

    // getCommand() stores invalid commands too, and transition writes the real execution effect only for commands that can be processed.
    engine->transition(cmd);

    // Print the command and its effect
    cout << *cmd << endl; // Raw command text from the user
    cout << *engine << endl; // Print state of the engine
    cout << "Valid commands: " << engine->getNextValidCommand() << "\n" << endl; // Print current possible commands

    // Stop if the game has ended
    if (engine->getCurrentState() == END) {
      cout << "[Driver] Game ended (quit received)." << endl;
      break;
    }
  }

  printBanner("Final CommandProcessor State");
  cout << *processor << endl;

  delete processor;
  delete engine;
  return 0;
}

int main(int argc, char *argv[]) {
  return run_command_processing_driver(argc, argv);
}

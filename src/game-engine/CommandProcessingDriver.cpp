#include "CommandProcessing.h"
#include "GameEngine.h"

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

// To separate sections
static void printBanner(const string &title) {
  cout << "\n--- " << title << " ---" << endl;
}

// Driver entry point is declared as friend of CommandProcessor so it can call
// the protected readCommand() directly since we do not want to expose this method to other classes

/**
 * Parses commands and runs the demo:
 *   -console            : reads commands from console
 *   -file <filename>    : reads commands from text file
 */
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
    cout << "Commands (name or number):\n"
         << "  load-map(1), validate-map(2), add-player(3),\n"
         << "  assign-countries(4), issue-order(5), end-issue-orders(6),\n"
         << "  exec-order(7), end-exec-orders(8), win(9), play(10), end(11)\n"
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

  } else {
    cout << "ERROR: Unknown mode \"" << mode << "\"." << endl;
    cout << "Use -console or -file <filename>." << endl;
    delete engine;
    return 1;
  }

  // Main command loop
  printBanner("Starting Command Processing Loop");
  cout << *engine << endl;
  cout << "Valid commands: " << engine->getNextValidCommand() << "\n" << endl;

  while (true) {
    // Step 1: read the next command from the source (console or file).
    //         readCommand() stores the Command internally via saveCommand().
    Command *cmd = processor->readCommand();

    // If EOF break the loop
    if (cmd == nullptr) {
      cout << "\n[Driver] No more commands from source. Stopping." << endl;
      break;
    }

    // Step 2: validate the command and saves effect
    bool valid = processor->validate(cmd);

    // Step 3: if valid, apply the state transition
    if (valid) {
      engine->transition(cmd->getCommand());
    }

    // Print the command and its effect
    cout << *cmd << endl;
    cout << *engine << endl;
    cout << "Valid commands: " << engine->getNextValidCommand() << "\n" << endl;

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

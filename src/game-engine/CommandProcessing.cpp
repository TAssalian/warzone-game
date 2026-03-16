#include "CommandProcessing.h"
#include "GameEngine.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::ifstream;
using std::ostream;
using std::string;
using std::vector;


Command::Command(const string &cmd)
    : commandText(new string(cmd)), effect(new string("")) {}

Command::Command(const Command &other)
    : commandText(new string(*other.commandText)),
      effect(new string(*other.effect)) {}

Command &Command::operator=(const Command &other) {
  if (this != &other) {
    delete commandText;
    delete effect;
    commandText = new string(*other.commandText);
    effect = new string(*other.effect);
  }
  return *this;
}

Command::~Command() {
  delete commandText;
  delete effect;
}

// Returns command stored in Command object
string Command::getCommand() const { return *commandText; }

string Command::getEffect() const { return *effect; }

// Saves the effect string for this Command
void Command::saveEffect(const string &eff) {
  delete effect;
  effect = new string(eff);
  notify(this); // Notify observers of the change
}

ostream &operator<<(ostream &os, const Command &cmd) {
  os << "Command: \"" << *cmd.commandText << "\"" << " | Effect: \"" << *cmd.effect << "\"";
  return os;
}


CommandProcessor::CommandProcessor(GameEngine *ge)
    : engine(ge) {}

CommandProcessor::CommandProcessor(const CommandProcessor &other)
    : engine(other.engine) {
  for (Command *c : other.commandList) {
    commandList.push_back(new Command(*c));
  }
}

CommandProcessor &CommandProcessor::operator=(const CommandProcessor &other) {
  if (this != &other) {
    for (Command *c : commandList) {
      delete c;
    }
    commandList.clear();
    engine = other.engine;
    for (Command *c : other.commandList) {
      commandList.push_back(new Command(*c));
    }
  }
  return *this;
}

CommandProcessor::~CommandProcessor() {
  for (Command *c : commandList) {
    delete c;
  }
}

// Get the command from the terminal in console mode
Command *CommandProcessor::readCommand() {
  cout << "[CommandProcessor] Enter command: ";
  string line;
  if (!std::getline(std::cin, line)) {
    return nullptr; // EOF or input failure
  }
  return new Command(line);
}

void CommandProcessor::saveCommand(Command *cmd) {
  commandList.push_back(cmd);
  notify(this); // Notify observers of the new command
}

Command *CommandProcessor::getCommand() {
  Command *cmd = readCommand();
  if (cmd == nullptr) {
    return nullptr;
  }

  saveCommand(cmd);
  validate(cmd);
  return cmd;
}

// Returns a string representation of the CommandProcessor for logging purposes
string CommandProcessor::stringToLog() const {
    if (commandList.empty()) return "CommandProcessor: No commands entered yet.";
    return "CommandProcessor::saveCommand(): [" + commandList.back()->getCommand() + "]";
}
string Command::stringToLog() const {
    return "Command::saveEffect(): [" + *commandText + "] | Effect: [" + *effect + "]";
}

// Validates whether Command object is legal in the current game state
// If the command is invalid, saves a corresponding error message as the command's effect
bool CommandProcessor::validate(Command *cmd) {
  string text = cmd->getCommand();
  string stateName = engine->getCurrentStateName();

  if (engine->validateCommand(text)) { //
    return true;
  } else {
    cmd->saveEffect("ERROR: \"" + text + "\" is not a valid command in state " + stateName + ".");
    return false;
  }
}

ostream &operator<<(ostream &os, const CommandProcessor &cp) {
  os << "CommandProcessor [\n";
  for (const Command *c : cp.commandList) {
    os << "  " << *c << "\n";
  }
  os << "]";
  return os;
}

FileLineReader::FileLineReader(const string &fn)
    : filename(new string(fn)), fileStream(new ifstream(fn)) {
  if (!fileStream->is_open()) {
    std::cerr << "[FileLineReader] ERROR: Could not open file: " << fn << endl;
  }
}

FileLineReader::FileLineReader(const FileLineReader &other)
    : filename(new string(*other.filename)),
      fileStream(new ifstream(*other.filename)) {}

FileLineReader &FileLineReader::operator=(const FileLineReader &other) {
  if (this != &other) {
    fileStream->close();
    delete fileStream;
    delete filename;

    filename = new string(*other.filename);
    fileStream = new ifstream(*filename);
  }
  return *this;
}

FileLineReader::~FileLineReader() {
  if (fileStream->is_open()) {
    fileStream->close();
  }
  delete fileStream;
  delete filename;
}

// Reads next non-empty line from commands/game file
string FileLineReader::readLineFromFile() {
  string line;
  while (std::getline(*fileStream, line)) {
    // Skip blank lines
    if (!line.empty()) {
      return line;
    }
  }
  return "";
}

bool FileLineReader::eof() const { return fileStream->eof(); }

ostream &operator<<(ostream &os, const FileLineReader &flr) {
  os << "FileLineReader [file: " << *flr.filename << "]";
  return os;
}


// Constructs the adapter with a GameEngine and a FileLineReader for the file from a path
FileCommandProcessorAdapter::FileCommandProcessorAdapter(GameEngine *ge, const string &filename)
    : CommandProcessor(ge), flr(new FileLineReader(filename)) {}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(
    const FileCommandProcessorAdapter &other)
    : CommandProcessor(other), flr(new FileLineReader(*other.flr)) {}

FileCommandProcessorAdapter &
FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter &other) {
  if (this != &other) {
    CommandProcessor::operator=(other);
    delete flr;
    flr = new FileLineReader(*other.flr);
  }
  return *this;
}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() { delete flr; }

// Overrides readCommand() to read from the file instead of the console
// Adapter design pattern: adapts FileLineReader interface to the CommandProcessor readCommand() interface
// The only difference between this and normal CommandProcessor class. After each returning a Command object in their own way, validation and storage is all done the same using CommandProcessor methods.
Command *FileCommandProcessorAdapter::readCommand() {
  if (flr->eof()) {
    return nullptr;
  }
  string line = flr->readLineFromFile();
  if (line.empty()) {
    return nullptr;
  }
  cout << "[FileCommandProcessorAdapter] Read command from file: \"" << line
       << "\"" << endl;
  return new Command(line);
}

ostream &operator<<(ostream &os, const FileCommandProcessorAdapter &fcpa) {
  os << "FileCommandProcessorAdapter [" << *fcpa.flr << "]";
  return os;
}

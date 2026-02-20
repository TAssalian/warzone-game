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

/**
 * Returns the command text.
 */
string Command::getCommand() const { return *commandText; }

/**
 * Saves the effect string for this command (e.g., resulting state or error).
 */
void Command::saveEffect(const string &eff) {
  delete effect;
  effect = new string(eff);
}

string Command::getEffect() const { return *effect; }

ostream &operator<<(ostream &os, const Command &cmd) {
  os << "Command: \"" << *cmd.commandText << "\"" << " | Effect: \"" << *cmd.effect << "\"";
  return os;
}


CommandProcessor::CommandProcessor(GameEngine *ge)
    : commands(new vector<Command *>()), engine(ge), currentIndex(0) {}

CommandProcessor::CommandProcessor(const CommandProcessor &other)
    : commands(new vector<Command *>()), engine(other.engine),
      currentIndex(other.currentIndex) {
  for (Command *c : *other.commands) {
    commands->push_back(new Command(*c));
  }
}

CommandProcessor &CommandProcessor::operator=(const CommandProcessor &other) {
  if (this != &other) {
    for (Command *c : *commands) {
      delete c;
    }
    delete commands;

    commands = new vector<Command *>();
    engine = other.engine;
    currentIndex = other.currentIndex;
    for (Command *c : *other.commands) {
      commands->push_back(new Command(*c));
    }
  }
  return *this;
}

CommandProcessor::~CommandProcessor() {
  for (Command *c : *commands) {
    delete c;
  }
  delete commands;
  // engine is non-owning so do not delete
}

Command *CommandProcessor::readCommand() {
  cout << "[CommandProcessor] Enter command: ";
  string line;
  if (!std::getline(std::cin, line)) {
    return nullptr; // EOF or input failure
  }
  Command *cmd = new Command(line);
  saveCommand(cmd);
  return cmd;
}

void CommandProcessor::saveCommand(Command *cmd) {
  commands->push_back(cmd);
}

/**
 * Public getter
 * Returns the next Command object from the stored commands,
 * advancing the currentIndex cursor. All previous commands remain in the
 * collection for logging/tracking purposes.
 */
Command *CommandProcessor::getCommand() {
  if (currentIndex >= static_cast<int>(commands->size())) {
    return nullptr; // no more stored commands
  }
  return (*commands)[currentIndex++];
}

/**
 * Validates whether cmd is legal in the current game state.
 * If the command is invalid, saves a corresponding error message as the command's effect.
 */
bool CommandProcessor::validate(Command *cmd) {
  string text = cmd->getCommand();
  string stateName = engine->getCurrentStateName();

  if (engine->validateCommand(text)) {
    cmd->saveEffect("Command \"" + text + "\" is valid in state " + stateName + ".");
    return true;
  } else {
    cmd->saveEffect("ERROR: \"" + text + "\" is not a valid command in state " + stateName + ".");
    return false;
  }
}

ostream &operator<<(ostream &os, const CommandProcessor &cp) {
  os << "CommandProcessor [\n";
  for (const Command *c : *cp.commands) {
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

/**
 * Reads next non-empty line from the file.
 */
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


/**
 * Constructs the adapter with a GameEngine and a path to the command file.
 * Opens a FileLineReader for the given file.
 */
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

/**
 * Overrides readCommand() to read from the file via FileLineReader instead of the console 
 * Adapter pattern: adapts FileLineReader interface to the CommandProcessor readCommand() interface).
 */
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
  Command *cmd = new Command(line);
  saveCommand(cmd);
  return cmd;
}

ostream &operator<<(ostream &os, const FileCommandProcessorAdapter &fcpa) {
  os << "FileCommandProcessorAdapter [" << *fcpa.flr << "]";
  return os;
}

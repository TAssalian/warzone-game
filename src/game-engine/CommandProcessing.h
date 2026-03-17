#pragma once
#include "../Observer/LoggingObserver.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class GameEngine;

class Command: public ILoggable, public Subject {
private:
  std::string *commandText; // The command string entered by the user
  std::string *effect;      // Effect/result message saved after execution

public:
  Command(const std::string &cmd);
  Command(const Command &other);
  Command &operator=(const Command &other);
  ~Command();

  std::string getCommand() const;
  void saveEffect(const std::string &eff);
  std::string getEffect() const;

  std::string stringToLog() const override;

  friend std::ostream &operator<<(std::ostream &os, const Command &cmd);
};


// Reads commands from the console, stores them, and validates each command against the current GameEngine state.
class CommandProcessor: public ILoggable, public Subject {
private:
  GameEngine *engine;               // Pointer to the game engine
  virtual Command *readCommand();
  void saveCommand(Command *cmd);

public:
  std::vector<Command *> commandList; // Owns Command objects

  explicit CommandProcessor(GameEngine *ge);
  CommandProcessor(const CommandProcessor &other);
  CommandProcessor &operator=(const CommandProcessor &other);
  virtual ~CommandProcessor();

  // Public entry point that reads, stores, validates, and returns the next command
  Command *getCommand();

  // Validates whether cmd is legal in the current game state.
  // If invalid, saves an error message as the command's effect.
  bool validate(Command *cmd);


  std::string stringToLog() const override;

  friend std::ostream &operator<<(std::ostream &os, const CommandProcessor &cp);
};

// Helper class used by FileCommandProcessorAdapater that opens a text file and returns one non-empty line at a time
class FileLineReader {
private:
  std::string *filename;
  std::ifstream *fileStream;

public:
  // Opens the file at the file path
  FileLineReader(const std::string &fn);
  FileLineReader(const FileLineReader &other);
  FileLineReader &operator=(const FileLineReader &other);
  ~FileLineReader();

  std::string readLineFromFile();

  // Returns true if the end of file has been reached
  bool eof() const;

  friend std::ostream &operator<<(std::ostream &os, const FileLineReader &flr);
};

// Gets commands from a text file instead of the console
// All methods are the same as CommandProcessor to validate and store, except for readCommand() which is overridden to read from a file instead of the console
class FileCommandProcessorAdapter : public CommandProcessor {
private:
  FileLineReader *flr;

protected:
  Command *readCommand() override;

public:
  FileCommandProcessorAdapter(GameEngine *ge, const std::string &filename);
  FileCommandProcessorAdapter(const FileCommandProcessorAdapter &other);
  FileCommandProcessorAdapter &operator=(const FileCommandProcessorAdapter &other);
  ~FileCommandProcessorAdapter() override;

  friend std::ostream &operator<<(std::ostream &os, const FileCommandProcessorAdapter &fcpa);
};

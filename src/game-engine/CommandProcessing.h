#pragma once

// Forward-declare the driver entry point so it can be a friend of CommandProcessor.
int run_command_processing_driver(int argc, char *argv[]);

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class GameEngine;

class Command {
private:
  std::string *commandText; // The command string entered by the user
  std::string *effect;      // Effect/result message saved after execution

public:
  Command(const std::string &cmd);
  Command(const Command &other);
  Command &operator=(const Command &other);
  ~Command();

  std::string getCommand() const;
  /// Stores the effect string for this command
  void saveEffect(const std::string &eff);
  /// Returns the effect string
  std::string getEffect() const;

  friend std::ostream &operator<<(std::ostream &os, const Command &cmd);
};

/**
 * CommandProcessor
 * Reads commands from the console (stdin), stores them internally, and
 * validates each command against the current GameEngine state.
 */
class CommandProcessor {
private:
  std::vector<Command *> *commands; // Owns Command objects
  GameEngine *engine;               // Pointer to the game engine
  int currentIndex;                 // Index of the next command to return via getCommand()

protected:
  virtual Command *readCommand();

  void saveCommand(Command *cmd);

public:
  explicit CommandProcessor(GameEngine *ge);
  CommandProcessor(const CommandProcessor &other);
  CommandProcessor &operator=(const CommandProcessor &other);
  virtual ~CommandProcessor();

  /// Reads the next command, validates it, stores it, and returns a pointer.
  Command *getCommand();

  /// Validates whether cmd is legal in the current game state.
  /// If invalid, saves an error message as the command's effect.
  bool validate(Command *cmd);

  friend std::ostream &operator<<(std::ostream &os, const CommandProcessor &cp);

  // Grant the driver function access to the protected readCommand().
  friend int run_command_processing_driver(int argc, char *argv[]);
};

/**
 * FileLineReader
 * Helper class used by FileCommandProcessorAdapater that opens a text file and returns one line at a time.
 */
class FileLineReader {
private:
  std::string *filename;
  std::ifstream *fileStream;

public:
  /// Opens the file at the file path
  FileLineReader(const std::string &fn);
  FileLineReader(const FileLineReader &other);
  FileLineReader &operator=(const FileLineReader &other);
  ~FileLineReader();

  std::string readLineFromFile();

  /// Returns true if the end of file has been reached
  bool eof() const;

  friend std::ostream &operator<<(std::ostream &os, const FileLineReader &flr);
};

// ---------------------------------------------------------------------------

/**
 * FileCommandProcessorAdapter
 * Re-uses CommandProcessor's interface but gets commands from a text file instead of the console.
 * All methods are the same as CommandProcessor except for readCommand() which is overridden to read from a file.
 */
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

#include "src/game-engine/GameEngine.h"
#include "src/game-engine/CommandProcessing.h"
#include "src/Observer/LoggingObserver.h"

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

void printUsage() {
    cout << "\nUsage: ./TournamentDriver tournament -M <maps> -P <strategies> -G <games> -D <turns>\n" << endl;
    cout << "Parameters:" << endl;
    cout << "  -M: 1 to 5 map files" << endl;
    cout << "  -P: 2 to 4 player strategies (Aggressive, Benevolent, Neutral, Cheater)" << endl;
    cout << "  -G: 1 to 5 games per map" << endl;
    cout << "  -D: 10 to 50 max turns per game\n" << endl;
}

int main(int argc, char* argv[]) {
    // build command string from argv
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    string command;
    for (int i = 1; i < argc; i++) {
        if (i > 1) command += " ";
        command += argv[i];
    }
    
    // check if it starts with "tournament"
    if (command.substr(0, 10) != "tournament") {
        cout << "Error: Command must start with 'tournament'" << endl;
        printUsage();
        return 1;
    }
    
    // create game engine and log observer
    GameEngine* engine = new GameEngine();
    LogObserver* logObserver = new LogObserver("gamelog.txt");
    engine->attach(logObserver);
    
    // create command processor
    CommandProcessor* processor = new CommandProcessor(engine);
    processor->attach(logObserver);
    
    // parse the tournament command
    TournamentParams params;
    string errorMsg;
    
    if (!processor->parseTournamentCommand(command, params, errorMsg)) {
        cout << "Error: " << errorMsg << endl;
        printUsage();
        delete processor;
        delete logObserver;
        delete engine;
        return 1;
    }
    
    // run the tournament
    TournamentResults results = engine->runTournament(params);
    
    // print and log the results
    engine->printTournamentResults(results);
    engine->logTournamentResults(results);
    
    // cleanup
    delete processor;
    delete logObserver;
    delete engine;
    
    return 0;
}

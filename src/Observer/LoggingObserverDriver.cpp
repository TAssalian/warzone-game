

#include "LoggingObserver.h"
#include "../game-engine/CommandProcessing.h"
#include "../game-engine/GameEngine.h"
#include "../orders/Orders.h"
#include "../player/Player.h"
#include "../map/Map.h"

#include <iostream>
#include <fstream>

using std::cout;

// Writes a small command file for the FileCommandProcessorAdapter .
static void writeCommandFile(const std::string& path) {
    std::ofstream f(path);
    f << "loadmap maps/earth.map\n"
        << "validatemap\n"
        << "addplayer Alice\n"
        << "addplayer Bob\n"
        << "gamestart\n"
        << "issueorders\n"
        << "endissueorders\n"
        << "win\n"
        << "quit\n";
}

int main() {

    cout << "Part 5: Game Log Observer Driver\n\n";

    LogObserver log("gamelog.txt");

    // (1) subclass check
    cout << "-----------------------------------------------------------------------------------\n";
    static_assert(std::is_base_of<Subject, Command>::value, "Command must inherit Subject");
    static_assert(std::is_base_of<ILoggable, Command>::value, "Command must inherit ILoggable");
    static_assert(std::is_base_of<Subject, CommandProcessor>::value, "CommandProcessor must inherit Subject");
    static_assert(std::is_base_of<ILoggable, CommandProcessor>::value, "CommandProcessor must inherit ILoggable");
    static_assert(std::is_base_of<Subject, Order>::value, "Order must inherit Subject");
    static_assert(std::is_base_of<ILoggable, Order>::value, "Order must inherit ILoggable");
    static_assert(std::is_base_of<Subject, OrderList>::value, "OrderList must inherit Subject");
    static_assert(std::is_base_of<ILoggable, OrderList>::value, "OrderList must inherit ILoggable");
    static_assert(std::is_base_of<Subject, GameEngine>::value, "GameEngine must inherit Subject");
    static_assert(std::is_base_of<ILoggable, GameEngine>::value, "GameEngine must inherit ILoggable");
    cout << "(1) All classes inherit Subject and ILoggable - OK\n\n";

    //(2)(6) GameEngine::transition() 
    cout << "(2)(6) GameEngine::transition() -> gamelog.txt\n";
    GameEngine ge;
    ge.attach(&log);
    ge.transition("loadmap maps/earth.map");
    ge.transition("validatemap");
    ge.transition("addplayer Alice");
    ge.transition("gamestart");
    ge.transition("issueorders");
    ge.transition("endissueorders");
    ge.transition("win");
    ge.transition("quit");
    cout << "State: " << ge.getCurrentStateName() << "\n\n\n";

    // (2)(3) CommandProcessor::saveCommand() + Command::saveEffect()
    cout << "(2)(3) CommandProcessor::saveCommand() and Command::saveEffect() -> gamelog.txt\n";
	cout << "-----------------------------------------------------------------------------------\n";
    writeCommandFile("commands.txt");
    GameEngine ge2;
	ge2.attach(&log); //log now watches ge2, so all transitions and commands will be logged
    FileCommandProcessorAdapter fcp(&ge2, "commands.txt");
	fcp.attach(&log); //log watches fcp, so all commands read and saved by fcp will be logged
    Command* cmd = nullptr;
    //getCommand() will read one line from command.txt
    // it will then create a command object
	//saveCommand() will be called to push it into the command list and log it
    
    while ((cmd = fcp.getCommand()) != nullptr) {
        cmd->attach(&log);
        ge2.transition(cmd);
        if (ge2.getCurrentStateName() == "END") break;
    }
    cout << "\n";

    // (2)(4)(5) OrderList::addOrder() + Order::execute()
    cout << "(2)(4)(5) OrderList::addOrder() and Order::execute() -> gamelog.txt\n";
    OrderList ol;
    ol.attach(&log);

    Order* o1 = new Order("Deploy", nullptr);
    Order* o2 = new Order("Advance", nullptr);
    Order* o3 = new Order("Bomb", nullptr);

    Order* orders[] = { o1, o2, o3 };

    for (Order* o : orders) {
        o->attach(&log);
        ol.addOrder(o);   // (4) triggers log
        o->execute();     // (5) triggers log
    }

    cout << "\n=== Driver complete. See gamelog.txt ===\n";
    return 0;
}
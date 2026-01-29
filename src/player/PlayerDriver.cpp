#include <iostream>
#include "Player.h"
#include "../map/Map.h"

int main() {
    std::cout << "Driver starts:" << std::endl;
    Player p1("Leroy Jenkins");
    Player p2("Jane Doe");

    Territory* t1 = new Territory();
    Territory* t2 = new Territory();
    Territory* t3 = new Territory();

    t1->id = new int(0);
    t1->name = new std::string("Canada");
    t1->continentId = new int(0);
    t1->neighborsIds = new std::vector<int*>();
    t1->playerId = new int(-1);
    t1->armiesNum = new int(0);

    t2->id = new int(1);
    t2->name = new std::string("USA");
    t2->continentId = new int(0);
    t2->neighborsIds = new std::vector<int*>();
    t2->playerId = new int(-1);
    t2->armiesNum = new int(0);

    t3->id = new int(2);
    t3->name = new std::string("Mexico");
    t3->continentId = new int(0);
    t3->neighborsIds = new std::vector<int*>();
    t3->playerId = new int(-1);
    t3->armiesNum = new int(0);


    p1.addTerritory(t1);
    p1.addTerritory(t2);
    p2.addTerritory(t3);

    std::cout << "Player p1:" << std::endl;
    std::cout << p1 << std::endl;
    std::cout << "Player p2:" << std::endl;
    std::cout << p2 << std::endl;

    std::cout << "Leroy owns:" << std::endl;
    for (Territory* t : p1.getTerritories()) {
        std::cout << " - " << *t << std::endl;
    }

    std::cout << "Territories that Leroy is defending:" << std::endl;
    for (Territory* t : p1.toDefend()) {
        std::cout << " - " << *t << std::endl;
    }

    std::cout << "Territories that Leroy is attacking:" << std::endl;
    for (Territory* t : p1.toAttack()) {
        std::cout << " - " << *t << std::endl;
    }

    Player p3(p1);
    std::cout << "Copied player p1 into new Player p3:" << std::endl;
    std::cout << "Player p1:" << std::endl;
    std::cout << p1 << std::endl;
    std::cout << "Player p3:" << std::endl;
    std::cout << p3 << std::endl;

    p2 = p1;
    std::cout << "Assigned p2 to p1:" << std::endl;
    std::cout << "Player p1:" << std::endl;
    std::cout << p1 << std::endl;
    std::cout << "Player p2:" << std::endl;
    std::cout << p3 << std::endl;

    // Include IssueOrder() verification

    std::cout << "Driver ends:" << std::endl;
    return 0;
}

#pragma once

#include <string>
#include <vector>
#include <iostream>

class Territory;
//class OrdersList;
//class Hand;

class Player {
private:
    const int* id;
    static int* nextId;

    std::string* name;
    std::vector<Territory*>* territories;
    //OrdersList* orders;
    //Hand* hand;

public:
    Player();
    Player(std::string name);

    Player(const Player& other);
    Player& operator=(const Player& other);
    ~Player();

    friend std::ostream& operator<<(std::ostream& os, const Player& p);

    std::string getName() const;
    const std::vector<Territory*>& getTerritories() const;
    //Hand* getHand() const;
    //OrdersList* getOrders() const;

    void addTerritory(Territory* territory);
    void removeTerritory(Territory* territory);

    std::vector<Territory*> toDefend() const;
    std::vector<Territory*> toAttack() const;

    //void issueOrder();

};


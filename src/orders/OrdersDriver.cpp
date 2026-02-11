#include <iostream>
#include "Orders.h"
#include "../player/Player.h"
#include "../map/Map.h"

using namespace std;

int run_orders_driver(){
    cout<<"Creating players, territories and orders for testing..."<<endl;
    Player* player1=new Player("Alice");
    cout<<"Player created: "<<player1->getName()<<endl;
    Player* player2=new Player("Bob");
    cout<<"Player created: "<<player2->getName()<<endl;
    Territory* t1=new Territory(1,"Territory1", 1);
    cout<<"Territory created: "<<*(t1->getName())<<endl;
    Territory* t2=new Territory(2,"Territory2", 1);
    cout<<"Territory created: "<<*(t2->getName())<<endl;
    player1->addTerritory(t1);
    player2->addTerritory(t2);
    cout<<"Territories assigned to players."<<endl;

    cout<<"Creating deploy order..."<<endl;
    DeployOrder* deployOrder=new DeployOrder(player1,5, t1);

    cout<<"Deploy order added to player1's order list."<<endl;
    player1->getOrders()->addOrder(deployOrder); 
    
    cout<<"Creating advance order..."<<endl;
    AdvanceOrder* advanceOrder=new AdvanceOrder(player1,3,t1,t2);

    cout<<"Advance order added to player1's order list."<<endl;
    player1->getOrders()->addOrder(advanceOrder);

    cout<<"Creating bomb order..."<<endl;
    BombOrder* bombOrder=new BombOrder(player1,t2);
    cout<<"Bomb order added to player1's order list."<<endl;
    player1->getOrders()->addOrder(bombOrder);

    cout<<"Creating blockade order..."<<endl;
    BlocakdeOrder* blockadeOrder=new BlocakdeOrder(player1,t2);
    cout<<"Blockade order added to player1's order list."<<endl;
    player1->getOrders()->addOrder(blockadeOrder);

    cout<<"Creating airlift order..."<<endl;
    AirliftOrder* airliftOrder=new AirliftOrder(player1,4,t1,t2);
    cout<<"Airlift order added to player1's order list."<<endl;
    player1->getOrders()->addOrder(airliftOrder);

    cout<<"Creating negotiate order..."<<endl;
    NegotiateOrder* negotiateOrder=new NegotiateOrder(player1,player2);
    cout<<"Negotiate order added to player1's order list."<<endl;
    player1->getOrders()->addOrder(negotiateOrder);

    cout << "Player1's Orders after creation:" << endl;
    cout << *(player1->getOrders()) << endl;
    
    cout<<"Moving airlift order to the top of the list..."<<endl;
    player1->getOrders()->move(4, 0);
    cout << "Player1's Orders after moving negotiate order to the top:" << endl;
    cout << *(player1->getOrders()) << endl;

    cout<<"Removing blockade order from the list..."<<endl;
    player1->getOrders()->remove(4);
    cout << "Player1's Orders after removing blockade order:" << endl;
    cout << *(player1->getOrders()) << endl;

    cout<<"Executing deploy order..."<<endl;
    deployOrder->execute();
    cout<<"Executing advance order..."<<endl;
    advanceOrder->execute();
    cout<<"Executing bomb order..."<<endl;
    bombOrder->execute();
    cout<<"Executing airlift order..."<<endl;
    airliftOrder->execute();
    cout<<"Executing negotiate order..."<<endl;
    negotiateOrder->execute();
    cout << "Player1's Orders after execution:" << endl;
    cout << *(player1->getOrders()) << endl;


}
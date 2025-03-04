#include<iostream>
#include<string>
#define log(x) std::cout<<x<<std::endl

class Entity
{
        int entityID;
    public:
        Entity():entityID(0)
        {
            log("Created Entity!");
        }
        Entity(int ID):entityID(ID)
        {
            log("Created Entity: " + std::to_string(ID));
        }
        ~Entity()
        {
            log("Entity Destroyed!");
        }
        virtual std::string GetClassName()
        {
            return "Entity";
        }
};

class Player : public Entity
{
    std::string Player_Name;
    public:
        Player(): Entity(7), Player_Name("X")
        {
            log("Created Player: " + Player_Name);
        }

        ~Player()
        {
            log("Player Destroyed!");
        }
        
        std::string GetClassName() override
        {
            return "Player";
        }

};

void printName(Entity *obj)
{
    log("Printing class name: " + obj->GetClassName());
}

int main()
{
    Player pl1;
    printName(&pl1);
    
}
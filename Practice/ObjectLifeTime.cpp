#include<iostream>
#define log(x) std::cout<<x<<std::endl;
class Entity
{
    public:
    Entity()
    {
        log("Entity created!");
    }
    ~Entity()
    {
        log("Destroyed Entity!");
    }
};
class ScopePointer
{
    Entity* m_entity;
public:
    ScopePointer(Entity* e):m_entity(e){}
    ~ScopePointer()
    {
        delete m_entity;
    }
};

int main()
{
    {
        ScopePointer sp = ScopePointer(new Entity());
    }
    return 0;
}
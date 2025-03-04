#include<iostream>
#include<string>

class Entity
{
private:
    int m_age;
    std::string m_name;
public:
    explicit Entity(int age):m_age(age), m_name("Unknown"){} // constructors should be declared explicit 
    explicit Entity(const std::string& name):m_name(name), m_age(-1){}
};

int main()
{
    //Entity e1 = 22; // Implicit conversion it allows to converts any type to any other implicitally  - should be avoided
    Entity e1(22);// Explicit conversion
    Entity e2("Saumit Kumar");
    return 0;
}
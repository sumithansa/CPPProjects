/* Shared pointer


*/

#include<iostream>
#include <cstring>
#include<string>
#include<array>

#define log(x) std::cout<<x<<std::endl

class Base
{
public:
    Base(){log("Base constructed!");}
    virtual ~Base(){log("Base Destructed!");}
};

class Derived: public Base
{
    int* m_int;
public:
    Derived(): m_int(new int[20])
    {
        log("Derived constructed!");
    }
    ~Derived()
    {
        delete[] m_int;
        log("Derived Destructed!");
    }
};

int main()
{
    Base* b = new Base();
    delete b;
    log("-----------------------------------");
    Derived* d = new Derived();
    delete d;
    log("-----------------------------------");
    Base* bd = new Derived();
    delete bd;
}

/* This is member initializer list: 
members will be initialized in the order they are declared in class. 
why it is required?:  It saves memory by not creating tempory objects, example is shown below
*/
#include<iostream>
#include<string>

class Example
{
    public:
        Example()
        {
            std::cout<<"Created Example!"<<std::endl;
        }
        Example(int x)
        {
            std::cout<<"Created Example "<<x<<" !"<<std::endl;
        }
};

class Entity
{
private:
    int m_Score;
    std::string m_Name;
    Example m_ex;
public:
    Entity(): m_Name("Unknown"), m_Score(0), m_ex(8)
    {
        //m_Name = "Unknown";
        //m_ex = Example(8); this will cause creation of temporary example object 
    }
    Entity(const std::string & name, const int & score, int x): m_Name(name), m_Score(score), m_ex(x)
    {
        //m_Name = name;
    }
    const std::string & GetName() const{return m_Name;}
};

int main()
{
    Entity e0;
    std::cout<<e0.GetName()<<std::endl;

    Entity e1("Sumit", 7, 1800);
    std::cout<<e1.GetName()<<std::endl;
    return 0;
}

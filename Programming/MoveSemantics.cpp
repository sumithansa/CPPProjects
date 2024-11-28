#include<iostream>
#include<string.h>
#include <cstdint>

class String
{
    private:
        char* m_name;
        uint32_t m_size;

    public:
        String() = default;
        String(const char* string)
        {
            // constructor
            std::cout<<"Constructed !"<<std::endl;
            m_size = strlen(string);
            m_name = new char[m_size + 1];
            memcpy(m_name, string, m_size);
             m_name[m_size] = '\0';
        }
        String(const String& other)
        {
            // copy constructor
            std::cout<<"Copied !"<<std::endl;
            m_size = other.m_size;
            m_name = new char[m_size + 1];
            memcpy(m_name, other.m_name, m_size);
            m_name[m_size] = '\0';
        }

        String(String&& other) noexcept
        {
            // move constructor
            std::cout<<"Moved !"<<std::endl;
           
            m_size = other.m_size;
            m_name = other.m_name; // this & next 2 lines are necessary for move constructor to work
            
            other.m_name = nullptr;
            other.m_size = 0;

        }

        String& operator=(String&& other) noexcept
        {
            // move assignment operator
            std::cout<<"Move Assignemt !"<<std::endl;
            if(this != &other)
            {
                m_size = other.m_size;
                m_name = other.m_name;
                
                other.m_name = nullptr;
                other.m_size = 0;
            }
            return *this;;
        }

        String& operator = (const String& other)
        {
            // copy assignment
            std::cout<<"Copy assigned !"<<std::endl;
            m_size = other.m_size;
            m_name = new char[m_size + 1];
            memcpy(m_name, other.m_name, m_size);
            m_name[m_size] = '\0';
            return *this;
        }
        ~String()
        {
            std::cout<<"Destructed !"<<std::endl;
            delete[] m_name;
        }

        void Print()
        {
            for(auto i=0; i<m_size; i++)
            {
                std::cout<<m_name[i];
            }
            std::cout<<std::endl;
        }
};

class Entity
{
    public:
        Entity(const String& name):m_Name(name){}
        Entity(String&& name):m_Name(std::move(name)){} // const is removed for move constructor
        //Entity(String&& name):m_Name((String&&)name){} // this and above lines are same
        ~Entity()
        {
            std::cout<<"Destructed Entity!"<<std::endl;
        }
        void PrintName()
        {
            m_Name.Print();
        }

    private:
        String m_Name;
};

int main()
{
    // can't use default constructor here as it is not assigning heap memory to m_name
    Entity entity("Sumit");
    entity.PrintName();

    std::cout<<"------------------------------------------"<<std::endl;
    String source = "Hello";            // constructor
    String dest0 = source;              // copy constructor
    std::cout<<"------------------------------------------"<<std::endl;
    String dest(std::move(source));     // move constructor
    std::cout<<"------------------------------------------"<<std::endl;
    String dest2;
    dest2 = dest;                       // copy assignement operator
    std::cout<<"------------------------------------------"<<std::endl;
    String dest3;
    dest3 = std::move(dest);            // move assignement operator
    std::cout<<"------------------------------------------"<<std::endl;
    return 0;
}
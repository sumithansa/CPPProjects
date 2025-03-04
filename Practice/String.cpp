/* String
    1. raw pointer
    2. size
    3. constructor/Destructor
    4. copy constructor + copy assignemnt
    5. move
    6. return size
    7. index operator overload
    8. output operator overload


*/

#include<iostream>
#include <cstring>
#include<string>
#define log(x) std::cout<<x<<std::endl



class String
{
    char *m_string;
    size_t m_size;
public:
    String(const char* string)
    {
        m_size = strlen(string);
        m_string = new char[m_size + 1];
        memcpy(m_string, string, m_size);
        m_string[m_size] = 0;
        log("Constructed string!");
    }
    ~String()
    {
        if(m_string)
            delete[] m_string;
        log("Destroyed String!");
    }
    String(String& other)
    {
        if(other.m_size != 0)
        {
            m_size = other.m_size;            
            m_string = new char[m_size];
            memcpy(m_string, other.m_string, m_size);
            log("Copied string!");
        }
    }
    String& operator=(const String& other)
    {
        if(other.m_size != 0)
        {
            m_size = other.m_size;            
            m_string = new char[m_size];
            memcpy(m_string, other.m_string, m_size);
            log("Assigned string!");
        }
        return *this;
    }

    String(String&& other)
    {
        m_size = other.m_size;            
        m_string = other.m_string;
        other.m_size = 0;
        other.m_string = nullptr;
        log("Moved string!");
    }

    String& operator=(String&& other)
    {
        m_size = other.m_size;            
        m_string = other.m_string;
        other.m_size = 0;
        other.m_string = nullptr;
        log("Move Assigned string!");
        return *this;
    }


    size_t size() const
    {
        return m_size;
    }

    char& operator[](int index)const{return m_string[index];}
    friend std::ostream& operator<<(std::ostream& out, const String& str);
};

std::ostream& operator<<(std::ostream& out, const String& str)
{
    out<<str.m_string<<std::endl;
    return out;
}


int main()
{
    String a("My Name is: Saumit Kumar !");
    log(a);
    String s1("Saumit");
    {
        String s2(s1);
        {
            String s3 = s2;
            {
                std::cout<<"s3[2]: "<<s3[2]<<std::endl;
            }
        }
    }
    std::cout<<s1<<std::endl;
}
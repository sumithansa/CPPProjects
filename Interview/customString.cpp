/*
Creating a customString struct in C to handle string operations.
Below is a list of operations:

• init() This function initializes the string with memory   [my own]
• free() deletes memory   [my own]
• operator = This function copies one string to another   [my own]
• length() This function returns the length of the string   [my own]
• begin() This function returns an iterator to the beginning of the string. [referenced + improvised]
• end() This function returns an iterator to the next to the end of the string. [referenced + improvised]
• substr(“char array”, pos, len) returns the substring of a string starting from pos till length [referenced + improvised]
• concat(str1, str2) concatenates str1 with str2   [my own]
• compare(str1, str2) compares str1 with str2 and returns true if both are equal.   [my own]
*/

#include<iostream>
#include <cstring>
#include<string>

#define log(x) std::cout<<x<<std::endl

// [Refernced this from google and improvised according tou the requirement]
struct customStringIterator
{
    customStringIterator(char* ptr): m_ptr(ptr){}
    customStringIterator& operator++() //prefix increment operator
    {
        m_ptr++;
        return *this;
    }
    customStringIterator operator++(int) //postfix increment operator
    {
        customStringIterator iterator = *this;
        ++(*this);
        return iterator;
    }
    customStringIterator& operator--() //prefix decrement operator
    {
        m_ptr--;
        return *this;
    }
    customStringIterator operator--(int) //postfix decrement operator
    {
        customStringIterator iterator = *this;
        --(*this);
        return iterator;
    }

    char& operator[](int index)
    {
        return *(m_ptr + index);
    }

    char operator*()
    {
        return *m_ptr;
    }

    char* operator->()
    {
        return m_ptr;
    }

    bool operator==(const customStringIterator& other)const
    {
        return m_ptr == other.m_ptr;
    }

    bool operator!=(const customStringIterator& other)const
    {
        return !(*this == other);
    }


private:
    char* m_ptr;    
};

struct customString
{
    char *m_string;
    size_t m_size;

    customString(): m_size(0), m_string(nullptr)
    { }

    customString(const char* string)
    {
        m_size = strlen(string);
        m_string = new char[m_size + 1];
        memcpy(m_string, string, m_size);
        m_string[m_size] = 0;
        log("Constructed string!");
    }

    ~customString()
    {
        m_size = 0;
        if(m_string)
            delete[] m_string;
        log("Freed customString!");
    }
    
    void init(const char* string)       // assuming that input string is provided by the client
    {
        m_size = strlen(string);
        m_string = new char[m_size + 1];
        memcpy(m_string, string, m_size);
        m_string[m_size] = 0;           // Null-terminate the substring
        log("Initialized customString!");
    }
    void free()
    {
        this->~customString();
    }

    customString(const customString& other)
    {
        if(other.m_size != 0)
        {
            m_size = other.m_size;            
            m_string = new char[m_size +1];
            memcpy(m_string, other.m_string, m_size);
            m_string[m_size] = 0;       // Null-terminate the substring
            log("Copied string!");
        }
    }
    customString& operator=(const customString& other)
    {
        if(other.m_size != 0)
        {
            m_size = other.m_size;            
            m_string = new char[m_size];
            memcpy(m_string, other.m_string, m_size);            
            m_string[m_size] = 0;       // Null-terminate the substring
            log("Assigned string!");
        }
        return *this;
    }

    size_t length() const
    {
        return m_size;
    }
    customStringIterator begin()   //This function returns an iterator to the beginning of the string.
    {
        // [Refernced this from google and improvised according tou the requirement]
        return customStringIterator(m_string); 
    }
    customStringIterator end()     //This function returns an iterator to the next to the end of the string.
    {
        // [Refernced this from google and improvised according tou the requirement]
        return customStringIterator(m_string + m_size);
    }
    customString substr(size_t pos, size_t len) const // returns the substring of a string starting from pos till length
    {
        if(pos >= m_size)
            return customString("");
         // Adjust length if it exceeds available characters
         len = (pos + len > m_size) ? (m_size - pos) : len;
         
         customString result;
         result.m_string = new char[len+1];
         result.m_size = len;
         memcpy(result.m_string, m_string + pos, len);
         result.m_string[len] = '\0'; 
         return result;
    }

    void concat(customString& str1)// concatenates str1
    {
        size_t total_size = m_size + str1.m_size + 1;
        char *temp = new char[total_size];
        strcpy(temp, m_string);
        strcat(temp, str1.m_string);
        free();
        m_string = temp;
        m_size = total_size;
    }

    char& operator[](int index)const{return m_string[index];}
    bool compare(const customString& str1) //compares str1 with current string and returns true if both are equal.
    {
        if(str1.length() != m_size)
            return false;
        for(int i = 0; i<str1.length(); i++)
        {
            if(str1[i] != m_string[i])
                return false;
        }
        return true;
    }
    friend customString concat_new(const customString& str1, const customString& str2);// concatenates str1 with str2
    friend std::ostream& operator<<(std::ostream& out, const customString& str);
    
};
std::ostream& operator<<(std::ostream& out, const customString& str)
{
    out<<str.m_string;
    return out;
}
customString concat_new(const customString& str1, const customString& str2)// concatenates str1 with str2
{
    size_t total_size = str1.m_size + str2.m_size + 1;
    char *temp = new char[total_size];
    strcpy(temp, str1.m_string);
    strcat(temp, str2.m_string);
    customString out;
    out.m_string = temp;
    out.m_size = total_size;
    return out;
}

int main()
{
    customString c1;
    c1.init("abcdef");
    log(c1);
    log(c1.length());
    for(auto it = c1.begin(); it != c1.end(); it++)
    {
        log(*it);
    }

    log(*(c1.begin()));
    customString c2;
    c2.init("defghij");
    log(c2);
    log(c1.compare(c2));
    c1.concat(c2);
    log(c1);
    log(c1.m_size);
    customString c3 = c1.substr(10,3);
    log(c3.length());
    std::cout<<c3<<std::endl;
    log(c3);
    customString c4 = concat_new(c2, c3);
    log(c4);
    c1.free();
    return 0;
}
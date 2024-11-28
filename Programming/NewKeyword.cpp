/*
Demonstartion of heap and stack:
Heap vs Stack:
1: stack objects are scoped objects and they live only in their scope.
However, heap objcts can live until program lives.
2: stack is limited so should be used with smaller objects.
3: stack is faster than heap.

In order to keep track of a heap object out of its scope, it needs to be pointed to 
a global pointer or a pointer outside that scope. As shown in example below.

*/
#include<iostream>
#include<string>

class Example
{
    private:
        std::string m_Name;
    public:
        Example():m_Name("Unknown"){}
        Example(const std::string& name):m_Name(name){}
        void displayName()
        {
            std::cout<<"Name is: "<<m_Name<<std::endl;
        }
};

int main()
{
    Example stackObject;
    Example *ptr = &stackObject;
    {
        Example *heapPtr = new Example("Sumit");
        ptr = heapPtr;
    }
    ptr->displayName();
    delete ptr;
}

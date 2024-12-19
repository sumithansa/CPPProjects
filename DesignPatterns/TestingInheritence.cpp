// Online C++ compiler to run C++ program online
#include <iostream>

class A
{
    int a;
protected:
    int e;
public:
    int b;
    virtual void fun1() = 0;
};
class B: public A
{
    int c;
public:
    int d;
    void fun1()
    {
        e =6;
        c =7;
        d = 8;
        b =9;
        std::cout<<"Hi"<<std::endl;
    }
};

template <typename T>
void func(const T& a)
{
    std::cout<<a<<std::endl;
}
int main() {
    // Write C++ code here
    std::cout << "Try programiz.pro\n";
    B b;
    A* ptr = &b;
    ptr->fun1();
    
    B* ptr2 = &b;
    ptr2->fun1();

    func(1.0f);
    func('a');

    const int &d = 1000;
    func(d);

    return 0;
}
#include<iostream>

template<class T>
class Arithmetic
{
    private:
    T num1, num2;

    public:
    Arithmetic(T a, T b):num1(a),num2(b){};
    T add();
    T sub();
};

template<class T>
T Arithmetic<T>::add()
{
    return num1 + num2;
}

template<class T>
T Arithmetic<T>::sub()
{
    return num1-num2;
}

int main()
{
    Arithmetic<int> a(1,7);
    std::cout<<"Addition: "<<a.add()<<std::endl;    
    std::cout<<"Substraction: "<<a.sub()<<std::endl;
    char c;
    std::cin>>c;
    return 0;
}
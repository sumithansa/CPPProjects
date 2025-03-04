#include<iostream>

template <typename T>
T summers(T val)
{
    return val;
}

template<typename T, typename ... Args>
T summers(T val, Args... args)
{
    return summers(args...) + val;
}

int main()
{
    int a = 2, b = 3, c = 4, d = 5, e = 6;
    std::cout<<summers<int>(a,b,c,d,e);
    return 0;
}
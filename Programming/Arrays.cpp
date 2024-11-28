#include<iostream>

int main()
{
    int a[] ={1,2,3};
    std::cout<<a[2]<<std::endl;

    int* p = new int[3]{5,6};
    p[0] =7;p[2]=8;
    std::cout<<p[0]<<std::endl;

    delete[] p;

    char ch[] = {'z','x','y', '\0'};
    
    std::cout<<ch<<std::endl;
    return 0;
}
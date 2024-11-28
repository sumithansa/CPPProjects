#include<iostream>

void recursion(int n)
{
    if(n>0)
    {
        recursion(n-1);             // calling phase
        std::cout<<n<<std::endl;    // returning phase - this is the diff b/w loop and recursion
                                    // in loops there is only calling phase and no returning phase.
    }
}
int main()
{
    recursion(7);
    char ch;
    std::cin>>ch;
    return 0;
}
#include<iostream>
#include<cmath>

bool prime_number(int num)
{
    for(int i = 4; i < sqrt(num+1); i++)
    {
        if(num%i == 0)
            return false;
        
    }
    return true;
}

int main()
{
    for(int i = 2; i <20; i++)
    {
        if(prime_number(i))
            std::cout<<i<<std::endl;
    }
}
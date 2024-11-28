/*
Given a signed 32-bit integer x, return x with its digits reversed. 
If reversing x causes the value to go outside the signed 32-bit integer 
range [-231, 231 - 1], then return 0.
*/
// This is the best solution on leetcode

#include<iostream>
#include <cstdint>  // required for INT32_MAX, INT32_MIN

bool limitExceeds(int64_t & z)
{
    if(z > INT32_MAX || z < INT32_MIN)
    {
        return true;
    }
    return false;
}
int revrse(int x)
{
    bool isNegative = false;
    if( x < 0)
    {
        int64_t z = (int64_t)x * (-1);
        if(limitExceeds(z))
        {
            return 0;
        }
        x = x*(-1);
        isNegative = true;
    }
    int y = 0, temp;
    while(x > 0)
    {
        temp = x % 10;  //3, 2, 1
        x = x / 10;     //12, 1, 0
        int64_t z = (int64_t)y * 10;
        if(limitExceeds(z))
        {
            return 0;
        }       
        y = y*10 + temp; //3, 32, 321
    }
    if(isNegative)
    {
        y = y*(-1);
    }
    return y;    
}

int main()
{
    INT32_MIN, INT32_MAX;
    int x = 964632435;  
      
    int64_t y = (int64_t)x * 10;
    if(y > INT32_MAX)
    {
        std::cout<<"y = "<<y<<std::endl;
        x = 0;
    }
    std::cout<<"x = "<<x<<std::endl;
    std::cout<<"reverse of x = "<<revrse(x)<<std::endl;

    return 0;
}
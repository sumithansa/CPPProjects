//Problem: Given an integer x, return true if x is a palindrome, and false otherwise.

#include<iostream>
#include<vector>

void isPalindrome_using_string(int number)
{
    std::cout<<"isPalindrome_using_string"<<std::endl;
    std::string sNumber = std::to_string(number);
    auto itr = sNumber.begin();
    auto ritr = sNumber.rbegin();

    for(;itr<ritr.base();++itr,++ritr)
    {
        if(*itr != *ritr)
        {
            std::cout<<number<<" is not a Palindrome!"<<std::endl;
            return;
        }
    }
    std::cout<<number<<" is a Palindrome!"<<std::endl;
}

void isPalindrome_without_using_string(int x)
{
    int number =x;
    std::cout<<"isPalindrome_without_using_string"<<std::endl;
    if(x<0)
        std::cout<<x<<" is not a Palindrome!"<<std::endl;

    std::vector<int> v;
    while( x > 0)
    {
        v.push_back(x % 10);
        x = x /10;
    }

    auto itr = v.begin();
    auto ritr = v.rbegin();
    for(;itr<ritr.base();++itr,++ritr)
    {
        if(*itr != *ritr)
        {
            std::cout<<number<<" is not a Palindrome!"<<std::endl;
            return;
        }
    }
    std::cout<<number<<" is a Palindrome!"<<std::endl;
}

int main()
{
    isPalindrome_using_string(1221);
    isPalindrome_without_using_string(1221);
    char ch;
    std::cin>>ch;
    return 0; 
}
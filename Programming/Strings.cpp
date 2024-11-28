#include<string>
#include<iostream>

void string_length(const std::string &str)
{
    int count = 0;
    for(auto itr = str.begin(); itr != str.end(); itr++)
    {
        count ++;
    }
    std::cout<<"\nThe length of the string is: "<<count<<std::endl;
}

void reverse_string(const std::string & str)
{
    std::string str1(str.rbegin(), str.rend()); // reversed the string
    /*for(auto itr = str.rbegin(); itr != str.rend(); itr++)
    {
        str1.push_back(*itr);
    }*/
    std::cout<<"The reverse of the string is: "<<str1<<std::endl;
}

void change_case(std::string & str1)
{
    std::string str = str1;
    for (auto itr = str.begin(); itr != str.end(); itr ++)
    {
        if(*itr >= 65 && *itr <=90)
            *itr += 32;
        else if(*itr >= 97 && *itr <=122)
            *itr -= 32;
    }
    std::cout<<"After changing case of the string: "<<str<<std::endl;
}

void check_palindrome(const std::string & str)
{
    auto itr = str.begin();
    auto ritr = str.rbegin();
    while(*(itr) == *(ritr) && itr < ritr.base()) // reverse iterator needs base to compare with iterator
    {
        std::cout<<"begin: "<<*itr<<"\trbegin: "<<*ritr<<std::endl;
        ++ritr;
        ++itr;
    }

    if(*itr != *ritr)
    {
        std::cout<<"Not a Palindrome!"<<std::endl;
    }
    else        
        std::cout<<"It's a Palindrome!"<<std::endl;

}

void find_username()
{
    std::string email;
    std::cout<<"Enter complete username: ";
    std::cin>>email;
    std::cout<<"Username is: "<<email.substr(0, email.find('@'))<<std::endl;
}
int main()
{
    /*std::string uname;
    std::cout<<"Enter your name: ";
    std::cin>>uname;
    std::cout<<"Your name is: "<<uname<<std::endl;
    std::cin>>uname;
    
    //std::getline(std::cin, uname);
    std::cout<<uname<<std::endl;*/
    std::string str1;
    std::cout<<"Enter string:";
    //std::cin>>str1;
    std::getline(std::cin, str1); // get string from user
    string_length(str1); // Find length of a string
    reverse_string(str1);// Reverse the string
    change_case(str1);  // Change case of the string
    check_palindrome(str1); // check if string is palindrome
    find_username(); // return username from emailid
    std::cout<<str1<<std::endl;

    return 0;
}
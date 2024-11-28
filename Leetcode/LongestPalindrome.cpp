/*Given a string s, return the longest palindromic substring in s.*/
#include<iostream>
#include<map>
#include<format>
#include<string>

bool thisIsPalindrome(const std::string& str)
{
    std::string rev_str(str.rbegin(), str.rend());
    return (str == rev_str);
}

std::string lengthOfLongestPalindrome(const std::string &s)
{
    std::string longestPalindromeString;
    std::string temp;
    int max_length = 0;
    int i = 0;
    while(i<s.size())
    {
        temp =s[i];
        //auto str2 = s.substr(i+1);
        auto pos = s.find_last_of(temp);
        int no_of_elements = ++pos;
        if( pos != std::string::npos && pos != i)
        {
            temp = s.substr(i, no_of_elements);
            //std::cout<<"\n"<<temp<<std::endl;
            if(thisIsPalindrome(temp) && ! temp.empty())
            {
                if(max_length < temp.size())
                {
                    longestPalindromeString = temp;
                    max_length = temp.size();
                }

                if( max_length == s.size())
                {
                    break;
                }
            }            
        }
        else
        {
            if(max_length < temp.size())
            {
                longestPalindromeString = temp;
            }
        }
        i++;           
    }

    return longestPalindromeString;
}

int main()
{
    std::string s = "cbbd"; // ac, abcccccc, babad, a, cbbd, bb
    std::cout<<"Longest Palindrome is: "<<lengthOfLongestPalindrome(s)<<std::endl;

    //int max_length = 0;
    //lengthOfLongestSubstring2(s, "", 0, max_length);
    //std::cout<<"Length Of longest substring2 is: "<<max_length;

    //std::cin.get();
    return 0;
}
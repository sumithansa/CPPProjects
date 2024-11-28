/* Given a string s, find the length of the longest substring without repeating characters.*/


#include<iostream>
#include<string>
#include<map>
#include<unordered_map>
#include<vector>

void lengthOfLongestSubstring2(const std::string &s, std::string current, int index, int& max_length )
{
    // This is the fatest among both solutions
    std::string temp;
    for(int i=0; i<s.size();i++)
    {
        //std::cout<<s[i]<<std::endl;
        temp = "";
        for (int j =i; j<s.size(); j++)
        {
            //std::cout<<s[j]<<std::endl;
            if(temp.find(s[j]) != std::string::npos) // This is still not supported in gcc +23 std: temp.contains(s[j])
            {
                // this char is present in temp
                break;
            }
            
            temp+=s[j];
        }
        if(temp.length()>max_length)
        {
            max_length = temp.length();
        }
        std::cout<<temp<<std::endl;
    }
}

int lengthOfLongestSubstring(std::string &s)
{
    // s = abcabcbb
    const int size = s.size();
    if(size == 1)
        return 1;
    
    std::map<int, std::string> ps;
    for (int i = 0, j=0; i<size && j<size;i++, j++)
    {
        std::string temp = "";
        for(int j = i; j<size;j++)
        {
            if(temp.find(s[j]) == std::string::npos)
            {
                temp += s[j];
            }
            else
            {
                break;
            }
        }
        ps[temp.length()]=temp;
    }

    /*for(auto v:ps)
    {
        std::cout<<v.first<<std::endl;
    }
    std::cout<<ps.at(ps.size())<<std::endl;*/

    auto lastpair = std::prev(ps.end());
    std::cout<<lastpair->second<<std::endl;
    auto sz = ps.size();
    return ps.at(sz).length();

    //return lastpair->first;
}

int main()
{
    std::string s = "pwwkew";
    std::cout<<"Length Of longest substring is: "<<lengthOfLongestSubstring(s)<<std::endl;

    int max_length = 0;
    lengthOfLongestSubstring2(s, "", 0, max_length);
    std::cout<<"Length Of longest substring2 is: "<<max_length;

    //std::cin.get();
    return 0;
}
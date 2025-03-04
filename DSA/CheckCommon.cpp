/*
we have 2 input vectors and we want to know if they have any thing common
*/
#include<iostream>
#include<vector>
#include<unordered_set>
#include<algorithm>
#include<string>

int common_item(std::vector<int>& v1, std::vector<int>& v2)
{
    std::unordered_set set(v1.begin(), v1.end());
    for(auto & i: v2)
    {
        if(set.find(i) != set.end())
            return i;
    }
    return 0;
}

int main()
{
    std::vector v1{2,4,5}; 
    std::vector v2{1,3,5};
    std::cout<<"Common Item is: "<<common_item(v1, v2)<<std::endl;
    std::string a = "atee";
    std::cout<<"Before sort: "<<a<<std::endl;
    std::sort(a.begin(), a.end());
    std::cout<<"After sort: "<<a<<std::endl;
    return 0;
}
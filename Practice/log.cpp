#include "log.h"

void PrintLevel(const Log& log)
{
    std::cout<<"Level: "<<log.GetLevel()<<std::endl;
}
int main()
{
    int x = 2;
    Log log(x);
    log.warn("Hello!");
    PrintLevel(log);
    std::string s = x==12?"Hi":"No Hi!";
    std::cout<<s<<std::endl;
}
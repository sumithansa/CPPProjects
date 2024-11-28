#include<iostream>
#include<string.h>

int main()
{
    //int size;
    //std::cout<<"Enter Size: ";
    //std::cin>>size;
    //int *p = new int [size];
    std::cout<<"Hello\n";
    int A[3] = {1,2};
    for(auto a :A)
    std::cout<<a<<std::endl;
    char name[10];
    std::cout<<"\nEnter name:";
    std::cin>>name;
    //std::cin.get(name,20);
    std::cout<<"\nEntered name is "<<name<<"\n";
    char s1[5] = "Good";
    char s2[8] = "Morning";
    std::cout<<strcat(s1,s2)<<"\nlength: "<<strlen(s1);


    return 0;
}
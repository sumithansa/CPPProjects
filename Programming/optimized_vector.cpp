/*
STL vectors can be optimized by using reserve () and
emplace_back() instead of push_back(), push_back() first constructs and then copies it to vector
1. First optimization - use reserve to tell compiler, we need 3 objects
2. Second optimization - use emplace_back() will eliminate call to copy constructor 
   as emplace_back constructs directly from constructor argument
*/
#include<iostream>
#include<string>
#include<vector>

class Example
{
    private:
        int a, b;
    public:
        Example(int x, int y):a(x), b(y)
        {
            std::cout<<"constructed! -> "<<a<<", "<<b<<std::endl;
        }

        Example(const Example& other): a(other.a), b(other.b)
        {
            std::cout<<"copied! -> "<<a<<", "<<b<<std::endl;
        }
};

int main()
{
    // un-optimized code
    std::vector<Example> obj1;
    obj1.push_back(Example(1,2));
    obj1.push_back(Example(3,4));
    obj1.push_back(Example(5,6));

    std::cout<<"----------------------------------------"<<std::endl;
    // optimized code
    std::vector<Example> obj2;
    //First optimization - use reserve to tell compiler, we need 3 objects
    obj2.reserve(3);
    //Second optimization - use emplace_back() will eliminate call to copy constructor 
    //as emplace_back constructs directly from constructor arguments
    obj2.emplace_back(1,2);
    obj2.emplace_back(3,4);
    obj2.emplace_back(5,6);
    return 0;
}
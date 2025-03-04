/* Shared pointer


*/

#include<iostream>
#include <cstring>
#include<string>
#include<array>

#define log(x) std::cout<<x<<std::endl

template<typename T, std::size_t N>
class array
{
    T m_array[N];
public:
    array()
    {
        log("Constructed array!");
    }

    size_t Size()const{return N;}
    T& operator[](int index)const{return m_array[index];}
    T& begin()const{return m_array[0];}
    T& end()const{return m_array[N-1];}

};

template<typename T, std::size_t N>
void print_array(const std::array<T, N> &arr)
{
    for(auto &i : arr)
        log(i);
}

int main()
{
    std::array<int, 7> my_array;
    print_array(my_array);
}

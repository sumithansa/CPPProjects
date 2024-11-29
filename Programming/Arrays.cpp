/*
Creating own Array class
- alloca() -> it allocates memory on stack, which can definitely increase performance in comparison to
malloc() or new or heap. But, the issue is compiler doesn't know how big the size this is. So it should 
be avoided.
*/
#include<iostream>
#include <string.h>
template<typename T, size_t S>
class Array
{
    public:
        /*
        Array(int size)
        {
            //m_Data = (int*) alloca(size); // it allocates memory on stack            
        }*/
       constexpr size_t Size() const{return S;} // we are not storing additional size info here.

       T& operator[](size_t index){return m_Data[index];}// remeber to return reference
       const T& operator[](size_t index)const {return m_Data[index];}// for const references
       
       T* Data(){return m_Data;}
       const T* Data()const{return m_Data;}
    private:
        // int* m_Data; // used for alloca
        T m_Data[S];
};

int main()
{
    //const size_t size = 5;
    Array<int, 10> data;
    memset(data.Data(), 0, data.Size()*sizeof(int));
    const auto& arrayOfReferences = data;
    std::cout<<"Array size :" << data.Size()<<std::endl;
    data[2] = 7;
    std::cout<<"Reference Array[2] :" << arrayOfReferences[2]<<std::endl;
    std::cout<<"Array[2] :" << data[2]<<std::endl;



    // playing with inbuilt []
    int a[] ={1,2,3};
    std::cout<<a[2]<<std::endl;

    int* p = new int[3]{5,6};
    p[0] =7;p[2]=8;
    std::cout<<p[0]<<std::endl;

    delete[] p;

    char ch[] = {'z','x','y', '\0'};
    
    std::cout<<ch<<std::endl;
    return 0;
}
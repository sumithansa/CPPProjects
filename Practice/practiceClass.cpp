#include<iostream>

template<typename T>
class Rectangle
{
    public:
        Rectangle():length(0), breadth(0){}
        Rectangle(T len):length(len), breadth(len){}
        Rectangle(T len, T br): length(len), breadth(br){}
        Rectangle(const Rectangle& other):length(other.length), breadth(other.breadth){}
        Rectangle operator=(const Rectangle& other)
        {
            length = other.length;
            breadth = other.breadth;
            return *this;
        }

        T area()const;
        T perimeter()const;
    private:
        T length;
        T breadth;
};

template<typename T>
T Rectangle<T>::area()const
{
    return length*breadth;
}

template<typename T>
T Rectangle<T>::perimeter()const
{
    return 2*(length + breadth);
}

// returning a heap 
int * fun()
{
    int* a = new int(10);
    return a;
}

int main()
{
    Rectangle r1(3,4);
    Rectangle rec = r1;
    std::cout<<"Area: "<<rec.area()<<std::endl;    
    std::cout<<"Perimeter: "<<rec.perimeter()<<std::endl;

    int *ptr = new int[10];
    std::cout<<"Size of ptr: "<<sizeof(ptr)<<"\t Size of int: "<<sizeof(int)<<std::endl;
    std::cout<<"Size: "<<sizeof(ptr)/sizeof(int)<<std::endl;

    std::string a = "Hi my name";
    char carry[10]= {'h', 'i', ' ', 'j'};
    std::cout<<a<<"      "<<carry;

    std::cout<<std::endl<<"Value = "<<*fun()<<std::endl;

    return 0;
}
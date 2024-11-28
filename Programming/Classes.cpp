#include<iostream>
#include<memory>    // smart pointer header

class Rectangle
{
    private:
    int length;
    char breadth;

    public:
    Rectangle():length(1), breadth(1){};
    Rectangle(int a):length(a), breadth(a){};
    Rectangle(int len, int bre):length(len), breadth(bre){};
    ~Rectangle(){std::cout<<"\nDestructor called for length: "<<length<<"\n";};
    int area() const;
    int perimeter() const;
};

int Rectangle::area() const
{
    return length * breadth;
}

int Rectangle::perimeter() const
{
    return 2*(length + breadth);
}

int main()
{
    Rectangle rec, rec2(2), rec3(21,31);
    std::unique_ptr<Rectangle> rec5(new Rectangle(410,510));    // smart pointer used
    Rectangle *rec4 = new Rectangle(41,51);
    std::cout<<"\nArea of rectangles: 1["<<rec.area()<<"]\t 2["<<rec2.area()<<"]\t 3["<<rec3.area()<<"]\t 4["<<rec4->area()<<"]\t 4["<<rec4->area()<<"]";
    std::cout<<"\nPerimeter of rectangles: 1["<<rec.perimeter()<<"]\t 2["<<rec2.perimeter()<<"]\t 3["<<rec3.perimeter()<<"]\n";
    delete rec4; // explicitly called for raw pointers

    return 0;
}
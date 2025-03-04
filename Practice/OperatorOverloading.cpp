#include<iostream>

#define print(x) std::cout<<x<<std::endl

class Vector2
{
    float m_x, m_y;
public:
    Vector2(float x, float y): m_x(x), m_y(y){}

    Vector2 operator+(const Vector2& obj)
    {
        this->m_x += obj.m_x;
        this->m_y += obj.m_y;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& stream, const Vector2& obj );
};

std::ostream& operator<<(std::ostream& stream, const Vector2& obj )
{
    stream << "x:" <<obj.m_x <<", y:"<< obj.m_y<<std::endl;
    return stream;
}
int main()
{
    Vector2 position(4.0f, 4.0f);
    Vector2 speed(0.5f, 1.5f);

    Vector2 a = position + speed;
    //print(a.m_x);
    //print(a.m_y);
    std::cout<<a;
}
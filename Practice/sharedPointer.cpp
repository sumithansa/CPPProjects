#include<iostream>

template <class T>
class shared_ptr
{
    T* m_ptr;
    int* m_reference_count;
public:
    explicit shared_ptr(T* ptr = nullptr): m_ptr(ptr), m_reference_count(new int(1))
    {
        std::cout<<"Constructed!  Reference count: "<<*m_reference_count<<std::endl;
    }
    ~shared_ptr()
    {
        if(m_reference_count>0)
        {
            --*m_reference_count;
            if(*m_reference_count == 0)
            {
                delete m_ptr;
                delete m_reference_count;
                m_ptr = nullptr;
                m_reference_count = nullptr;
                std::cout<<"Destroyed completely!"<<std::endl;
            }
            std::cout<<"Reference count: "<<*m_reference_count<<". Reference decremented!"<<std::endl;
        }
    }
    shared_ptr(const shared_ptr& other)
    {
        if(this != &other)
        {
            this->m_ptr = other.m_ptr;
            this->m_reference_count = other.m_reference_count;
            (*m_reference_count)++;
        }
        std::cout<<"Copied! Reference count: "<<*m_reference_count<<std::endl;
    }
    shared_ptr& operator=(const shared_ptr& other)
    {
        if(this != &other)
        {
            // should have called release method() on the this
            this->m_ptr = other.m_ptr;
            this->m_reference_count = other.m_reference_count;
            (*m_reference_count)++;
        }
        std::cout<<"Assigned! Reference count: "<<*m_reference_count<<std::endl;
        return *this;
    }

};

int main()
{
    shared_ptr<int> sh_ptr(new int(5));
    {
        shared_ptr<int> sh_ptr1 = sh_ptr;
        {
            shared_ptr<int> sh_ptr2(sh_ptr);
            {
                shared_ptr<int> sh_ptr3;
                sh_ptr3 = sh_ptr;
            }
        }
    }
}
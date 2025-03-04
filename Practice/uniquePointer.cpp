/* unique_ptr
features:
    1. raw pointer
    2. all types support
    3. move
    4. release
    5. get
    6. delete / destructor
    7. overloaded *
    8. overloaded ->
constratings:
    1. not copyabe
    2. not assignable
*/
#include<iostream>

#define log(x) std::cout<<x<<std::endl
template<typename T>    // all types support
class unique_ptr
{
    T* m_ptr;           // raw pointer
public:
    explicit unique_ptr(T *ptr = nullptr): m_ptr(ptr){log("Constructed Unique Pointer!");} // constructor
    ~unique_ptr()                                       // destructor
    {
        log("Destroyed  Unique Pointer!!");
        if(m_ptr)
            delete m_ptr;
    }

    unique_ptr(unique_ptr&) = delete;                   // copy constructor
    unique_ptr(unique_ptr&& other):m_ptr(other.m_ptr)   // move constructor
    {
        other.m_ptr = nullptr;
    } 
    unique_ptr& operator=(unique_ptr&) = delete;        // copy assignment
    unique_ptr& operator&=(unique_ptr&& other)          // move assignemnt
    {
        if(this != other)
        {
            delete m_ptr;
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        log("Moved!");
        return *this;
    }
    T& operator*()const {return *m_ptr;}    // overloaded derefence
    T* operator->()const {return m_ptr;}    // overloaded arrow

    T* get()const{return m_ptr;}            // get raw pointer
    T* release()const                     // release
    {
        T* temp = m_ptr;
        m_ptr = nullptr;
        log("Released!");
        return temp;        
    }
};

class Entity
{
    int num;
    public:
        Entity():num(0){log("Constructed Entity!");}
        ~Entity(){log("Deleted Entity!");}
        void get_num(){log(num);}
};

int main()
{
    unique_ptr<Entity> my_ptr(new Entity());
    my_ptr->get_num();

    int a = 7;
    int *ptr =  &a;
    unique_ptr<int> my_int_ptr(ptr);
    log(*my_int_ptr);
    my_int_ptr.release();
}
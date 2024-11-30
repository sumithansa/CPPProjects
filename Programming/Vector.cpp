
/*
My own vector implementation

*/
#include<iostream>
#include<vector>
using std::cout;
using std::endl;


template<typename T>
class Vector
{
    public:
        Vector()
        {              
            m_size = 1;
            m_data = new T[1]();
            reAlloc(2);
        }
        Vector(T data)
        {   
            cout<<" Constructed !"<<endl;
            m_size = 1;
            m_data = new T[1]();
            *m_data = data;
            reAlloc(2);        
        }

        Vector(const Vector& other)
        {
            cout<<" Copied !"<<endl;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            m_data = new T[m_size];
            *m_data = *other.m_data;
        }

        Vector(Vector&& other)
        {
            if(this != &other)
            {
                cout<<" Moved !"<<endl;
                m_capacity = other.m_capacity;
                m_size = other.m_size;
                m_data = other.m_data;
                other.m_size = 0;
                other.m_capacity = 0;
                other.m_data = nullptr;
            }
            
        }

        Vector& operator=(const Vector& other)
        {
            cout<<" Assigned !"<<endl;
            this->m_capacity = other.m_capacity;
            this->m_size = other.m_size;
            this->m_data = new T[m_size];
            *this->m_data = *other.m_data;
            return *this;
        }

        Vector&& operator=(Vector &&other)
        {
            cout<<" Move Assigned !"<<endl;
            this->m_data = other.m_data;
            this->m_size = other.m_size;
            other.m_data = {};
            other.m_size = 0;
            return *this;
        }

        ~Vector()
        {
            if(m_size>0)
            {
                cout<<" Destructed : "<< *m_data <<endl;
                //if(m_data)
                    delete[] m_data;
            }
        }

        T& operator[](size_t index)
        {
            // non-const version if we want to modify
            if(index >= m_size)
            {

            }
            return this->m_data[index];
        }

        const T& operator[](size_t index) const
        {
            return this->m_data[index];
        }

        T& at(size_t index)
        {
            return this->m_data[index];
        }

        const T& at(size_t index) const
        {
            return this->m_data[index];
        }

        void push_back(const T& data)
        {
            // push data
            if(m_size > m_capacity)
                reAlloc(m_capacity + m_capacity/2); // growing by 1.5 times 

            cout<<" Pushing: "<< data <<endl;
            m_data[m_size] = data;
            ++m_size;            
        }
        
        void pop_back()
        {
            cout<<" Popping: "<< endl;
            if(m_size>0)
            {
                m_size--;
                //m_data[m_size].~T();        // important to call destructor 
            }
        }

        void clear()
        {
            cout<<" Cleaning: "<< endl;
            delete [] m_data;
            /*for(size_t i =0; i<m_size; i++)
                {
                    //~T();
                    m_data[i].~T();
                }*/

            m_size = 0;
        }

        template<typename... Args>
        T& emplace_back(Args&&... data)     // Most important function, its different
        {
            // push data
            if(m_size > m_capacity)
                reAlloc(m_capacity + m_capacity/2); // growing by 1.5 times 

            cout<<" Emplacing: "<< endl;
            new(&m_data[m_size]) T(std::forward<Args>(data)...);     // elimiates use of move
            //m_data[m_size] = T(std::forward<Args>(data)...); 
            return m_data[m_size++];            
        }



        /*constexpr std::iterator begin()
        {
            return &m_data[0];
        }*/
        void resize(int newSize)
        {
            m_capacity = newSize;
        }
        const size_t Size()const {return m_size;}
        const size_t Capacity()const {return m_capacity;}

    private:
        size_t m_size = 0;
        size_t m_capacity = 0;  // used mainly for too much reallocation
        T* m_data = nullptr;

        void reAlloc(size_t newCapacity)
        {
            // 1. allocate a new block of memory
            // 2. copy/move old elements in new block
            // 3. delete
            // cout<<" Reallocation started !"<<endl;
            // if downsizing
            if(newCapacity < m_size)
                m_size = newCapacity;

            //T* temp = new T[newCapacity]; // calls constructor it can be avoided
            T* temp = (T*)::operator new(newCapacity * sizeof(T));
            for(int i=0; i< m_size; i++)
            {
                //cout<<" Reallocation: "<< endl;
                temp[i] = std::move(m_data[i]);
            }
            //clear();
            ::operator delete(m_data, m_capacity*sizeof(T));
            /*if(m_size > 1)
                delete[] m_data;
            else
                delete m_data;*/
            m_data = temp;
            m_capacity = newCapacity;
        }
};

int main()
{
    {
        Vector<int> v1(5);
        Vector<int> v2 = v1;
        Vector<int> v3(std::move(v2));
        Vector<int> v4;
        v4 = v3;
        Vector<int> v5(std::move(v4));
        cout<<"size of v5: "<<v5.Size()<<endl;
        cout<<"capacity of v5: "<<v5.Capacity()<<endl;
        cout<<"v5[0]: "<<v5.at(0)<<endl;
        //v5.resize(5);
        v5.push_back(7);
        v5.emplace_back(8);
        v5.emplace_back(81);
        v5.push_back(9);
        v5.push_back(10);
        
        cout<<"size of v5: "<<v5.Size()<<endl;
        cout<<"capacity of v5: "<<v5.Capacity()<<endl;

        v5.pop_back();
        cout<<"size of v5: "<<v5.Size()<<endl;
        cout<<"capacity of v5: "<<v5.Capacity()<<endl;
        cout<<"v5: ";
        for(int i = 0; i<v5.Size(); i++)
            cout<<"\t: "<<v5[i];
        cout<<endl;
        v5.clear();        
        cout<<"size of v5: "<<v5.Size()<<endl;
        cout<<"capacity of v5: "<<v5.Capacity()<<endl;

        cout<<endl;
    }
    cout<<"Ended"<<endl;


    cout<<"Real std::Vector"<<endl;
    std::vector<int> sv1;
    sv1.emplace_back(1);
    cout<<"sv1.capacity(): "<<sv1.capacity()<<endl;
    cout<<"sv1.size(): "<<sv1.size()<<endl;
    cout<<"sv1: "<<sv1.at(0)<<endl;
    return 0;
}
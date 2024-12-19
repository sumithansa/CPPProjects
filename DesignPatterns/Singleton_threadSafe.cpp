// Example of thread safe & lazy initialization singleton pattern

#include<iostream>
#include<mutex>

class Singleton
{
    private:
        static Singleton* instance;     // private instance
        static std::mutex mtx;          // mutex for thread safety
        Singleton(){}
    public:
        static Singleton* getInstance()
        {
            if(instance == nullptr)
            {
                std::lock_guard<std::mutex> lock(mtx);      // this is for thread safety
                if(instance == nullptr)
                {
                    std::cout<<"Instance created";
                    instance = new Singleton();
                }
            }
            return instance;
        }
        Singleton(const Singleton&) = delete;
        Singleton operator=(const Singleton&) = delete;
};
Singleton* Singleton::instance = 0;     // static member initilization is rnecessary
std::mutex Singleton::mtx;              // static member initilization is rnecessary

int main()
{
    auto inst = Singleton::getInstance(); // this is called lazy initialization
    return 0;
}
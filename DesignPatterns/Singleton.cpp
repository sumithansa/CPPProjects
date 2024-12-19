#include<iostream>
#include<string>
#include<fstream>
#include<map>
#include<cstdint>

class SingletonDatabase
{
    private:
        std::map<std::string, int32_t> capials;
        SingletonDatabase()
        {
            //std::cout<<"Initializing Database"<<std::endl;
            std::ifstream ifs("capitals.txt");
            
            std::string s1, s2;
            while(std::getline(ifs, s1))
            {
                std::getline(ifs, s2);
                auto pop = static_cast<int>(std::stoi(s2));
                capials[s1] = pop;
            }
        }
    public:
        SingletonDatabase (SingletonDatabase const&) = delete;  // no copy constructor
        void operator=(SingletonDatabase const&) = delete;      // no assignment operator

        static SingletonDatabase& get()
        {
            static SingletonDatabase db;        // single, static object
            return db;
        }

        int get_population(std::string& cityName)
        {
            return capials[cityName];
        }
};

int main()
{
    std::string city = "Kolkata";
    std::cout<<city<<" Population is: "
                <<SingletonDatabase::get().get_population(city)<<std::endl;
    return 0;
}
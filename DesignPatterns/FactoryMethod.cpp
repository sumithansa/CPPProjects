/*
Definition:- It's a creational Design Pattern which is used to create objects in a separate class
Client(main function) -> Factory class -> Product Interface -> concrete product.


*/
#include<iostream>
#include<string>


/*enum class color
{
    White,
    Silver,
    Red,
    Black
};*/

class Car // Product interface: it is used by Factory class.
{
protected:
    std::string carName;
public:
    Car(std::string name):carName(name)
    {
        std::cout<<"Starting Factory for "<<carName<<" !"<<std::endl;
    }
    std::string getName() const
    {
        return carName;
    }
    virtual std::string getColor() = 0;
    virtual void genertateCar() = 0;
    ~Car() = default;
};

class TataSafari: public Car    // concrete product
{
    std::string carName;
    std::string m_color;
public:
    TataSafari(std::string name, std::string _color): Car(name), carName(name), m_color(_color)
    {
        std::cout<<" - Creating Safari for you !"<<std::endl;
    }
    std::string getColor()
    {
        return m_color;
    }
    void genertateCar()
    {
        std::cout<<" - Succesfully generated "<<getColor()<<" Tata Safari. It is ready for delivery !"<<std::endl;        
    }
    ~TataSafari() = default;
};

class TataHarier: public Car    // concrete product
{
    std::string carName;
    std::string m_color;
public:
    TataHarier(std::string name, std::string _color): Car(name), carName(name), m_color(_color)
    {
        std::cout<<" - Creating Harier for you !"<<std::endl;
    }
    std::string getColor()
    {
        return m_color;
    }
    void genertateCar()
    {
        std::cout<<" - Succesfully generated "<<getColor()<<" Tata Safari. It is ready for delivery !"<<std::endl;
        getColor();
    }
    ~TataHarier() = default;
};

class Carfactory        // Factory class
{
    Car* newCar;
public:
    Carfactory() = default;
    ~Carfactory()
    {
        std::cout<<"  -> Deleting Factory"<<std::endl;
        delete newCar;
    }
    Car* getCar()
    {           
        char carOption = chooseCarOption();
        std::string colorOption = chooseColorOption();

        switch (carOption)
        {
        case '1':
        {
            newCar = new TataSafari("Safari", colorOption);
            newCar->genertateCar();
            return newCar;
        }
        case '2':
        {
            newCar = new TataHarier("Harier", colorOption);
            newCar->genertateCar();
            return newCar;
        }       
        default:
            std::cout<<">>>>>  Wrong option. Try Agaian.. <<<<<<"<<std::endl;
            return nullptr;
        }
    }
    char chooseCarOption()
    {
        std::cout<<" - Choose your Car to manufacture:"<<std::endl;
        std::cout<<"    1. Safari"<<std::endl;
        std::cout<<"    2. Harier"<<std::endl;
        std::cout<<" - Please enter option: ";
        char carOption;
        std::cin>>carOption;
        return carOption;
    }
    std::string chooseColorOption()
    {
        std::cout<<" - Please Enter your color choice: ";
        std::string colorOption;
        std::cin>>colorOption;
        return colorOption;
    } 
};

int main()      // Client
{
    Carfactory carFact;
    Car* car = carFact.getCar();
    return 0;
}


